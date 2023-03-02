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

#include "gtest/gtest.h"

#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/checkbox/checkbox_model_ng.h"
#include "core/components_ng/pattern/checkbox/checkbox_paint_property.h"
#include "core/components_ng/pattern/checkboxgroup/checkboxgroup_model_ng.h"
#include "core/components_ng/pattern/checkboxgroup/checkboxgroup_paint_property.h"
#include "core/components_ng/test/mock/rosen/mock_canvas.h"
#include "core/components_ng/test/mock/theme/mock_theme_manager.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "core/pipeline_ng/test/mock/mock_pipeline_base.h"
// Add the following two macro definitions to test the private and protected method.
#define private public
#define protected public
#include "core/components/checkable/checkable_theme.h"
#include "core/components_ng/pattern/checkbox/checkbox_pattern.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
const std::string NAME = "checkbox";
const std::string GROUP_NAME = "checkboxGroup";
const std::string GROUP_NAME_CHANGE = "checkboxGroupChange";
const std::string TAG = "CHECKBOX_TAG";
const Dimension WIDTH = 50.0_vp;
const Dimension HEIGHT = 50.0_vp;
const NG::PaddingPropertyF PADDING = NG::PaddingPropertyF();
const bool SELECTED = true;
const Color SELECTED_COLOR = Color::BLUE;
const Color BOARD_COLOR = Color::BLUE;
const Color CHECK_COLOR = Color::WHITE;
const Color BORDER_COLOR = Color::GRAY;
const Color SHADOW_COLOR = Color::BLUE;
const SizeF CONTENT_SIZE = SizeF(400.0, 500.0);
const OffsetF CONTENT_OFFSET = OffsetF(50.0, 60.0);
} // namespace

class CheckBoxPropertyTestNg : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void CheckBoxPropertyTestNg::SetUpTestCase()
{
    MockPipelineBase::SetUp();
}
void CheckBoxPropertyTestNg::TearDownTestCase()
{
    MockPipelineBase::TearDown();
}
void CheckBoxPropertyTestNg::SetUp() {}
void CheckBoxPropertyTestNg::TearDown() {}

