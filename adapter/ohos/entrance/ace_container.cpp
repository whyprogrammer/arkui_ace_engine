/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "adapter/ohos/entrance/ace_container.h"

#include <fstream>
#include <functional>

#include "ability_context.h"
#include "ability_info.h"
#include "wm/wm_common.h"

#include "adapter/ohos/entrance/ace_application_info.h"
#include "adapter/ohos/entrance/ace_view_ohos.h"
#include "adapter/ohos/entrance/data_ability_helper_standard.h"
#include "adapter/ohos/entrance/file_asset_provider.h"
#include "adapter/ohos/entrance/hap_asset_provider.h"
#include "base/i18n/localization.h"
#include "base/log/ace_trace.h"
#include "base/log/dump_log.h"
#include "base/log/event_report.h"
#include "base/log/frame_report.h"
#include "base/log/log.h"
#include "base/subwindow/subwindow_manager.h"
#include "base/utils/system_properties.h"
#include "base/utils/utils.h"
#include "bridge/card_frontend/card_frontend.h"
#include "bridge/card_frontend/form_frontend_declarative.h"
#include "bridge/common/utils/engine_helper.h"
#include "bridge/declarative_frontend/declarative_frontend.h"
#include "bridge/js_frontend/engine/common/js_engine_loader.h"
#include "bridge/js_frontend/js_frontend.h"
#include "core/common/ace_engine.h"
#include "core/common/connect_server_manager.h"
#include "core/common/container_scope.h"
#include "core/common/flutter/flutter_asset_manager.h"
#include "core/common/flutter/flutter_task_executor.h"
#include "core/common/hdc_register.h"
#include "core/common/platform_window.h"
#include "core/common/text_field_manager.h"
#include "core/common/window.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_manager_impl.h"
#include "core/components_ng/pattern/text_field/text_field_manager.h"
#include "core/components_ng/render/adapter/form_render_window.h"
#include "core/components_ng/render/adapter/rosen_window.h"
#include "core/pipeline/pipeline_context.h"
#include "core/pipeline_ng/pipeline_context.h"

#if defined(ENABLE_ROSEN_BACKEND) and !defined(UPLOAD_GPU_DISABLED)
#include "adapter/ohos/entrance/ace_rosen_sync_task.h"
#endif

