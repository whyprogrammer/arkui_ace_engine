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
#define protected public
#define private public
#include "base/geometry/dimension.h"
#include "core/components/checkable/checkable_theme.h"
#include "core/components/toggle/toggle_theme.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/button/toggle_button_event_hub.h"
#include "core/components_ng/pattern/button/toggle_button_paint_property.h"
#include "core/components_ng/pattern/button/toggle_button_pattern.h"
#include "core/components_ng/pattern/checkbox/checkbox_event_hub.h"
#include "core/components_ng/pattern/checkbox/checkbox_paint_property.h"
#include "core/components_ng/pattern/checkbox/checkbox_pattern.h"
#include "core/components_ng/pattern/toggle/switch_event_hub.h"
#include "core/components_ng/pattern/toggle/switch_paint_method.h"
#include "core/components_ng/pattern/toggle/switch_paint_property.h"
#include "core/components_ng/pattern/toggle/switch_pattern.h"
#include "core/components_ng/pattern/toggle/toggle_model.h"
#include "core/components_ng/pattern/toggle/toggle_model_ng.h"
#include "core/components_ng/test/mock/rosen/mock_canvas.h"
#include "core/components_ng/test/mock/theme/mock_theme_manager.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline_ng/test/mock/mock_pipeline_base.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
constexpr bool IS_ON = true;
constexpr bool SKIP_MEASURE = true;
constexpr bool NO_SKIP_MEASURE = false;
constexpr float CONTAINER_WIDTH = 200.0f;
constexpr float CONTAINER_HEIGHT = 100.0f;
constexpr Color SELECTED_COLOR = Color(0XFFFF0000);
constexpr Color SWITCH_POINT_COLOR = Color(0XFFFFFF00);
constexpr Color BACKGROUND_COLOR = Color(0XFFFF0000);
constexpr Dimension ZERO = 0.0_px;
constexpr Dimension TOGGLE_WIDTH = 60.0_px;
constexpr Dimension TOGGLE_HEIGH = 20.0_px;
constexpr float SWITCH_WIDTH = 100.0f;
constexpr float SWITCH_HEIGHT = 50.0f;
constexpr Dimension PADDING = Dimension(5.0);
const SizeF CONTAINER_SIZE(CONTAINER_WIDTH, CONTAINER_HEIGHT);
const Alignment ALIGNMENT = Alignment::BOTTOM_RIGHT;
const std::vector<NG::ToggleType> TOGGLE_TYPE = { ToggleType::CHECKBOX, ToggleType::BUTTON, ToggleType::SWITCH };
} // namespace

class TogglePatternTestNg : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

protected:
    static PaddingPropertyF CreatePadding(Dimension length);
};

void TogglePatternTestNg::SetUpTestCase()
{
    MockPipelineBase::SetUp();
}

void TogglePatternTestNg::TearDownTestCase()
{
    MockPipelineBase::TearDown();
}

PaddingPropertyF TogglePatternTestNg::CreatePadding(Dimension length)
{
    PaddingPropertyF padding;
    padding.left = length.ConvertToPx();
    padding.right = length.ConvertToPx();
    padding.top = length.ConvertToPx();
    padding.bottom = length.ConvertToPx();
    return padding;
}

/**
 * @tc.name: TogglePatternTest001
 * @tc.desc: test the process of toggle created with checkbox.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create toggle and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[0], IS_ON);
    toggleModelNG.SetSelectedColor(SELECTED_COLOR);
    auto checkBoxFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(checkBoxFrameNode, nullptr);
    EXPECT_EQ(checkBoxFrameNode->GetTag(), V2::CHECKBOX_ETS_TAG);

    /**
     * @tc.steps: step2.get checkBox property and check whether the property value is correct.
     */
    auto pattern = AceType::DynamicCast<CheckBoxPattern>(checkBoxFrameNode->GetPattern());
    EXPECT_NE(pattern, nullptr);
    auto paintProperty = pattern->GetPaintProperty<CheckBoxPaintProperty>();
    EXPECT_NE(paintProperty, nullptr);
    EXPECT_EQ(paintProperty->GetCheckBoxSelect(), IS_ON);
    EXPECT_EQ(paintProperty->GetCheckBoxSelectedColor(), SELECTED_COLOR);
    ViewStackProcessor::GetInstance()->ClearStack();

    // update different toggle type
    for (size_t i = 0; i < TOGGLE_TYPE.size(); ++i) {
        ToggleModelNG toggleModelNG;
        toggleModelNG.Create(TOGGLE_TYPE[1], IS_ON);

        auto isOn = i % 2 == 0 ? true : false;
        toggleModelNG.Create(TOGGLE_TYPE[i], isOn);
        auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
        EXPECT_NE(frameNode, nullptr);

        auto pattern = frameNode->GetPattern();
        EXPECT_NE(pattern, nullptr);
        if (AceType::InstanceOf<CheckBoxPattern>(pattern)) {
            auto paintProperty = pattern->GetPaintProperty<CheckBoxPaintProperty>();
            EXPECT_NE(paintProperty, nullptr);
            EXPECT_EQ(paintProperty->GetCheckBoxSelect(), isOn);
        }
        if (AceType::InstanceOf<SwitchPattern>(pattern)) {
            auto paintProperty = pattern->GetPaintProperty<SwitchPaintProperty>();
            EXPECT_NE(paintProperty, nullptr);
            EXPECT_EQ(paintProperty->GetIsOn(), isOn);
        }
        if (AceType::InstanceOf<ToggleButtonPattern>(pattern)) {
            auto paintProperty = pattern->GetPaintProperty<ToggleButtonPaintProperty>();
            EXPECT_NE(paintProperty, nullptr);
            EXPECT_EQ(paintProperty->GetIsOn(), isOn);
        }
        ViewStackProcessor::GetInstance()->ClearStack();
    }
}

