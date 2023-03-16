/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/slider/slider_layout_algorithm.h"

#include "base/utils/utils.h"
#include "core/components_ng/layout/layout_wrapper.h"
#include "core/components_ng/pattern/slider/slider_layout_property.h"
#include "core/components_ng/pattern/slider/slider_pattern.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
namespace {
constexpr float HALF = 0.5f;
} // namespace

std::optional<SizeF> SliderLayoutAlgorithm::MeasureContent(
    const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper)
{
    auto frameNode = layoutWrapper->GetHostNode();
    CHECK_NULL_RETURN(frameNode, std::nullopt);
    auto sliderLayoutProperty = DynamicCast<SliderLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_RETURN(sliderLayoutProperty, std::nullopt);
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, std::nullopt);
    auto theme = pipeline->GetTheme<SliderTheme>();
    CHECK_NULL_RETURN(theme, std::nullopt);

    float width = contentConstraint.selfIdealSize.Width().value_or(contentConstraint.maxSize.Width());
    float height = contentConstraint.selfIdealSize.Height().value_or(contentConstraint.maxSize.Height());

    Axis direction = sliderLayoutProperty->GetDirection().value_or(Axis::HORIZONTAL);
    if (direction == Axis::HORIZONTAL && GreaterOrEqualToInfinity(width)) {
        width = static_cast<float>(theme->GetLayoutMaxLength().ConvertToPx());
    }
    if (direction == Axis::VERTICAL && GreaterOrEqualToInfinity(height)) {
        height = static_cast<float>(theme->GetLayoutMaxLength().ConvertToPx());
    }
    Dimension themeTrackThickness = sliderLayoutProperty->GetSliderMode().value_or(SliderModel::SliderMode::OUTSET) ==
                                            SliderModel::SliderMode::OUTSET
                                        ? theme->GetOutsetTrackThickness()
                                        : theme->GetInsetTrackThickness();
    trackThickness_ =
        static_cast<float>(sliderLayoutProperty->GetThickness().value_or(themeTrackThickness).ConvertToPx());
    float scaleValue = trackThickness_ / static_cast<float>(themeTrackThickness.ConvertToPx());
    Dimension themeBlockSize = sliderLayoutProperty->GetSliderMode().value_or(SliderModel::SliderMode::OUTSET) ==
                                       SliderModel::SliderMode::OUTSET
                                   ? theme->GetOutsetBlockSize()
                                   : theme->GetInsetBlockSize();
    auto blockDiameter = scaleValue * static_cast<float>(themeBlockSize.ConvertToPx());
    blockSize_ = sliderLayoutProperty->GetBlockSizeValue(SizeF(blockDiameter, blockDiameter));
    Dimension themeBlockHotSize = sliderLayoutProperty->GetSliderMode().value_or(SliderModel::SliderMode::OUTSET) ==
                                          SliderModel::SliderMode::OUTSET
                                      ? theme->GetOutsetBlockHotSize()
                                      : theme->GetInsetBlockHotSize();
    Dimension hotBlockShadowWidth = sliderLayoutProperty->GetSliderMode().value_or(SliderModel::SliderMode::OUTSET) ==
                                            SliderModel::SliderMode::OUTSET
                                        ? theme->GetOutsetHotBlockShadowWidth()
                                        : theme->GetInsetHotBlockShadowWidth();
    blockHotSize_ = scaleValue * static_cast<float>(themeBlockHotSize.ConvertToPx());
    auto blockWidth = direction == Axis::HORIZONTAL ? blockSize_.Height() : blockSize_.Width();
    auto sliderWidth = static_cast<float>(theme->GetMeasureContentDefaultWidth().ConvertToPx());
    sliderWidth = std::max(sliderWidth, trackThickness_);
    sliderWidth = std::max(sliderWidth, blockHotSize_);
    sliderWidth = std::max(sliderWidth, blockWidth + static_cast<float>(hotBlockShadowWidth.ConvertToPx()) / HALF);
    sliderWidth = std::clamp(sliderWidth, 0.0f, direction == Axis::HORIZONTAL ? height : width);
    float sliderLength = direction == Axis::HORIZONTAL ? width : height;
    return direction == Axis::HORIZONTAL ? SizeF(sliderLength, sliderWidth) : SizeF(sliderWidth, sliderLength);
}

void SliderLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    auto layoutConstraint = layoutWrapper->GetLayoutProperty()->CreateChildConstraint();
    layoutConstraint.UpdateSelfMarginSizeWithCheck(OptionalSizeF(blockSize_.Width(), blockSize_.Height()));
    if (layoutWrapper->GetTotalChildCount() != 0) {
        auto child = layoutWrapper->GetOrCreateChildByIndex(0);
        child->Measure(layoutConstraint);
    }
    PerformMeasureSelf(layoutWrapper);
}

void SliderLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    PerformLayout(layoutWrapper);
    const auto& children = layoutWrapper->GetAllChildrenWithBuild();
    if (children.empty()) {
        return;
    }

    auto host = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(host);
    auto pattern = DynamicCast<SliderPattern>(host->GetPattern());
    CHECK_NULL_VOID(pattern);
    auto sliderLayoutProperty = host->GetLayoutProperty<SliderLayoutProperty>();
    CHECK_NULL_VOID(sliderLayoutProperty);
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SliderTheme>();
    CHECK_NULL_VOID(theme);

    auto selfSize = layoutWrapper->GetGeometryNode()->GetContentSize();
    auto axis = sliderLayoutProperty->GetDirection().value_or(Axis::HORIZONTAL);
    auto reverse = sliderLayoutProperty->GetReverseValue(false);
    auto mode = sliderLayoutProperty->GetSliderMode().value_or(SliderModel::SliderMode::OUTSET);
    Dimension hotBlockShadowWidth = mode == SliderModel::SliderMode::OUTSET ? theme->GetOutsetHotBlockShadowWidth()
                                                                            : theme->GetInsetHotBlockShadowWidth();
    auto length = axis == Axis::HORIZONTAL ? selfSize.Width() : selfSize.Height();
    float BlockShadowWidth = static_cast<float>(hotBlockShadowWidth.ConvertToPx());
    auto blockSize = axis == Axis::HORIZONTAL ? blockSize_.Width() : blockSize_.Height();
    auto borderBlank = std::max(trackThickness_, blockSize + BlockShadowWidth / HALF);
    auto sliderLength = length >= borderBlank ? length - borderBlank : 1;
    borderBlank = (length - sliderLength) * HALF;
    auto selectOffset = borderBlank + pattern->GetValueRatio() * sliderLength;

    CalculateBlockOffset(layoutWrapper, selfSize, selectOffset, axis, reverse);
}

void SliderLayoutAlgorithm::CalculateBlockOffset(
    LayoutWrapper* layoutWrapper, const SizeF& selfSize, float selectOffset, Axis axis, bool reverse)
{
    auto host = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(host);
    auto pattern = DynamicCast<SliderPattern>(host->GetPattern());
    CHECK_NULL_VOID(pattern);

    const auto& children = layoutWrapper->GetAllChildrenWithBuild();
    auto child = children.front();
    auto childSize_ = child->GetGeometryNode()->GetMarginFrameSize();
    float blockOffset = 0;
    if (axis == Axis::HORIZONTAL) {
        blockOffset = childSize_.Width() * HALF;
    } else {
        blockOffset = childSize_.Height() * HALF;
    }

    OffsetF circleCenter;
    if (pattern->GetBlockCenter() == OffsetF()) {
        if (!reverse) {
            if (axis == Axis::HORIZONTAL) {
                circleCenter.SetX(selectOffset);
                circleCenter.SetY(selfSize.Height() * HALF);
            } else {
                circleCenter.SetX(selfSize.Width() * HALF);
                circleCenter.SetY(selectOffset);
            }
        } else {
            if (axis == Axis::HORIZONTAL) {
                circleCenter.SetX(selfSize.Width() - selectOffset);
                circleCenter.SetY(selfSize.Height() * HALF);
            } else {
                circleCenter.SetX(selfSize.Width() * HALF);
                circleCenter.SetY(selfSize.Height() - selectOffset);
            }
        }
    } else {
        circleCenter = pattern->GetBlockCenter();
    }

    OffsetF imageNodeOffset(
        circleCenter.GetX() - childSize_.Width() * HALF, circleCenter.GetY() - childSize_.Height() * HALF);

    child->GetGeometryNode()->SetMarginFrameOffset(imageNodeOffset);
    child->Layout();
}

} // namespace OHOS::Ace::NG
