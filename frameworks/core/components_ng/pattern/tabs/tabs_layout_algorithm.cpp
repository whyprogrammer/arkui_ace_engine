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

#include "core/components_ng/pattern/tabs/tabs_layout_algorithm.h"

#include "base/geometry/axis.h"
#include "base/geometry/ng/offset_t.h"
#include "base/geometry/ng/size_t.h"
#include "base/log/ace_trace.h"
#include "base/utils/utils.h"
#include "core/components_ng/layout/layout_algorithm.h"
#include "core/components_ng/property/layout_constraint.h"
#include "core/components_ng/property/measure_property.h"
#include "core/components_ng/property/measure_utils.h"

namespace OHOS::Ace::NG {

void TabsLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    auto layoutProperty = AceType::DynamicCast<TabsLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_VOID(layoutProperty);
    auto geometryNode = layoutWrapper->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);
    auto axis = layoutProperty->GetAxis().value_or(Axis::HORIZONTAL);
    auto constraint = layoutProperty->GetLayoutConstraint();
    auto idealSize = CreateIdealSize(constraint.value(), Axis::HORIZONTAL, layoutProperty->GetMeasureType(), true);
    if (GreatOrEqual(idealSize.Width(), Infinity<float>()) || GreatOrEqual(idealSize.Height(), Infinity<float>())) {
        LOGW("Size is infinity.");
        geometryNode->SetFrameSize(SizeF());
        return;
    }
    geometryNode->SetFrameSize(idealSize);
    auto childLayoutConstraint = layoutProperty->CreateChildConstraint();
    childLayoutConstraint.parentIdealSize = OptionalSizeF(idealSize);

    // Measure tab bar.
    auto tabBarWrapper = layoutWrapper->GetOrCreateChildByIndex(0);
    SizeF tabBarSize;
    if (tabBarWrapper) {
        tabBarWrapper->Measure(childLayoutConstraint);
        tabBarSize = tabBarWrapper->GetGeometryNode()->GetFrameSize();
    }

    // Measure swiper.
    auto swiperWrapper = layoutWrapper->GetOrCreateChildByIndex(1);
    if (swiperWrapper) {
        SizeF parentIdealSize = idealSize;
        if (axis == Axis::HORIZONTAL) {
            childLayoutConstraint.maxSize.SetHeight(childLayoutConstraint.maxSize.Height() - tabBarSize.Height());
            parentIdealSize.SetHeight(idealSize.Height() - tabBarSize.Height());
        } else if (axis == Axis::VERTICAL) {
            childLayoutConstraint.maxSize.SetWidth(childLayoutConstraint.maxSize.Width() - tabBarSize.Width());
            parentIdealSize.SetWidth(idealSize.Width() - tabBarSize.Width());
        }
        childLayoutConstraint.parentIdealSize = OptionalSizeF(parentIdealSize);
        swiperWrapper->Measure(childLayoutConstraint);
    }
}

void TabsLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    CHECK_NULL_VOID(layoutWrapper);
    auto geometryNode = layoutWrapper->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);
    auto frameSize = geometryNode->GetFrameSize();
    if (!frameSize.IsPositive()) {
        LOGW("Frame size is not positive.");
        return;
    }

    auto tabBarWrapper = layoutWrapper->GetOrCreateChildByIndex(0);
    auto swiperWrapper = layoutWrapper->GetOrCreateChildByIndex(1);
    if (!tabBarWrapper || !swiperWrapper) {
        return;
    }

    auto axis = GetAxis(layoutWrapper);
    auto barPosition = GetBarPosition(layoutWrapper);
    auto tabBarGeometryNode = tabBarWrapper->GetGeometryNode();
    CHECK_NULL_VOID(tabBarGeometryNode);
    auto tabBarFrameSize = tabBarGeometryNode->GetFrameSize();

    OffsetF tabBarOffset;
    OffsetF swiperOffset;
    if (axis == Axis::HORIZONTAL) {
        if (barPosition == BarPosition::START) {
            tabBarOffset = OffsetF();
            swiperOffset = OffsetF(0.0f, tabBarFrameSize.MainSize(Axis::VERTICAL));
        } else {
            tabBarOffset = OffsetF(0.0f, frameSize.MainSize(Axis::VERTICAL) - tabBarFrameSize.MainSize(Axis::VERTICAL));
            swiperOffset = OffsetF();
        }
    } else {
        if (barPosition == BarPosition::START) {
            tabBarOffset = OffsetF();
            swiperOffset = OffsetF(tabBarFrameSize.MainSize(Axis::HORIZONTAL), 0.0f);
        } else {
            tabBarOffset =
                OffsetF(frameSize.MainSize(Axis::HORIZONTAL) - tabBarFrameSize.MainSize(Axis::HORIZONTAL), 0.0f);
            swiperOffset = OffsetF();
        }
    }

    auto parentOffset = geometryNode->GetParentGlobalOffset() + geometryNode->GetFrameOffset();
    tabBarGeometryNode->SetFrameOffset(tabBarOffset);
    tabBarWrapper->Layout(parentOffset);

    swiperWrapper->GetGeometryNode()->SetFrameOffset(swiperOffset);
    swiperWrapper->Layout(parentOffset);
}

BarPosition TabsLayoutAlgorithm::GetBarPosition(LayoutWrapper* layoutWrapper) const
{
    auto layoutProperty = AceType::DynamicCast<TabsLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_RETURN(layoutProperty, BarPosition::START);
    return layoutProperty->GetTabBarPosition().value_or(BarPosition::START);
}

Axis TabsLayoutAlgorithm::GetAxis(LayoutWrapper* layoutWrapper) const
{
    auto layoutProperty = AceType::DynamicCast<TabsLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_RETURN(layoutProperty, Axis::HORIZONTAL);
    return layoutProperty->GetAxis().value_or(Axis::HORIZONTAL);
}

} // namespace OHOS::Ace::NG
