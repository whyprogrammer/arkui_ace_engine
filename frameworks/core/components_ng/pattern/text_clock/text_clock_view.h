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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TEXT_CLOCK_VIEW_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TEXT_CLOCK_VIEW_H

#include <string>

#include "base/geometry/dimension.h"
#include "base/utils/macros.h"
#include "core/components/text_clock/text_clock_controller.h"
#include "core/components_ng/pattern/text_clock/text_clock_event_hub.h"

namespace OHOS::Ace::NG {
class ACE_EXPORT TextClockView {
public:
    static RefPtr<TextClockController> Create();
    static void SetFormat(const std::string& format);
    static void SetHoursWest(const int32_t& hoursWest);
    static void SetOnDateChange(ChangeEvent&& onChange);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_TEXT_CLOCK_VIEW_H
