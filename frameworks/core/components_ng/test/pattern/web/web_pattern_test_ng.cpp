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
#define private public
#include "core/components/web/resource/web_delegate.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/web/web_pattern.h"
#include "core/components_ng/pattern/web/web_view.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "frameworks/base/utils/system_properties.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
class WebPatternTestNg : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WebPatternTestNg::SetUpTestCase() {}
void WebPatternTestNg::TearDownTestCase() {}
void WebPatternTestNg::SetUp() {}
void WebPatternTestNg::TearDown() {}


/**
 * @tc.name: WebPatternTestNg_001
 * @tc.desc: Test webPattern.cpp.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTestNg, WebPatternTestNg_001, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    WebPattern webpattern;
    webpattern.delegate_ = nullptr;
    EXPECT_EQ(webpattern.delegate_, nullptr);
    webpattern.HandleFocusEvent();
    webpattern.OnWebSrcUpdate();
    webpattern.OnWebDataUpdate();
    webpattern.OnJsEnabledUpdate(true);
    webpattern.OnMediaPlayGestureAccessUpdate(true);
    webpattern.OnFileAccessEnabledUpdate(true);
    webpattern.OnOnLineImageAccessEnabledUpdate(true);
    webpattern.OnDomStorageAccessEnabledUpdate(true);
    webpattern.OnImageAccessEnabledUpdate(true);
    webpattern.OnMixedModeUpdate(MixedModeContent::MIXED_CONTENT_ALWAYS_ALLOW);
    webpattern.OnZoomAccessEnabledUpdate(true);
    webpattern.OnGeolocationAccessEnabledUpdate(true);
    webpattern.OnUserAgentUpdate("chrome");
    webpattern.OnCacheModeUpdate(WebCacheMode::DEFAULT);
    webpattern.OnWebCursiveFontUpdate("test");
    webpattern.OnWebFantasyFontUpdate("test");
    webpattern.OnWebSansSerifFontUpdate("test");
    webpattern.OnWebSerifFontUpdate("test");
    webpattern.OnWebStandardFontUpdate("test");
    webpattern.OnDefaultFixedFontSizeUpdate(0);
    webpattern.OnDefaultFontSizeUpdate(0);
    webpattern.OnMinFontSizeUpdate(0);
    webpattern.OnMinLogicalFontSizeUpdate(0);
    webpattern.OnWebFixedFontUpdate("test");
    webpattern.OnBlockNetworkUpdate(true);
    webpattern.OnDarkModeUpdate(WebDarkMode::On);
    webpattern.OnForceDarkAccessUpdate(true);
    webpattern.OnAllowWindowOpenMethodUpdate(true);
    webpattern.OnHorizontalScrollBarAccessEnabledUpdate(true);
    webpattern.OnVerticalScrollBarAccessEnabledUpdate(true);
    webpattern.OnAudioResumeIntervalUpdate(0);
    webpattern.OnAudioExclusiveUpdate(true);
#endif
}

/**
 * @tc.name: WebPatternTestNg_002
 * @tc.desc: Test webPattern.cpp.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTestNg, WebPatternTestNg_002, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto frameNode =
        FrameNode::GetOrCreateFrameNode(V2::WEB_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<WebPattern>(); });
    stack->Push(frameNode);
    auto webPattern = frameNode->GetPattern<WebPattern>();
    EXPECT_NE(webPattern, nullptr);
    webPattern->OnModifyDone();
    EXPECT_NE(webPattern->delegate_, nullptr);
    webPattern->HandleFocusEvent();
    webPattern->OnWebSrcUpdate();
    webPattern->OnWebDataUpdate();
    webPattern->isUrlLoaded_ = true;
    webPattern->OnWebSrcUpdate();
    webPattern->OnWebDataUpdate();
    webPattern->OnJsEnabledUpdate(true);
    webPattern->OnMediaPlayGestureAccessUpdate(true);
    webPattern->OnFileAccessEnabledUpdate(true);
    webPattern->OnOnLineImageAccessEnabledUpdate(true);
    webPattern->OnDomStorageAccessEnabledUpdate(true);
    webPattern->OnImageAccessEnabledUpdate(true);
    webPattern->OnMixedModeUpdate(MixedModeContent::MIXED_CONTENT_ALWAYS_ALLOW);
    webPattern->OnZoomAccessEnabledUpdate(true);
    webPattern->OnGeolocationAccessEnabledUpdate(true);
    webPattern->OnUserAgentUpdate("chrome");
    webPattern->OnCacheModeUpdate(WebCacheMode::DEFAULT);
    webPattern->OnWebCursiveFontUpdate("test");
    webPattern->OnWebFantasyFontUpdate("test");
    webPattern->OnWebSansSerifFontUpdate("test");
    webPattern->OnWebSerifFontUpdate("test");
    webPattern->OnWebStandardFontUpdate("test");
    webPattern->OnDefaultFixedFontSizeUpdate(0);
    webPattern->OnDefaultFontSizeUpdate(0);
    webPattern->OnMinFontSizeUpdate(0);
    webPattern->OnMinLogicalFontSizeUpdate(0);
    webPattern->OnWebFixedFontUpdate("test");
    webPattern->OnBlockNetworkUpdate(true);
    webPattern->OnAllowWindowOpenMethodUpdate(true);
    webPattern->OnHorizontalScrollBarAccessEnabledUpdate(true);
    webPattern->OnVerticalScrollBarAccessEnabledUpdate(true);
#endif
}

/**
 * @tc.name: WebPatternTestNg_003
 * @tc.desc: Test webPattern.cpp.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTestNg, WebPatternTestNg_003, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto frameNode =
        FrameNode::GetOrCreateFrameNode(V2::WEB_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<WebPattern>(); });
    stack->Push(frameNode);
    auto webPattern = frameNode->GetPattern<WebPattern>();
    EXPECT_NE(webPattern, nullptr);
    webPattern->OnModifyDone();
    EXPECT_NE(webPattern->delegate_, nullptr);
    GestureEvent event;
    event.SetInputEventType(InputEventType::TOUCH_SCREEN);
    webPattern->HandleDragMove(event);
    event.SetInputEventType(InputEventType::AXIS);
    webPattern->HandleDragMove(event);
    webPattern->OnDarkModeUpdate(WebDarkMode::On);
    webPattern->OnForceDarkAccessUpdate(true);
    webPattern->OnAudioResumeIntervalUpdate(0);
    webPattern->OnAudioExclusiveUpdate(true);
    webPattern->isEnhanceSurface_ = true;
    webPattern->delegate_ = nullptr;
    webPattern->OnModifyDone();
#endif
}

/**
 * @tc.name: OnScrollBarColorUpdate004
 * @tc.desc: OnScrollBarColorUpdate.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTestNg, OnScrollBarColorUpdate004, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    auto* stack = ViewStackProcessor::GetInstance();
    EXPECT_NE(stack, nullptr);
    auto nodeId = stack->ClaimNodeId();
    auto frameNode =
        FrameNode::GetOrCreateFrameNode(V2::WEB_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<WebPattern>(); });
    EXPECT_NE(frameNode, nullptr);
    stack->Push(frameNode);
    auto webPattern = frameNode->GetPattern<WebPattern>();
    EXPECT_NE(webPattern, nullptr);
    webPattern->OnModifyDone();
    EXPECT_NE(webPattern->delegate_, nullptr);
    webPattern->OnAreaChangedInner();
    webPattern->isInWindowDrag_ = true;
    webPattern->OnAreaChangedInner();
    const std::string value;
    webPattern->OnScrollBarColorUpdate(value);
    SelectOverlayInfo selectInfo;
    webPattern->RegisterSelectOverlayEvent(selectInfo);
    TouchEventInfo event("webtest");
    selectInfo.onTouchMove(event);
    webPattern->OnWindowSizeChanged(0, 0, WindowSizeChangeReason::DRAG_START);
    webPattern->OnWindowSizeChanged(0, 0, WindowSizeChangeReason::DRAG_END);
    webPattern->OnWindowSizeChanged(0, 0, WindowSizeChangeReason::DRAG_END);
    webPattern->OnWindowSizeChanged(0, 0, WindowSizeChangeReason::DRAG);
    webPattern->OnCompleteSwapWithNewSize();
    webPattern->OnResizeNotWork();
    webPattern->isInWindowDrag_ = true;
    webPattern->OnCompleteSwapWithNewSize();
    webPattern->OnResizeNotWork();
    webPattern->OnWindowSizeChanged(0, 0, WindowSizeChangeReason::DRAG);
    webPattern->isWaiting_ = true;
    webPattern->OnWindowSizeChanged(0, 0, WindowSizeChangeReason::DRAG);
    webPattern->OnCompleteSwapWithNewSize();
    webPattern->OnResizeNotWork();
    webPattern->isInWindowDrag_ = false;
    webPattern->OnCompleteSwapWithNewSize();
    webPattern->OnResizeNotWork();
#endif
}

/**
 * @tc.name: OnScrollBarColorUpdate005
 * @tc.desc: OnScrollBarColorUpdate.
 * @tc.type: FUNC
 */
HWTEST_F(WebPatternTestNg, OnScrollBarColorUpdate005, TestSize.Level1)
{
#ifdef OHOS_STANDARD_SYSTEM
    WebPattern webpattern;
    webpattern.delegate_ = nullptr;
    EXPECT_EQ(webpattern.delegate_, nullptr);
    const std::string value;
    webpattern.OnScrollBarColorUpdate(value);
    SystemProperties::SetExtSurfaceEnabled(true);
    webpattern.InitEnhanceSurfaceFlag();
    SystemProperties::SetExtSurfaceEnabled(false);
    webpattern.InitEnhanceSurfaceFlag();
#endif
}
} // namespace OHOS::Ace::NG