namespace OHOS::Ace::Platform {
namespace {

constexpr char ARK_ENGINE_SHARED_LIB[] = "libace_engine_ark.z.so";
constexpr char DECLARATIVE_ARK_ENGINE_SHARED_LIB[] = "libace_engine_declarative_ark.z.so";

#ifdef _ARM64_
const std::string ASSET_LIBARCH_PATH = "/lib/arm64";
#else
const std::string ASSET_LIBARCH_PATH = "/lib/arm";
#endif

const char* GetEngineSharedLibrary()
{
    return ARK_ENGINE_SHARED_LIB;
}

const char* GetDeclarativeSharedLibrary()
{
    return DECLARATIVE_ARK_ENGINE_SHARED_LIB;
}

} // namespace

AceContainer::AceContainer(int32_t instanceId, FrontendType type, std::shared_ptr<OHOS::AppExecFwk::Ability> aceAbility,
    std::unique_ptr<PlatformEventCallback> callback, bool useCurrentEventRunner, bool useNewPipeline)
    : instanceId_(instanceId), type_(type), aceAbility_(aceAbility), useCurrentEventRunner_(useCurrentEventRunner)
{
    ACE_DCHECK(callback);
    if (useNewPipeline) {
        SetUseNewPipeline();
    }
    InitializeTask();
    platformEventCallback_ = std::move(callback);
    useStageModel_ = false;
    auto ability = aceAbility_.lock();
    if (ability) {
        abilityInfo_ = ability->GetAbilityInfo();
    }
}

AceContainer::AceContainer(int32_t instanceId, FrontendType type,
    std::weak_ptr<OHOS::AbilityRuntime::Context> runtimeContext,
    std::weak_ptr<OHOS::AppExecFwk::AbilityInfo> abilityInfo, std::unique_ptr<PlatformEventCallback> callback,
    bool useCurrentEventRunner, bool isSubAceContainer, bool useNewPipeline)
    : instanceId_(instanceId), type_(type), runtimeContext_(std::move(runtimeContext)),
      abilityInfo_(std::move(abilityInfo)), useCurrentEventRunner_(useCurrentEventRunner),
      isSubContainer_(isSubAceContainer)
{
    ACE_DCHECK(callback);
    if (useNewPipeline) {
        SetUseNewPipeline();
    }
    if (!isSubContainer_) {
        InitializeTask();
    }
    platformEventCallback_ = std::move(callback);
    useStageModel_ = true;
}

void AceContainer::InitializeTask()
{
    auto flutterTaskExecutor = Referenced::MakeRefPtr<FlutterTaskExecutor>();
    flutterTaskExecutor->InitPlatformThread(useCurrentEventRunner_);
    taskExecutor_ = flutterTaskExecutor;
    // No need to create JS Thread for DECLARATIVE_JS
    if (type_ == FrontendType::DECLARATIVE_JS) {
        GetSettings().useUIAsJSThread = true;
    } else {
        flutterTaskExecutor->InitJsThread();
    }
}

void AceContainer::Initialize()
{
    ContainerScope scope(instanceId_);
    // For DECLARATIVE_JS frontend use UI as JS Thread, so InitializeFrontend after UI thread created.
    if (type_ != FrontendType::DECLARATIVE_JS) {
        InitializeFrontend();
    }
}

void AceContainer::Destroy()
{
    ContainerScope scope(instanceId_);
    if (pipelineContext_ && taskExecutor_) {
        // 1. Destroy Pipeline on UI thread.
        RefPtr<PipelineBase> context;
        context.Swap(pipelineContext_);
        if (GetSettings().usePlatformAsUIThread) {
            context->Destroy();
        } else {
            taskExecutor_->PostTask([context]() { context->Destroy(); }, TaskExecutor::TaskType::UI);
        }

        if (isSubContainer_) {
            // SubAceContainer just return.
            return;
        }

        // 2. Destroy Frontend on JS thread.
        RefPtr<Frontend> frontend;
        frontend_.Swap(frontend);
        if (GetSettings().usePlatformAsUIThread && GetSettings().useUIAsJSThread) {
            frontend->UpdateState(Frontend::State::ON_DESTROY);
            frontend->Destroy();
        } else {
            frontend->UpdateState(Frontend::State::ON_DESTROY);
            taskExecutor_->PostTask([frontend]() { frontend->Destroy(); }, TaskExecutor::TaskType::JS);
        }
    }
    resRegister_.Reset();
    assetManager_.Reset();
}

void AceContainer::DestroyView()
{
    ContainerScope scope(instanceId_);
    CHECK_NULL_VOID_NOLOG(aceView_);
    auto aceView = static_cast<AceViewOhos*>(aceView_);
    if (aceView) {
        aceView->DecRefCount();
    }
    aceView_ = nullptr;
}

void AceContainer::InitializeFrontend()
{
    auto aceAbility = aceAbility_.lock();
    if (type_ == FrontendType::JS) {
        frontend_ = Frontend::Create();
        auto jsFrontend = AceType::DynamicCast<JsFrontend>(frontend_);
        auto& loader = Framework::JsEngineLoader::Get(GetEngineSharedLibrary());
        auto jsEngine = loader.CreateJsEngine(instanceId_);
        jsEngine->AddExtraNativeObject("ability", aceAbility.get());
        EngineHelper::AddEngine(instanceId_, jsEngine);
        jsFrontend->SetJsEngine(jsEngine);
        jsFrontend->SetNeedDebugBreakPoint(AceApplicationInfo::GetInstance().IsNeedDebugBreakPoint());
        jsFrontend->SetDebugVersion(AceApplicationInfo::GetInstance().IsDebugVersion());
    } else if (type_ == FrontendType::JS_CARD) {
        AceApplicationInfo::GetInstance().SetCardType();
        frontend_ = AceType::MakeRefPtr<CardFrontend>();
    } else if (type_ == FrontendType::DECLARATIVE_JS) {
        if (isFormRender_) {
            LOGI("Init Form Frontend");
            frontend_ = AceType::MakeRefPtr<FormFrontendDeclarative>();
            auto cardFrontend = AceType::DynamicCast<FormFrontendDeclarative>(frontend_);
            auto& loader = Framework::JsEngineLoader::GetDeclarative(GetDeclarativeSharedLibrary());
            RefPtr<Framework::JsEngine> jsEngine;
            if (GetSettings().usingSharedRuntime) {
                jsEngine = loader.CreateJsEngineUsingSharedRuntime(instanceId_, sharedRuntime_);
            } else {
                jsEngine = loader.CreateJsEngine(instanceId_);
            }
            jsEngine->AddExtraNativeObject("ability", aceAbility.get());
            EngineHelper::AddEngine(instanceId_, jsEngine);
            cardFrontend->SetJsEngine(jsEngine);
            cardFrontend->SetPageProfile(pageProfile_);
            cardFrontend->SetNeedDebugBreakPoint(AceApplicationInfo::GetInstance().IsNeedDebugBreakPoint());
            cardFrontend->SetDebugVersion(AceApplicationInfo::GetInstance().IsDebugVersion());
            // Card front
            cardFrontend->SetRunningCardId(0); // ArkTsCard : nodeId, Host->FMS->FRS->innersdk
            cardFrontend->SetIsFormRender(true);
        } else if (!isSubContainer_) {
            frontend_ = AceType::MakeRefPtr<DeclarativeFrontend>();
            auto declarativeFrontend = AceType::DynamicCast<DeclarativeFrontend>(frontend_);
            auto& loader = Framework::JsEngineLoader::GetDeclarative(GetDeclarativeSharedLibrary());
            RefPtr<Framework::JsEngine> jsEngine;
            if (GetSettings().usingSharedRuntime) {
                jsEngine = loader.CreateJsEngineUsingSharedRuntime(instanceId_, sharedRuntime_);
            } else {
                jsEngine = loader.CreateJsEngine(instanceId_);
            }
            jsEngine->AddExtraNativeObject("ability", aceAbility.get());
            EngineHelper::AddEngine(instanceId_, jsEngine);
            declarativeFrontend->SetJsEngine(jsEngine);
            declarativeFrontend->SetPageProfile(pageProfile_);
            declarativeFrontend->SetNeedDebugBreakPoint(AceApplicationInfo::GetInstance().IsNeedDebugBreakPoint());
            declarativeFrontend->SetDebugVersion(AceApplicationInfo::GetInstance().IsDebugVersion());
        } else {
            frontend_ = OHOS::Ace::Platform::AceContainer::GetContainer(parentId_)->GetFrontend();
            return;
        }
    } else {
        LOGE("Frontend type not supported");
        EventReport::SendAppStartException(AppStartExcepType::FRONTEND_TYPE_ERR);
        return;
    }
    ACE_DCHECK(frontend_);
    auto abilityInfo = abilityInfo_.lock();
    std::shared_ptr<AppExecFwk::AbilityInfo> info = aceAbility ? aceAbility->GetAbilityInfo() : abilityInfo;
    if (info && info->isLauncherAbility) {
        frontend_->DisallowPopLastPage();
    }
    frontend_->Initialize(type_, taskExecutor_);
}

RefPtr<AceContainer> AceContainer::GetContainer(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_RETURN_NOLOG(container, nullptr);
    auto aceContainer = AceType::DynamicCast<AceContainer>(container);
    return aceContainer;
}

bool AceContainer::OnBackPressed(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_RETURN_NOLOG(container, false);
    // When the container is for overlay, it need close the overlay first.
    if (container->IsSubContainer()) {
        if (container->IsUseNewPipeline()) {
            LOGI("back press for remove overlay node");
            ContainerScope scope(instanceId);
            auto subPipelineContext = DynamicCast<NG::PipelineContext>(container->GetPipelineContext());
            CHECK_NULL_RETURN_NOLOG(subPipelineContext, false);
            auto overlayManager = subPipelineContext->GetOverlayManager();
            CHECK_NULL_RETURN_NOLOG(overlayManager, false);
            return overlayManager->RemoveOverlayInSubwindow();
        }
        SubwindowManager::GetInstance()->CloseMenu();
        return true;
    }
    ContainerScope scope(instanceId);
    auto context = container->GetPipelineContext();
    CHECK_NULL_RETURN_NOLOG(context, false);
    if (context->PopPageStackOverlay()) {
        return true;
    }
    return context->CallRouterBackToPopPage();
}

void AceContainer::OnShow(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_VOID(container);
    ContainerScope scope(instanceId);
    auto taskExecutor = container->GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    if (!container->UpdateState(Frontend::State::ON_SHOW)) {
        return;
    }

    auto jsTask = [container, front = container->GetFrontend()]() {
        if (front && !container->IsSubContainer()) {
            front->UpdateState(Frontend::State::ON_SHOW);
            front->OnShow();
        }
    };

    auto uiTask = [container]() {
        std::unordered_map<int64_t, WeakPtr<Frontend>> cardFrontendMap;
        container->GetCardFrontendMap(cardFrontendMap);
        for (const auto& [_, weakCardFront] : cardFrontendMap) {
            auto cardFront = weakCardFront.Upgrade();
            if (!cardFront) {
                LOGE("cardFront is null");
                continue;
            }
            cardFront->OnShow();
        }
        auto pipelineBase = container->GetPipelineContext();
        CHECK_NULL_VOID(pipelineBase);
        pipelineBase->OnShow();
        pipelineBase->SetForegroundCalled(true);
    };

    // stege model needn't post task when already run on UI
    if (container->GetSettings().useUIAsJSThread && taskExecutor->WillRunOnCurrentThread(TaskExecutor::TaskType::UI)) {
        jsTask();
        uiTask();
    } else {
        taskExecutor->PostTask(jsTask, TaskExecutor::TaskType::JS);
        taskExecutor->PostTask(uiTask, TaskExecutor::TaskType::UI);
    }
}

void AceContainer::OnHide(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_VOID(container);
    ContainerScope scope(instanceId);
    auto taskExecutor = container->GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    if (!container->UpdateState(Frontend::State::ON_HIDE)) {
        return;
    }
    std::unordered_map<int64_t, WeakPtr<Frontend>> cardFrontendMap;
    container->GetCardFrontendMap(cardFrontendMap);

    auto jsTask = [container, front = container->GetFrontend(), cardFrontendMap]() {
        if (front && !container->IsSubContainer()) {
            front->UpdateState(Frontend::State::ON_HIDE);
            front->OnHide();
            front->TriggerGarbageCollection();
        }
        for (const auto& [_, weakCardFront] : cardFrontendMap) {
            auto cardFront = weakCardFront.Upgrade();
            if (!cardFront) {
                LOGE("cardFront is null");
                continue;
            }
            cardFront->TriggerGarbageCollection();
        }
    };

    auto uiTask = [container, cardFrontendMap]() {
        for (const auto& [_, weakCardFront] : cardFrontendMap) {
            auto cardFront = weakCardFront.Upgrade();
            if (!cardFront) {
                LOGE("cardFront is null");
                continue;
            }
            cardFront->OnHide();
        }
        auto pipelineBase = container->GetPipelineContext();
        CHECK_NULL_VOID(pipelineBase);
        pipelineBase->OnHide();
    };

    // stege model needn't post task when already run on UI
    if (container->GetSettings().useUIAsJSThread && taskExecutor->WillRunOnCurrentThread(TaskExecutor::TaskType::UI)) {
        jsTask();
        uiTask();
    } else {
        taskExecutor->PostTask(jsTask, TaskExecutor::TaskType::JS);
        taskExecutor->PostTask(uiTask, TaskExecutor::TaskType::UI);
    }
}

void AceContainer::OnActive(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_VOID(container);
    ContainerScope scope(instanceId);
    auto taskExecutor = container->GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);

    auto front = container->GetFrontend();
    if (front && !container->IsSubContainer()) {
        WeakPtr<Frontend> weakFrontend = front;
        taskExecutor->PostTask(
            [weakFrontend]() {
                auto frontend = weakFrontend.Upgrade();
                if (frontend) {
                    frontend->UpdateState(Frontend::State::ON_ACTIVE);
                    frontend->OnActive();
                }
            },
            TaskExecutor::TaskType::JS);
    }

    taskExecutor->PostTask(
        [container]() {
            auto pipelineContext = container->GetPipelineContext();
            if (!pipelineContext) {
                LOGE("pipeline context is null, OnActive failed.");
                return;
            }
            pipelineContext->WindowFocus(true);
        },
        TaskExecutor::TaskType::UI);
}

void AceContainer::OnInactive(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_VOID(container);
    ContainerScope scope(instanceId);
    auto taskExecutor = container->GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);

    auto front = container->GetFrontend();
    if (front && !container->IsSubContainer()) {
        WeakPtr<Frontend> weakFrontend = front;
        taskExecutor->PostTask(
            [weakFrontend]() {
                auto frontend = weakFrontend.Upgrade();
                if (frontend) {
                    frontend->UpdateState(Frontend::State::ON_INACTIVE);
                    frontend->OnInactive();
                }
            },
            TaskExecutor::TaskType::JS);
    }

    taskExecutor->PostTask(
        [container]() {
            auto pipelineContext = container->GetPipelineContext();
            if (!pipelineContext) {
                LOGE("pipeline context is null, OnInactive failed.");
                return;
            }
            pipelineContext->WindowFocus(false);
        },
        TaskExecutor::TaskType::UI);
}

