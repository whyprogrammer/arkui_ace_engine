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
#include "gtest/gtest.h"

#define private public
#include "base/log/log_wrapper.h"
#include "core/components/container_modal/container_modal_constants.h"
#include "core/components/theme/theme_constants.h"
#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/container_modal/container_modal_pattern.h"
#include "core/components_ng/pattern/image/image_layout_property.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/test/mock/render/mock_render_context.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "core/pipeline_ng/test/mock/mock_pipeline_base.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
const std::string CONTAINER_MODAL_NODE_TAG = "ContainerModalNode";
const std::string TITLE_NODE_TAG = "TitleNode";
const std::string TITLE_LABEL_NODE_TAG = "TitleLabelNode";
} // namespace
class ContainerModelTestNg : public testing::Test {
public:
    static void SetUpTestSuite();
    static void TearDownTestSuite();
};

void ContainerModelTestNg::SetUpTestSuite()
{
    const std::string tag = "test";
    const auto frameNode = AceType::MakeRefPtr<FrameNode>(tag, 0, AceType::MakeRefPtr<Pattern>());
    ViewStackProcessor::GetInstance()->Push(frameNode);
    MockPipelineBase::SetUp();
}

void ContainerModelTestNg::TearDownTestSuite()
{
    MockPipelineBase::TearDown();
}

/**
 * @tc.name: ContainerModalPatternTest001
 * @tc.desc: Test IsMeasureBoundary and IsAtomicNode.
 * @tc.type: FUNC
 */
HWTEST_F(ContainerModelTestNg, ContainerModalPatternTest001, TestSize.Level1)
{
    auto containerModalPattern = AceType::MakeRefPtr<ContainerModalPattern>();
    EXPECT_TRUE(containerModalPattern->IsMeasureBoundary());
    EXPECT_FALSE(containerModalPattern->IsAtomicNode());
}

/**
 * @tc.name: ContainerModalPatternTest002
 * @tc.desc: Test OnWindowFocused and OnWindowUnfocused.
 * @tc.type: FUNC
 */
HWTEST_F(ContainerModelTestNg, ContainerModalPatternTest002, TestSize.Level1)
{
    auto containerModalPattern = AceType::MakeRefPtr<ContainerModalPattern>();
    std::string tag = "test";
    auto frameNode = AceType::MakeRefPtr<FrameNode>(tag, 0, AceType::MakeRefPtr<Pattern>());
    frameNode->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ViewStackProcessor::GetInstance()->Push(frameNode);
    ASSERT_NE(frameNode, nullptr);

    auto frameNodeChild = AceType::MakeRefPtr<FrameNode>(tag, 1, AceType::MakeRefPtr<Pattern>());
    frameNodeChild->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ASSERT_NE(frameNodeChild, nullptr);
    frameNode->AddChild(frameNodeChild);
    containerModalPattern->OnWindowFocused();
    containerModalPattern->OnWindowUnfocused();
}

/**
 * @tc.name: ContainerModalPatternTest003
 * @tc.desc: Test InitContainerEvent.
 * @tc.type: FUNC
 */
HWTEST_F(ContainerModelTestNg, ContainerModalPatternTest003, TestSize.Level1)
{
    std::string tag = "test";
    auto frameNode = AceType::MakeRefPtr<FrameNode>(tag, 0, AceType::MakeRefPtr<Pattern>());
    frameNode->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ViewStackProcessor::GetInstance()->Push(frameNode);
    ASSERT_NE(frameNode, nullptr);

    auto frameNodeChild = AceType::MakeRefPtr<FrameNode>(tag, 1, AceType::MakeRefPtr<Pattern>());
    frameNodeChild->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ASSERT_NE(frameNodeChild, nullptr);
    frameNode->AddChild(frameNodeChild);

    ContainerModalPattern containerModalPattern;
    containerModalPattern.AttachToFrameNode(frameNode);
    containerModalPattern.InitContainerEvent();
}

/**
 * @tc.name: ContainerModalPatternTest004
 * @tc.desc: Test ShowTitle is true.
 * @tc.type: FUNC
 */
