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

#include "core/components_ng/pattern/toggle/switch_layout_algorithm.h"

#include "hilog/log_c.h"

#include "base/geometry/ng/size_t.h"
#include "core/components/checkable/checkable_theme.h"
#include "core/components_ng/base/frame_node.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
std::optional<SizeF> SwitchLayoutAlgorithm::MeasureContent(
    const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper)
{
    auto frameNode = layoutWrapper->GetHostNode();
    CHECK_NULL_RETURN(frameNode, std::nullopt);
    auto pipeline = frameNode->GetContext();
    CHECK_NULL_RETURN(pipeline, std::nullopt);
    const auto& layoutProperty = layoutWrapper->GetLayoutProperty();
    CHECK_NULL_RETURN(layoutProperty, std::nullopt);
    float frameHeight = 0.0f;
    float frameWidth = 0.0f;
    auto themeManager = pipeline->GetThemeManager();
    CHECK_NULL_RETURN(themeManager, std::nullopt);
    auto switchTheme = themeManager->GetTheme<SwitchTheme>();
    CHECK_NULL_RETURN(switchTheme, std::nullopt);
    if (contentConstraint.selfIdealSize.Width().has_value()) {
        frameWidth = contentConstraint.selfIdealSize.Width().value();
    } else {
        frameWidth =
            static_cast<float>((switchTheme->GetWidth() - switchTheme->GetHotZoneHorizontalPadding() * 2).Value());
    }
    if (contentConstraint.selfIdealSize.Height().has_value()) {
        frameHeight = contentConstraint.selfIdealSize.Height().value();
    } else {
        frameHeight =
            static_cast<float>((switchTheme->GetHeight() - switchTheme->GetHotZoneVerticalPadding() * 2).Value());
    }

    if ((frameWidth / frameHeight) < ratio_) {
        width_ = frameWidth;
        height_ = width_ / ratio_;
    } else if ((frameWidth / frameHeight) > ratio_) {
        height_ = frameHeight;
        width_ = height_ * ratio_;
    } else {
        height_ = frameHeight;
        width_ = frameWidth;
    }

    if (layoutProperty->GetPositionProperty()) {
        layoutProperty->UpdateAlignment(
            layoutProperty->GetPositionProperty()->GetAlignment().value_or(Alignment::CENTER));
    } else {
        layoutProperty->UpdateAlignment(Alignment::CENTER);
    }

    return SizeF(width_, height_);
}

} // namespace OHOS::Ace::NG