/**
 * @tc.name: TogglePatternTest002
 * @tc.desc: test the process of toggle created with button.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create toggle and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[1], IS_ON);
    toggleModelNG.SetSelectedColor(SELECTED_COLOR);
    auto buttonFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(buttonFrameNode, nullptr);
    EXPECT_EQ(buttonFrameNode->GetTag(), V2::TOGGLE_ETS_TAG);

    /**
     * @tc.steps: step2.get button property and check whether the property value is correct.
     */
    auto paintProperty = buttonFrameNode->GetPaintProperty<ToggleButtonPaintProperty>();
    EXPECT_NE(paintProperty, nullptr);
    EXPECT_EQ(paintProperty->GetIsOn(), IS_ON);
    EXPECT_EQ(paintProperty->GetSelectedColor(), SELECTED_COLOR);
}

/**
 * @tc.name: TogglePatternTest003
 * @tc.desc: test the process of toggle created with switch.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create toggle and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    toggleModelNG.SetSelectedColor(SELECTED_COLOR);
    toggleModelNG.SetSwitchPointColor(SWITCH_POINT_COLOR);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(switchFrameNode, nullptr);
    EXPECT_EQ(switchFrameNode->GetTag(), V2::TOGGLE_ETS_TAG);
    EXPECT_EQ(toggleModelNG.IsToggle(), false);

    /**
     * @tc.steps: step2.get switch property and check whether the property value is correct.
     */
    auto pattern = AceType::DynamicCast<SwitchPattern>(switchFrameNode->GetPattern());
    EXPECT_NE(pattern, nullptr);
    auto paintProperty = pattern->GetPaintProperty<SwitchPaintProperty>();
    EXPECT_NE(paintProperty, nullptr);
    EXPECT_EQ(paintProperty->GetIsOn(), IS_ON);
    EXPECT_EQ(paintProperty->GetSelectedColor(), SELECTED_COLOR);
    EXPECT_EQ(paintProperty->GetSwitchPointColor(), SWITCH_POINT_COLOR);
}

/**
 * @tc.name: TogglePatternTest004
 * @tc.desc: test toggle created SetWidth SetHeight SetBackgroundColor.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create toggle and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[1], IS_ON);
    toggleModelNG.SetSelectedColor(SELECTED_COLOR);
    toggleModelNG.SetWidth(TOGGLE_WIDTH);
    toggleModelNG.SetHeight(TOGGLE_HEIGH);
    toggleModelNG.SetBackgroundColor(BACKGROUND_COLOR);
    auto padding = CreatePadding(ZERO);
    toggleModelNG.SetPadding(padding);
    auto buttonFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(buttonFrameNode, nullptr);
    EXPECT_EQ(buttonFrameNode->GetTag(), V2::TOGGLE_ETS_TAG);

    /**
     * @tc.steps: step2.get button property and check whether the property value is correct.
     */
    auto paintProperty = buttonFrameNode->GetPaintProperty<ToggleButtonPaintProperty>();
    EXPECT_NE(paintProperty, nullptr);
    EXPECT_EQ(paintProperty->GetIsOn(), IS_ON);
    EXPECT_EQ(paintProperty->GetSelectedColor(), SELECTED_COLOR);
    EXPECT_EQ(paintProperty->GetBackgroundColor(), BACKGROUND_COLOR);
}

/**
 * @tc.name: TogglePatternTest005
 * @tc.desc: Test event function of toggle with checkbox.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create toggle and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[0], IS_ON);
    toggleModelNG.SetSelectedColor(SELECTED_COLOR);

    /**
     * @tc.steps: step2. set toggle event.
     * @tc.expected: step2. function is called.
     */
    bool stateChange = true;
    auto onChange = [&stateChange](bool flag) { stateChange = flag; };
    toggleModelNG.OnChange(onChange);

    /**
     * @tc.steps: step3. call the event entry function.
     * @tc.expected: step3. check whether the value is correct.
     */
    auto checkBoxFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(checkBoxFrameNode, nullptr);
    EXPECT_EQ(checkBoxFrameNode->GetTag(), V2::CHECKBOX_ETS_TAG);
    auto eventHub = checkBoxFrameNode->GetEventHub<CheckBoxEventHub>();
    EXPECT_FALSE(eventHub == nullptr);
    eventHub->UpdateChangeEvent(false);
    EXPECT_EQ(stateChange, false);

    eventHub->UpdateChangeEvent(true);
    EXPECT_EQ(stateChange, true);
}

/**
 * @tc.name: TogglePatternTest006
 * @tc.desc: Test event function of toggle with button.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create toggle and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[1], IS_ON);
    toggleModelNG.SetSelectedColor(SELECTED_COLOR);

    /**
     * @tc.steps: step2. set toggle event.
     * @tc.expected: step2. function is called.
     */
    bool stateChange = true;
    auto onChange = [&stateChange](bool flag) { stateChange = flag; };
    toggleModelNG.OnChange(onChange);

    /**
     * @tc.steps: step3. call the event entry function.
     * @tc.expected: step3. check whether the value is correct.
     */
    auto buttonFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(buttonFrameNode, nullptr);
    EXPECT_EQ(buttonFrameNode->GetTag(), V2::TOGGLE_ETS_TAG);
    auto eventHub = buttonFrameNode->GetEventHub<ToggleButtonEventHub>();
    EXPECT_FALSE(eventHub == nullptr);
    eventHub->UpdateChangeEvent(false);
    EXPECT_EQ(stateChange, false);

    eventHub->UpdateChangeEvent(true);
    EXPECT_EQ(stateChange, true);
}

