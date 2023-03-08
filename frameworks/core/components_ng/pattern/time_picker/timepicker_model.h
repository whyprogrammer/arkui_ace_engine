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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TIME_PICKER_TIME_PICKER_MODEL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TIME_PICKER_TIME_PICKER_MODEL_H

#include "core/components/picker/picker_data.h"
#include "core/components/picker/picker_theme.h"
#include "core/components_ng/pattern/picker/picker_type_define.h"

namespace OHOS::Ace {
using ChangeEvent = std::function<void(const BaseEventInfo* info)>;
class TimePickerModel {
public:
    static TimePickerModel* GetInstance();
    virtual ~TimePickerModel() = default;

    virtual void CreateTimePicker(RefPtr<PickerTheme> pickerTheme) = 0;
    virtual void SetSelectedTime(const PickerTime& value) = 0;
    virtual void SetOnChange(ChangeEvent&& onChange) = 0;
    virtual void SetHour24(bool isUseMilitaryTime) = 0;
    virtual void SetDisappearTextStyle(const NG::PickerTextStyle& value) = 0;
    virtual void SetNormalTextStyle(const NG::PickerTextStyle& value) = 0;
    virtual void SetSelectedTextStyle(const NG::PickerTextStyle& value) = 0;
private:
    static std::unique_ptr<TimePickerModel> timePickerInstance_;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_TIME_PICKER_TIME_PICKER_MODEL_H
