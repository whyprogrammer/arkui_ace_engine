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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_UI_EXTENSION_UI_EXTENSION_MODEL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_UI_EXTENSION_UI_EXTENSION_MODEL_H

#include <memory>
#include <mutex>
#include <string>

#include "base/utils/macros.h"

namespace OHOS::Ace {
class ACE_EXPORT UIExtensionModel {
public:
    static UIExtensionModel* GetInstance();
    virtual ~UIExtensionModel() = default;

    virtual void Create(const std::string& bundleName, const std::string& abilityName);
    virtual void SetWant(const std::string& want);
    virtual void SetOnConnect(std::function<void()>&& onConnect);
    virtual void SetOnDisconnect(std::function<void()>&& onDisconnect);
    virtual void SetOnError(std::function<void()>&& onError);
    virtual void SetOnCall(std::function<void()>&& onCall);
    virtual void SetOnResult(std::function<void()>&& onResult);

private:
    static std::unique_ptr<UIExtensionModel> instance_;
    static std::mutex mutex_;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_UI_EXTENSION_UI_EXTENSION_MODEL_H