/**
 * @tc.name: TogglePatternTest007
 * @tc.desc: Test event function of toggle with switch.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create toggle and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    toggleModelNG.SetSelectedColor(SELECTED_COLOR);
    toggleModelNG.SetSwitchPointColor(SWITCH_POINT_COLOR);

    /**
     * @tc.steps: step2. set toggle event.
     * @tc.expected: step2. function is called.
     */
    bool stateChange = true;
    auto onChange = [&stateChange](bool flag) { stateChange = flag; };
    toggleModelNG.OnChange(onChange);

    /**
     * @tc.steps: step3. call the event entry function.
     * @tc.expected: step3. check whether the value is correct.
     */
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(switchFrameNode, nullptr);
    EXPECT_EQ(switchFrameNode->GetTag(), V2::TOGGLE_ETS_TAG);
    auto eventHub = switchFrameNode->GetEventHub<SwitchEventHub>();
    EXPECT_NE(eventHub, nullptr);
    eventHub->UpdateChangeEvent(true);
    EXPECT_EQ(stateChange, true);
}

/**
 * @tc.name: TogglePatternTest008
 * @tc.desc: Test toggle OnModifyDone.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create switch and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(switchFrameNode, nullptr);

    /**
     * @tc.steps: step2. create switch frameNode, get switchPattern.
     * @tc.expected: step2. get switchPattern success.
     */
    auto switchPattern = switchFrameNode->GetPattern<SwitchPattern>();
    EXPECT_NE(switchPattern, nullptr);
    auto layoutProperty = switchFrameNode->GetLayoutProperty();

    // set switchTheme to themeManager before using themeManager to get switchTheme
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<SwitchTheme>()));
    switchPattern->OnModifyDone();

    // update layoutProperty and execute OnModifyDone again
    layoutProperty->UpdateAlignment(ALIGNMENT);
    MarginProperty margin;
    margin.left = CalcLength(PADDING.ConvertToPx());
    margin.right = CalcLength(PADDING.ConvertToPx());
    margin.top = CalcLength(PADDING.ConvertToPx());
    margin.bottom = CalcLength(PADDING.ConvertToPx());
    layoutProperty->UpdateMargin(margin); // GetMarginProperty
    switchPattern->isOn_ = false;
    switchPattern->OnModifyDone();
    EXPECT_EQ(ALIGNMENT, layoutProperty->GetPositionProperty()->GetAlignmentValue());
    EXPECT_EQ(layoutProperty->GetMarginProperty()->left.value(), CalcLength(PADDING.ConvertToPx()));
    EXPECT_EQ(layoutProperty->GetMarginProperty()->right.value(), CalcLength(PADDING.ConvertToPx()));
    EXPECT_EQ(layoutProperty->GetMarginProperty()->top.value(), CalcLength(PADDING.ConvertToPx()));
    EXPECT_EQ(layoutProperty->GetMarginProperty()->bottom.value(), CalcLength(PADDING.ConvertToPx()));

    auto geometryNode = switchFrameNode->GetGeometryNode();
    geometryNode->SetContentSize(SizeF(SWITCH_WIDTH, SWITCH_HEIGHT));
    auto paintProperty = switchFrameNode->GetPaintProperty<SwitchPaintProperty>();
    switchPattern->isOn_ = false;
    paintProperty->UpdateIsOn(true);
    switchPattern->OnModifyDone();
    EXPECT_EQ(switchPattern->isOn_, false);
    EXPECT_EQ(paintProperty->GetIsOnValue(), true);
    paintProperty->UpdateCurve(Curves::LINEAR);
    switchPattern->PlayTranslateAnimation(0.0f, 1.0f);
}

/**
 * @tc.name: TogglePatternTest009
 * @tc.desc: Test toggle OnDirtyLayoutWrapperSwap.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create switch and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(switchFrameNode, nullptr);
    /**
     * @tc.steps: step2.  get switchPattern and switchWrapper.
     * @tc.expected: step2. get switchPattern success.
     */
    auto switchPattern = AceType::DynamicCast<SwitchPattern>(switchFrameNode->GetPattern());
    EXPECT_NE(switchPattern, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = switchFrameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(switchFrameNode, geometryNode, layoutProperty);
    EXPECT_NE(layoutWrapper, nullptr);

    /**
     * @tc.steps: step3. call switchPattern OnDirtyLayoutWrapperSwap function, compare result.
     * @tc.expected: step3. OnDirtyLayoutWrapperSwap success and result correct.
     */
    auto switchLayoutAlgorithm = AceType::MakeRefPtr<SwitchLayoutAlgorithm>();
    RefPtr<LayoutAlgorithmWrapper> layoutAlgorithmWrapper =
        AceType::MakeRefPtr<LayoutAlgorithmWrapper>(switchLayoutAlgorithm, SKIP_MEASURE);
    layoutWrapper->SetLayoutAlgorithm(layoutAlgorithmWrapper);

    /**
     * @tc.steps: step4. call switchPattern OnDirtyLayoutWrapperSwap function, compare result.
     * @tc.expected: step4. OnDirtyLayoutWrapperSwap success and result correct.
     */
    bool first_case = switchPattern->OnDirtyLayoutWrapperSwap(layoutWrapper, true, true);
    EXPECT_FALSE(first_case);
    bool second_case = switchPattern->OnDirtyLayoutWrapperSwap(layoutWrapper, false, false);
    EXPECT_FALSE(second_case);
    layoutAlgorithmWrapper = AceType::MakeRefPtr<LayoutAlgorithmWrapper>(switchLayoutAlgorithm, NO_SKIP_MEASURE);
    layoutWrapper->SetLayoutAlgorithm(layoutAlgorithmWrapper);
    switchPattern->isOn_ = true;
    bool third_case = switchPattern->OnDirtyLayoutWrapperSwap(layoutWrapper, true, false);
    EXPECT_FALSE(third_case);
    bool forth_case = switchPattern->OnDirtyLayoutWrapperSwap(layoutWrapper, false, true);
    EXPECT_TRUE(forth_case);
    switchPattern->isOn_ = false;
    bool fifth_case = switchPattern->OnDirtyLayoutWrapperSwap(layoutWrapper, false, true);
    EXPECT_TRUE(fifth_case);
}