/**
 * @tc.name: CheckBoxPaintPropertyTest001
 * @tc.desc: Set CheckBox value into CheckBoxPaintProperty and get it.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPaintPropertyTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBox node
     */
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);

    /**
     * @tc.steps: step2. Set parameters to CheckBox property
     */
    checkBoxModelNG.SetSelect(SELECTED);
    checkBoxModelNG.SetSelectedColor(SELECTED_COLOR);
    checkBoxModelNG.SetWidth(WIDTH);
    checkBoxModelNG.SetHeight(HEIGHT);
    checkBoxModelNG.SetPadding(PADDING);

    /**
     * @tc.steps: step3. Get paint property and get CheckBox property
     * @tc.expected: step3. Check the CheckBox property value
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto checkBoxPaintProperty = frameNode->GetPaintProperty<CheckBoxPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    EXPECT_EQ(checkBoxPaintProperty->GetCheckBoxSelect(), SELECTED);
    EXPECT_EQ(checkBoxPaintProperty->GetCheckBoxSelectedColor(), SELECTED_COLOR);
}

/**
 * @tc.name: CheckBoxEventHubPropertyTest002
 * @tc.desc: Set CheckBox value into CheckBoxEventHub and get it.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxEventHubPropertyTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBox node
     */
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);

    /**
     * @tc.steps: step2. Get CheckBox event hub and get property
     * @tc.expected: step2. Check the CheckBox property value
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto eventHub = frameNode->GetEventHub<NG::CheckBoxEventHub>();
    ASSERT_NE(eventHub, nullptr);
    EXPECT_EQ(eventHub->GetName(), NAME);
    EXPECT_EQ(eventHub->GetGroupName(), GROUP_NAME);
}

/**
 * @tc.name: CheckBoxEventTest003
 * @tc.desc: Test CheckBox onChange event.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxEventTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBox node
     */
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);

    /**
     * @tc.steps: step2. Init change result and onChange function
     */
    bool isSelected = false;
    auto onChange = [&isSelected](bool select) { isSelected = select; };
    checkBoxModelNG.SetOnChange(onChange);

    /**
     * @tc.steps: step3. Get event hub and call UpdateChangeEvent function
     * @tc.expected: step3. Check the event result value
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto eventHub = frameNode->GetEventHub<NG::CheckBoxEventHub>();
    ASSERT_NE(eventHub, nullptr);
    eventHub->UpdateChangeEvent(true);
    EXPECT_EQ(isSelected, true);
}

/**
 * @tc.name: CheckBoxPatternTest004
 * @tc.desc: Test CheckBox onModifyDone.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPatternTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBox node
     */
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);

    /**
     * @tc.steps: step2. Test CheckBox onModifyDone method
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    frameNode->MarkModifyDone();
    auto pattern = frameNode->GetPattern<CheckBoxPattern>();
    ASSERT_NE(pattern, nullptr);
    pattern->SetPreGroup(GROUP_NAME);
    frameNode->MarkModifyDone();
    pattern->SetPreGroup(GROUP_NAME_CHANGE);
    frameNode->MarkModifyDone();
}

/**
 * @tc.name: CheckBoxMeasureTest005
 * @tc.desc: Test CheckBox Measure.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxMeasureTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBox node
     */
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);

    /**
     * @tc.steps: step2. Create LayoutWrapper and set checkBoxLayoutAlgorithm.
     */
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    LayoutWrapper layoutWrapper = LayoutWrapper(frameNode, geometryNode, frameNode->GetLayoutProperty());
    auto checkBoxPattern = frameNode->GetPattern<CheckBoxPattern>();
    ASSERT_NE(checkBoxPattern, nullptr);
    auto checkBoxLayoutAlgorithm = checkBoxPattern->CreateLayoutAlgorithm();
    ASSERT_NE(checkBoxLayoutAlgorithm, nullptr);
    layoutWrapper.SetLayoutAlgorithm(AceType::MakeRefPtr<LayoutAlgorithmWrapper>(checkBoxLayoutAlgorithm));

    /**
     * @tc.steps: step3. Test CheckBox Measure method
     * @tc.expected: step3. Check the CheckBox frame size and frame offset value
     */
    LayoutConstraintF layoutConstraintSize;
    layoutConstraintSize.selfIdealSize.SetSize(SizeF(WIDTH.ConvertToPx(), HEIGHT.ConvertToPx()));
    layoutWrapper.GetLayoutProperty()->UpdateLayoutConstraint(layoutConstraintSize);
    layoutWrapper.GetLayoutProperty()->UpdateContentConstraint();
    checkBoxLayoutAlgorithm->Measure(&layoutWrapper);
    // Test the size set by codes.
    EXPECT_EQ(layoutWrapper.GetGeometryNode()->GetFrameSize(), SizeF(WIDTH.ConvertToPx(), HEIGHT.ConvertToPx()));
}

/**
 * @tc.name: CheckBoxPatternTest006
 * @tc.desc: Test CheckBox pattern method OnTouchUp.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPatternTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBox node
     */
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);

    /**
     * @tc.steps: step2. Get CheckBox pattern object
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<CheckBoxPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step3. Set CheckBox pattern variable and call OnTouchUp
     * @tc.expected: step3. Check the CheckBox pattern value
     */
    pattern->isTouch_ = true;
    pattern->OnTouchUp();
    EXPECT_EQ(pattern->isTouch_, false);
}

/**
 * @tc.name: CheckBoxPatternTest007
 * @tc.desc: Test CheckBox pattern method OnTouchDown.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPatternTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBox node
     */
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);

    /**
     * @tc.steps: step2. Get CheckBox pattern object
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<CheckBoxPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step3. Set CheckBox pattern variable and call OnTouchDown
     * @tc.expected: step3. Check the CheckBox pattern value
     */
    pattern->isTouch_ = false;
    pattern->OnTouchDown();
    EXPECT_EQ(pattern->isTouch_, true);
}

/**
 * @tc.name: CheckBoxPatternTest008
 * @tc.desc: Test CheckBox pattern method OnClick.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPatternTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBox node
     */
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);

    /**
     * @tc.steps: step2. Get CheckBox pattern object
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<CheckBoxPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step3. Set CheckBox paint property variable and call OnClick
     * @tc.expected: step3. Check the CheckBox paint property value
     */
    pattern->OnClick();
    auto checkBoxPaintProperty = frameNode->GetPaintProperty<CheckBoxPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    auto select1 = checkBoxPaintProperty->GetCheckBoxSelectValue();
    EXPECT_EQ(select1, true);
    pattern->OnClick();
    auto select2 = checkBoxPaintProperty->GetCheckBoxSelectValue();
    EXPECT_EQ(select2, false);
}