void AceContainer::OnNewWant(int32_t instanceId, const std::string& data)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_VOID(container);
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    CHECK_NULL_VOID(front);
    front->OnNewWant(data);
}

bool AceContainer::OnStartContinuation(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_RETURN(container, false);
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    CHECK_NULL_RETURN(front, false);
    return front->OnStartContinuation();
}

std::string AceContainer::OnSaveData(int32_t instanceId)
{
    std::string result = "false";
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_RETURN(container, result);
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    CHECK_NULL_RETURN(front, result);
    front->OnSaveData(result);
    return result;
}

bool AceContainer::OnRestoreData(int32_t instanceId, const std::string& data)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_RETURN(container, false);
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    CHECK_NULL_RETURN(front, false);
    return front->OnRestoreData(data);
}

void AceContainer::OnCompleteContinuation(int32_t instanceId, int result)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_VOID(container);
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    CHECK_NULL_VOID(front);
    front->OnCompleteContinuation(result);
}

void AceContainer::OnRemoteTerminated(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_VOID(container);
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    CHECK_NULL_VOID(front);
    front->OnRemoteTerminated();
}

void AceContainer::OnConfigurationUpdated(int32_t instanceId, const std::string& configuration)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_VOID(container);
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    CHECK_NULL_VOID(front);
    front->OnConfigurationUpdated(configuration);
}

void AceContainer::OnNewRequest(int32_t instanceId, const std::string& data)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_VOID_NOLOG(container);
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    CHECK_NULL_VOID_NOLOG(front);
    front->OnNewRequest(data);
}