/**
 * @tc.name: TogglePatternTest0010
 * @tc.desc: Test toggle OnDirtyLayoutWrapperSwap.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest0010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create switch and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(switchFrameNode, nullptr);
    /**
     * @tc.steps: step2.  get switchPattern and switchWrapper.
     * @tc.expected: step2. get switchPattern success.
     */
    auto switchPattern = AceType::DynamicCast<SwitchPattern>(switchFrameNode->GetPattern());
    EXPECT_NE(switchPattern, nullptr);

    /**
     * @tc.steps: step3  execute event function.
     * @tc.expected: step2. check whether event function is executed successfully.
     */
    auto hub = switchPattern->GetEventHub<EventHub>();
    EXPECT_NE(hub, nullptr);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    EXPECT_NE(gestureHub, nullptr);
    // InitPanEvent()
    switchPattern->InitPanEvent(gestureHub);
    GestureEvent info;
    info.SetInputEventType(InputEventType::AXIS);
    switchPattern->panEvent_->actionStart_(info);
    switchPattern->panEvent_->actionUpdate_(info);
    switchPattern->panEvent_->actionEnd_(info);
    switchPattern->panEvent_->actionCancel_();
    info.SetInputEventType(InputEventType::TOUCH_SCREEN);
    switchPattern->panEvent_->actionStart_(info);
    switchPattern->panEvent_->actionUpdate_(info);
    switchPattern->panEvent_->actionEnd_(info);
    switchPattern->InitPanEvent(gestureHub);
    // InitClickEvent()
    switchPattern->InitClickEvent();
    switchPattern->InitClickEvent();
    // InitTouchEvent()
    switchPattern->InitTouchEvent();
    switchPattern->InitTouchEvent();
    TouchEventInfo touchInfo("onTouch");
    TouchLocationInfo touchInfo1(1);
    touchInfo1.SetTouchType(TouchType::DOWN);
    touchInfo.AddTouchLocationInfo(std::move(touchInfo1));
    switchPattern->touchListener_->GetTouchEventCallback()(touchInfo);
    TouchLocationInfo touchInfo2(2);
    touchInfo2.SetTouchType(TouchType::UP);
    touchInfo.AddTouchLocationInfo(std::move(touchInfo2));
    switchPattern->touchListener_->GetTouchEventCallback()(touchInfo);
    TouchLocationInfo touchInfo3(3);
    touchInfo2.SetTouchType(TouchType::CANCEL);
    touchInfo.AddTouchLocationInfo(std::move(touchInfo3));
    switchPattern->touchListener_->GetTouchEventCallback()(touchInfo);

    // InitMouseEvent()
    switchPattern->InitMouseEvent();
    EXPECT_NE(switchPattern->mouseEvent_, nullptr);
    switchPattern->InitMouseEvent();

    switchPattern->isOn_ = true;
    switchPattern->OnClick();
    switchPattern->OnTouchDown();
    switchPattern->OnTouchUp();
    switchPattern->UpdateChangeEvent();

    // execute Handle function
    switchPattern->HandleMouseEvent(true);
    info.SetMainDelta(10.0f);
    switchPattern->HandleDragUpdate(info);
    info.SetMainDelta(0.0f);
    switchPattern->HandleDragUpdate(info);

    switchPattern->HandleDragEnd();
    switchPattern->isOn_ = false;
    switchPattern->HandleDragEnd();
    // other branch
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    geometryNode->SetContentSize(SizeF(20, 10));
    EXPECT_NE(geometryNode, nullptr);
    RefPtr<LayoutProperty> layoutProperty = switchFrameNode->GetLayoutProperty();
    EXPECT_NE(layoutProperty, nullptr);
    RefPtr<LayoutWrapper> layoutWrapper =
        AceType::MakeRefPtr<LayoutWrapper>(switchFrameNode, geometryNode, layoutProperty);
    EXPECT_NE(layoutWrapper, nullptr);
    auto switchLayoutAlgorithm = AceType::MakeRefPtr<SwitchLayoutAlgorithm>();
    RefPtr<LayoutAlgorithmWrapper> layoutAlgorithmWrapper =
        AceType::MakeRefPtr<LayoutAlgorithmWrapper>(switchLayoutAlgorithm, SKIP_MEASURE);
    layoutWrapper->SetLayoutAlgorithm(layoutAlgorithmWrapper);
    EXPECT_EQ(geometryNode->GetContentSize().Width(), 20);
    switchPattern->HandleDragEnd();
    switchPattern->isOn_ = true;
    switchPattern->HandleDragEnd();
    switchPattern->controller_ = CREATE_ANIMATOR();
    switchPattern->controller_->status_ = Animator::Status::RUNNING;
    switchPattern->OnClick();
}

/**
 * @tc.name: TogglePaintTest0011
 * @tc.desc: Test toggle SetSelectedColor(undefined).
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest0011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create switch and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    std::optional<Color> selectedColor = std::optional<Color>();

    /**
     * @tc.steps: step1. test checkbox
     */
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto checkboxTheme = AceType::MakeRefPtr<CheckboxTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(checkboxTheme));

    toggleModelNG.Create(TOGGLE_TYPE[0], IS_ON);
    toggleModelNG.SetSelectedColor(selectedColor);

    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);

    auto checkboxPattern = AceType::DynamicCast<CheckBoxPattern>(frameNode->GetPattern());
    EXPECT_NE(checkboxPattern, nullptr);
    auto checkboxPaintProperty = checkboxPattern->GetPaintProperty<CheckBoxPaintProperty>();
    EXPECT_NE(checkboxPaintProperty, nullptr);
    EXPECT_EQ(checkboxPaintProperty->GetCheckBoxSelectedColor(), checkboxTheme->GetActiveColor());

    /**
     * @tc.steps: step2. test button
     */
    themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto toggleButtonTheme = AceType::MakeRefPtr<ToggleTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(toggleButtonTheme));

    toggleModelNG.Create(TOGGLE_TYPE[1], IS_ON);
    toggleModelNG.SetSelectedColor(selectedColor);

    frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);
    auto buttonPaintProperty = frameNode->GetPaintProperty<ToggleButtonPaintProperty>();
    EXPECT_NE(buttonPaintProperty, nullptr);
    EXPECT_EQ(buttonPaintProperty->GetSelectedColor(), toggleButtonTheme->GetCheckedColor());

    /**
     * @tc.steps: step3. test switch
     */
    themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto switchTheme = AceType::MakeRefPtr<SwitchTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(switchTheme));

    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    toggleModelNG.SetSelectedColor(selectedColor);

    frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(frameNode, nullptr);
    auto switchPattern = AceType::DynamicCast<SwitchPattern>(frameNode->GetPattern());
    EXPECT_NE(switchPattern, nullptr);
    auto switchPaintProperty = switchPattern->GetPaintProperty<SwitchPaintProperty>();
    EXPECT_NE(switchPaintProperty, nullptr);
    EXPECT_EQ(switchPaintProperty->GetSelectedColor(), switchTheme->GetActiveColor());
}