/**
 * @tc.name: CheckBoxPatternTest009
 * @tc.desc: Test CheckBox pattern method HandleMouseEvent.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPatternTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBox node
     */
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);

    /**
     * @tc.steps: step2. Get CheckBox pattern object
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<CheckBoxPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step3. Set CheckBox pattern variable and call HandleMouseEvent
     * @tc.expected: step3. Check the CheckBox pattern value
     */
    pattern->isTouch_ = true;
    pattern->isHover_ = false;
    pattern->HandleMouseEvent(true);
    EXPECT_EQ(pattern->isHover_, true);
    EXPECT_EQ(pattern->isTouch_, true);
}

/**
 * @tc.name: CheckBoxPatternTest010
 * @tc.desc: Test CheckBox pattern Init methods.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPatternTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBox node
     */
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);

    /**
     * @tc.steps: step2. Get CheckBox pattern object
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<CheckBoxPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step3. Set CheckBox pattern variable and call Init methods
     * @tc.expected: step3. Check the CheckBox pattern value
     */
    // InitMouseEvent()
    pattern->InitMouseEvent();
    ASSERT_NE(pattern->mouseEvent_, nullptr);
    pattern->InitMouseEvent();
    pattern->mouseEvent_->GetOnHoverEventFunc()(true);
    // InitTouchEvent()
    pattern->InitTouchEvent();
    ASSERT_NE(pattern->touchListener_, nullptr);
    pattern->InitTouchEvent();
    TouchEventInfo info("onTouch");
    TouchLocationInfo touchInfo1(1);
    touchInfo1.SetTouchType(TouchType::DOWN);
    info.AddTouchLocationInfo(std::move(touchInfo1));
    pattern->touchListener_->GetTouchEventCallback()(info);
    TouchLocationInfo touchInfo2(2);
    touchInfo2.SetTouchType(TouchType::UP);
    info.AddTouchLocationInfo(std::move(touchInfo2));
    pattern->touchListener_->GetTouchEventCallback()(info);
    TouchLocationInfo touchInfo3(3);
    touchInfo2.SetTouchType(TouchType::CANCEL);
    info.AddTouchLocationInfo(std::move(touchInfo3));
    pattern->touchListener_->GetTouchEventCallback()(info);
    // InitClickEvent()
    pattern->InitClickEvent();
    ASSERT_NE(pattern->clickListener_, nullptr);
    pattern->InitClickEvent();
}

/**
 * @tc.name: CheckBoxPatternTest011
 * @tc.desc: Test CheckBox pattern Update methods.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPatternTest011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBox node
     */
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);

    /**
     * @tc.steps: step2. Get CheckBox pattern object
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<CheckBoxPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step3. Set CheckBox pattern variable and call Init methods
     * @tc.expected: step3. Check the CheckBox pattern value
     */
    // UpdateUIStatus(bool check)
    pattern->uiStatus_ = UIStatus::ON_TO_OFF;
    pattern->UpdateUIStatus(true);
    EXPECT_EQ(pattern->uiStatus_, UIStatus::OFF_TO_ON);
    // UpdateUnSelect()
    pattern->uiStatus_ = UIStatus::ON_TO_OFF;
    auto checkBoxPaintProperty = frameNode->GetPaintProperty<CheckBoxPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    pattern->UpdateUnSelect();
    EXPECT_EQ(pattern->uiStatus_, UIStatus::ON_TO_OFF);
    checkBoxPaintProperty->UpdateCheckBoxSelect(false);
    pattern->UpdateUnSelect();
    EXPECT_EQ(pattern->uiStatus_, UIStatus::UNSELECTED);
}