void AceContainer::InitializeCallback()
{
    ACE_FUNCTION_TRACE();

    ACE_DCHECK(aceView_ && taskExecutor_ && pipelineContext_);
    auto&& touchEventCallback = [context = pipelineContext_, id = instanceId_](
                                    const TouchEvent& event, const std::function<void()>& markProcess) {
        ContainerScope scope(id);
        context->GetTaskExecutor()->PostTask(
            [context, event, markProcess]() {
                context->OnTouchEvent(event);
                CHECK_NULL_VOID_NOLOG(markProcess);
                markProcess();
            },
            TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterTouchEventCallback(touchEventCallback);

    auto&& mouseEventCallback = [context = pipelineContext_, id = instanceId_](
                                    const MouseEvent& event, const std::function<void()>& markProcess) {
        ContainerScope scope(id);
        context->GetTaskExecutor()->PostTask(
            [context, event, markProcess]() {
                context->OnMouseEvent(event);
                CHECK_NULL_VOID_NOLOG(markProcess);
                markProcess();
            },
            TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterMouseEventCallback(mouseEventCallback);

    auto&& axisEventCallback = [context = pipelineContext_, id = instanceId_](
                                   const AxisEvent& event, const std::function<void()>& markProcess) {
        ContainerScope scope(id);
        context->GetTaskExecutor()->PostTask(
            [context, event, markProcess]() {
                context->OnAxisEvent(event);
                CHECK_NULL_VOID_NOLOG(markProcess);
                markProcess();
            },
            TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterAxisEventCallback(axisEventCallback);

    auto&& keyEventCallback = [context = pipelineContext_, id = instanceId_](const KeyEvent& event) {
        ContainerScope scope(id);
        bool result = false;
        context->GetTaskExecutor()->PostSyncTask(
            [context, event, &result]() { result = context->OnKeyEvent(event); }, TaskExecutor::TaskType::UI);
        return result;
    };
    aceView_->RegisterKeyEventCallback(keyEventCallback);

    auto&& rotationEventCallback = [context = pipelineContext_, id = instanceId_](const RotationEvent& event) {
        ContainerScope scope(id);
        bool result = false;
        context->GetTaskExecutor()->PostSyncTask(
            [context, event, &result]() { result = context->OnRotationEvent(event); }, TaskExecutor::TaskType::UI);
        return result;
    };
    aceView_->RegisterRotationEventCallback(rotationEventCallback);

    auto&& viewChangeCallback = [context = pipelineContext_, id = instanceId_](int32_t width, int32_t height,
                                    WindowSizeChangeReason type,
                                    const std::shared_ptr<Rosen::RSTransaction>& rsTransaction) {
        ContainerScope scope(id);
        ACE_SCOPED_TRACE("ViewChangeCallback(%d, %d)", width, height);
        context->GetTaskExecutor()->PostTask(
            [context, width, height, type, rsTransaction, id]() {
                context->OnSurfaceChanged(width, height, type, rsTransaction);
                if (type == WindowSizeChangeReason::ROTATION) {
                    auto subwindow = SubwindowManager::GetInstance()->GetSubwindow(id);
                    CHECK_NULL_VOID_NOLOG(subwindow);
                    subwindow->ResizeWindow();
                }
            },
            TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterViewChangeCallback(viewChangeCallback);

    auto&& viewPositionChangeCallback = [context = pipelineContext_, id = instanceId_](int32_t posX, int32_t posY) {
        ContainerScope scope(id);
        ACE_SCOPED_TRACE("ViewPositionChangeCallback(%d, %d)", posX, posY);
        context->GetTaskExecutor()->PostTask(
            [context, posX, posY]() { context->OnSurfacePositionChanged(posX, posY); }, TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterViewPositionChangeCallback(viewPositionChangeCallback);

    auto&& densityChangeCallback = [context = pipelineContext_, id = instanceId_](double density) {
        ContainerScope scope(id);
        ACE_SCOPED_TRACE("DensityChangeCallback(%lf)", density);
        context->GetTaskExecutor()->PostTask(
            [context, density]() { context->OnSurfaceDensityChanged(density); }, TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterDensityChangeCallback(densityChangeCallback);

    auto&& systemBarHeightChangeCallback = [context = pipelineContext_, id = instanceId_](
                                               double statusBar, double navigationBar) {
        ContainerScope scope(id);
        ACE_SCOPED_TRACE("SystemBarHeightChangeCallback(%lf, %lf)", statusBar, navigationBar);
        context->GetTaskExecutor()->PostTask(
            [context, statusBar, navigationBar]() { context->OnSystemBarHeightChanged(statusBar, navigationBar); },
            TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterSystemBarHeightChangeCallback(systemBarHeightChangeCallback);

    auto&& surfaceDestroyCallback = [context = pipelineContext_, id = instanceId_]() {
        ContainerScope scope(id);
        context->GetTaskExecutor()->PostTask(
            [context]() { context->OnSurfaceDestroyed(); }, TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterSurfaceDestroyCallback(surfaceDestroyCallback);

    auto&& dragEventCallback = [context = pipelineContext_, id = instanceId_](
                                   int32_t x, int32_t y, const DragEventAction& action) {
        ContainerScope scope(id);
        context->GetTaskExecutor()->PostTask(
            [context, x, y, action]() { context->OnDragEvent(x, y, action); }, TaskExecutor::TaskType::UI);
    };
    aceView_->RegisterDragEventCallback(dragEventCallback);
}

void AceContainer::CreateContainer(int32_t instanceId, FrontendType type, const std::string& instanceName,
    std::shared_ptr<OHOS::AppExecFwk::Ability> aceAbility, std::unique_ptr<PlatformEventCallback> callback,
    bool useCurrentEventRunner, bool useNewPipeline)
{
    auto aceContainer = AceType::MakeRefPtr<AceContainer>(
        instanceId, type, aceAbility, std::move(callback), useCurrentEventRunner, useNewPipeline);
    AceEngine::Get().AddContainer(instanceId, aceContainer);
    ConnectServerManager::Get().SetDebugMode();
    HdcRegister::Get().StartHdcRegister(instanceId);
    aceContainer->Initialize();
    ContainerScope scope(instanceId);
    auto front = aceContainer->GetFrontend();
    if (front) {
        front->UpdateState(Frontend::State::ON_CREATE);
        front->SetJsMessageDispatcher(aceContainer);
    }

    auto jsFront = AceType::DynamicCast<JsFrontend>(front);
    CHECK_NULL_VOID_NOLOG(jsFront);
    jsFront->SetInstanceName(instanceName);
}

void AceContainer::DestroyContainer(int32_t instanceId, const std::function<void()>& destroyCallback)
{
    SubwindowManager::GetInstance()->CloseDialog(instanceId);
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_VOID(container);
    HdcRegister::Get().StopHdcRegister(instanceId);
    container->Destroy();
    // unregister watchdog before stop thread to avoid UI_BLOCK report
    AceEngine::Get().UnRegisterFromWatchDog(instanceId);
    auto taskExecutor = container->GetTaskExecutor();
    if (taskExecutor) {
        taskExecutor->PostSyncTask([] { LOGI("Wait UI thread..."); }, TaskExecutor::TaskType::UI);
        taskExecutor->PostSyncTask([] { LOGI("Wait JS thread..."); }, TaskExecutor::TaskType::JS);
    }
    container->DestroyView(); // Stop all threads(ui,gpu,io) for current ability.
    auto removeContainerTask = [instanceId, destroyCallback] {
        LOGI("Remove on Platform thread...");
        EngineHelper::RemoveEngine(instanceId);
        AceEngine::Get().RemoveContainer(instanceId);
        ConnectServerManager::Get().RemoveInstance(instanceId);
        CHECK_NULL_VOID_NOLOG(destroyCallback);
        destroyCallback();
    };
    if (container->GetSettings().usePlatformAsUIThread) {
        removeContainerTask();
    } else {
        taskExecutor->PostTask(removeContainerTask, TaskExecutor::TaskType::PLATFORM);
    }
}

void AceContainer::SetView(AceView* view, double density, int32_t width, int32_t height,
    sptr<OHOS::Rosen::Window> rsWindow, UIEnvCallback callback)
{
    CHECK_NULL_VOID(view);
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(view->GetInstanceId()));
    CHECK_NULL_VOID(container);
#ifdef ENABLE_ROSEN_BACKEND
    auto taskExecutor = container->GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);

    auto window = std::make_shared<NG::RosenWindow>(rsWindow, taskExecutor, view->GetInstanceId());
#else
    auto platformWindow = PlatformWindow::Create(view);
    CHECK_NULL_VOID(platformWindow);
    auto window = std::make_shared<Window>(std::move(platformWindow));
#endif
    AceContainer::SetUIWindow(view->GetInstanceId(), rsWindow);
    container->AttachView(window, view, density, width, height, rsWindow->GetWindowId(), callback);
}

void AceContainer::SetViewNew(
    AceView* view, double density, int32_t width, int32_t height, sptr<OHOS::Rosen::Window> rsWindow)
{
#ifdef ENABLE_ROSEN_BACKEND
    CHECK_NULL_VOID(view);
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(view->GetInstanceId()));
    CHECK_NULL_VOID(container);
    auto taskExecutor = container->GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    AceContainer::SetUIWindow(view->GetInstanceId(), rsWindow);

    if (container->isFormRender_) {
        auto window = std::make_shared<FormRenderWindow>(taskExecutor, view->GetInstanceId());
        container->AttachView(window, view, density, width, height, view->GetInstanceId(), nullptr);
    } else {
        auto window = std::make_shared<NG::RosenWindow>(rsWindow, taskExecutor, view->GetInstanceId());
        container->AttachView(window, view, density, width, height, rsWindow->GetWindowId(), nullptr);
    }
#endif
}

void AceContainer::SetView(
    AceView* view, double density, int32_t width, int32_t height, const std::shared_ptr<Window>& window)
{
#ifdef ENABLE_ROSEN_BACKEND
    CHECK_NULL_VOID(view);
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(view->GetInstanceId()));
    CHECK_NULL_VOID(container);
    auto taskExecutor = container->GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    CHECK_NULL_VOID(window);
    window->SetTaskExecutor(taskExecutor);
    window->SetInstanceId(view->GetInstanceId());
    window->Init();

    container->AttachView(window, view, density, width, height, window->GetWindowId(), nullptr);
#endif
}

void AceContainer::SetUIWindow(int32_t instanceId, sptr<OHOS::Rosen::Window> uiWindow)
{
    CHECK_NULL_VOID_NOLOG(uiWindow);
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(instanceId));
    CHECK_NULL_VOID_NOLOG(container);
    container->SetUIWindowInner(uiWindow);
}

sptr<OHOS::Rosen::Window> AceContainer::GetUIWindow(int32_t instanceId)
{
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(instanceId));
    CHECK_NULL_RETURN_NOLOG(container, nullptr);
    return container->GetUIWindowInner();
}

OHOS::AppExecFwk::Ability* AceContainer::GetAbility(int32_t instanceId)
{
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(instanceId));
    CHECK_NULL_RETURN_NOLOG(container, nullptr);
    return container->GetAbilityInner().lock().get();
}

bool AceContainer::RunPage(int32_t instanceId, int32_t pageId, const std::string& content, const std::string& params)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_RETURN_NOLOG(container, false);
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    CHECK_NULL_RETURN_NOLOG(front, false);
    LOGD("RunPage content=[%{private}s]", content.c_str());
    front->RunPage(pageId, content, params);
    return true;
}

void AceContainer::ClearEngineCache(int32_t instanceId)
{
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(instanceId));
    CHECK_NULL_VOID(container);
    ContainerScope scope(instanceId);
    if (!container->IsFormRender()) {
        return;
    }
    auto formFrontend = AceType::DynamicCast<FormFrontendDeclarative>(container->GetFrontend());
    CHECK_NULL_VOID(formFrontend);
    formFrontend->ClearEngineCache();
}

bool AceContainer::PushPage(int32_t instanceId, const std::string& content, const std::string& params)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_RETURN_NOLOG(container, false);
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    CHECK_NULL_RETURN_NOLOG(front, false);
    front->PushPage(content, params);
    return true;
}

bool AceContainer::UpdatePage(int32_t instanceId, int32_t pageId, const std::string& content)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_RETURN_NOLOG(container, false);
    ContainerScope scope(instanceId);
    auto context = container->GetPipelineContext();
    CHECK_NULL_RETURN_NOLOG(context, false);
    return context->CallRouterBackToPopPage();
}

void AceContainer::SetHapPath(const std::string& hapPath)
{
    if (hapPath.empty()) {
        LOGI("SetHapPath, Use .index to load resource");
        return;
    }
    LOGI("SetHapPath, Use hap path to load resource");
    webHapPath_ = hapPath;
    resourceInfo_.SetHapPath(hapPath);
    SystemProperties::SetUnZipHap(false);
}

void AceContainer::Dispatch(
    const std::string& group, std::vector<uint8_t>&& data, int32_t id, bool replyToComponent) const
{
    return;
}

void AceContainer::DispatchPluginError(int32_t callbackId, int32_t errorCode, std::string&& errorMessage) const
{
    auto front = GetFrontend();
    CHECK_NULL_VOID(front);
    ContainerScope scope(instanceId_);
    taskExecutor_->PostTask(
        [front, callbackId, errorCode, errorMessage = std::move(errorMessage)]() mutable {
            front->TransferJsPluginGetError(callbackId, errorCode, std::move(errorMessage));
        },
        TaskExecutor::TaskType::BACKGROUND);
}

bool AceContainer::Dump(const std::vector<std::string>& params, std::vector<std::string>& info)
{
    if (isDumping_.test_and_set()) {
        LOGI("another dump is still running");
        return false;
    }
    ContainerScope scope(instanceId_);
    auto result = false;
    if (!SystemProperties::GetDebugEnabled()) {
        std::unique_ptr<std::ostream> ss = std::make_unique<std::ostringstream>();
        CHECK_NULL_RETURN(ss, false);
        DumpLog::GetInstance().SetDumpFile(std::move(ss));
        result = DumpInfo(params);
        const auto& infoFile = DumpLog::GetInstance().GetDumpFile();
        auto* ostringstream = static_cast<std::ostringstream*>(infoFile.get());
        info.emplace_back(ostringstream->str().substr(0, DumpLog::MAX_DUMP_LENGTH));
        DumpLog::GetInstance().Reset();
    } else {
        auto dumpFilePath = AceApplicationInfo::GetInstance().GetDataFileDirPath() + "/arkui.dump";
        std::unique_ptr<std::ostream> ss = std::make_unique<std::ofstream>(dumpFilePath);
        CHECK_NULL_RETURN(ss, false);
        DumpLog::GetInstance().SetDumpFile(std::move(ss));
        result = DumpInfo(params);
        info.emplace_back("dumpFilePath: " + dumpFilePath);
        DumpLog::GetInstance().Reset();
    }
    if (!result) {
        DumpLog::ShowDumpHelp(info);
    }
    isDumping_.clear();
    return true;
}

bool AceContainer::DumpInfo(const std::vector<std::string>& params)
{
    if (aceView_ && aceView_->Dump(params)) {
        return true;
    }

    if (OnDumpInfo(params)) {
        return true;
    }
    CHECK_NULL_RETURN_NOLOG(pipelineContext_, false);
    return pipelineContext_->Dump(params);
}

bool AceContainer::OnDumpInfo(const std::vector<std::string>& params)
{
    if (!params.empty() && params[0] == "-basicinfo") {
        DumpLog::GetInstance().Print("BasicInfo: ");
        DumpLog::GetInstance().Print(1, "InstanceId: " + std::to_string(instanceId_));
        DumpLog::GetInstance().Print(1,
            "FrontendType: " + std::to_string(static_cast<typename std::underlying_type<FrontendType>::type>(type_)));
        DumpLog::GetInstance().Print(1, "NewPipeline: " + std::string(IsUseNewPipeline() ? "true" : "false"));
        DumpLog::GetInstance().Print(1, "WindowName: " + windowName_);
        DumpLog::GetInstance().Print(
            1, "WindowState: " +
                   (!frontend_ ? "frontend is null"
                               : std::to_string(static_cast<typename std::underlying_type<Frontend::State>::type>(
                                     frontend_->GetState()))));
        DumpLog::GetInstance().Print(1, "Language: " + AceApplicationInfo::GetInstance().GetLocaleTag());
        DumpLog::GetInstance().Print(
            1, "RTL: " + std::string(AceApplicationInfo::GetInstance().IsRightToLeft() ? "true" : "false"));
        DumpLog::GetInstance().Print(
            1, "ColorMode: " + std::string(SystemProperties::GetColorMode() == ColorMode::DARK ? "Dark" : "Light"));
        DumpLog::GetInstance().Print(1,
            "DeviceOrientation: " + std::string(SystemProperties::GetDeviceOrientation() == DeviceOrientation::LANDSCAPE
                                                    ? "Landscape"
                                                    : "Portrait"));
        DumpLog::GetInstance().Print(1, "Resolution: " + std::to_string(SystemProperties::GetDeviceWidth()) + "*" +
                                            std::to_string(SystemProperties::GetDeviceHeight()));
        if (pipelineContext_) {
            DumpLog::GetInstance().Print(1, "AppBgColor: " + pipelineContext_->GetAppBgColor().ColorToString());
            DumpLog::GetInstance().Print(1, "Density: " + std::to_string(pipelineContext_->GetDensity()));
            DumpLog::GetInstance().Print(1, "ViewScale: " + std::to_string(pipelineContext_->GetViewScale()));
            DumpLog::GetInstance().Print(
                1, "DisplayWindowRect: " + pipelineContext_->GetDisplayWindowRectInfo().ToString());
        }
        DumpLog::GetInstance().Print(1, "ApiVersion: " + SystemProperties::GetApiVersion());
        DumpLog::GetInstance().Print(1, "ReleaseType: " + SystemProperties::GetReleaseType());
        DumpLog::GetInstance().Print(1, "DeviceType: " + SystemProperties::GetParamDeviceType());
        return true;
    }
    return false;
}

void AceContainer::TriggerGarbageCollection()
{
    ContainerScope scope(instanceId_);
#if !defined(OHOS_PLATFORM) || !defined(ENABLE_NATIVE_VIEW)
    // GPU and IO thread is standalone while disable native view
    taskExecutor_->PostTask([] { PurgeMallocCache(); }, TaskExecutor::TaskType::GPU);
    taskExecutor_->PostTask([] { PurgeMallocCache(); }, TaskExecutor::TaskType::IO);
#endif
    taskExecutor_->PostTask([] { PurgeMallocCache(); }, TaskExecutor::TaskType::UI);
    taskExecutor_->PostTask(
        [frontend = WeakPtr<Frontend>(frontend_)] {
            auto sp = frontend.Upgrade();
            if (sp) {
                sp->TriggerGarbageCollection();
            }
            PurgeMallocCache();
        },
        TaskExecutor::TaskType::JS);
}

void AceContainer::DumpHeapSnapshot(bool isPrivate)
{
    taskExecutor_->PostTask(
        [isPrivate, frontend = WeakPtr<Frontend>(frontend_)] {
            auto sp = frontend.Upgrade();
            CHECK_NULL_VOID_NOLOG(sp);
            sp->DumpHeapSnapshot(isPrivate);
        },
        TaskExecutor::TaskType::JS);
}

void AceContainer::SetLocalStorage(NativeReference* storage, NativeReference* context)
{
    ContainerScope scope(instanceId_);
    taskExecutor_->PostTask(
        [frontend = WeakPtr<Frontend>(frontend_), storage, context, id = instanceId_] {
            auto sp = frontend.Upgrade();
            CHECK_NULL_VOID_NOLOG(sp);
            auto declarativeFrontend = AceType::DynamicCast<DeclarativeFrontend>(sp);
            auto jsEngine = declarativeFrontend->GetJsEngine();
            if (context) {
                jsEngine->SetContext(id, context);
            }
            if (storage) {
                jsEngine->SetLocalStorage(id, storage);
            }
        },
        TaskExecutor::TaskType::JS);
}

void AceContainer::AddAssetPath(int32_t instanceId, const std::string& packagePath, const std::string& hapPath,
    const std::vector<std::string>& paths)
{
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(instanceId));
    CHECK_NULL_VOID_NOLOG(container);
    RefPtr<FlutterAssetManager> flutterAssetManager;
    if (container->assetManager_) {
        flutterAssetManager = AceType::DynamicCast<FlutterAssetManager>(container->assetManager_);
    } else {
        flutterAssetManager = Referenced::MakeRefPtr<FlutterAssetManager>();
        container->assetManager_ = flutterAssetManager;
        if (container->type_ != FrontendType::DECLARATIVE_JS) {
            container->frontend_->SetAssetManager(flutterAssetManager);
        }
    }
    CHECK_NULL_VOID_NOLOG(flutterAssetManager);
    if (!hapPath.empty()) {
        auto assetProvider = AceType::MakeRefPtr<HapAssetProvider>();
        if (assetProvider->Initialize(hapPath, paths)) {
            LOGI("Push AssetProvider to queue.");
            flutterAssetManager->PushBack(std::move(assetProvider));
        }
    }
    if (!packagePath.empty()) {
        auto assetProvider = AceType::MakeRefPtr<FileAssetProvider>();
        if (assetProvider->Initialize(packagePath, paths)) {
            LOGI("Push AssetProvider to queue.");
            flutterAssetManager->PushBack(std::move(assetProvider));
        }
    }
}

void AceContainer::AddLibPath(int32_t instanceId, const std::vector<std::string>& libPath)
{
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(instanceId));
    CHECK_NULL_VOID_NOLOG(container);
    RefPtr<FlutterAssetManager> flutterAssetManager;
    if (container->assetManager_) {
        flutterAssetManager = AceType::DynamicCast<FlutterAssetManager>(container->assetManager_);
    } else {
        flutterAssetManager = Referenced::MakeRefPtr<FlutterAssetManager>();
        container->assetManager_ = flutterAssetManager;
        if (container->type_ != FrontendType::DECLARATIVE_JS) {
            container->frontend_->SetAssetManager(flutterAssetManager);
        }
    }
    CHECK_NULL_VOID_NOLOG(flutterAssetManager);
    flutterAssetManager->SetLibPath("default", libPath);
}

void AceContainer::AttachView(std::shared_ptr<Window> window, AceView* view, double density, int32_t width,
    int32_t height, int32_t windowId, UIEnvCallback callback)
{
    aceView_ = view;
    auto instanceId = aceView_->GetInstanceId();
    auto flutterTaskExecutor = AceType::DynamicCast<FlutterTaskExecutor>(taskExecutor_);
    if (!isSubContainer_) {
        auto* aceView = static_cast<AceViewOhos*>(aceView_);
        ACE_DCHECK(aceView != nullptr);
        flutterTaskExecutor->InitOtherThreads(aceView->GetThreadModel());
    }
    ContainerScope scope(instanceId);
    if (type_ == FrontendType::DECLARATIVE_JS) {
        // For DECLARATIVE_JS frontend display UI in JS thread temporarily.
        flutterTaskExecutor->InitJsThread(false);
        InitializeFrontend();
        auto front = GetFrontend();
        if (front) {
            front->UpdateState(Frontend::State::ON_CREATE);
            front->SetJsMessageDispatcher(AceType::Claim(this));
            front->SetAssetManager(assetManager_);
        }
    } else if (type_ != FrontendType::JS_CARD) {
        aceView_->SetCreateTime(createTime_);
    }
    resRegister_ = aceView_->GetPlatformResRegister();
    if (useNewPipeline_) {
        LOGI("New pipeline version creating...");
        pipelineContext_ = AceType::MakeRefPtr<NG::PipelineContext>(
            window, taskExecutor_, assetManager_, resRegister_, frontend_, instanceId);
        pipelineContext_->SetTextFieldManager(AceType::MakeRefPtr<NG::TextFieldManagerNG>());
    } else {
        pipelineContext_ = AceType::MakeRefPtr<PipelineContext>(
            window, taskExecutor_, assetManager_, resRegister_, frontend_, instanceId);
        pipelineContext_->SetTextFieldManager(AceType::MakeRefPtr<TextFieldManager>());
    }

    if (isFormRender_) {
        pipelineContext_->SetIsFormRender(isFormRender_);
        auto cardFrontend = AceType::DynamicCast<FormFrontendDeclarative>(frontend_);
        if (cardFrontend) {
            cardFrontend->SetTaskExecutor(taskExecutor_);
            cardFrontend->SetLoadCardCallBack(WeakPtr<PipelineBase>(pipelineContext_));
        }
    }

    pipelineContext_->SetRootSize(density, width, height);
    if (isFormRender_) {
        pipelineContext_->OnSurfaceDensityChanged(density);
    }
    pipelineContext_->SetIsRightToLeft(AceApplicationInfo::GetInstance().IsRightToLeft());
    pipelineContext_->SetWindowId(windowId);
    pipelineContext_->SetWindowModal(windowModal_);
    if (uiWindow_) {
        auto windowType = uiWindow_->GetType();
        pipelineContext_->SetIsAppWindow(
            windowType < Rosen::WindowType::SYSTEM_WINDOW_BASE && windowType >= Rosen::WindowType::APP_WINDOW_BASE);
    }
    if (installationFree_) {
        pipelineContext_->SetInstallationFree(installationFree_);
        pipelineContext_->SetSharePanelCallback(std::move(sharePanelCallback_));
        std::shared_ptr<AppExecFwk::AbilityInfo> info = abilityInfo_.lock();
        if (info != nullptr) {
            pipelineContext_->SetAppLabelId(info->labelId);
        }
    }
    if (isSubContainer_) {
        pipelineContext_->SetIsSubPipeline(true);
    }

    pipelineContext_->SetDrawDelegate(aceView_->GetDrawDelegate());
    InitWindowCallback();
    InitializeCallback();

    auto&& finishEventHandler = [weak = WeakClaim(this), instanceId] {
        auto container = weak.Upgrade();
        CHECK_NULL_VOID(container);
        ContainerScope scope(instanceId);
        auto context = container->GetPipelineContext();
        CHECK_NULL_VOID(context);
        context->GetTaskExecutor()->PostTask(
            [weak = WeakPtr<AceContainer>(container)] {
                auto container = weak.Upgrade();
                CHECK_NULL_VOID(container);
                container->OnFinish();
            },
            TaskExecutor::TaskType::PLATFORM);
    };
    pipelineContext_->SetFinishEventHandler(finishEventHandler);

    auto&& startAbilityHandler = [weak = WeakClaim(this), instanceId](const std::string& address) {
        auto container = weak.Upgrade();
        CHECK_NULL_VOID(container);
        ContainerScope scope(instanceId);
        auto context = container->GetPipelineContext();
        CHECK_NULL_VOID(context);
        context->GetTaskExecutor()->PostTask(
            [weak = WeakPtr<AceContainer>(container), address]() {
                auto container = weak.Upgrade();
                CHECK_NULL_VOID(container);
                container->OnStartAbility(address);
            },
            TaskExecutor::TaskType::PLATFORM);
    };
    pipelineContext_->SetStartAbilityHandler(startAbilityHandler);

    auto&& setStatusBarEventHandler = [weak = WeakClaim(this), instanceId](const Color& color) {
        auto container = weak.Upgrade();
        CHECK_NULL_VOID(container);
        ContainerScope scope(instanceId);
        auto context = container->GetPipelineContext();
        CHECK_NULL_VOID(context);
        context->GetTaskExecutor()->PostTask(
            [weak, color = color.GetValue()]() {
                auto container = weak.Upgrade();
                CHECK_NULL_VOID(container);
                if (container->platformEventCallback_) {
                    container->platformEventCallback_->OnStatusBarBgColorChanged(color);
                }
            },
            TaskExecutor::TaskType::PLATFORM);
    };
    pipelineContext_->SetStatusBarEventHandler(setStatusBarEventHandler);
    if (GetSettings().usePlatformAsUIThread) {
        FrameReport::GetInstance().Init();
    } else {
        taskExecutor_->PostTask([] { FrameReport::GetInstance().Init(); }, TaskExecutor::TaskType::UI);
    }

    // Load custom style at UI thread before frontend attach, for loading style before building tree.
    auto initThemeManagerTask = [pipelineContext = pipelineContext_, assetManager = assetManager_,
                                    colorScheme = colorScheme_, resourceInfo = resourceInfo_]() {
        ACE_SCOPED_TRACE("OHOS::LoadThemes()");
        LOGD("UIContent load theme");
        ThemeConstants::InitDeviceType();
        auto themeManager = AceType::MakeRefPtr<ThemeManagerImpl>();
        pipelineContext->SetThemeManager(themeManager);
        themeManager->InitResource(resourceInfo);
        themeManager->SetColorScheme(colorScheme);
        themeManager->LoadCustomTheme(assetManager);
        themeManager->LoadResourceThemes();
    };

    auto setupRootElementTask = [context = pipelineContext_, callback, isSubContainer = isSubContainer_]() {
        if (callback != nullptr) {
            callback(AceType::DynamicCast<PipelineContext>(context));
        }
        if (!isSubContainer) {
            context->SetupRootElement();
        }
    };
    if (GetSettings().usePlatformAsUIThread) {
        initThemeManagerTask();
        setupRootElementTask();
    } else {
        taskExecutor_->PostTask(initThemeManagerTask, TaskExecutor::TaskType::UI);
        taskExecutor_->PostTask(setupRootElementTask, TaskExecutor::TaskType::UI);
    }

    aceView_->Launch();

    if (!isSubContainer_) {
        // Only MainWindow instance in FA model will be registered to watch dog.
        if (!GetSettings().usingSharedRuntime && !AceApplicationInfo::GetInstance().IsNeedDebugBreakPoint()) {
            AceEngine::Get().RegisterToWatchDog(instanceId, taskExecutor_, GetSettings().useUIAsJSThread);
        }
        frontend_->AttachPipelineContext(pipelineContext_);
    } else {
        auto declarativeFrontend = AceType::DynamicCast<DeclarativeFrontend>(frontend_);
        if (declarativeFrontend) {
            declarativeFrontend->AttachSubPipelineContext(pipelineContext_);
        }
        return;
    }

    auto dataAbilityHelperImpl = [ability = GetAbilityInner(), runtimeContext = runtimeContext_,
                                     useStageModel = useStageModel_]() {
        return AceType::MakeRefPtr<DataAbilityHelperStandard>(ability.lock(), runtimeContext.lock(), useStageModel);
    };
    auto dataProviderManager = MakeRefPtr<DataProviderManagerStandard>(dataAbilityHelperImpl);
    pipelineContext_->SetDataProviderManager(dataProviderManager);

#if defined(ENABLE_ROSEN_BACKEND) and !defined(UPLOAD_GPU_DISABLED)
    pipelineContext_->SetPostRTTaskCallBack([](std::function<void()>&& task) {
        auto syncTask = std::make_shared<AceRosenSyncTask>(std::move(task));
        Rosen::RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(syncTask);
    });
#endif
}

void AceContainer::SetUIWindowInner(sptr<OHOS::Rosen::Window> uiWindow)
{
    uiWindow_ = uiWindow;
}

sptr<OHOS::Rosen::Window> AceContainer::GetUIWindowInner() const
{
    return uiWindow_;
}

std::weak_ptr<OHOS::AppExecFwk::Ability> AceContainer::GetAbilityInner() const
{
    return aceAbility_;
}

void AceContainer::SetFontScale(int32_t instanceId, float fontScale)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_VOID_NOLOG(container);
    ContainerScope scope(instanceId);
    auto pipelineContext = container->GetPipelineContext();
    CHECK_NULL_VOID(pipelineContext);
    pipelineContext->SetFontScale(fontScale);
}

void AceContainer::SetWindowStyle(int32_t instanceId, WindowModal windowModal, ColorScheme colorScheme)
{
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(instanceId));
    CHECK_NULL_VOID_NOLOG(container);
    ContainerScope scope(instanceId);
    container->SetWindowModal(windowModal);
    container->SetColorScheme(colorScheme);
}

void AceContainer::SetDialogCallback(int32_t instanceId, FrontendDialogCallback callback)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_VOID_NOLOG(container);
    auto front = container->GetFrontend();
    if (front && front->GetType() == FrontendType::JS) {
        front->SetDialogCallback(callback);
    }
}

