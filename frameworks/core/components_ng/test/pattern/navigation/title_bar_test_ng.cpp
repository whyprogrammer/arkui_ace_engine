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
#include "core/components/common/layout/constants.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/navigation/title_bar_node.h"
#include "core/components_ng/pattern/navigation/title_bar_pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::NG {
namespace {
constexpr float RET_VALUE = 0.0;
const std::string BAR_ITEM_ETS_TAG = "TitleBar";
const std::string EMPTY_TEXT = "";
const std::string TITLE_BAR_TEXT = "title";
} // namespace

class TitleBarTestNg : public testing::Test {
public:
    void InitTitleBarTestNg();
    void DestroyTitleBarObject();

    RefPtr<TitleBarNode> frameNode_;
    RefPtr<TitleBarPattern> titleBarPattern_;
    RefPtr<TitleBarAccessibilityProperty> titleBarAccessibilityProperty_;
};

void TitleBarTestNg::InitTitleBarTestNg()
{
    frameNode_ =
        AceType::MakeRefPtr<TitleBarNode>(V2::TITLE_BAR_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId());
    ASSERT_NE(frameNode_, nullptr);
    frameNode_->InitializePatternAndContext();
    titleBarPattern_ = frameNode_->GetPattern<TitleBarPattern>();
    ASSERT_NE(titleBarPattern_, nullptr);
    auto titleNode = FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<TextPattern>(); });
    frameNode_->SetTitle(titleNode);
    titleBarAccessibilityProperty_ = frameNode_->GetAccessibilityProperty<TitleBarAccessibilityProperty>();
    ASSERT_NE(titleBarAccessibilityProperty_, nullptr);
}

void TitleBarTestNg::DestroyTitleBarObject()
{
    frameNode_ = nullptr;
    titleBarPattern_ = nullptr;
    titleBarAccessibilityProperty_ = nullptr;
}

