/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_CONFIGURATION_OBSERVER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_CONFIGURATION_OBSERVER_H

#include "app_mgr_client.h"
#include "configuration_observer_stub.h"
#include "nweb_helper.h"
#include "web_delegate.h"

namespace OHOS::Ace {
class WebConfigurationObserver : public OHOS::AppExecFwk::ConfigurationObserverStub {
public:
    explicit WebConfigurationObserver(RefPtr<WebDelegate> delegate) : delegate_(delegate) {}
    ~WebConfigurationObserver() override = default;

    void OnConfigurationUpdated(const OHOS::AppExecFwk::Configuration& configuration) override;

private:
    RefPtr<WebDelegate> delegate_ = nullptr;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WEB_CONFIGURATION_OBSERVER_H