std::string AceContainer::RestoreRouterStack(int32_t instanceId, const std::string& contentInfo)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_RETURN_NOLOG(container, "");
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    CHECK_NULL_RETURN_NOLOG(front, "");
    return front->RestoreRouterStack(contentInfo);
}

std::string AceContainer::GetContentInfo(int32_t instanceId)
{
    auto container = AceEngine::Get().GetContainer(instanceId);
    CHECK_NULL_RETURN_NOLOG(container, "");
    ContainerScope scope(instanceId);
    auto front = container->GetFrontend();
    CHECK_NULL_RETURN_NOLOG(front, "");
    return front->GetContentInfo();
}

void AceContainer::SetWindowPos(int32_t left, int32_t top)
{
    CHECK_NULL_VOID_NOLOG(frontend_);
    auto accessibilityManager = frontend_->GetAccessibilityManager();
    CHECK_NULL_VOID_NOLOG(accessibilityManager);
    accessibilityManager->SetWindowPos(left, top, windowId_);
}

void AceContainer::InitializeSubContainer(int32_t parentContainerId)
{
    auto parentContainer = AceEngine::Get().GetContainer(parentContainerId);
    CHECK_NULL_VOID(parentContainer);
    auto taskExec = parentContainer->GetTaskExecutor();
    taskExecutor_ = AceType::DynamicCast<FlutterTaskExecutor>(std::move(taskExec));
    auto parentSettings = parentContainer->GetSettings();
    GetSettings().useUIAsJSThread = parentSettings.useUIAsJSThread;
    GetSettings().usePlatformAsUIThread = parentSettings.usePlatformAsUIThread;
    GetSettings().usingSharedRuntime = parentSettings.usingSharedRuntime;
}

