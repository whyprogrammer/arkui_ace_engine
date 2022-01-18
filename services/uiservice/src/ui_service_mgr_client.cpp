/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "ui_service_mgr_client.h"

#include "dialog_callback_stub.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "string_ex.h"
#include "system_ability_definition.h"

// external dependence
#include "ability_manager_client.h"
#include "display_manager.h"

namespace OHOS {
namespace Ace {
constexpr int UI_MGR_SERVICE_SA_ID = 7001;
constexpr int UI_DIALOG_PICKER_WIDTH = 519 * 2; // 519 vp
constexpr int UI_DIALOG_PICKER_HEIGHT = 256 * 2; // 256 vp
constexpr int UI_DEFAULT_WIDTH = 2560;
constexpr int UI_DEFAULT_HEIGHT = 1600;
std::shared_ptr<UIServiceMgrClient> UIServiceMgrClient::instance_ = nullptr;
std::mutex UIServiceMgrClient::mutex_;

std::shared_ptr<UIServiceMgrClient> UIServiceMgrClient::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> lock_l(mutex_);
        if (instance_ == nullptr) {
            instance_ = std::make_shared<UIServiceMgrClient>();
        }
    }
    return instance_;
}

UIServiceMgrClient::UIServiceMgrClient()
{}

UIServiceMgrClient::~UIServiceMgrClient()
{}

ErrCode UIServiceMgrClient::RegisterCallBack(const AAFwk::Want& want, const sptr<IUIService>& uiService)
{
    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
            return UI_SERVICE_NOT_CONNECTED;
        }
    }
    sptr<IUIServiceMgr> doms = iface_cast<IUIServiceMgr>(remoteObject_);
    return doms->RegisterCallBack(want, uiService);
}

ErrCode UIServiceMgrClient::UnregisterCallBack(const AAFwk::Want& want)
{
    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
            return UI_SERVICE_NOT_CONNECTED;
        }
    }
    sptr<IUIServiceMgr> doms = iface_cast<IUIServiceMgr>(remoteObject_);
    return doms->UnregisterCallBack(want);
}

ErrCode UIServiceMgrClient::Push(const AAFwk::Want& want, const std::string& name, const std::string& jsonPath,
    const std::string& data, const std::string& extraData)
{
    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
            return UI_SERVICE_NOT_CONNECTED;
        }
    }
    sptr<IUIServiceMgr> doms = iface_cast<IUIServiceMgr>(remoteObject_);
    return doms->Push(want, name, jsonPath, data, extraData);
}

ErrCode UIServiceMgrClient::Request(const AAFwk::Want& want, const std::string& name, const std::string& data)
{
    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
            return UI_SERVICE_NOT_CONNECTED;
        }
    }
    sptr<IUIServiceMgr> doms = iface_cast<IUIServiceMgr>(remoteObject_);
    return doms->Request(want, name, data);
}

ErrCode UIServiceMgrClient::ReturnRequest(const AAFwk::Want& want, const std::string& source,
    const std::string& data, const std::string& extraData)
{
    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
            return UI_SERVICE_NOT_CONNECTED;
        }
    }
    sptr<IUIServiceMgr> doms = iface_cast<IUIServiceMgr>(remoteObject_);
    return doms->ReturnRequest(want, source, data, extraData);
}

ErrCode UIServiceMgrClient::ShowDialog(const std::string& name,
                                       const std::string& params,
                                       OHOS::Rosen::WindowType windowType,
                                       int x,
                                       int y,
                                       int width,
                                       int height,
                                       DialogCallback callback)
{
    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
            return UI_SERVICE_NOT_CONNECTED;
        }
    }

    const sptr<DialogCallbackStub> dialogCallbackStub(new (std::nothrow)DialogCallbackStub(callback));
    sptr<IUIServiceMgr> doms = iface_cast<IUIServiceMgr>(remoteObject_);
    if (doms == nullptr) {
        HILOG_ERROR("doms is nullptr");
        return UI_SERVICE_GET_PROXY_FAILED;
    }
    return doms->ShowDialog(name, params, windowType, x, y, width, height, dialogCallbackStub);
}

ErrCode UIServiceMgrClient::CancelDialog(int32_t id)
{
    if (id < 0) {
        HILOG_INFO("invalid parameter");
        return UI_SERVICE_INVALID_PARAMETER;
    }

    if (remoteObject_ == nullptr) {
        ErrCode err = Connect();
        if (err != ERR_OK) {
            HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
            return UI_SERVICE_NOT_CONNECTED;
        }
    }

    sptr<IUIServiceMgr> doms = iface_cast<IUIServiceMgr>(remoteObject_);
    if (doms == nullptr) {
        HILOG_ERROR("doms is nullptr");
        return UI_SERVICE_GET_PROXY_FAILED;
    }
    return doms->CancelDialog(id);
}

