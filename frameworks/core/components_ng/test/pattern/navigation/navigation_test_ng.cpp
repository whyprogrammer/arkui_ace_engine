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
#include "base/json/json_util.h"
#include "core/components/button/button_theme.h"
#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/button/toggle_button_model_ng.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/navigation/nav_bar_layout_property.h"
#include "core/components_ng/pattern/navigation/nav_bar_node.h"
#include "core/components_ng/pattern/navigation/nav_bar_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_group_node.h"
#include "core/components_ng/pattern/navigation/navigation_layout_property.h"
#include "core/components_ng/pattern/navigation/navigation_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_view.h"
#include "core/components_ng/pattern/navigator/navigator_event_hub.h"
#include "core/components_ng/pattern/navigator/navigator_pattern.h"
#include "core/components_ng/pattern/stack/stack_layout_algorithm.h"
#include "core/components_ng/pattern/stack/stack_layout_property.h"
#include "core/components_ng/test/mock/theme/mock_theme_manager.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline_ng/test/mock/mock_pipeline_base.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::NG {
namespace {
constexpr int32_t TEST_DATA = 10;
const std::string NAVIGATION_TITLE = "NavigationTestNg";
const std::string TEST_TAG = "test";
constexpr Dimension SPLIT_WIDTH = 520.0_vp;
constexpr Dimension STACK_WIDTH = 510.0_vp;
constexpr float HEIGHT = 1000.0f;
constexpr float DEFAULT_ROOT_HEIGHT = 800.f;
constexpr float DEFAULT_ROOT_WIDTH = 480.f;
} // namespace

class NavigationTestNg : public testing::Test {
public:
    static void SetUpTestSuite();
    static void TearDownTestSuite();
    void MockPipelineContextGetTheme();
    static void RunMeasureAndLayout(RefPtr<LayoutWrapper>& layoutWrapper, float width = DEFAULT_ROOT_WIDTH);
};

void NavigationTestNg::SetUpTestSuite()
{
    MockPipelineBase::SetUp();
}

void NavigationTestNg::TearDownTestSuite()
{
    MockPipelineBase::TearDown();
}

void NavigationTestNg::RunMeasureAndLayout(RefPtr<LayoutWrapper>& layoutWrapper, float width)
{
    layoutWrapper->SetActive();
    layoutWrapper->SetRootMeasureNode();
    LayoutConstraintF LayoutConstraint;
    LayoutConstraint.parentIdealSize = { width, DEFAULT_ROOT_HEIGHT };
    LayoutConstraint.percentReference = { width, DEFAULT_ROOT_HEIGHT };
    LayoutConstraint.selfIdealSize = { width, DEFAULT_ROOT_HEIGHT };
    LayoutConstraint.maxSize = { width, DEFAULT_ROOT_HEIGHT };
    layoutWrapper->Measure(LayoutConstraint);
    layoutWrapper->Layout();
    layoutWrapper->MountToHostOnMainThread();
}

void NavigationTestNg::MockPipelineContextGetTheme()
{
    auto themeManager = AceType::MakeRefPtr<MockThemeManager>();
    MockPipelineBase::GetCurrent()->SetThemeManager(themeManager);
    EXPECT_CALL(*themeManager, GetTheme(_)).WillRepeatedly(Return(AceType::MakeRefPtr<NavigationBarTheme>()));
}

struct TestProperty {
    std::optional<bool> isOn = std::nullopt;
    std::optional<Color> selectedColor = std::nullopt;
    std::optional<Color> backgroundColor = std::nullopt;
};

