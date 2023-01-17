/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/menu/menu_item/menu_item_view.h"

#include "base/geometry/dimension.h"
#include "base/utils/utils.h"
#include "core/components/select/select_theme.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/flex/flex_layout_pattern.h"
#include "core/components_ng/pattern/image/image_model_ng.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/menu/menu_item/menu_item_pattern.h"
#include "core/components_ng/pattern/menu/menu_theme.h"
#include "core/components_ng/pattern/option/option_theme.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline/pipeline_base.h"

namespace OHOS::Ace::NG {
void MenuItemView::AddIcon(const std::optional<std::string>& icon, const RefPtr<FrameNode>& row)
{
    auto iconPath = icon.value_or("");
    if (!iconPath.empty()) {
        auto iconNode = FrameNode::CreateFrameNode(
            V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ImagePattern>());
        auto props = iconNode->GetLayoutProperty<ImageLayoutProperty>();
        props->UpdateImageSourceInfo(ImageSourceInfo(iconPath));
        props->UpdateImageFit(ImageFit::SCALE_DOWN);
        props->UpdateCalcMaxSize(CalcSize(ICON_SIDE_LENGTH, ICON_SIDE_LENGTH));
        props->UpdateAlignment(Alignment::CENTER);

        iconNode->MountToParent(row);
        iconNode->MarkModifyDone();
    }
}

void MenuItemView::AddContent(const std::string& content, const RefPtr<FrameNode>& row)
{
    auto contentNode = FrameNode::CreateFrameNode(
        V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
    CHECK_NULL_VOID(contentNode);
    auto contentProperty = contentNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(contentProperty);
    contentProperty->UpdateFontSize(MENU_FONT_SIZE);
    contentProperty->UpdateContent(content);

    contentNode->MountToParent(row);
    contentNode->MarkModifyDone();
}

void MenuItemView::AddLabelInfo(const std::optional<std::string>& labelInfo, const RefPtr<FrameNode>& row)
{
    auto labelStr = labelInfo.value_or("");
    if (!labelStr.empty()) {
        auto labelNode = FrameNode::CreateFrameNode(
            V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<TextPattern>());
        CHECK_NULL_VOID(labelNode);
        auto labelProperty = labelNode->GetLayoutProperty<TextLayoutProperty>();
        CHECK_NULL_VOID(labelProperty);
        labelProperty->UpdateFontSize(MENU_FONT_SIZE);
        labelProperty->UpdateContent(labelStr);
        auto pipeline = PipelineBase::GetCurrentContext();
        CHECK_NULL_VOID(pipeline);
        auto theme = pipeline->GetTheme<SelectTheme>();
        CHECK_NULL_VOID(theme);
        auto textColor = theme->GetNormalTextDisableColor();
        labelProperty->UpdateTextColor(textColor);

        labelNode->MountToParent(row);
        labelNode->MarkModifyDone();
    }
}

void MenuItemView::Create(const RefPtr<UINode>& customNode) {}

void MenuItemView::Create(const MenuItemProperties& menuItemProps)
{
    LOGI("MenuItemView::Create");
    auto* stack = ViewStackProcessor::GetInstance();
    int32_t nodeId = (stack == nullptr ? 0 : stack->ClaimNodeId());
    auto menuItem = FrameNode::GetOrCreateFrameNode(
        V2::MENU_ITEM_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<MenuItemPattern>(); });
    CHECK_NULL_VOID(menuItem);
    stack->Push(menuItem);

    // set border radius
    auto renderContext = menuItem->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    BorderRadiusProperty border;
    border.SetRadius(ROUND_RADIUS_PHONE);
    renderContext->UpdateBorderRadius(border);

    auto leftRow = FrameNode::CreateFrameNode(V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(false));
    CHECK_NULL_VOID(leftRow);
    auto leftRowLayoutProps = leftRow->GetLayoutProperty<LinearLayoutProperty>();
    CHECK_NULL_VOID(leftRowLayoutProps);
    leftRowLayoutProps->UpdateMainAxisAlign(FlexAlign::CENTER);
    leftRowLayoutProps->UpdateCrossAxisAlign(FlexAlign::CENTER);
    leftRowLayoutProps->UpdateSpace(MENU_ITEM_PADDING);

    leftRow->MountToParent(menuItem);
    AddIcon(menuItemProps.startIcon, leftRow);
    AddContent(menuItemProps.content, leftRow);
    auto rightRow = FrameNode::CreateFrameNode(V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        AceType::MakeRefPtr<LinearLayoutPattern>(false));
    CHECK_NULL_VOID(rightRow);
    auto rightRowLayoutProps = rightRow->GetLayoutProperty<LinearLayoutProperty>();
    CHECK_NULL_VOID(rightRowLayoutProps);
    rightRowLayoutProps->UpdateMainAxisAlign(FlexAlign::CENTER);
    rightRowLayoutProps->UpdateCrossAxisAlign(FlexAlign::CENTER);
    rightRowLayoutProps->UpdateSpace(MENU_ITEM_PADDING);

    rightRow->MountToParent(menuItem);
    AddLabelInfo(menuItemProps.labelInfo, rightRow);
    AddIcon(menuItemProps.endIcon, rightRow);

    auto buildFunc = menuItemProps.buildFunc;
    if (buildFunc.has_value()) {
        auto pattern = menuItem->GetPattern<MenuItemPattern>();
        CHECK_NULL_VOID(pattern);
        pattern->SetSubBuilder(buildFunc.value_or(nullptr));
    }
}

void MenuItemView::SetSelectIcon(bool isShow)
{
    auto pattern = ViewStackProcessor::GetInstance()->GetMainFrameNodePattern<MenuItemPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetSelectIcon(isShow);
}

void MenuItemView::SetOnChange(std::function<void(bool)>&& onChange)
{
    auto eventHub = ViewStackProcessor::GetInstance()->GetMainFrameNodeEventHub<MenuItemEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnChange(onChange);
}
} // namespace OHOS::Ace::NG