ErrCode UIServiceMgrClient::ShowAppPickerDialog(const AAFwk::Want& want,
                                                const std::vector<AppExecFwk::AbilityInfo>& abilityInfos)
{
    if (abilityInfos.size() == 0) {
        HILOG_INFO("abilityInfos size is zero");
        return UI_SERVICE_INVALID_PARAMETER;
    }

    // get windows size
    const int32_t half = 2;
    int32_t offsetX = 0;
    int32_t offsetY = 0;
    int32_t width = UI_DIALOG_PICKER_WIDTH;
    int32_t height = UI_DIALOG_PICKER_HEIGHT;
    auto display = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    if (display != nullptr) {
        offsetX = (display->GetWidth() - width) / half;
        offsetY = display->GetHeight() - height;
    } else {
        offsetX = (UI_DEFAULT_WIDTH - width) / half;
        offsetY = UI_DEFAULT_HEIGHT - height;
    }
    HILOG_DEBUG("share dialog position: width:%{public}d, height:%{public}d, offsetX:%{public}d, offsetY:%{public}d",
        width, height, offsetX, offsetY);

    const std::string param = GetPickerDialogParam(want, abilityInfos);
    HILOG_DEBUG("share dialog js param: %{public}s", param.c_str());

    const std::string jsBundleName = "dialog_picker_service";
    return ShowDialog(jsBundleName, param, OHOS::Rosen::WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW, offsetX, offsetY,
        width, height, [want](int32_t id, const std::string& event, const std::string& params) mutable {
            HILOG_DEBUG("dialog callback: event: %{public}s, params: %{public}s", event.c_str(), params.c_str());
            if (event == "SHARE_EVENT") {
                std::string bundleName;
                std::string abilityName;
                auto pos = params.find(";");
                if (pos != std::string::npos) {
                    bundleName = params.substr(0, pos);
                    abilityName = params.substr(pos + 1, params.length() - (pos + 1));
                }
                AAFwk::Want shareWant = want;
                shareWant.SetAction("");
                shareWant.SetElementName(bundleName, abilityName);
                auto abilityClient = AAFwk::AbilityManagerClient::GetInstance();
                if (abilityClient != nullptr) {
                    HILOG_INFO("dialog callback: start ability elementName: %{public}s-%{public}s",
                        bundleName.c_str(), abilityName.c_str());
                    abilityClient->StartAbility(shareWant);
                }
            }
            Ace::UIServiceMgrClient::GetInstance()->CancelDialog(id);
        });
}

/**
 * Connect ui_service manager service.
 *
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode UIServiceMgrClient::Connect()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (remoteObject_ != nullptr) {
        return ERR_OK;
    }
    sptr<ISystemAbilityManager> systemManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemManager == nullptr) {
        HILOG_ERROR("%{private}s:fail to get Registry", __func__);
        return GET_UI_SERVICE_FAILED;
    }
    remoteObject_ = systemManager->GetSystemAbility(UI_MGR_SERVICE_SA_ID);
    if (remoteObject_ == nullptr) {
        HILOG_ERROR("%{private}s:fail to connect UIMgrService", __func__);
        return GET_UI_SERVICE_FAILED;
    }
    HILOG_DEBUG("connect UIMgrService success");
    return ERR_OK;
}

const std::string UIServiceMgrClient::GetPickerDialogParam(
    const AAFwk::Want& want, const std::vector<AppExecFwk::AbilityInfo>& abilityInfos) const
{
    std::string param = "{"; // json head
    param += "\"previewCard\": { \"type\": \"";
    param += want.GetType();
    param += "\", \"icon\": \"";
    param += "";
    param += "\", \"mainText\": \"";
    param += "";
    param += "\", \"subText\": \"";
    param += "";
    param += "\"},";
    param += "\"hapList\": [";
    for (int i = 0; i < (int)abilityInfos.size(); i++) {
        const auto& abilityInfo = abilityInfos[i];
        param += "{ \"name\": \"";
        param += abilityInfo.label; // or int32_t abilityInfo.labelId
        param += "\", \"icon\": \"";
        param += abilityInfo.iconPath; // or int32_t abilityInfo.iconId
        param += "\", \"bundle\": \"";
        param += abilityInfo.bundleName;
        param += "\", \"ability\": \"";
        param += abilityInfo.name;
        param += "\" }";
        if (i != (int)abilityInfos.size() - 1) {
            param += ",";
        }
    }
    param += "]";
    param += "}"; // json tail
    return param;
}
}  // namespace Ace
}  // namespace OHOS