/**
 * @tc.name: NavigationPatternTest001
 * @tc.desc: Test REPLACE navigator.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationPatternTest001, TestSize.Level1)
{
    NavigationView navigationView;
    navigationView.Create();
    navigationView.SetTitle("navigationView", false);
    RefPtr<FrameNode> frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    RefPtr<NavigationLayoutProperty> navigationLayoutProperty =
        frameNode->GetLayoutProperty<NavigationLayoutProperty>();
    ASSERT_NE(navigationLayoutProperty, nullptr);
}

/**
 * @tc.name: NavigationPatternTest002
 * @tc.desc: Test NavigationPattern OnModifyDone.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationPatternTest002, TestSize.Level1)
{
    NavigationView navigationView;
    navigationView.Create();
    navigationView.SetTitle("navigationView", false);
    RefPtr<FrameNode> frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    RefPtr<NavigationLayoutProperty> navigationLayoutProperty =
        frameNode->GetLayoutProperty<NavigationLayoutProperty>();
    ASSERT_NE(navigationLayoutProperty, nullptr);
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(hostNode, nullptr);
    NavigationPattern navigationPattern;
    navigationPattern.AttachToFrameNode(frameNode);
    navigationPattern.OnModifyDone();
    navigationPattern.DetachFromFrameNode(AceType::RawPtr(frameNode));
}

/**
 * @tc.name: NavigationPatternTest003
 * @tc.desc: Various situations of Test NavigationPattern OnDirtyLayoutWrapperSwap.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationPatternTest003, TestSize.Level1)
{
    NavigationView navigationView;
    navigationView.Create();
    navigationView.SetTitle("navigationView", false);
    RefPtr<FrameNode> frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    RefPtr<NavigationLayoutProperty> navigationLayoutProperty =
        frameNode->GetLayoutProperty<NavigationLayoutProperty>();
    ASSERT_NE(navigationLayoutProperty, nullptr);

    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(hostNode, nullptr);
    RefPtr<NavigationPattern> pattern = frameNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    pattern->OnModifyDone();
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    auto layoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(frameNode, geometryNode, frameNode->GetLayoutProperty());
    ASSERT_NE(layoutWrapper, nullptr);

    DirtySwapConfig config;
    config.skipMeasure = true;
    config.skipLayout = true;
    std::vector<DirtySwapConfig> configValue;
    configValue.push_back(config);
    config.skipLayout = false;
    configValue.push_back(config);
    config.skipMeasure = false;
    configValue.push_back(config);
    config.skipLayout = true;
    configValue.push_back(config);

    for (auto& iter : configValue) {
        EXPECT_EQ(pattern->OnDirtyLayoutWrapperSwap(layoutWrapper, iter), false);
    }
}

/**
 * @tc.name: NavigationPatternTest004
 * @tc.desc: Test Two level nesting of layoutWrapper.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationPatternTest004, TestSize.Level1)
{
    NavigationView navigationView;
    navigationView.Create();
    navigationView.SetTitle("navigationView", false);
    RefPtr<FrameNode> frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    RefPtr<NavigationLayoutProperty> navigationLayoutProperty =
        frameNode->GetLayoutProperty<NavigationLayoutProperty>();
    ASSERT_NE(navigationLayoutProperty, nullptr);
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(hostNode, nullptr);

    RefPtr<NavigationPattern> pattern = frameNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    pattern->OnModifyDone();
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    auto layoutWrapper = AceType::MakeRefPtr<LayoutWrapper>(frameNode, geometryNode, frameNode->GetLayoutProperty());
    ASSERT_NE(layoutWrapper, nullptr);

    DirtySwapConfig config;
    config.skipMeasure = true;
    config.skipLayout = true;
    bool result = pattern->OnDirtyLayoutWrapperSwap(layoutWrapper, config);
    EXPECT_EQ(result, false);

    auto setNavigation = AceType::MakeRefPtr<NavigationLayoutAlgorithm>();
    auto layoutAlgorithmWrapper = AceType::MakeRefPtr<LayoutAlgorithmWrapper>(setNavigation);
    layoutWrapper->SetLayoutAlgorithm(layoutAlgorithmWrapper);
    result = pattern->OnDirtyLayoutWrapperSwap(layoutWrapper, config);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: NavigationPatternTest005
 * @tc.desc: Test REPLACE navigator.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationPatternTest005, TestSize.Level1)
{
    NavigationView navigationView;
    navigationView.Create();
    navigationView.SetTitle("navigationView", false);

    RefPtr<FrameNode> frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    ASSERT_NE(frameNode, nullptr);
    navigationView.SetCustomTitle(frameNode);
    RefPtr<UINode> uiNode = nullptr;
    navigationView.SetCustomTitle(uiNode);

    RefPtr<NavigationLayoutProperty> navigationLayoutProperty =
        frameNode->GetLayoutProperty<NavigationLayoutProperty>();
    ASSERT_NE(navigationLayoutProperty, nullptr);
}

/**
 * @tc.name: NavigationTestNg001
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationTestNg001, TestSize.Level1)
{
    NavigationView navigationView;
    navigationView.Create();
    navigationView.SetTitle("NavigationTestNg", false);
    int32_t nodeId = TEST_DATA;
    auto patternCreator = AceType::MakeRefPtr<OHOS::Ace::NG::Pattern>();
    NavigationGroupNode navigationGroupNode(TEST_TAG, nodeId, patternCreator);
    auto res = navigationGroupNode.GetFrameNode(TEST_TAG, nodeId);
    EXPECT_EQ(res, nullptr);
    navigationGroupNode.GetOrCreateGroupNode(TEST_TAG, nodeId, nullptr);
}

/**
 * @tc.name: NavigationTestNg002
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationTestNg002, TestSize.Level1)
{
    NavigationView navigationView;
    navigationView.Create();
    navigationView.SetTitle("NavigationTestNg", false);
    int32_t nodeId = TEST_DATA;
    auto patternCreator = AceType::MakeRefPtr<OHOS::Ace::NG::Pattern>();
    NavigationGroupNode navigationGroupNode(TEST_TAG, nodeId, patternCreator);
    std::unique_ptr<JsonValue> json = JsonUtil::Create(true);
    json->isRoot_ = true;
    ASSERT_NE(json, nullptr);
    RefPtr<NavBarNode> navBarNode = AceType::MakeRefPtr<OHOS::Ace::NG::NavBarNode>(TEST_TAG, nodeId, patternCreator);
    navigationGroupNode.SetNavBarNode(navBarNode);
    ASSERT_NE(AceType::DynamicCast<OHOS::Ace::NG::NavBarNode>(navigationGroupNode.GetNavBarNode()), nullptr);
    navigationGroupNode.ToJsonValue(json);
}

/**
 * @tc.name: NavigationTestNg003
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationTestNg003, TestSize.Level1)
{
    NavigationView navigationView;
    navigationView.Create();
    navigationView.SetTitle("NavigationTestNg", false);
    int32_t nodeId = TEST_DATA;
    auto patternCreator = AceType::MakeRefPtr<OHOS::Ace::NG::Pattern>();
    NavigationGroupNode navigationGroupNode(TEST_TAG, nodeId, patternCreator);
    RefPtr<FrameNode> frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->Finish());
    RefPtr<NavigationPattern> pattern = frameNode->GetPattern<NavigationPattern>();
    navigationGroupNode.pattern_ = pattern;
    ASSERT_NE(AceType::DynamicCast<NavigationPattern>(navigationGroupNode.GetPattern()), nullptr);
    RefPtr<NavBarNode> navBarNode = AceType::MakeRefPtr<OHOS::Ace::NG::NavBarNode>(TEST_TAG, nodeId, patternCreator);
    navigationGroupNode.SetNavBarNode(navBarNode);
    auto navBar = AceType::DynamicCast<OHOS::Ace::NG::NavBarNode>(navigationGroupNode.GetNavBarNode());
    navBar->SetNavBarContentNode(navBarNode);
    auto contentNode = navBar->GetNavBarContentNode();
    ASSERT_NE(navBar, nullptr);
    auto child = NavBarNode::GetOrCreateNavBarNode(
        V2::NAVBAR_ETS_TAG, TEST_DATA, []() { return AceType::MakeRefPtr<OHOS::Ace::NG::NavBarPattern>(); });
    ASSERT_NE(child, nullptr);
    navigationGroupNode.AddChildToGroup(child);
}

/**
 * @tc.name: NavigationViewTest001
 * @tc.desc: Test NavigationView SetTitleMode.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationViewTest001, TestSize.Level1)
{
    NavigationView navigationView;
    navigationView.Create();
    navigationView.SetTitle("navigationView", false);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    EXPECT_NE(frameNode, nullptr);
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    EXPECT_NE(navigationGroupNode, nullptr);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    EXPECT_NE(navBarNode, nullptr);
    auto navBarLayoutProperty = navBarNode->GetLayoutProperty<NavBarLayoutProperty>();
    EXPECT_NE(navBarLayoutProperty, nullptr);
    navigationView.SetTitleMode(NavigationTitleMode::FREE);
    navigationView.SetTitleMode(NavigationTitleMode::MINI);
    bool result = true;
    if (navBarLayoutProperty->GetTitleModeString() != "NavigationTitleMode.Mini") {
        result = false;
    }
    EXPECT_EQ(result, true);
}

/**
 * @tc.name: NavigationViewTest002
 * @tc.desc: Test NavigationView SetHideTitleBar false.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationViewTest002, TestSize.Level1)
{
    NavigationView navigationView;
    navigationView.Create();
    navigationView.SetTitle("navigationView", false);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    EXPECT_NE(frameNode, nullptr);
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    EXPECT_NE(navigationGroupNode, nullptr);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    EXPECT_NE(navBarNode, nullptr);
    auto navBarLayoutProperty = navBarNode->GetLayoutProperty<NavBarLayoutProperty>();
    EXPECT_NE(navBarLayoutProperty, nullptr);
    navigationView.SetHideTitleBar(false);
    EXPECT_EQ(navBarLayoutProperty->GetHideTitleBar().value_or(false), false);
}

/**
 * @tc.name: NavigationViewTest003
 * @tc.desc: Test NavigationView SetHideToolBar false.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationViewTest003, TestSize.Level1)
{
    NavigationView navigationView;
    navigationView.Create();
    navigationView.SetTitle("navigationView", false);
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    EXPECT_NE(frameNode, nullptr);
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    EXPECT_NE(navigationGroupNode, nullptr);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    EXPECT_NE(navBarNode, nullptr);
    auto navBarLayoutProperty = navBarNode->GetLayoutProperty<NavBarLayoutProperty>();
    EXPECT_NE(navBarLayoutProperty, nullptr);
    navigationView.SetHideToolBar(false);
    EXPECT_EQ(navBarLayoutProperty->GetHideToolBar().value_or(false), false);
}

/**
 * @tc.name: NavigationViewTest004
 * @tc.desc: Test NavigationView SetTitle & SetSubTitle.
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationViewTest004, TestSize.Level1)
{
    MockPipelineContextGetTheme();
    NavigationView navigationView;
    navigationView.Create();
    navigationView.SetTitle("navigationView", true);
    navigationView.SetSubtitle("subtitle");
    auto frameNode = AceType::DynamicCast<FrameNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    ASSERT_NE(frameNode, nullptr);
    auto navigationGroupNode = AceType::DynamicCast<NavigationGroupNode>(frameNode);
    ASSERT_NE(navigationGroupNode, nullptr);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
    ASSERT_NE(navBarNode, nullptr);
    auto titleNode = navBarNode->GetTitle();
    ASSERT_NE(titleNode, nullptr);
    auto subTitleNode = navBarNode->GetSubtitle();
    ASSERT_NE(subTitleNode, nullptr);
    navigationView.SetTitle("navigationView", false);
    auto newSubTitleNode = navBarNode->GetSubtitle();
    ASSERT_EQ(newSubTitleNode, nullptr);
}

/**
 * @tc.name: NavigationPatternTest_006
 * @tc.desc: Test Navigation DoAnimation
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationPatternTest_006, TestSize.Level1)
{
    NavigationView::Create();
    NavigationView::SetUsrNavigationMode(NavigationMode::SPLIT);
    RefPtr<FrameNode> frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    RefPtr<NavigationLayoutProperty> navigationLayoutProperty =
        frameNode->GetLayoutProperty<NavigationLayoutProperty>();
    ASSERT_NE(navigationLayoutProperty, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(SizeF(static_cast<float>(STACK_WIDTH.ConvertToPx()), HEIGHT));
    geometryNode->SetContentOffset(OffsetF(0, 0));

    // init SetUsrNavigationMode:SPLIT, expect navigationMode_:SPLIT after OnModifyDone
    RefPtr<NavigationPattern> pattern = frameNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    pattern->OnModifyDone();
    auto layoutWrapper = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::SPLIT);

    // SetUsrNavigationMode:STACK, expect navigationMode_:STACK after OnModifyDone
    NavigationView::SetUsrNavigationMode(NavigationMode::STACK);
    pattern->OnModifyDone();
    auto layoutWrapper1 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper1, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper1);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::STACK);

    // SetUsrNavigationMode:SPLIT, expect navigationMode_:SPLIT after OnModifyDone
    navigationLayoutProperty->UpdateUsrNavigationMode(NavigationMode::SPLIT);
    pattern->OnModifyDone();
    auto layoutWrapper2 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper2, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper2);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::SPLIT);

    // SetUsrNavigationMode:AUTO, expect navigationMode_:STACK in SPLIT_WIDTH case after OnModifyDone
    navigationLayoutProperty->UpdateUsrNavigationMode(NavigationMode::AUTO);
    pattern->OnModifyDone();
    auto layoutWrapper3 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper3, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper3);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::STACK);

    // SetUsrNavigationMode:AUTO, expect navigationMode_:SPILT in SPLIT_WIDTH case after OnModifyDone
    navigationLayoutProperty->UpdateUsrNavigationMode(NavigationMode::AUTO);
    pattern->OnModifyDone();
    auto layoutWrapper4 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper4, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper4, static_cast<float>(SPLIT_WIDTH.ConvertToPx()));
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::SPLIT);
}

/**
 * @tc.name: NavigationPatternTest_007
 * @tc.desc: Test Navigation DoAnimation
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationPatternTest_007, TestSize.Level1)
{
    NavigationView::Create();
    NavigationView::SetUsrNavigationMode(NavigationMode::STACK);
    RefPtr<FrameNode> frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    RefPtr<NavigationLayoutProperty> navigationLayoutProperty =
        frameNode->GetLayoutProperty<NavigationLayoutProperty>();
    ASSERT_NE(navigationLayoutProperty, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(SizeF(static_cast<float>(STACK_WIDTH.ConvertToPx()), HEIGHT));
    geometryNode->SetContentOffset(OffsetF(0, 0));

    // init SetUsrNavigationMode:STACK, expect navigationMode_:STACK after OnModifyDone
    RefPtr<NavigationPattern> pattern = frameNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    pattern->OnModifyDone();
    auto layoutWrapper = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::STACK);

    // SetUsrNavigationMode:SPLIT, expect navigationMode_:SPLIT after OnModifyDone
    NavigationView::SetUsrNavigationMode(NavigationMode::SPLIT);
    pattern->OnModifyDone();
    auto layoutWrapper1 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper1, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper1);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::SPLIT);

    // SetUsrNavigationMode:STACK, expect navigationMode_:STACK after OnModifyDone
    navigationLayoutProperty->UpdateUsrNavigationMode(NavigationMode::STACK);
    pattern->OnModifyDone();
    auto layoutWrapper2 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper2, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper2);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::STACK);

    // SetUsrNavigationMode:AUTO, expect navigationMode_:STACK in SPLIT_WIDTH case after OnModifyDone
    navigationLayoutProperty->UpdateUsrNavigationMode(NavigationMode::AUTO);
    pattern->OnModifyDone();
    auto layoutWrapper3 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper3, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper3);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::STACK);

    // SetUsrNavigationMode:AUTO, expect navigationMode_:SPILT in SPLIT_WIDTH case after OnModifyDone
    navigationLayoutProperty->UpdateUsrNavigationMode(NavigationMode::AUTO);
    pattern->OnModifyDone();
    auto layoutWrapper4 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper4, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper4, static_cast<float>(SPLIT_WIDTH.ConvertToPx()));
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::SPLIT);
}

/**
 * @tc.name: NavigationPatternTest_008
 * @tc.desc: Test Navigation DoAnimation
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationPatternTest_008, TestSize.Level1)
{
    NavigationView::Create();
    NavigationView::SetUsrNavigationMode(NavigationMode::AUTO);
    RefPtr<FrameNode> frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    RefPtr<NavigationLayoutProperty> navigationLayoutProperty =
        frameNode->GetLayoutProperty<NavigationLayoutProperty>();
    ASSERT_NE(navigationLayoutProperty, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(SizeF(static_cast<float>(STACK_WIDTH.ConvertToPx()), HEIGHT));
    geometryNode->SetContentOffset(OffsetF(0, 0));

    // init SetUsrNavigationMode:AUTO, expect navigationMode_:STACK after OnModifyDone
    RefPtr<NavigationPattern> pattern = frameNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    pattern->OnModifyDone();
    auto layoutWrapper = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::STACK);

    // SetUsrNavigationMode:SPLIT, expect navigationMode_:SPLIT after OnModifyDone
    NavigationView::SetUsrNavigationMode(NavigationMode::SPLIT);
    pattern->OnModifyDone();
    auto layoutWrapper1 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper1, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper1);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::SPLIT);

    // SetUsrNavigationMode:STACK, expect navigationMode_:STACK after OnModifyDone
    navigationLayoutProperty->UpdateUsrNavigationMode(NavigationMode::STACK);
    pattern->OnModifyDone();
    auto layoutWrapper2 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper2, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper2);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::STACK);

    // SetUsrNavigationMode:AUTO, expect navigationMode_:STACK in SPLIT_WIDTH case after OnModifyDone
    navigationLayoutProperty->UpdateUsrNavigationMode(NavigationMode::AUTO);
    pattern->OnModifyDone();
    auto layoutWrapper3 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper3, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper3);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::STACK);

    // SetUsrNavigationMode:AUTO, expect navigationMode_:SPILT in SPLIT_WIDTH case after OnModifyDone
    navigationLayoutProperty->UpdateUsrNavigationMode(NavigationMode::AUTO);
    pattern->OnModifyDone();
    auto layoutWrapper4 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper4, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper4, static_cast<float>(SPLIT_WIDTH.ConvertToPx()));
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::SPLIT);
}

/**
 * @tc.name: NavigationPatternTest_009
 * @tc.desc: Test Navigation DoAnimation
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationPatternTest_009, TestSize.Level1)
{
    NavigationView::Create();
    NavigationView::SetUsrNavigationMode(NavigationMode::AUTO);
    RefPtr<FrameNode> frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    RefPtr<NavigationLayoutProperty> navigationLayoutProperty =
        frameNode->GetLayoutProperty<NavigationLayoutProperty>();
    ASSERT_NE(navigationLayoutProperty, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    geometryNode->SetContentSize(SizeF(static_cast<float>(STACK_WIDTH.ConvertToPx()), HEIGHT));
    geometryNode->SetContentOffset(OffsetF(0, 0));

    // init SetUsrNavigationMode:AUTO, expect navigationMode_:STACK after OnModifyDone
    RefPtr<NavigationPattern> pattern = frameNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    pattern->OnModifyDone();
    auto layoutWrapper = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::STACK);

    // SetUsrNavigationMode:STACK, expect navigationMode_:STACK after OnModifyDone
    NavigationView::SetUsrNavigationMode(NavigationMode::STACK);
    pattern->OnModifyDone();
    auto layoutWrapper1 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper1, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper1);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::STACK);

    // SetUsrNavigationMode:SPLIT, expect navigationMode_:SPLIT after OnModifyDone
    navigationLayoutProperty->UpdateUsrNavigationMode(NavigationMode::SPLIT);
    pattern->OnModifyDone();
    auto layoutWrapper2 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper2, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper2);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::SPLIT);

    // SetUsrNavigationMode:AUTO, expect navigationMode_:STACK in SPLIT_WIDTH case after OnModifyDone
    navigationLayoutProperty->UpdateUsrNavigationMode(NavigationMode::AUTO);
    pattern->OnModifyDone();
    auto layoutWrapper3 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper3, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper3);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::STACK);

    // SetUsrNavigationMode:AUTO, expect navigationMode_:SPILT in SPLIT_WIDTH case after OnModifyDone
    navigationLayoutProperty->UpdateUsrNavigationMode(NavigationMode::AUTO);
    pattern->OnModifyDone();
    auto layoutWrapper4 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper4, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper4, static_cast<float>(SPLIT_WIDTH.ConvertToPx()));
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::SPLIT);
}

/**
 * @tc.name: NavigationLayoutTest_001
 * @tc.desc: Test Navigation Measure and Layout
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationLayoutTest_001, TestSize.Level1)
{
    NavigationView::Create();
    NavigationView::SetUsrNavigationMode(NavigationMode::SPLIT);
    NavigationView::SetNavBarPosition(NavBarPosition::START);
    RefPtr<FrameNode> frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    RefPtr<NavigationLayoutProperty> navigationLayoutProperty =
        frameNode->GetLayoutProperty<NavigationLayoutProperty>();
    ASSERT_NE(navigationLayoutProperty, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    RefPtr<NavigationPattern> pattern = frameNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto layoutWrapper = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper, nullptr);
    SizeF value(static_cast<float>(SPLIT_WIDTH.ConvertToPx()), HEIGHT);
    navigationLayoutProperty->UpdateMarginSelfIdealSize(value);
    navigationLayoutProperty->UpdateContentConstraint();

    NavigationTestNg::RunMeasureAndLayout(layoutWrapper);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::SPLIT);

    // SetUsrNavigationMode:STACK, expect navigationMode_:STACK
    NavigationView::SetUsrNavigationMode(NavigationMode::STACK);
    auto layoutWrapper1 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper1, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper1);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::STACK);

    // SetUsrNavigationMode:SPLIT, expect navigationMode_:SPLIT
    navigationLayoutProperty->UpdateUsrNavigationMode(NavigationMode::SPLIT);
    auto layoutWrapper2 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper2, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper2);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::SPLIT);
}

/**
 * @tc.name: NavigationLayoutTest_002
 * @tc.desc: Test Navigation Measure and Layout
 * @tc.type: FUNC
 */