/**
 * @tc.name: TogglePatternTest0012
 * @tc.desc: Test toggle OnModifyDone default margin.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest0012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create switch and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(switchFrameNode, nullptr);

    /**
     * @tc.steps: step2. create switch frameNode, get switchPattern.
     * @tc.expected: step2. get switchPattern success.
     */
    auto switchPattern = switchFrameNode->GetPattern<SwitchPattern>();
    EXPECT_NE(switchPattern, nullptr);
    auto layoutProperty = switchFrameNode->GetLayoutProperty();

    // set switchTheme to themeManager before using themeManager to get switchTheme
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto switchTheme = AceType::MakeRefPtr<SwitchTheme>();
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(switchTheme));

    MarginProperty margin;
    margin.left = CalcLength(PADDING.ConvertToPx());
    layoutProperty->UpdateMargin(margin); // GetMarginProperty

    switchPattern->OnModifyDone();
    EXPECT_EQ(layoutProperty->GetMarginProperty()->left.value(), CalcLength(PADDING.ConvertToPx()));
    EXPECT_EQ(layoutProperty->GetMarginProperty()->right.value(),
        CalcLength(switchTheme->GetHotZoneHorizontalPadding().Value()));
    EXPECT_EQ(
        layoutProperty->GetMarginProperty()->top.value(), CalcLength(switchTheme->GetHotZoneVerticalPadding().Value()));
    EXPECT_EQ(layoutProperty->GetMarginProperty()->bottom.value(),
        CalcLength(switchTheme->GetHotZoneVerticalPadding().Value()));

    MarginProperty margin1;
    margin1.right = CalcLength(PADDING.ConvertToPx());
    layoutProperty->UpdateMargin(margin1); // GetMarginProperty

    switchPattern->OnModifyDone();
    EXPECT_EQ(layoutProperty->GetMarginProperty()->right.value(), CalcLength(PADDING.ConvertToPx()));
    EXPECT_EQ(layoutProperty->GetMarginProperty()->left.value(),
        CalcLength(switchTheme->GetHotZoneHorizontalPadding().Value()));
    EXPECT_EQ(
        layoutProperty->GetMarginProperty()->top.value(), CalcLength(switchTheme->GetHotZoneVerticalPadding().Value()));
    EXPECT_EQ(layoutProperty->GetMarginProperty()->bottom.value(),
        CalcLength(switchTheme->GetHotZoneVerticalPadding().Value()));
}

/**
 * @tc.name: TogglePatternTest0013
 * @tc.desc: Test Toggle pattern method HandleMouseEvent, OnTouchUp and OnTouchDown.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest0013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Init Toggle node
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);

    /**
     * @tc.steps: step2. Get Toggle pattern object
     */
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<SwitchPattern>();
    ASSERT_NE(pattern, nullptr);

    /**
     * @tc.steps: step3. Set Toggle pattern variable and call HandleMouseEvent, OnTouchUp and OnTouchDown
     * @tc.expected: step3. Check the Toggle pattern value
     */
    pattern->isTouch_ = true;
    pattern->isHover_ = false;
    pattern->HandleMouseEvent(true);
    EXPECT_EQ(pattern->touchHoverType_, TouchHoverAnimationType::HOVER);
    EXPECT_EQ(pattern->isTouch_, true);
    pattern->HandleMouseEvent(false);
    EXPECT_EQ(pattern->touchHoverType_, TouchHoverAnimationType::NONE);

    pattern->isTouch_ = true;
    pattern->isHover_ = false;
    pattern->OnTouchUp();
    EXPECT_EQ(pattern->isTouch_, false);
    EXPECT_EQ(pattern->touchHoverType_, TouchHoverAnimationType::NONE);
    pattern->isHover_ = true;
    pattern->OnTouchUp();
    EXPECT_EQ(pattern->touchHoverType_, TouchHoverAnimationType::PRESS_TO_HOVER);

    pattern->isTouch_ = false;
    pattern->isHover_ = false;
    pattern->OnTouchDown();
    EXPECT_EQ(pattern->isTouch_, true);
    EXPECT_EQ(pattern->touchHoverType_, TouchHoverAnimationType::PRESS);
    pattern->isHover_ = true;
    pattern->OnTouchDown();
    EXPECT_EQ(pattern->touchHoverType_, TouchHoverAnimationType::HOVER_TO_PRESS);
}

