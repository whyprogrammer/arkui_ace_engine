/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "bridge/declarative_frontend/jsview/window_scene/js_host_window_scene.h"

#include "core/components_ng/pattern/window_scene/scene/host/host_window_scene_model.h"

namespace OHOS::Ace::Framework {
void JSHostWindowScene::JSBind(BindingTarget globalObj)
{
    JSClass<JSHostWindowScene>::Declare("HostWindowScene");
    JSClass<JSHostWindowScene>::StaticMethod("create", &JSHostWindowScene::Create, MethodOptions::NONE);

    JSClass<JSHostWindowScene>::Inherit<JSViewAbstract>();
    JSClass<JSHostWindowScene>::Inherit<JSInteractableView>();
    JSClass<JSHostWindowScene>::Bind(globalObj);
}

void JSHostWindowScene::Create(const JSCallbackInfo& info)
{
    if (!Container::IsCurrentUseNewPipeline()) {
        LOGE("Window scene is only supported on new pipeline!");
        return;
    }

    if (info.Length() != 1) {
        LOGE("The arg is wrong, it is supposed to have 1 argument");
        return;
    }

    if (!info[0]->IsNumber()) {
        LOGE("The arg is not a number");
        return;
    }

    auto persistentId = static_cast<uint64_t>(info[0]->ToNumber<double>());
    NG::HostWindowSceneModel::Create(persistentId);
}
} // namespace OHOS::Ace::Framework