HWTEST_F(NavigationTestNg, NavigationLayoutTest_002, TestSize.Level1)
{
    NavigationView::Create();
    NavigationView::SetUsrNavigationMode(NavigationMode::STACK);
    NavigationView::SetNavBarPosition(NavBarPosition::START);
    RefPtr<FrameNode> frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    ASSERT_NE(frameNode, nullptr);
    RefPtr<NavigationLayoutProperty> navigationLayoutProperty =
        frameNode->GetLayoutProperty<NavigationLayoutProperty>();
    ASSERT_NE(navigationLayoutProperty, nullptr);
    RefPtr<GeometryNode> geometryNode = AceType::MakeRefPtr<GeometryNode>();
    ASSERT_NE(geometryNode, nullptr);
    RefPtr<NavigationPattern> pattern = frameNode->GetPattern<NavigationPattern>();
    ASSERT_NE(pattern, nullptr);
    auto layoutWrapper = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper, nullptr);
    SizeF value(static_cast<float>(SPLIT_WIDTH.ConvertToPx()), HEIGHT);
    navigationLayoutProperty->UpdateMarginSelfIdealSize(value);
    navigationLayoutProperty->UpdateContentConstraint();

    NavigationTestNg::RunMeasureAndLayout(layoutWrapper);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::STACK);

    // SetUsrNavigationMode:SPLIT, expect navigationMode_:SPLIT
    NavigationView::SetUsrNavigationMode(NavigationMode::SPLIT);
    auto layoutWrapper1 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper1, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper1);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::SPLIT);

    // SetUsrNavigationMode:STACK, expect navigationMode_:STACK
    navigationLayoutProperty->UpdateUsrNavigationMode(NavigationMode::STACK);
    auto layoutWrapper2 = frameNode->CreateLayoutWrapper();
    ASSERT_NE(layoutWrapper2, nullptr);
    NavigationTestNg::RunMeasureAndLayout(layoutWrapper2);
    EXPECT_EQ(pattern->navigationMode_, NavigationMode::STACK);
}
} // namespace OHOS::Ace::NG
