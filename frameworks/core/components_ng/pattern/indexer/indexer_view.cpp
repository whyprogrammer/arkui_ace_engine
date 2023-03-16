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

#include "core/components_ng/pattern/indexer/indexer_view.h"

#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/indexer/indexer_pattern.h"
#include "core/components_ng/pattern/indexer/indexer_theme.h"
#include "core/components_ng/pattern/list/list_pattern.h"
#include "core/components_ng/pattern/text/text_layout_property.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"

namespace OHOS::Ace::NG {

void IndexerView::Create(const std::vector<std::string>& arrayValue, int32_t selected)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto frameNode = FrameNode::GetOrCreateFrameNode(
        V2::INDEXER_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<IndexerPattern>(); });

    frameNode->Clean();
    int32_t indexerSize = arrayValue.size();
    for (int32_t index = 0; index < indexerSize; index++) {
        auto indexerChildNode = FrameNode::CreateFrameNode(V2::TEXT_ETS_TAG, -1, AceType::MakeRefPtr<TextPattern>());
        CHECK_NULL_VOID(indexerChildNode);
        auto textLayoutProperty = indexerChildNode->GetLayoutProperty<TextLayoutProperty>();
        CHECK_NULL_VOID(textLayoutProperty);
        textLayoutProperty->UpdateContent(arrayValue[index]);
        frameNode->AddChild(indexerChildNode);
    }
    stack->Push(frameNode);
    ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, ArrayValue, arrayValue);
    if (selected >= 0 && selected < indexerSize) {
        ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, Selected, selected);
    } else {
        ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, Selected, 0);
    }
}

void IndexerView::SetColor(const Color& color)
{
    ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, Color, color);
}

void IndexerView::SetSelectedColor(const Color& selectedColor)
{
    ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, SelectedColor, selectedColor);
}

void IndexerView::SetPopupColor(const Color& popupColor)
{
    ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, PopupColor, popupColor);
}

void IndexerView::SetSelectedBackgroundColor(const Color& selectedBackgroundColor)
{
    ACE_UPDATE_PAINT_PROPERTY(IndexerPaintProperty, SelectedBackgroundColor, selectedBackgroundColor);
}

void IndexerView::SetPopupBackground(const Color& popupBackground)
{
    ACE_UPDATE_PAINT_PROPERTY(IndexerPaintProperty, PopupBackground, popupBackground);
}

void IndexerView::SetUsingPopup(bool usingPopup)
{
    ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, UsingPopup, usingPopup);
}

void IndexerView::SetSelectedFont(const TextStyle& selectedFont)
{
    ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, SelectedFont, selectedFont);
}

void IndexerView::SetPopupFont(const TextStyle& popupFont)
{
    ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, PopupFont, popupFont);
}

void IndexerView::SetFont(const TextStyle& font)
{
    ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, Font, font);
}

void IndexerView::SetItemSize(const Dimension& itemSize)
{
    auto itemSizeValue = itemSize.Value();
    if (itemSizeValue > 0) {
        ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, ItemSize, itemSize);
    } else {
        ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, ItemSize, Dimension(INDEXER_ITEM_SIZE, DimensionUnit::VP));
    }
}

void IndexerView::SetAlignStyle(NG::AlignStyle alignStyle)
{
    ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, AlignStyle, alignStyle);
}

void IndexerView::SetSelected(int32_t selected)
{
    if (selected >= 0) {
        ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, Selected, selected);
    } else {
        ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, Selected, 0);
    }
}

void IndexerView::SetPopupPositionX(float popupPositionX)
{
    ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, PopupPositionX, popupPositionX);
}

void IndexerView::SetPopupPositionY(float popupPositionY)
{
    ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, PopupPositionY, popupPositionY);
}

void IndexerView::SetOnSelected(OnSelectedEvent&& onSelected)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<IndexerEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnSelected(std::move(onSelected));
}

void IndexerView::SetOnRequestPopupData(OnRequestPopupDataEvent&& onRequestPopupData)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<IndexerEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnRequestPopupData(std::move(onRequestPopupData));
}

void IndexerView::SetOnPopupSelected(OnPopupSelectedEvent&& onPopupSelectedEvent)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<IndexerEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnPopupSelected(std::move(onPopupSelectedEvent));
}

void IndexerView::SetPopupSelectedColor(const std::optional<Color>& popupSelectedColor)
{
    if (popupSelectedColor.has_value()) {
        ACE_UPDATE_PAINT_PROPERTY(IndexerPaintProperty, PopupSelectedColor, popupSelectedColor.value());
    } else {
        LOGW("PopupSelectedColor value is not valid");
        ACE_RESET_PAINT_PROPERTY(IndexerPaintProperty, PopupSelectedColor);
    }
}

void IndexerView::SetPopupUnselectedColor(const std::optional<Color>& popupUnselectedColor)
{
    if (popupUnselectedColor.has_value()) {
        ACE_UPDATE_PAINT_PROPERTY(IndexerPaintProperty, PopupUnselectedColor, popupUnselectedColor.value());
    } else {
        LOGW("PopupUnselectedColor value is not valid");
        ACE_RESET_PAINT_PROPERTY(IndexerPaintProperty, PopupUnselectedColor);
    }
}

void IndexerView::SetFontSize(const Dimension& fontSize)
{
    if (fontSize.IsValid()) {
        ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, FontSize, fontSize);
    } else {
        LOGW("FontSize value is not valid");
        ACE_RESET_LAYOUT_PROPERTY(IndexerLayoutProperty, FontSize);
    }
}

void IndexerView::SetFontWeight(FontWeight weight)
{
    ACE_UPDATE_LAYOUT_PROPERTY(IndexerLayoutProperty, FontWeight, weight);
}

void IndexerView::SetPopupItemBackground(const std::optional<Color>& popupItemBackground)
{
    if (popupItemBackground.has_value()) {
        ACE_UPDATE_PAINT_PROPERTY(IndexerPaintProperty, PopupItemBackground, popupItemBackground.value());
    } else {
        LOGW("PopupItemBackgroundColor value is not valid");
        ACE_RESET_PAINT_PROPERTY(IndexerPaintProperty, PopupItemBackground);
    }
}

void IndexerView::SetPopupHorizontalSpace(const Dimension& popupHorizontalSpace)
{
    auto spaceValue = popupHorizontalSpace.Value();
    if (spaceValue >= 0) {
        ACE_UPDATE_PAINT_PROPERTY(IndexerPaintProperty, PopupHorizontalSpace, popupHorizontalSpace);
    } else {
        LOGW("PopupHorizontalSpace value is not valid");
        ACE_RESET_PAINT_PROPERTY(IndexerPaintProperty, PopupHorizontalSpace);
    }
}
} // namespace OHOS::Ace::NG