void AceContainer::InitWindowCallback()
{
    LOGD("AceContainer InitWindowCallback");
    if (!pipelineContext_ || !uiWindow_) {
        return;
    }
    auto& windowManager = pipelineContext_->GetWindowManager();
    std::shared_ptr<AppExecFwk::AbilityInfo> info = abilityInfo_.lock();
    if (info != nullptr) {
        windowManager->SetAppLabelId(info->labelId);
        windowManager->SetAppIconId(info->iconId);
    }
    windowManager->SetWindowMinimizeCallBack([window = uiWindow_]() { window->Minimize(); });
    windowManager->SetWindowMaximizeCallBack([window = uiWindow_]() { window->Maximize(); });
    windowManager->SetWindowRecoverCallBack([window = uiWindow_]() { window->Recover(); });
    windowManager->SetWindowCloseCallBack([window = uiWindow_]() { window->Close(); });
    windowManager->SetWindowStartMoveCallBack([window = uiWindow_]() { window->StartMove(); });
    windowManager->SetWindowSplitCallBack(
        [window = uiWindow_]() { window->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_SPLIT_PRIMARY); });
    windowManager->SetWindowGetModeCallBack(
        [window = uiWindow_]() -> WindowMode { return static_cast<WindowMode>(window->GetMode()); });

    pipelineContext_->SetGetWindowRectImpl([window = uiWindow_]() -> Rect {
        Rect rect;
        CHECK_NULL_RETURN_NOLOG(window, rect);
        auto windowRect = window->GetRect();
        rect.SetRect(windowRect.posX_, windowRect.posY_, windowRect.width_, windowRect.height_);
        return rect;
    });

    pipelineContext_->SetGetViewSafeAreaImpl([window = uiWindow_, this]() -> SafeAreaEdgeInserts {
        return SetViewSafeArea(window);
    });
}

