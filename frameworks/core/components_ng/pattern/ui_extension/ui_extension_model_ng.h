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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_UI_EXTENSION_UI_EXTENSION_MODEL_NG_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_UI_EXTENSION_UI_EXTENSION_MODEL_NG_H

#include <memory>
#include <string>

#include "core/components_ng/pattern/ui_extension/ui_extension_model.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT UIExtensionModelNG : public UIExtensionModel {
public:
    void Create(const std::string& bundleName, const std::string& abilityName) override;
    void SetWant(const std::string& want) override;
    void SetOnConnect(std::function<void()>&& onConnect) override;
    void SetOnDisconnect(std::function<void()>&& onDisconnect) override;
    void SetOnError(std::function<void()>&& onError) override;
    void SetOnCall(std::function<void()>&& onCall) override;
    void SetOnResult(std::function<void()>&& onResult) override;
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_UI_EXTENSION_UI_EXTENSION_MODEL_NG_H