/**
 * @tc.name: ToggleLayoutTest001
 * @tc.desc: Test toggle layout.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, ToggleLayoutTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create switch and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(switchFrameNode, nullptr);

    /**
     * @tc.steps: step2.  get switchPattern and switchWrapper.
     * @tc.expected: step2. get switchPattern success.
     */
    auto switchPattern = AceType::DynamicCast<SwitchPattern>(switchFrameNode->GetPattern());
    EXPECT_NE(switchPattern, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    EXPECT_NE(switchFrameNode, nullptr);
    LayoutWrapper layoutWrapper = LayoutWrapper(switchFrameNode, geometryNode, switchFrameNode->GetLayoutProperty());
    auto switchLayoutAlgorithm = AceType::DynamicCast<SwitchLayoutAlgorithm>(switchPattern->CreateLayoutAlgorithm());
    EXPECT_NE(switchLayoutAlgorithm, nullptr);
    layoutWrapper.SetLayoutAlgorithm(AccessibilityManager::MakeRefPtr<LayoutAlgorithmWrapper>(switchLayoutAlgorithm));

    /**
     * @tc.steps: step3. update layoutWrapper.
     */
    // set switchTheme to themeManager before using themeManager to get switchTheme
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);

    // update switchTheme values
    auto switchTheme = AceType::MakeRefPtr<SwitchTheme>();
    switchTheme->width_ = TOGGLE_WIDTH;
    switchTheme->height_ = TOGGLE_HEIGH;
    switchTheme->hotZoneHorizontalPadding_ = ZERO;
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(switchTheme));

    LayoutConstraintF layoutConstraintSize;
    layoutConstraintSize.maxSize = CONTAINER_SIZE;
    auto switchSize = switchLayoutAlgorithm->MeasureContent(layoutConstraintSize, &layoutWrapper);
    EXPECT_EQ(switchSize->Width(), 36);

    LayoutConstraintF layoutConstraintSizeFirst;
    layoutConstraintSizeFirst.maxSize = SizeF(30, 10);

    // different branch: update frameWidth = frameHeight = 0
    auto switchSizeOne = switchLayoutAlgorithm->MeasureContent(layoutConstraintSizeFirst, &layoutWrapper);
    EXPECT_EQ(switchSizeOne->Width(), 18);

    LayoutConstraintF layoutConstraintSizeSecond;
    layoutConstraintSizeSecond.maxSize = CONTAINER_SIZE;
    // different branch: update (frameWidth / frameHeight) > ratio = 1.8
    layoutConstraintSizeSecond.selfIdealSize.SetSize(SizeF(SWITCH_WIDTH, SWITCH_HEIGHT));
    auto switchSizeSecond = switchLayoutAlgorithm->MeasureContent(layoutConstraintSizeSecond, &layoutWrapper);
    EXPECT_EQ(switchSizeSecond->Width(), 90);

    LayoutConstraintF layoutConstraintSizeThird;
    layoutConstraintSizeThird.maxSize = CONTAINER_SIZE;
    // different branch: update (frameWidth / frameHeight) < ratio = 1.8
    layoutConstraintSizeThird.selfIdealSize.SetSize(SizeF(60, 40));
    auto switchSizeThird = switchLayoutAlgorithm->MeasureContent(layoutConstraintSizeThird, &layoutWrapper);
    EXPECT_EQ(switchSizeThird->Width(), 60);

    LayoutConstraintF layoutConstraintSizeForth;
    layoutConstraintSizeForth.maxSize = CONTAINER_SIZE;
    // different branch: update (frameWidth / frameHeight) = ratio = 1.8
    layoutConstraintSizeForth.selfIdealSize.SetSize(SizeF(180, 100));
    auto switchSizeForth = switchLayoutAlgorithm->MeasureContent(layoutConstraintSizeForth, &layoutWrapper);
    EXPECT_EQ(switchSizeForth->Width(), 180);
}

/**
 * @tc.name: TogglePaintTest001
 * @tc.desc: Test toggle PaintContent.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePaintTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create switch and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    EXPECT_NE(switchFrameNode, nullptr);
    auto switchModifier = AceType::MakeRefPtr<SwitchModifier>(false, SELECTED_COLOR, 0.0f);
    SwitchPaintMethod switchPaintMethod = SwitchPaintMethod(switchModifier);

    /**
     * @tc.steps: step2. get paintWrapper
     * @tc.expected: paintWrapper is not null
     */
    RefPtr<RenderContext> renderContext;
    auto switchPaintProperty = switchFrameNode->GetPaintProperty<SwitchPaintProperty>();
    EXPECT_NE(switchPaintProperty, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    auto* paintWrapper = new PaintWrapper(renderContext, geometryNode, switchPaintProperty);
    EXPECT_NE(paintWrapper, nullptr);

    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto switchTheme = AceType::MakeRefPtr<SwitchTheme>();
    switchTheme->width_ = TOGGLE_WIDTH;
    switchTheme->height_ = TOGGLE_HEIGH;
    switchTheme->hotZoneHorizontalPadding_ = ZERO;
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(switchTheme));

    Testing::MockCanvas rsCanvas;
    EXPECT_CALL(rsCanvas, AttachBrush(_)).WillRepeatedly(ReturnRef(rsCanvas));
    EXPECT_CALL(rsCanvas, DrawRoundRect(_)).Times(AtLeast(1));
    EXPECT_CALL(rsCanvas, DrawCircle(_, _)).Times(AtLeast(1));

    auto contentSize = SizeF(100, 50);
    auto contentOffset = OffsetF(0, 0);
    switchPaintMethod.switchModifier_->PaintSwitch(rsCanvas, contentOffset, contentSize);
}

