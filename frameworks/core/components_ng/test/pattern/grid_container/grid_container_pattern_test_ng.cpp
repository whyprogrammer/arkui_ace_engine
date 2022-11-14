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

#include <optional>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "base/geometry/dimension.h"
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/grid_container/grid_container_model_ng.h"
#include "core/components_ng/pattern/grid_container/grid_container_pattern.h"
#include "core/components_ng/pattern/text/text_model_ng.h"
#include "core/components_ng/pattern/text/text_layout_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {

using std::cout;
using std::endl;

namespace {

constexpr float DEFAULT_WIDTH = 800;
constexpr int32_t DEFAULT_COLUMN_NUM = 8;
constexpr float DEFAULT_SPAN_WIDTH = 71; // To modify
constexpr float DEFAULT_OFFSET = 95; // To modify

void MeasureLayout(RefPtr<LayoutWrapper>& container, RefPtr<LayoutWrapper>& text)
{
    auto algorithm = container->GetLayoutAlgorithm();
    algorithm->Measure(AceType::RawPtr(container));
    algorithm->Layout(AceType::RawPtr(container));

    auto textLayoutProperty = text->GetLayoutProperty();
    textLayoutProperty->BuildGridProperty(text->GetHostNode());
    auto textAlgorithm = text->GetLayoutAlgorithm();
    textAlgorithm->Measure(AceType::RawPtr(text));
    textAlgorithm->Layout(AceType::RawPtr(text));
    textLayoutProperty->UpdateGridOffset(AceType::RawPtr(text));
}

} // namespace

class GridContainerPatternTestNg : public testing::Test {
public:
    RefPtr<LayoutWrapper> CreateGridContainerWithChild()
    {
        GridContainerInfo::Builder builder;
        GridContainerModelNG modelNG;
        modelNG.Create(builder.Build());

        TextModelNG text;
        text.Create("child");
        textNode_ = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
        textNode_->GetLayoutProperty()->UpdateGridProperty(1, 1);
        ViewStackProcessor::GetInstance()->Pop();

        containerNode_ = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->GetMainElementNode());
        ViewStackProcessor::GetInstance()->PopContainer();

        auto layoutWrapper = containerNode_->CreateLayoutWrapper();
        LayoutConstraintF constraint;
        OptionalSizeF size;
        size.SetWidth(DEFAULT_WIDTH);
        constraint.UpdateIllegalSelfIdealSizeWithCheck(size);
        auto layoutProperty = layoutWrapper->GetLayoutProperty();
        layoutProperty->UpdateLayoutConstraint(constraint);
        auto geometryNode = layoutWrapper->GetGeometryNode();
        geometryNode->SetParentLayoutConstraint(constraint);
        layoutProperty->UpdateContentConstraint();

        return layoutWrapper;
    }

private:
    RefPtr<FrameNode> textNode_;
    RefPtr<FrameNode> containerNode_;
};

/**
 * @tc.name: DefaultProperty000
 * @tc.desc: Test default properties of grid-container.
 * @tc.type: FUNC
 */
HWTEST_F(GridContainerPatternTestNg, DefaultProperty000, TestSize.Level1)
{
    GridContainerInfo::Builder builder;
    GridContainerModelNG modelNG;
    modelNG.Create(builder.Build());

    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);
    auto layoutProperty = frameNode->GetLayoutProperty<GridContainerLayoutProperty>();
    EXPECT_NE(layoutProperty, nullptr);
    auto clone = AceType::DynamicCast<GridContainerLayoutProperty>(layoutProperty->Clone());
    EXPECT_EQ(*clone, *layoutProperty);

    // rubbish code for coverity
    clone.Reset();
    auto json = JsonUtil::Create(true);
    layoutProperty->ToJsonValue(json);
}

/**
 * @tc.name: BuildContainer000
 * @tc.desc: Test GridContainer build width.
 * @tc.type: FUNC
 */
HWTEST_F(GridContainerPatternTestNg, BuildContainer000, TestSize.Level1)
{
    auto layoutWrapper = CreateGridContainerWithChild();
    auto textWrapper = layoutWrapper->GetOrCreateChildByIndex(0);

    /* first time layout to build size */
    MeasureLayout(layoutWrapper, textWrapper);
    auto rect = textWrapper->GetGeometryNode()->GetFrameRect();
    EXPECT_NE(rect.Width(), DEFAULT_WIDTH / DEFAULT_COLUMN_NUM);
    EXPECT_EQ(rect.GetX(), DEFAULT_OFFSET);

    /* update property to mark-dirty children */
    GridContainerInfo::Builder builder;
    builder.SetColumns(DEFAULT_COLUMN_NUM);
    auto layoutProperty = AceType::DynamicCast<GridContainerLayoutProperty>(layoutWrapper->GetLayoutProperty());
    layoutProperty->UpdateContainerInfo(*builder.Build());

    /* second time layout */
    MeasureLayout(layoutWrapper, textWrapper);
    rect = textWrapper->GetGeometryNode()->GetFrameRect();
    EXPECT_EQ(rect.Width(), DEFAULT_SPAN_WIDTH);
    EXPECT_EQ(rect.GetX(), DEFAULT_OFFSET);
}

} // namespace OHOS::Ace::NG