HWTEST_F(ContainerModelTestNg, ContainerModalPatternTest004, TestSize.Level1)
{
    std::string tag = "test";
    auto frameNode = AceType::MakeRefPtr<FrameNode>(tag, 0, AceType::MakeRefPtr<Pattern>());
    frameNode->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ViewStackProcessor::GetInstance()->Push(frameNode);
    ASSERT_NE(frameNode, nullptr);

    auto frameNodeChild = AceType::MakeRefPtr<FrameNode>(tag, 1, AceType::MakeRefPtr<Pattern>());
    frameNodeChild->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ASSERT_NE(frameNodeChild, nullptr);

    auto frameNodeChildTwo = AceType::MakeRefPtr<FrameNode>(tag, 2, AceType::MakeRefPtr<Pattern>());
    frameNodeChildTwo->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ASSERT_NE(frameNodeChildTwo, nullptr);

    frameNode->AddChild(frameNodeChild);
    frameNode->AddChild(frameNodeChildTwo);
    auto containerNode = AceType::DynamicCast<FrameNode>(frameNode->GetChildren().front());
    ASSERT_NE(containerNode, nullptr);
    EXPECT_NE(AceType::DynamicCast<FrameNode>(frameNode->GetChildren().back()), nullptr);

    ContainerModalPattern containerModalPattern;
    containerModalPattern.AttachToFrameNode(frameNode);
    containerModalPattern.ShowTitle(true);
}

/**
 * @tc.name: ContainerModalPatternTest005
 * @tc.desc: Test ShowTitle is false.
 * @tc.type: FUNC
 */
HWTEST_F(ContainerModelTestNg, ContainerModalPatternTest005, TestSize.Level1)
{
    std::string tag = "test";
    auto frameNode = AceType::MakeRefPtr<FrameNode>(tag, 0, AceType::MakeRefPtr<Pattern>());
    frameNode->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ViewStackProcessor::GetInstance()->Push(frameNode);
    ASSERT_NE(frameNode, nullptr);

    auto frameNodeChild = AceType::MakeRefPtr<FrameNode>(tag, 1, AceType::MakeRefPtr<Pattern>());
    frameNodeChild->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ASSERT_NE(frameNodeChild, nullptr);

    auto frameNodeChildTwo = AceType::MakeRefPtr<FrameNode>(tag, 2, AceType::MakeRefPtr<Pattern>());
    frameNodeChildTwo->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ASSERT_NE(frameNodeChildTwo, nullptr);

    frameNode->AddChild(frameNodeChild);
    frameNode->AddChild(frameNodeChildTwo);
    auto containerNode = AceType::DynamicCast<FrameNode>(frameNode->GetChildren().front());
    ASSERT_NE(containerNode, nullptr);
    ASSERT_NE(AceType::DynamicCast<FrameNode>(frameNode->GetChildren().back()), nullptr);

    ContainerModalPattern containerModalPattern;
    containerModalPattern.AttachToFrameNode(frameNode);
    containerModalPattern.ShowTitle(false);
}

/**
 * @tc.name: ContainerModalPatternTest006
 * @tc.desc: Test ShowTitle CHECK_NULL_VOID.
 * @tc.type: FUNC
 */
HWTEST_F(ContainerModelTestNg, ContainerModalPatternTest006, TestSize.Level1)
{
    ContainerModalPattern containerModalPattern;
    containerModalPattern.ShowTitle(false);
    std::string tag = "test";
    auto frameNode = AceType::MakeRefPtr<FrameNode>(tag, 0, AceType::MakeRefPtr<Pattern>());
    frameNode->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ViewStackProcessor::GetInstance()->Push(frameNode);

    ASSERT_NE(frameNode, nullptr);
    containerModalPattern.AttachToFrameNode(frameNode);
    containerModalPattern.ShowTitle(false);

    auto frameNodeChild = AceType::MakeRefPtr<FrameNode>(tag, 1, AceType::MakeRefPtr<Pattern>());
    frameNodeChild->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ASSERT_NE(frameNodeChild, nullptr);

    auto frameNodeChildTwo = AceType::MakeRefPtr<FrameNode>(tag, 2, AceType::MakeRefPtr<Pattern>());
    frameNodeChildTwo->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ASSERT_NE(frameNodeChildTwo, nullptr);

    frameNode->AddChild(frameNodeChild);
    frameNode->AddChild(frameNodeChildTwo);
    auto containerNode = AceType::DynamicCast<FrameNode>(frameNode->GetChildren().front());
    ASSERT_NE(containerNode, nullptr);
    ASSERT_NE(AceType::DynamicCast<FrameNode>(frameNode->GetChildren().back()), nullptr);

    containerModalPattern.AttachToFrameNode(frameNode);
    containerModalPattern.ShowTitle(false);
}

/**
 * @tc.name: ContainerModalPatternTest007
 * @tc.desc: Test SetAppIcon.
 * @tc.type: FUNC
 */