/**
 * @tc.name: TogglePaintTest002
 * @tc.desc: Test Toggle UpdateAnimatableProperty and SetBoardColor.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePaintTest002, TestSize.Level1)
{
    auto switchModifier = AceType::MakeRefPtr<SwitchModifier>(false, SELECTED_COLOR, 0.0f);
    switchModifier->hoverColor_ = Color::RED;
    switchModifier->clickEffectColor_ = Color::BLUE;
    switchModifier->touchHoverType_ = TouchHoverAnimationType::HOVER;
    switchModifier->UpdateAnimatableProperty();
    switchModifier->animateTouchHoverColor_ =
        AceType::MakeRefPtr<AnimatablePropertyColor>(LinearColor(Color::TRANSPARENT));
    switchModifier->touchHoverType_ = TouchHoverAnimationType::PRESS_TO_HOVER;
    switchModifier->UpdateAnimatableProperty();
    EXPECT_EQ(switchModifier->animateTouchHoverColor_->Get(), LinearColor(Color::RED));
    switchModifier->touchHoverType_ = TouchHoverAnimationType::NONE;
    switchModifier->UpdateAnimatableProperty();
    EXPECT_EQ(switchModifier->animateTouchHoverColor_->Get(), LinearColor(Color::RED.BlendOpacity(0)));
    switchModifier->touchHoverType_ = TouchHoverAnimationType::HOVER_TO_PRESS;
    switchModifier->UpdateAnimatableProperty();
    EXPECT_EQ(switchModifier->animateTouchHoverColor_->Get(), LinearColor(Color::BLUE));
    switchModifier->touchHoverType_ = TouchHoverAnimationType::PRESS;
    switchModifier->UpdateAnimatableProperty();
    EXPECT_EQ(switchModifier->animateTouchHoverColor_->Get(), LinearColor(Color::BLUE));
}

/**
 * @tc.name: TogglePaintTest004
 * @tc.desc: Test SwitchPaintMethod GetContentModifier UpdateContentModifier.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePaintTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create switch and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(switchFrameNode, nullptr);
    auto switchModifier = AceType::MakeRefPtr<SwitchModifier>(IS_ON, SELECTED_COLOR, 0.0f);
    SwitchPaintMethod switchPaintMethod = SwitchPaintMethod(switchModifier);
    /**
     * @tc.steps: step2. get paintWrapper
     * @tc.expected: paintWrapper is not null
     */
    auto renderContext = switchFrameNode->GetRenderContext();
    ASSERT_NE(renderContext, nullptr);
    auto geometryNode = switchFrameNode->GetGeometryNode();
    ASSERT_NE(geometryNode, nullptr);
    auto paintProperty = switchFrameNode->GetPaintProperty<SwitchPaintProperty>();
    ASSERT_NE(paintProperty, nullptr);
    auto* paintWrapper = new PaintWrapper(renderContext, geometryNode, paintProperty);
    ASSERT_NE(paintWrapper, nullptr);

    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    auto switchTheme = AceType::MakeRefPtr<SwitchTheme>();
    switchTheme->width_ = TOGGLE_WIDTH;
    switchTheme->height_ = TOGGLE_HEIGH;
    switchTheme->hotZoneHorizontalPadding_ = ZERO;
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(switchTheme));

    EXPECT_EQ(switchModifier, switchPaintMethod.GetContentModifier(paintWrapper));
    switchPaintMethod.UpdateContentModifier(paintWrapper);
    EXPECT_EQ(switchModifier->activeColor_, switchTheme->GetActiveColor());
}

/**
 * @tc.name: TogglePaintTest003
 * @tc.desc: Test SwitchPaintMethod GetSwitchWidth.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePaintTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create switch and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(switchFrameNode, nullptr);
    auto switchModifier = AceType::MakeRefPtr<SwitchModifier>(IS_ON, SELECTED_COLOR, 0.0f);
    SwitchPaintMethod switchPaintMethod = SwitchPaintMethod(switchModifier);
    
    auto switchTheme = MockPipelineBase::GetCurrent()->GetTheme<SwitchTheme>();
    ASSERT_NE(switchTheme, nullptr);
    switchTheme->height_ = TOGGLE_HEIGH;
    switchTheme->hotZoneVerticalPadding_ = ZERO;

    SizeF size(80.0f, 20.0f);
    auto switchWidth = switchModifier->GetSwitchWidth(size);
    EXPECT_EQ(switchWidth, 62.0f);
}

/**
 * @tc.name: TogglePatternTest0017
 * @tc.desc: Test toggle AddHotZoneRect RemoveLastHotZoneRect.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest0017, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create switch and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(switchFrameNode, nullptr);
    auto switchPattern = switchFrameNode->GetPattern<SwitchPattern>();
    ASSERT_NE(switchPattern, nullptr);
    auto geometryNode = switchFrameNode->GetGeometryNode();
    ASSERT_NE(geometryNode, nullptr);
    OffsetF offsetF(0.0f, 0.0f);
    SizeF sizeF(80.0f, 20.0f);
    geometryNode->SetContentOffset(offsetF);
    geometryNode->SetContentSize(sizeF);
    auto layoutProperty = switchFrameNode->GetLayoutProperty();
    ASSERT_NE(layoutProperty, nullptr);

    RefPtr<LayoutWrapper> layoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(
        switchFrameNode, geometryNode, layoutProperty);
    ASSERT_NE(layoutWrapper, nullptr);
    auto switchLayoutAlgorithm = AceType::MakeRefPtr<SwitchLayoutAlgorithm>();
    RefPtr<LayoutAlgorithmWrapper> layoutAlgorithmWrapper =
        AceType::MakeRefPtr<LayoutAlgorithmWrapper>(switchLayoutAlgorithm, false);
    layoutWrapper->SetLayoutAlgorithm(layoutAlgorithmWrapper);
    bool result = switchPattern->OnDirtyLayoutWrapperSwap(layoutWrapper, false, false);
    EXPECT_TRUE(result);
    /**
     * cover method AddHotZoneRect
    */
    switchPattern->AddHotZoneRect();
    EXPECT_EQ(switchPattern->hotZoneOffset_.GetX(), 0.0f);
    EXPECT_EQ(switchPattern->hotZoneSize_.Width(), 80.0f);
    /**
     * cover method RemoveLastHotZoneRect
    */
    switchPattern->RemoveLastHotZoneRect();
    int count = switchFrameNode->GetOrCreateGestureEventHub()->responseRegion_.size();
    for (size_t i = 0; i < count; i++)
    {
        switchPattern->RemoveLastHotZoneRect();
    }
    
    EXPECT_EQ(switchFrameNode->GetOrCreateGestureEventHub()->isResponseRegion_, false);
}