/**
 * @tc.name: CheckBoxEventHubPropertyTest012
 * @tc.desc: Set CheckBox value into CheckBoxEventHub and get it.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxEventHubPropertyTest012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBox node
     */
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, std::nullopt, TAG);

    /**
     * @tc.steps: step2. Get CheckBox event hub and get property
     * @tc.expected: step2. Check the CheckBox property value
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto eventHub = frameNode->GetEventHub<NG::CheckBoxEventHub>();
    ASSERT_NE(eventHub, nullptr);
    EXPECT_EQ(eventHub->GetName(), NAME);
    EXPECT_EQ(eventHub->GetGroupName(), "");
}

/**
 * @tc.name: CheckBoxEventHubPropertyTest013
 * @tc.desc: Set CheckBox value into CheckBoxEventHub and get it.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxEventHubPropertyTest013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init CheckBox node
     */
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(std::nullopt, GROUP_NAME, TAG);

    /**
     * @tc.steps: step2. Get CheckBox event hub and get property
     * @tc.expected: step2. Check the CheckBox property value
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto eventHub = frameNode->GetEventHub<NG::CheckBoxEventHub>();
    ASSERT_NE(eventHub, nullptr);
    EXPECT_EQ(eventHub->GetName(), "");
    EXPECT_EQ(eventHub->GetGroupName(), GROUP_NAME);
}

/**
 * @tc.name: CheckBoxPatternTest014
 * @tc.desc: Test CheckBox onModifyDone.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPatternTest014, TestSize.Level1)
{
    // create mock theme manager
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto checkboxTheme = AceType::MakeRefPtr<CheckboxTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(checkboxTheme));
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);

    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    frameNode->MarkModifyDone();
    auto pattern = frameNode->GetPattern<CheckBoxPattern>();
    ASSERT_NE(pattern, nullptr);
    pattern->SetPreGroup(GROUP_NAME);
    frameNode->MarkModifyDone();
    pattern->SetPreGroup(GROUP_NAME_CHANGE);
    frameNode->MarkModifyDone();
    ASSERT_NE(pattern, nullptr);
}

/**
 * @tc.name: CheckBoxPatternTest015
 * @tc.desc: Test UpdateAnimation.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPatternTest015, TestSize.Level1)
{
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);
    checkBoxModelNG.SetSelect(false);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    frameNode->MarkModifyDone();
    auto pattern = frameNode->GetPattern<CheckBoxPattern>();
    ASSERT_NE(pattern, nullptr);
}

/**
 * @tc.name: CheckBoxPatternTest016
 * @tc.desc: Test UpdateAnimation.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPatternTest016, TestSize.Level1)
{
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);
    checkBoxModelNG.SetSelect(true);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    frameNode->MarkModifyDone();
    auto pattern = frameNode->GetPattern<CheckBoxPattern>();
    ASSERT_NE(pattern, nullptr);
}

/**
 * @tc.name: CheckBoxPatternTest017
 * @tc.desc: Test UpdateCheckBoxGroupStatus.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPatternTest017, TestSize.Level1)
{
    CheckBoxModelNG checkBoxModelNG1;
    checkBoxModelNG1.Create(NAME, GROUP_NAME, TAG);
    auto frameNode1 = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode1, nullptr);
    auto pattern1 = frameNode1->GetPattern<CheckBoxPattern>();
    EXPECT_NE(pattern1, nullptr);
    CheckBoxModelNG checkBoxModelNG2;
    checkBoxModelNG2.Create(NAME, GROUP_NAME, TAG);
    auto frameNode2 = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode2, nullptr);
    CheckBoxModelNG checkBoxModelNG3;
    checkBoxModelNG3.Create(NAME, GROUP_NAME, TAG);
    auto frameNode3 = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode3, nullptr);
    CheckBoxGroupModelNG checkBoxGroupModelNG;
    checkBoxGroupModelNG.Create(GROUP_NAME);
    auto groupFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(groupFrameNode, nullptr);

    std::unordered_map<std::string, std::list<WeakPtr<FrameNode>>> checkBoxGroupMap;
    checkBoxGroupMap[GROUP_NAME].push_back(frameNode1);
    checkBoxGroupMap[GROUP_NAME].push_back(frameNode2);
    checkBoxGroupMap[GROUP_NAME].push_back(frameNode3);
    checkBoxGroupMap[GROUP_NAME].push_back(groupFrameNode);
    bool isSelected = true;
    pattern1->UpdateCheckBoxGroupStatus(frameNode1, checkBoxGroupMap, isSelected);
    auto checkBoxPaintProperty1 = frameNode1->GetPaintProperty<CheckBoxPaintProperty>();
    EXPECT_NE(checkBoxPaintProperty1, nullptr);
    checkBoxPaintProperty1->UpdateCheckBoxSelect(true);
    auto checkBoxPaintProperty2 = frameNode2->GetPaintProperty<CheckBoxPaintProperty>();
    EXPECT_NE(checkBoxPaintProperty2, nullptr);
    checkBoxPaintProperty2->UpdateCheckBoxSelect(false);
    pattern1->UpdateCheckBoxGroupStatus(frameNode1, checkBoxGroupMap, isSelected);
    isSelected = false;
    pattern1->UpdateCheckBoxGroupStatus(frameNode1, checkBoxGroupMap, isSelected);
    checkBoxPaintProperty2->UpdateCheckBoxSelect(true);
    auto checkBoxPaintProperty3 = frameNode3->GetPaintProperty<CheckBoxPaintProperty>();
    EXPECT_NE(checkBoxPaintProperty3, nullptr);
    checkBoxPaintProperty3->UpdateCheckBoxSelect(true);
    isSelected = true;
    pattern1->UpdateCheckBoxGroupStatus(frameNode1, checkBoxGroupMap, isSelected);
}

/**
 * @tc.name: CheckBoxPatternTest018
 * @tc.desc: Test CheckBoxGroupIsTrue.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPatternTest018, TestSize.Level1)
{
    CheckBoxModelNG checkBoxModelNG1;
    checkBoxModelNG1.Create(NAME, GROUP_NAME, TAG);
    auto frameNode1 = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    EXPECT_NE(frameNode1, nullptr);
    auto pattern1 = frameNode1->GetPattern<CheckBoxPattern>();
    EXPECT_NE(pattern1, nullptr);
    pattern1->CheckBoxGroupIsTrue();
}

/**
 * @tc.name: CheckBoxPatternTest019
 * @tc.desc: Test GetInnerFocusPaintRect.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPatternTest019, TestSize.Level1)
{
    CheckBoxModelNG checkBoxModelNG;
    checkBoxModelNG.Create(NAME, GROUP_NAME, TAG);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    EXPECT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<CheckBoxPattern>();
    EXPECT_NE(pattern, nullptr);
    RefPtr<EventHub> eventHub = AccessibilityManager::MakeRefPtr<EventHub>();
    RefPtr<FocusHub> focusHub = AccessibilityManager::MakeRefPtr<FocusHub>(eventHub, FocusType::DISABLE, false);
    pattern->InitOnKeyEvent(focusHub);
    RoundRect paintRect;
    pattern->GetInnerFocusPaintRect(paintRect);
}

/**
 * @tc.name: CheckBoxPaintMethodTest001
 * @tc.desc: Test CheckBox PaintMethod PaintCheckBox.
 * @tc.type: FUNC
 */