// Get SafeArea by Window
SafeAreaEdgeInserts AceContainer::SetViewSafeArea(sptr<OHOS::Rosen::Window> window)
{
    SafeAreaEdgeInserts viewSafeArea;
    CHECK_NULL_RETURN_NOLOG(window, viewSafeArea);

    Rosen::AvoidArea systemAvoidArea;
    Rosen::AvoidArea cutoutAvoidArea;
    Rosen::WMError systemRet = window->GetAvoidAreaByType(Rosen::AvoidAreaType::TYPE_SYSTEM, systemAvoidArea);
    Rosen::WMError cutoutRet = window->GetAvoidAreaByType(Rosen::AvoidAreaType::TYPE_CUTOUT, cutoutAvoidArea);
    Rect topRect;
    Rect leftRect;
    Rect rightRect;
    Rect bottomRect;
    if (systemRet == Rosen::WMError::WM_OK) {
        topRect = Rect(static_cast<double>(systemAvoidArea.topRect_.posX_),
            static_cast<double>(systemAvoidArea.topRect_.posY_), static_cast<double>(systemAvoidArea.topRect_.width_),
            static_cast<double>(systemAvoidArea.topRect_.height_));
        leftRect = Rect(static_cast<double>(systemAvoidArea.leftRect_.posX_),
            static_cast<double>(systemAvoidArea.leftRect_.posY_), static_cast<double>(systemAvoidArea.leftRect_.width_),
            static_cast<double>(systemAvoidArea.leftRect_.height_));
        rightRect = Rect(static_cast<double>(systemAvoidArea.rightRect_.posX_),
            static_cast<double>(systemAvoidArea.rightRect_.posY_),
            static_cast<double>(systemAvoidArea.rightRect_.width_),
            static_cast<double>(systemAvoidArea.rightRect_.height_));
        bottomRect = Rect(static_cast<double>(systemAvoidArea.bottomRect_.posX_),
            static_cast<double>(systemAvoidArea.bottomRect_.posY_),
            static_cast<double>(systemAvoidArea.bottomRect_.width_),
            static_cast<double>(systemAvoidArea.bottomRect_.height_));
    }
    if (cutoutRet == Rosen::WMError::WM_OK) {
        topRect.CombineRect(Rect(static_cast<double>(cutoutAvoidArea.topRect_.posX_),
            static_cast<double>(cutoutAvoidArea.topRect_.posY_), static_cast<double>(cutoutAvoidArea.topRect_.width_),
            static_cast<double>(cutoutAvoidArea.topRect_.height_)));
        leftRect.CombineRect(Rect(static_cast<double>(cutoutAvoidArea.leftRect_.posX_),
            static_cast<double>(cutoutAvoidArea.leftRect_.posY_), static_cast<double>(cutoutAvoidArea.leftRect_.width_),
            static_cast<double>(cutoutAvoidArea.leftRect_.height_)));
        rightRect.CombineRect(Rect(static_cast<double>(cutoutAvoidArea.rightRect_.posX_),
            static_cast<double>(cutoutAvoidArea.rightRect_.posY_),
            static_cast<double>(cutoutAvoidArea.rightRect_.width_),
            static_cast<double>(cutoutAvoidArea.rightRect_.height_)));
        bottomRect.CombineRect(Rect(static_cast<double>(cutoutAvoidArea.bottomRect_.posX_),
            static_cast<double>(cutoutAvoidArea.bottomRect_.posY_),
            static_cast<double>(cutoutAvoidArea.bottomRect_.width_),
            static_cast<double>(cutoutAvoidArea.bottomRect_.height_)));
    }
    viewSafeArea.SetRect(leftRect, topRect, rightRect, bottomRect);
    return viewSafeArea;
}

std::shared_ptr<OHOS::AbilityRuntime::Context> AceContainer::GetAbilityContextByModule(
    const std::string& bundle, const std::string& module)
{
    auto context = runtimeContext_.lock();
    CHECK_NULL_RETURN(context, nullptr);
    return context->CreateModuleContext(bundle, module);
}

void AceContainer::UpdateConfiguration(
    const std::string& colorMode, const std::string& deviceAccess, const std::string& languageTag)
{
    if (colorMode.empty() && deviceAccess.empty() && languageTag.empty()) {
        LOGW("AceContainer::OnConfigurationUpdated param is empty");
        return;
    }
    CHECK_NULL_VOID(pipelineContext_);
    auto themeManager = pipelineContext_->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto resConfig = GetResourceConfiguration();
    if (!colorMode.empty()) {
        if (colorMode == "dark") {
            SystemProperties::SetColorMode(ColorMode::DARK);
            SetColorScheme(ColorScheme::SCHEME_DARK);
            resConfig.SetColorMode(ColorMode::DARK);
        } else {
            SystemProperties::SetColorMode(ColorMode::LIGHT);
            SetColorScheme(ColorScheme::SCHEME_LIGHT);
            resConfig.SetColorMode(ColorMode::LIGHT);
        }
    }
    if (!deviceAccess.empty()) {
        // Event of accessing mouse or keyboard
        SystemProperties::SetDeviceAccess(deviceAccess == "true");
        resConfig.SetDeviceAccess(deviceAccess == "true");
    }
    if (!languageTag.empty()) {
        std::string language;
        std::string script;
        std::string region;
        Localization::ParseLocaleTag(languageTag, language, script, region, false);
        if (!language.empty() || !script.empty() || !region.empty()) {
            AceApplicationInfo::GetInstance().SetLocale(language, region, script, "");
        }
    }
    SetResourceConfiguration(resConfig);
    themeManager->UpdateConfig(resConfig);
    themeManager->LoadResourceThemes();
    NotifyConfigurationChange(!deviceAccess.empty());
}

