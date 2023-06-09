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

#include "core/components_ng/pattern/patternlock/patternlock_paint_method.h"

#include "core/components/theme/theme_manager.h"
#include "core/components_ng/pattern/patternlock/patternlock_paint_property.h"
#include "core/components_ng/pattern/patternlock/patternlock_pattern.h"
#include "core/components_ng/property/measure_utils.h"
#include "core/components_ng/render/drawing.h"
#include "core/components_ng/render/drawing_prop_convertor.h"
#include "core/components_v2/pattern_lock/pattern_lock_theme.h"

namespace OHOS::Ace::NG {
void PatternLockPaintMethod::UpdateContentModifier(PaintWrapper* paintWrapper)
{
    CHECK_NULL_VOID(patternlockModifier_);
    GetThemeProp();
    auto paintProperty = DynamicCast<PatternLockPaintProperty>(paintWrapper->GetPaintProperty());
    CHECK_NULL_VOID(paintProperty);
    sideLength_ = paintProperty->GetSideLength().value_or(sideLength_);
    circleRadius_ = paintProperty->GetCircleRadius().value_or(circleRadius_);
    regularColor_ = paintProperty->GetRegularColor().value_or(regularColor_);
    selectedColor_ = paintProperty->GetSelectedColor().value_or(selectedColor_);
    activeColor_ = paintProperty->GetActiveColor().value_or(activeColor_);
    pathColor_ = paintProperty->GetPathColor().value_or(pathColor_);
    pathStrokeWidth_ = paintProperty->GetPathStrokeWidth().value_or(pathStrokeWidth_);

    patternlockModifier_->SetSideLength(sideLength_.ConvertToPx());
    patternlockModifier_->SetCircleRadius(circleRadius_.ConvertToPx());
    patternlockModifier_->SetRegularColor(LinearColor(regularColor_));
    patternlockModifier_->SetSelectColor(LinearColor(selectedColor_));
    patternlockModifier_->SetActiveColor(LinearColor(activeColor_));
    patternlockModifier_->SetPathColor(LinearColor(pathColor_));
    patternlockModifier_->SetPathStrokeWidth(pathStrokeWidth_.ConvertToPx());
    patternlockModifier_->SetIsMoveEventValid(isMoveEventValid_);
    patternlockModifier_->SetCellCenterOffset(cellCenter_);
    patternlockModifier_->SetContentOffset(paintWrapper->GetContentOffset());
    patternlockModifier_->SetChoosePoint(choosePoint_);
}

void PatternLockPaintMethod::GetThemeProp()
{
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto patternLockTheme = pipeline->GetTheme<V2::PatternLockTheme>();
    CHECK_NULL_VOID(patternLockTheme);
    regularColor_ = patternLockTheme->GetRegularColor();
    selectedColor_ = patternLockTheme->GetSelectedColor();
    activeColor_ = patternLockTheme->GetActiveColor();
    pathColor_ = patternLockTheme->GetPathColor();
}
} // namespace OHOS::Ace::NG