/**
 * @tc.name: TogglePatternTest0014
 * @tc.desc: Test toggle clickCallback of InitClickEvent InitMouseEvent.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest0014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create switch and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(switchFrameNode, nullptr);
    switchFrameNode->MarkModifyDone();

    auto gesture = switchFrameNode->GetOrCreateGestureEventHub();
    ASSERT_NE(gesture, nullptr);
    /**
     * fire click event
    */
    gesture->ActClick();
    /**
     * fire mouse event
    */
    auto eventHub = switchFrameNode->GetEventHub<SwitchEventHub>();
    auto inputHub = eventHub->GetOrCreateInputEventHub();
    ASSERT_NE(inputHub, nullptr);
    auto hoverEventActuator = inputHub->hoverEventActuator_;
    ASSERT_NE(hoverEventActuator, nullptr);
    auto mouseEvents = hoverEventActuator->inputEvents_;
    ASSERT_NE(mouseEvents.size(), 0);
    for (const auto& callback : mouseEvents) {
        (*callback)(false);
    }
}

/**
 * @tc.name: TogglePatternTest0015
 * @tc.desc: Test toggle clickCallback of InitOnKeyEvent.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest0015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create switch and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(switchFrameNode, nullptr);
    switchFrameNode->MarkModifyDone();

    auto eventHub = switchFrameNode->GetFocusHub();
    ASSERT_NE(eventHub, nullptr);
    /**
     * test event.action != KeyAction::DOWN
    */
    KeyEvent keyEventOne(KeyCode::KEY_A, KeyAction::UP);
    eventHub->onKeyEventInternal_(keyEventOne);
    /**
     * test event.action == KeyAction::DOWN and event.code == KeyCode::KEY_ENTER
    */
    KeyEvent keyEventTwo(KeyCode::KEY_A, KeyAction::DOWN);
    eventHub->onKeyEventInternal_(keyEventTwo);
    /**
     * test event.action == KeyAction::DOWN and event.code != KeyCode::KEY_ENTER
    */
    KeyEvent keyEventThr(KeyCode::KEY_ENTER, KeyAction::DOWN);
    eventHub->onKeyEventInternal_(keyEventThr);
}

/**
 * @tc.name: TogglePatternTest0016
 * @tc.desc: Test toggle GetInnerFocusPaintRect.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, TogglePatternTest0016, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create switch and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(switchFrameNode, nullptr);
    switchFrameNode->MarkModifyDone();

    auto eventHub = switchFrameNode->GetFocusHub();
    ASSERT_NE(eventHub, nullptr);
    RoundRect paintRect;
    eventHub->getInnerFocusRectFunc_(paintRect);
}

/**
 * @tc.name: ToggleModelTest001
 * @tc.desc: Test toggle create.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, ToggleModelTest001, TestSize.Level1)
{
    const RefPtr<FrameNode> frameParent =
    FrameNode::CreateFrameNode("parent", 0, AceType::MakeRefPtr<Pattern>(), true);
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    /**
     * @tc.steps: step1. create switch and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(switchFrameNode, nullptr);
    EXPECT_EQ(switchFrameNode->GetId(), 100);
    switchFrameNode->SetParent(frameParent);
    /**
     * Create again,cover all branches in function Create for switch
    */
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    toggleModelNG.Create(TOGGLE_TYPE[0], IS_ON);
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    toggleModelNG.Create(TOGGLE_TYPE[1], IS_ON);
}

/**
 * @tc.name: ToggleModelTest002
 * @tc.desc: Test toggle create.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, ToggleModelTest002, TestSize.Level1)
{
    const RefPtr<FrameNode> frameParent =
    FrameNode::CreateFrameNode("parent", 0, AceType::MakeRefPtr<Pattern>(), true);
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    /**
     * @tc.steps: step1. create checkbox and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[0], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(switchFrameNode, nullptr);
    EXPECT_EQ(switchFrameNode->GetId(), 100);
    switchFrameNode->SetParent(frameParent);
    /**
     * Create again,cover all branches in function Create for checkbox
    */
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    toggleModelNG.Create(TOGGLE_TYPE[0], IS_ON);
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    toggleModelNG.Create(TOGGLE_TYPE[1], IS_ON);
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    toggleModelNG.Create(TOGGLE_TYPE[0], IS_ON);
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
}

/**
 * @tc.name: ToggleModelTest003
 * @tc.desc: Test toggle create.
 * @tc.type: FUNC
 */
HWTEST_F(TogglePatternTestNg, ToggleModelTest003, TestSize.Level1)
{
    const RefPtr<FrameNode> frameParent =
    FrameNode::CreateFrameNode("parent", 0, AceType::MakeRefPtr<Pattern>(), true);
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    /**
     * @tc.steps: step1. create ToggleButton and get frameNode.
     */
    ToggleModelNG toggleModelNG;
    toggleModelNG.Create(TOGGLE_TYPE[1], IS_ON);
    auto switchFrameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(switchFrameNode, nullptr);
    EXPECT_EQ(switchFrameNode->GetId(), 100);
    switchFrameNode->SetParent(frameParent);
    /**
     * Create again,cover all branches in function Create for ToggleButton
    */
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    toggleModelNG.Create(TOGGLE_TYPE[1], IS_ON);
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    toggleModelNG.Create(TOGGLE_TYPE[0], IS_ON);
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    toggleModelNG.Create(TOGGLE_TYPE[1], IS_ON);
    ViewStackProcessor::GetInstance()->StartGetAccessRecordingFor(100);
    toggleModelNG.Create(TOGGLE_TYPE[2], IS_ON);
}
} // namespace OHOS::Ace::NG