HWTEST_F(CheckBoxPropertyTestNg, CheckBoxPaintMethodTest001, TestSize.Level1)
{
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(CONTENT_SIZE);
    geometryNode->SetContentOffset(CONTENT_OFFSET);
    auto checkBoxPaintProperty = AceType::MakeRefPtr<CheckBoxPaintProperty>();
    ASSERT_NE(checkBoxPaintProperty, nullptr);
    PaintWrapper paintWrapper(nullptr, geometryNode, checkBoxPaintProperty);
    /**
     * @tc.case: case. CheckBoxPaintMethod's PaintCheckBox will be called.
     */
    auto checkBoxModifier =
        AceType::MakeRefPtr<CheckBoxModifier>(false, BOARD_COLOR, CHECK_COLOR, BORDER_COLOR, SHADOW_COLOR);
    CheckBoxPaintMethod checkBoxPaintMethod(checkBoxModifier);
    Testing::MockCanvas canvas;
    EXPECT_CALL(canvas, AttachBrush(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, AttachPen(_)).WillRepeatedly(ReturnRef(canvas));
    EXPECT_CALL(canvas, DrawRoundRect(_)).Times(3);
    checkBoxPaintMethod.checkboxModifier_->PaintCheckBox(canvas, CONTENT_OFFSET, CONTENT_SIZE);
}

} // namespace OHOS::Ace::NG