void AceContainer::NotifyConfigurationChange(bool needReloadTransition)
{
    auto taskExecutor = GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    taskExecutor->PostTask(
        [instanceId = instanceId_, weak = WeakClaim(this), needReloadTransition]() {
            ContainerScope scope(instanceId);
            auto container = weak.Upgrade();
            CHECK_NULL_VOID(container);
            auto frontend = container->GetFrontend();
            if (frontend) {
                LOGI("AceContainer::UpdateConfiguration frontend MarkNeedUpdate");
                frontend->FlushReload();
            }
            auto taskExecutor = container->GetTaskExecutor();
            CHECK_NULL_VOID(taskExecutor);
            taskExecutor->PostTask(
                [instanceId, weak, needReloadTransition]() {
                    ContainerScope scope(instanceId);
                    auto container = weak.Upgrade();
                    CHECK_NULL_VOID(container);
                    auto pipeline = container->GetPipelineContext();
                    CHECK_NULL_VOID(pipeline);
                    pipeline->NotifyConfigurationChange();
                    pipeline->FlushReload();
                    if (needReloadTransition) {
                        // reload transition animation
                        pipeline->FlushReloadTransition();
                    }
                },
                TaskExecutor::TaskType::UI);
        },
        TaskExecutor::TaskType::JS);
}

void AceContainer::HotReload()
{
    auto taskExecutor = GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    taskExecutor->PostTask(
        [instanceId = instanceId_, weak = WeakClaim(this)]() {
            ContainerScope scope(instanceId);
            auto container = weak.Upgrade();
            CHECK_NULL_VOID(container);
            auto frontend = container->GetFrontend();
            CHECK_NULL_VOID(frontend);
            LOGI("AceContainer::Flush Frontend for HotReload");
            frontend->HotReload();

            auto pipeline = container->GetPipelineContext();
            CHECK_NULL_VOID(pipeline);
            pipeline->FlushReload();
        },
        TaskExecutor::TaskType::UI);
}

void AceContainer::SetToken(sptr<IRemoteObject>& token)
{
    std::lock_guard<std::mutex> lock(cardTokensMutex_);
    if (token) {
        token_ = token;
    }
}

sptr<IRemoteObject> AceContainer::GetToken()
{
    std::lock_guard<std::mutex> lock(cardTokensMutex_);
    if (token_) {
        return token_;
    }
    LOGE("fail to get Token");
    return nullptr;
}

std::shared_ptr<AbilityRuntime::Context> AceContainer::GetAbilityRuntimeContext()
{
    return runtimeContext_.lock();
}

// ArkTsCard start
std::shared_ptr<Rosen::RSSurfaceNode> AceContainer::GetFormSurfaceNode(int32_t instanceId)
{
    auto container = AceType::DynamicCast<AceContainer>(AceEngine::Get().GetContainer(instanceId));
    CHECK_NULL_RETURN_NOLOG(container, nullptr);
    auto context = AceType::DynamicCast<NG::PipelineContext>(container->GetPipelineContext());
    CHECK_NULL_RETURN(context, nullptr);
    auto window = static_cast<FormRenderWindow*>(context->GetWindow());
    CHECK_NULL_RETURN(window, nullptr);
    return window->GetRSSurfaceNode();
}

void AceContainer::UpdateFormData(const std::string& data)
{
    auto frontend = AceType::DynamicCast<FormFrontendDeclarative>(frontend_);
    CHECK_NULL_VOID(frontend);
    frontend->UpdateData(data);
}

void AceContainer::UpdateFormSharedImage(const std::map<std::string, sptr<AppExecFwk::FormAshmem>>& imageDataMap)
{
    std::vector<std::string> picNameArray;
    std::vector<int> fileDescriptorArray;
    std::vector<int> byteLenArray;
    if (!imageDataMap.empty()) {
        for (auto& imageData : imageDataMap) {
            picNameArray.push_back(imageData.first);
            fileDescriptorArray.push_back(imageData.second->GetAshmemFd());
            byteLenArray.push_back(imageData.second->GetAshmemSize());
        }
        GetNamesOfSharedImage(picNameArray);
        UpdateSharedImage(picNameArray, byteLenArray, fileDescriptorArray);
    }
}

void AceContainer::GetNamesOfSharedImage(std::vector<std::string>& picNameArray)
{
    if (picNameArray.empty()) {
        LOGE("picNameArray is null!");
        return;
    }
    auto context = AceType::DynamicCast<NG::PipelineContext>(GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto sharedImageManager = context->GetOrCreateSharedImageManager();
    auto nameSize = picNameArray.size();
    for (uint32_t i = 0; i < nameSize; i++) {
        // get name of picture
        auto name = picNameArray[i];
        sharedImageManager->AddPictureNamesToReloadMap(std::move(name));
    }
}

void AceContainer::UpdateSharedImage(
    std::vector<std::string>& picNameArray, std::vector<int32_t>& byteLenArray, std::vector<int>& fileDescriptorArray)
{
    auto context = GetPipelineContext();
    CHECK_NULL_VOID(context);
    if (picNameArray.empty() || byteLenArray.empty() || fileDescriptorArray.empty()) {
        LOGE("array is null! when try UpdateSharedImage");
        return;
    }
    auto nameArraySize = picNameArray.size();
    if (nameArraySize != byteLenArray.size()) {
        LOGE("nameArraySize does not equal to fileDescriptorArraySize, please check!");
        return;
    }
    if (nameArraySize != fileDescriptorArray.size()) {
        LOGE("nameArraySize does not equal to fileDescriptorArraySize, please check!");
        return;
    }
    // now it can be assured that all three arrays are of the same size

    std::string picNameCopy;
    for (uint32_t i = 0; i < nameArraySize; i++) {
        // get name of picture
        auto picName = picNameArray[i];
        // save a copy of picName and ReleaseStringUTFChars immediately to avoid memory leak
        picNameCopy = picName;

        // get fd ID
        auto fd = fileDescriptorArray[i];

        auto newFd = dup(fd);
        if (newFd < 0) {
            LOGE("dup fd fail, fail reason: %{public}s, fd: %{public}d, picName: %{private}s, length: %{public}d",
                strerror(errno), fd, picNameCopy.c_str(), byteLenArray[i]);
            continue;
        }

        auto ashmem = Ashmem(newFd, byteLenArray[i]);
        GetImageDataFromAshmem(picNameCopy, ashmem, context, byteLenArray[i]);
        ashmem.UnmapAshmem();
        ashmem.CloseAshmem();
    }
}

void AceContainer::GetImageDataFromAshmem(
    const std::string& picName, Ashmem& ashmem, const RefPtr<PipelineBase>& pipelineContext, int len)
{
    bool ret = ashmem.MapReadOnlyAshmem();
    // if any exception causes a [return] before [AddSharedImage], the memory image will not show because [RenderImage]
    // will never be notified to start loading.
    if (!ret) {
        LOGE("MapReadOnlyAshmem fail, fail reason: %{public}s, picName: %{private}s, length: %{public}d, "
             "fd: %{public}d",
            strerror(errno), picName.c_str(), len, ashmem.GetAshmemFd());
        return;
    }
    const uint8_t* imageData = reinterpret_cast<const uint8_t*>(ashmem.ReadFromAshmem(len, 0));
    if (imageData == nullptr) {
        LOGE("imageData is nullptr, errno is: %{public}s, picName: %{private}s, length: %{public}d, fd: %{public}d",
            strerror(errno), picName.c_str(), len, ashmem.GetAshmemFd());
        return;
    }
    auto context = AceType::DynamicCast<NG::PipelineContext>(pipelineContext);
    CHECK_NULL_VOID(context);
    RefPtr<SharedImageManager> sharedImageManager = context->GetOrCreateSharedImageManager();
    if (sharedImageManager) {
        // read image data from shared memory and save a copy to sharedImageManager
        sharedImageManager->AddSharedImage(picName, std::vector<uint8_t>(imageData, imageData + len));
    }
}

// ArkTsCard end

extern "C" ACE_FORCE_EXPORT void OHOS_ACE_HotReloadPage()
{
    AceEngine::Get().NotifyContainers([](const RefPtr<Container>& container) {
        LOGI("starting hotReload");
        if (Container::IsCurrentUseNewPipeline()) {
            container->HotReload();
        } else {
            container->NotifyConfigurationChange(true);
        }
    });
}

} // namespace OHOS::Ace::Platform