HWTEST_F(ContainerModelTestNg, ContainerModalPatternTest007, TestSize.Level1)
{
    std::string tag = "test";
    auto frameNode = AceType::MakeRefPtr<FrameNode>(tag, 0, AceType::MakeRefPtr<Pattern>());
    frameNode->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ViewStackProcessor::GetInstance()->Push(frameNode);
    ASSERT_NE(frameNode, nullptr);

    auto frameNodeChild = AceType::MakeRefPtr<FrameNode>(tag, 1, AceType::MakeRefPtr<Pattern>());
    frameNodeChild->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ViewStackProcessor::GetInstance()->Push(frameNodeChild);
    ASSERT_NE(frameNodeChild, nullptr);

    auto frameNodeChildTwo = AceType::MakeRefPtr<FrameNode>(tag, 2, AceType::MakeRefPtr<Pattern>());
    frameNodeChildTwo->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ViewStackProcessor::GetInstance()->Push(frameNodeChildTwo);
    ASSERT_NE(frameNodeChildTwo, nullptr);

    frameNode->AddChild(frameNodeChild);
    frameNodeChild->AddChild(frameNodeChildTwo);
    auto containerNode = AceType::DynamicCast<FrameNode>(frameNode->GetChildren().front());
    ASSERT_NE(AceType::DynamicCast<FrameNode>(frameNode->GetChildren().back()), nullptr);

    ContainerModalPattern containerModalPattern;
    containerModalPattern.AttachToFrameNode(frameNode);

    RefPtr<PixelMap> pixelMap;
    containerModalPattern.SetAppIcon(pixelMap);
}

/**
 * @tc.name: ContainerModalPatternTest009
 * @tc.desc: Test ChangeFloatingTitle.
 * @tc.type: FUNC
 */
HWTEST_F(ContainerModelTestNg, ContainerModalPatternTest008, TestSize.Level1)
{
    auto containerModalPattern = AceType::MakeRefPtr<ContainerModalPattern>();

    const std::string tag = "test";
    containerModalPattern->frameNode_ = AceType::MakeRefPtr<FrameNode>(tag, 0, AceType::MakeRefPtr<Pattern>());
    RefPtr<FrameNode> result = containerModalPattern->GetHost();
    containerModalPattern->ChangeFloatingTitle(result, true);
}

/**
 * @tc.name: ContainerModalPatternTest010
 * @tc.desc: Test ChangeTitleButtonIcon.
 * @tc.type: FUNC
 */
HWTEST_F(ContainerModelTestNg, ContainerModalPatternTest009, TestSize.Level1)
{
    std::string tag = "Button";
    auto frameNode = AceType::MakeRefPtr<FrameNode>(tag, 0, AceType::MakeRefPtr<Pattern>());
    frameNode->renderContext_ = AceType::MakeRefPtr<MockRenderContext>();
    ViewStackProcessor::GetInstance()->Push(frameNode);
    ASSERT_NE(frameNode, nullptr);

    auto containerModalPattern = AceType::MakeRefPtr<ContainerModalPattern>();
    containerModalPattern->ChangeTitleButtonIcon(
        frameNode, InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_SPLIT_LEFT, true);
}

/**
 * @tc.name: ContainerModalAccessibilityPropertyGetText001
 * @tc.desc: Test GetText of containerModal.
 * @tc.type: FUNC
 */
HWTEST_F(ContainerModelTestNg, ContainerModalAccessibilityPropertyGetText001, TestSize.Level1)
{
    auto containerModalNode = AceType::MakeRefPtr<FrameNode>(
        CONTAINER_MODAL_NODE_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<Pattern>());
    ASSERT_NE(containerModalNode, nullptr);

    auto columnNode = AceType::MakeRefPtr<FrameNode>(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(true));
    ASSERT_NE(columnNode, nullptr);

    auto titleNode = AceType::MakeRefPtr<FrameNode>(V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(false));
    ASSERT_NE(titleNode, nullptr);

    auto titleLabelNodeOne = AceType::MakeRefPtr<FrameNode>(
        TITLE_LABEL_NODE_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
    ASSERT_NE(titleLabelNodeOne, nullptr);

    auto titleLabelNodeTwo = AceType::MakeRefPtr<FrameNode>(
        TITLE_LABEL_NODE_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
    ASSERT_NE(titleLabelNodeTwo, nullptr);

    auto textLayoutProperty = titleLabelNodeTwo->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    textLayoutProperty->UpdateContent(CONTAINER_MODAL_NODE_TAG);

    titleNode->AddChild(titleLabelNodeOne);
    titleNode->AddChild(titleLabelNodeTwo);
    columnNode->AddChild(titleNode);
    containerModalNode->AddChild(columnNode);

    ContainerModalAccessibilityProperty containerModalAccessibilityProperty;
    containerModalAccessibilityProperty.SetHost(containerModalNode);

    EXPECT_EQ(containerModalAccessibilityProperty.GetText(), CONTAINER_MODAL_NODE_TAG);
}
} // namespace OHOS::Ace::NG