/**
 * @tc.name: TitleBarPattern001
 * @tc.desc: Test TitleBarPattern interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, TitleBarPattern001, TestSize.Level1)
{
    auto titleBarPattern = AceType::MakeRefPtr<TitleBarPattern>();
    auto ret = titleBarPattern->CreateLayoutProperty();
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: CreateLayoutAlgorithm001
 * @tc.desc: Test CreateLayoutAlgorithm interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, CreateLayoutAlgorithm001, TestSize.Level1)
{
    auto titleBarPattern = AceType::MakeRefPtr<TitleBarPattern>();
    auto ret = titleBarPattern->CreateLayoutAlgorithm();
    ASSERT_NE(ret, nullptr);
}

/**
 * @tc.name: IsAtomicNode001
 * @tc.desc: Test IsAtomicNode interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, IsAtomicNode001, TestSize.Level1)
{
    auto titleBarPattern = AceType::MakeRefPtr<TitleBarPattern>();
    auto ret = titleBarPattern->IsAtomicNode();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: IsAtomicNode002
 * @tc.desc: Test IsAtomicNode interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, IsAtomicNode002, TestSize.Level1)
{
    std::string barTag = BAR_ITEM_ETS_TAG;
    auto nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto titleBarNode = AceType::MakeRefPtr<TitleBarNode>(barTag, nodeId);
    auto ret = titleBarNode->IsAtomicNode();
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: GetTempTitleBarHeight001
 * @tc.desc: Test GetTempTitleBarHeight interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, GetTempTitleBarHeight001, TestSize.Level1)
{
    auto titleBarPattern = AceType::MakeRefPtr<TitleBarPattern>();
    auto ret = titleBarPattern->GetTempTitleBarHeight();
    EXPECT_EQ(ret, RET_VALUE);
}

/**
 * @tc.name: GetDefaultTitleBarHeight001
 * @tc.desc: Test GetDefaultTitleBarHeight interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, GetDefaultTitleBarHeight001, TestSize.Level1)
{
    auto titleBarPattern = AceType::MakeRefPtr<TitleBarPattern>();
    auto ret = titleBarPattern->GetDefaultTitleBarHeight();
    EXPECT_EQ(ret, RET_VALUE);
}

/**
 * @tc.name: GetTempTitleOffsetY001
 * @tc.desc: Test GetTempTitleOffsetY interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, GetTempTitleOffsetY001, TestSize.Level1)
{
    auto titleBarPattern = AceType::MakeRefPtr<TitleBarPattern>();
    auto ret = titleBarPattern->GetTempTitleOffsetY();
    EXPECT_EQ(ret, RET_VALUE);
}

/**
 * @tc.name: GetTempSubTitleOffsetY001
 * @tc.desc: Test GetTempSubTitleOffsetY interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, GetTempSubTitleOffsetY001, TestSize.Level1)
{
    auto titleBarPattern = AceType::MakeRefPtr<TitleBarPattern>();
    auto ret = titleBarPattern->GetTempSubTitleOffsetY();
    EXPECT_EQ(ret, RET_VALUE);
}

/**
 * @tc.name: GetMaxTitleBarHeight001
 * @tc.desc: Test GetMaxTitleBarHeight interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, GetMaxTitleBarHeight001, TestSize.Level1)
{
    auto titleBarPattern = AceType::MakeRefPtr<TitleBarPattern>();
    auto ret = titleBarPattern->GetMaxTitleBarHeight();
    EXPECT_EQ(ret, RET_VALUE);
}

/**
 * @tc.name: IsInitialTitle001
 * @tc.desc: Test IsInitialTitle interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, IsInitialTitle001, TestSize.Level1)
{
    auto titleBarPattern = AceType::MakeRefPtr<TitleBarPattern>();
    titleBarPattern->IsInitialTitle();
    EXPECT_EQ(titleBarPattern->isInitialTitle_, true);
}

/**
 * @tc.name: MarkIsInitialTitle001
 * @tc.desc: Test MarkIsInitialTitle interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, MarkIsInitialTitle001, TestSize.Level1)
{
    auto titleBarPattern = AceType::MakeRefPtr<TitleBarPattern>();
    titleBarPattern->MarkIsInitialTitle(true);
    EXPECT_EQ(titleBarPattern->isInitialTitle_, true);
}

/**
 * @tc.name: IsInitialSubtitle001
 * @tc.desc: Test IsInitialSubtitle interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, IsInitialSubtitle001, TestSize.Level1)
{
    auto titleBarPattern = AceType::MakeRefPtr<TitleBarPattern>();
    auto ret = titleBarPattern->IsInitialSubtitle();
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: MarkIsInitialSubtitle001
 * @tc.desc: Test MarkIsInitialSubtitle interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, MarkIsInitialSubtitle001, TestSize.Level1)
{
    auto titleBarPattern = AceType::MakeRefPtr<TitleBarPattern>();
    titleBarPattern->MarkIsInitialSubtitle(true);
    EXPECT_EQ(titleBarPattern->isInitialSubtitle_, true);
}

/**
 * @tc.name: SetBackButton001
 * @tc.desc: Test SetBackButton interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, SetBackButton001, TestSize.Level1)
{
    std::string barTag = BAR_ITEM_ETS_TAG;
    auto nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto titleBarNode = AceType::MakeRefPtr<TitleBarNode>(barTag, nodeId);
    titleBarNode->SetBackButton(nullptr);
    EXPECT_EQ(titleBarNode->backButton_, nullptr);
}

/**
 * @tc.name: GetBackButton001
 * @tc.desc: Test GetBackButton interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, GetBackButton001, TestSize.Level1)
{
    std::string barTag = BAR_ITEM_ETS_TAG;
    auto nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto titleBarNode = AceType::MakeRefPtr<TitleBarNode>(barTag, nodeId);
    auto ret = titleBarNode->GetBackButton();
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: SetTitle001
 * @tc.desc: Test SetTitle interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, SetTitle001, TestSize.Level1)
{
    std::string barTag = BAR_ITEM_ETS_TAG;
    auto nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto titleBarNode = AceType::MakeRefPtr<TitleBarNode>(barTag, nodeId);
    titleBarNode->SetTitle(nullptr);
    EXPECT_EQ(titleBarNode->title_, nullptr);
}

/**
 * @tc.name: GetTitle001
 * @tc.desc: Test GetTitle interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, GetTitle001, TestSize.Level1)
{
    std::string barTag = BAR_ITEM_ETS_TAG;
    auto nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto titleBarNode = AceType::MakeRefPtr<TitleBarNode>(barTag, nodeId);
    auto ret = titleBarNode->GetTitle();
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: SetSubtitle001
 * @tc.desc: Test SetSubtitle interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, SetSubtitle001, TestSize.Level1)
{
    std::string barTag = BAR_ITEM_ETS_TAG;
    auto nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto titleBarNode = AceType::MakeRefPtr<TitleBarNode>(barTag, nodeId);
    titleBarNode->SetSubtitle(nullptr);
    EXPECT_EQ(titleBarNode->subtitle_, nullptr);
}

/**
 * @tc.name: GetSubtitle001
 * @tc.desc: Test GetSubtitle interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, GetSubtitle001, TestSize.Level1)
{
    std::string barTag = BAR_ITEM_ETS_TAG;
    auto nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto titleBarNode = AceType::MakeRefPtr<TitleBarNode>(barTag, nodeId);
    auto ret = titleBarNode->GetSubtitle();
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: SetMenu001
 * @tc.desc: Test SetMenu interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, SetMenu001, TestSize.Level1)
{
    std::string barTag = BAR_ITEM_ETS_TAG;
    auto nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto titleBarNode = AceType::MakeRefPtr<TitleBarNode>(barTag, nodeId);
    titleBarNode->SetMenu(nullptr);
    EXPECT_EQ(titleBarNode->menu_, nullptr);
}

/**
 * @tc.name: GetMenu001
 * @tc.desc: Test GetMenu interface.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, GetMenu001, TestSize.Level1)
{
    std::string barTag = BAR_ITEM_ETS_TAG;
    auto nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto titleBarNode = AceType::MakeRefPtr<TitleBarNode>(barTag, nodeId);
    auto ret = titleBarNode->GetMenu();
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: GetOrCreateTitleBarNode001
 * @tc.desc: Test create title bar node.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, GetOrCreateTitleBarNode001, TestSize.Level1)
{
    std::string barTag = BAR_ITEM_ETS_TAG;
    auto nodeId = ElementRegister::GetInstance()->MakeUniqueId();
    auto barNode = AceType::MakeRefPtr<TitleBarNode>(barTag, nodeId);
    auto frameNode = FrameNode::GetFrameNode(barTag, nodeId);
    EXPECT_EQ(frameNode, nullptr);
    RefPtr<TitleBarNode> titleBarNode = barNode->GetOrCreateTitleBarNode(barTag, nodeId, nullptr);
    ASSERT_NE(titleBarNode, nullptr);
}

/**
 * @tc.name: TitleBarAccessibilityPropertyGetText001
 * @tc.desc: Test GetText of titleBar.
 * @tc.type: FUNC
 */
HWTEST_F(TitleBarTestNg, TitleBarAccessibilityPropertyGetText001, TestSize.Level1)
{
    InitTitleBarTestNg();
    EXPECT_EQ(titleBarAccessibilityProperty_->GetText(), EMPTY_TEXT);
    auto titleNode = FrameNode::GetOrCreateFrameNode(V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<TextPattern>(); });
    ASSERT_NE(titleNode, nullptr);
    auto textLayoutProperty = titleNode->GetLayoutProperty<TextLayoutProperty>();
    ASSERT_NE(textLayoutProperty, nullptr);
    textLayoutProperty->UpdateContent(TITLE_BAR_TEXT);
    frameNode_->SetTitle(titleNode);
    EXPECT_EQ(titleBarAccessibilityProperty_->GetText(), TITLE_BAR_TEXT);
    DestroyTitleBarObject();
}
} // namespace OHOS::Ace::NG
