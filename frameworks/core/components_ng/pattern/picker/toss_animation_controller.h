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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_DATE_PICKER_TOSS_ANIMATION_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_DATE_PICKER_TOSS_ANIMATION_CONTROLLER_H

#include "core/components/picker/picker_animation.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "frameworks/base/memory/ace_type.h"

namespace OHOS::Ace::NG {
class DatePickerColumnPattern;

class TossAnimationController : public virtual AceType {
    DECLARE_ACE_TYPE(TossAnimationController, AceType);

public:
    TossAnimationController() = default;
    ~TossAnimationController() = default;

    void SetColumn(const WeakPtr<DatePickerColumnPattern>& value)
    {
        column_ = value;
    }

    void SetPipelineContext(const WeakPtr<PipelineContext>& pipeline)
    {
        pipeline_ = pipeline;
    }

    void SetStart(double y);

    void SetEnd(double y);

    bool Play();

private:
    double GetCurrentTime() const;

    double yStart_ = 0.0;
    double yEnd_ = 0.0;
    double timeStart_ = 0.0;
    double timeEnd_ = 0.0;
    double speed_ = 0.0;

    RefPtr<PickerAnimation> toss_;
    WeakPtr<DatePickerColumnPattern> column_;
    WeakPtr<PipelineContext> pipeline_;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_DATE_PICKER_TOSS_ANIMATION_CONTROLLER_H
