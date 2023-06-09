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

#include "core/components_ng/pattern/select_overlay/select_overlay_node.h"

#include <cstdint>
#include <functional>
#include <optional>

#include "base/geometry/dimension.h"
#include "base/geometry/ng/offset_t.h"
#include "base/i18n/localization.h"
#include "base/utils/utils.h"
#include "core/animation/curves.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/shadow_config.h"
#include "core/components/custom_paint/rosen_render_custom_paint.h"
#include "core/components/text_overlay/text_overlay_theme.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/button/button_pattern.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/menu/menu_pattern.h"
#include "core/components_ng/pattern/menu/menu_view.h"
#include "core/components_ng/pattern/select_overlay/select_overlay_pattern.h"
#include "core/components_ng/pattern/select_overlay/select_overlay_property.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/property/calc_length.h"
#include "core/components_ng/property/property.h"
#include "core/gestures/gesture_info.h"
#include "core/pipeline/base/element_register.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
namespace {
constexpr char BUTTON_COPY_ALL[] = "textoverlay.select_all";
constexpr char BUTTON_CUT[] = "textoverlay.cut";
constexpr char BUTTON_COPY[] = "textoverlay.copy";
constexpr char BUTTON_PASTE[] = "textoverlay.paste";

constexpr int32_t OPTION_INDEX_CUT = 0;
constexpr int32_t OPTION_INDEX_COPY = 1;
constexpr int32_t OPTION_INDEX_PASTE = 2;
constexpr int32_t OPTION_INDEX_COPY_ALL = 3;
constexpr int32_t ANIMATION_DURATION1 = 350;
constexpr int32_t ANIMATION_DURATION2 = 150;

constexpr Dimension MORE_MENU_TRANSLATE = -7.5_vp;
constexpr Dimension MORE_MENU_INTERVAL = 8.0_vp;
constexpr Dimension MAX_DIAMETER = 3.5_vp;
constexpr Dimension MIN_DIAMETER = 1.5_vp;
constexpr Dimension MIN_ARROWHEAD_DIAMETER = 2.0_vp;
constexpr Dimension ANIMATION_TEXT_OFFSET = 12.0_vp;

RefPtr<FrameNode> BuildButton(const std::string& data, const std::function<void()>& callback, int32_t overlayId,
    float width, bool isSelectAll = false)
{
    auto button = FrameNode::GetOrCreateFrameNode("SelectMenuButton", ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ButtonPattern>(); });
    auto text = FrameNode::GetOrCreateFrameNode("SelectMenuButtonText", ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<TextPattern>(); });
    auto textLayoutProperty = text->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_RETURN(textLayoutProperty, button);
    textLayoutProperty->UpdateContent(data);
    text->MountToParent(button);
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, button);
    auto textOverlayTheme = pipeline->GetTheme<TextOverlayTheme>();
    CHECK_NULL_RETURN(textOverlayTheme, button);
    auto textStyle = textOverlayTheme->GetMenuButtonTextStyle();
    textLayoutProperty->UpdateFontSize(textStyle.GetFontSize());
    textLayoutProperty->UpdateTextColor(textStyle.GetTextColor());
    textLayoutProperty->UpdateFontWeight(textStyle.GetFontWeight());
    text->MarkModifyDone();

    auto buttonLayoutProperty = button->GetLayoutProperty<ButtonLayoutProperty>();
    CHECK_NULL_RETURN(buttonLayoutProperty, button);
    const auto& padding = textOverlayTheme->GetMenuButtonPadding();
    auto left = CalcLength(padding.Left().ConvertToPx());
    auto right = CalcLength(padding.Right().ConvertToPx());
    auto top = CalcLength(padding.Top().ConvertToPx());
    auto bottom = CalcLength(padding.Bottom().ConvertToPx());
    buttonLayoutProperty->UpdatePadding({ left, right, top, bottom });
    auto buttonWidth = width + padding.Left().ConvertToPx() + padding.Right().ConvertToPx();
    buttonLayoutProperty->UpdateUserDefinedIdealSize(
        { CalcLength(buttonWidth), CalcLength(textOverlayTheme->GetMenuButtonHeight()) });
    buttonLayoutProperty->UpdateFlexShrink(0);
    button->GetRenderContext()->UpdateBackgroundColor(Color::TRANSPARENT);

    button->GetOrCreateGestureEventHub()->SetUserOnClick([callback, overlayId, isSelectAll](GestureEvent& /*info*/) {
        if (callback) {
            callback();
        }
        // close text overlay.
        auto pipeline = PipelineContext::GetCurrentContext();
        CHECK_NULL_VOID(pipeline);
        auto overlayManager = pipeline->GetSelectOverlayManager();
        CHECK_NULL_VOID(overlayManager);
        if (!isSelectAll) {
            overlayManager->DestroySelectOverlay(overlayId);
        }
    });
    button->MarkModifyDone();
    return button;
}

RefPtr<FrameNode> BuildButton(
    const std::string& data, const std::function<void(std::string&)>& callback, int32_t overlayId, float& contentWidth)
{
    auto button = FrameNode::GetOrCreateFrameNode("SelectMenuButton", ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<ButtonPattern>(); });
    auto text = FrameNode::GetOrCreateFrameNode("SelectMenuButtonText", ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<TextPattern>(); });

    // Update text property and mount to button.
    auto textLayoutProperty = text->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_RETURN(textLayoutProperty, button);
    textLayoutProperty->UpdateContent(data);
    text->MountToParent(button);
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, button);
    auto textOverlayTheme = pipeline->GetTheme<TextOverlayTheme>();
    CHECK_NULL_RETURN(textOverlayTheme, button);
    auto textStyle = textOverlayTheme->GetMenuButtonTextStyle();
    textLayoutProperty->UpdateFontSize(textStyle.GetFontSize());
    textLayoutProperty->UpdateTextColor(textStyle.GetTextColor());
    textLayoutProperty->UpdateFontWeight(textStyle.GetFontWeight());
    text->MarkModifyDone();

    // Calculate the width of entension option include button padding.
    MeasureContext content;
    content.textContent = data;
    content.fontSize = textStyle.GetFontSize();
#ifdef ENABLE_ROSEN_BACKEND
    contentWidth = static_cast<float>(RosenRenderCustomPaint::MeasureTextSizeInner(content).Width());
#else
    contentWidth = 0.0;
#endif
    const auto& padding = textOverlayTheme->GetMenuButtonPadding();
    auto left = CalcLength(padding.Left().ConvertToPx());
    auto right = CalcLength(padding.Right().ConvertToPx());
    auto top = CalcLength(padding.Top().ConvertToPx());
    auto bottom = CalcLength(padding.Bottom().ConvertToPx());
    contentWidth = contentWidth + padding.Left().ConvertToPx() + padding.Right().ConvertToPx();

    // Update button property.
    auto buttonLayoutProperty = button->GetLayoutProperty<ButtonLayoutProperty>();
    CHECK_NULL_RETURN(buttonLayoutProperty, button);
    buttonLayoutProperty->UpdatePadding({ left, right, top, bottom });
    buttonLayoutProperty->UpdateUserDefinedIdealSize(
        { CalcLength(contentWidth), CalcLength(textOverlayTheme->GetMenuButtonHeight()) });
    buttonLayoutProperty->UpdateFlexShrink(0);
    button->GetRenderContext()->UpdateBackgroundColor(Color::TRANSPARENT);
    button->GetOrCreateGestureEventHub()->SetUserOnClick([callback, overlayId](GestureEvent& /*info*/) {
        auto pipeline = PipelineContext::GetCurrentContext();
        CHECK_NULL_VOID(pipeline);
        auto overlayManager = pipeline->GetSelectOverlayManager();
        CHECK_NULL_VOID(overlayManager);

        auto selectOverlay = overlayManager->GetSelectOverlayNode(overlayId);
        CHECK_NULL_VOID(selectOverlay);
        auto pattern = selectOverlay->GetPattern<SelectOverlayPattern>();
        auto selectInfo = pattern->GetSelectInfo();
        if (callback) {
            callback(selectInfo);
        }
        // close text overlay.
        overlayManager->DestroySelectOverlay(overlayId);
    });
    button->MarkModifyDone();
    return button;
}

RefPtr<FrameNode> BuildMoreOrBackButton(int32_t overlayId, bool isMoreButton)
{
    auto button = FrameNode::GetOrCreateFrameNode("SelectMoreOrBackButton",
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ButtonPattern>(); });
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, button);
    auto textOverlayTheme = pipeline->GetTheme<TextOverlayTheme>();
    CHECK_NULL_RETURN(textOverlayTheme, button);

    // Update property.
    auto buttonLayoutProperty = button->GetLayoutProperty<ButtonLayoutProperty>();
    CHECK_NULL_RETURN(buttonLayoutProperty, button);

    const auto& padding = textOverlayTheme->GetMenuPadding();

    auto sideWidth = CalcLength(textOverlayTheme->GetMenuToolbarHeight().ConvertToPx() - padding.Top().ConvertToPx() -
                                padding.Bottom().ConvertToPx());
    buttonLayoutProperty->UpdateUserDefinedIdealSize({ sideWidth, sideWidth });

    if (!isMoreButton) {
        const auto& padding = textOverlayTheme->GetMenuPadding();
        auto left = CalcLength(padding.Left().ConvertToPx());
        auto right = CalcLength(padding.Right().ConvertToPx());
        auto top = CalcLength(padding.Top().ConvertToPx());
        auto bottom = CalcLength(padding.Bottom().ConvertToPx());
        buttonLayoutProperty->UpdateMargin({ left, right, top, bottom });
    }

    button->GetRenderContext()->UpdateBackgroundColor(Color::TRANSPARENT);
    button->GetOrCreateGestureEventHub()->SetUserOnClick([overlayId, isMore = isMoreButton](GestureEvent& /*info*/) {
        auto pipeline = PipelineContext::GetCurrentContext();
        CHECK_NULL_VOID(pipeline);
        auto overlayManager = pipeline->GetSelectOverlayManager();
        CHECK_NULL_VOID(overlayManager);
        auto selectOverlay = overlayManager->GetSelectOverlayNode(overlayId);
        CHECK_NULL_VOID(selectOverlay);
        // When click button , change to extensionMenu or change to the default menu(selectMenu_).
        selectOverlay->MoreOrBackAnimation(isMore);
    });

    return button;
}

OffsetF GetPageOffset()
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, OffsetF());
    auto stageManager = pipeline->GetStageManager();
    CHECK_NULL_RETURN(stageManager, OffsetF());
    auto page = stageManager->GetLastPage();
    CHECK_NULL_RETURN(page, OffsetF());
    return page->GetOffsetRelativeToWindow();
}

std::vector<OptionParam> GetOptionsParams(const std::shared_ptr<SelectOverlayInfo>& info)
{
    std::vector<OptionParam> params;
    params.emplace_back(Localization::GetInstance()->GetEntryLetters(BUTTON_CUT), info->menuCallback.onCut);
    params.emplace_back(Localization::GetInstance()->GetEntryLetters(BUTTON_COPY), info->menuCallback.onCopy);
    params.emplace_back(Localization::GetInstance()->GetEntryLetters(BUTTON_PASTE), info->menuCallback.onPaste);
    params.emplace_back(Localization::GetInstance()->GetEntryLetters(BUTTON_COPY_ALL), info->menuCallback.onSelectAll);
    return params;
}

void SetOptionDisable(const RefPtr<FrameNode>& option)
{
    CHECK_NULL_VOID(option);
    auto optionEventHub = option->GetEventHub<OptionEventHub>();
    CHECK_NULL_VOID(optionEventHub);
    optionEventHub->SetEnabled(false);
}

void SetOptionsAction(const std::shared_ptr<SelectOverlayInfo>& info, const std::vector<RefPtr<FrameNode>>& options)
{
    if (options.empty()) {
        return;
    }
    if (!info->menuInfo.showCut) {
        SetOptionDisable(options[OPTION_INDEX_CUT]);
    }
    if (!info->menuInfo.showCopy) {
        SetOptionDisable(options[OPTION_INDEX_COPY]);
    }
    if (!info->menuInfo.showPaste) {
        SetOptionDisable(options[OPTION_INDEX_PASTE]);
    }
    if (!info->menuInfo.showCopyAll) {
        SetOptionDisable(options[OPTION_INDEX_COPY_ALL]);
    }
}
} // namespace

SelectOverlayNode::SelectOverlayNode(const std::shared_ptr<SelectOverlayInfo>& info)
    : FrameNode("SelectOverlay", ElementRegister::GetInstance()->MakeUniqueId(), MakeRefPtr<SelectOverlayPattern>(info))
{}

RefPtr<FrameNode> SelectOverlayNode::CreateSelectOverlayNode(const std::shared_ptr<SelectOverlayInfo>& info)
{
    if (info->isUsingMouse) {
        return CreateMenuNode(info);
    }
    auto selectOverlayNode = AceType::MakeRefPtr<SelectOverlayNode>(info);
    selectOverlayNode->InitializePatternAndContext();
    ElementRegister::GetInstance()->AddUINode(selectOverlayNode);
    selectOverlayNode->CreateToolBar();
    selectOverlayNode->UpdateToolBar(true);
    return selectOverlayNode;
}

void SelectOverlayNode::MoreOrBackAnimation(bool isMore)
{
    auto selectContext = selectMenu_->GetRenderContext();
    CHECK_NULL_VOID(selectContext);
    auto extensionContext = extensionMenu_->GetRenderContext();
    CHECK_NULL_VOID(extensionContext);
    auto selectMenuInnerContext = selectMenuInner_->GetRenderContext();
    CHECK_NULL_VOID(selectMenuInnerContext);

    auto extensionProperty = extensionMenu_->GetLayoutProperty();
    CHECK_NULL_VOID(extensionProperty);
    auto selectProperty = selectMenu_->GetLayoutProperty();
    CHECK_NULL_VOID(selectProperty);
    auto selectMenuInnerProperty = selectMenuInner_->GetLayoutProperty();
    CHECK_NULL_VOID(selectMenuInnerProperty);

    auto pattern = GetPattern<SelectOverlayPattern>();
    CHECK_NULL_VOID(pattern);
    auto modifier = pattern->GetOverlayModifier();
    CHECK_NULL_VOID(modifier);

    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);

    auto textOverlayTheme = pipeline->GetTheme<TextOverlayTheme>();
    CHECK_NULL_VOID(textOverlayTheme);

    auto meanuWidth = pattern->GetMenuWidth();

    if (isMore) {
        extensionProperty->UpdateVisibility(VisibleType::VISIBLE);
        AnimationOption extensionOption;
        extensionOption.SetDuration(ANIMATION_DURATION2);
        extensionOption.SetCurve(Curves::FAST_OUT_SLOW_IN);
        auto toolbarHeight = textOverlayTheme->GetMenuToolbarHeight();
        auto frameSize = CalcSize(CalcLength(toolbarHeight.ConvertToPx()), CalcLength(toolbarHeight.ConvertToPx()));

        FinishCallback callback = [selectMenuInnerProperty, extensionProperty, weak = Claim(this)]() {
            selectMenuInnerProperty->UpdateVisibility(VisibleType::INVISIBLE);
            extensionProperty->UpdateVisibility(VisibleType::VISIBLE);
        };
        AnimationUtils::Animate(
            extensionOption,
            [extensionContext, selectMenuInnerContext]() {
                extensionContext->UpdateOpacity(1.0);
                extensionContext->UpdateTransformTranslate({ 0.0f, 0.0f, 0.0f });
                selectMenuInnerContext->UpdateOpacity(0.0);
            },
            callback);
        modifier->SetOtherPointRadius(MIN_DIAMETER / 2.0f);
        modifier->SetHeadPointRadius(MIN_ARROWHEAD_DIAMETER / 2.0f);
        modifier->SetLineEndOffset(true);

        AnimationOption selectOption;
        selectOption.SetDuration(ANIMATION_DURATION1);
        selectOption.SetCurve(Curves::FRICTION);
        pipeline->FlushUITasks();
        AnimationUtils::OpenImplicitAnimation(selectOption, Curves::FRICTION, nullptr);
        selectProperty->UpdateUserDefinedIdealSize(frameSize);
        selectMenuInnerContext->UpdateTransformTranslate({ ANIMATION_TEXT_OFFSET.ConvertToPx(), 0.0f, 0.0f });
        selectMenu_->MarkDirtyNode(PROPERTY_UPDATE_LAYOUT);
        pipeline->FlushUITasks();
        AnimationUtils::CloseImplicitAnimation();

    } else {
        selectMenuInnerProperty->UpdateVisibility(VisibleType::VISIBLE);
        AnimationOption extensionOption;
        extensionOption.SetDuration(ANIMATION_DURATION2);
        extensionOption.SetCurve(Curves::FAST_OUT_SLOW_IN);

        FinishCallback callback = [extensionProperty, selectMenuInnerProperty, weak = Claim(this)]() {
            selectMenuInnerProperty->UpdateVisibility(VisibleType::VISIBLE);
            extensionProperty->UpdateVisibility(VisibleType::GONE);
        };
        AnimationUtils::Animate(
            extensionOption,
            [extensionContext, selectMenuInnerContext]() {
                extensionContext->UpdateOpacity(0.0);
                extensionContext->UpdateTransformTranslate({ 0.0f, MORE_MENU_TRANSLATE.ConvertToPx(), 0.0f });
                selectMenuInnerContext->UpdateOpacity(1.0);
            },
            callback);
        modifier->SetOtherPointRadius(MAX_DIAMETER / 2.0f);
        modifier->SetHeadPointRadius(MAX_DIAMETER / 2.0f);
        modifier->SetLineEndOffset(false);

        auto toolbarHeight = textOverlayTheme->GetMenuToolbarHeight();
        auto frameSize = CalcSize(CalcLength(meanuWidth), CalcLength(toolbarHeight.ConvertToPx()));

        AnimationOption selectOption;
        selectOption.SetDuration(ANIMATION_DURATION1);
        selectOption.SetCurve(Curves::FRICTION);
        pipeline->FlushUITasks();
        AnimationUtils::OpenImplicitAnimation(selectOption, Curves::FRICTION, nullptr);
        selectProperty->UpdateUserDefinedIdealSize(frameSize);
        selectMenuInnerContext->UpdateTransformTranslate({ 0.0f, 0.0f, 0.0f });
        selectContext->UpdateOffset(OffsetT<Dimension>(0.0_px, 0.0_px));
        selectMenu_->MarkDirtyNode(PROPERTY_UPDATE_LAYOUT);
        pipeline->FlushUITasks();
        AnimationUtils::CloseImplicitAnimation();
    }
}

void SelectOverlayNode::CreateExtensionToolBar(const std::vector<MenuOptionsParam>& menuOptionItems, int32_t index)
{
    extensionMenu_ = FrameNode::GetOrCreateFrameNode("SelectMoreMenu", ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
    extensionMenu_->GetLayoutProperty<LinearLayoutProperty>()->UpdateCrossAxisAlign(FlexAlign::FLEX_END);

    auto weak = Claim(this);
    extensionMenu_->MountToParent(weak);
    extensionMenu_->GetLayoutProperty()->UpdateVisibility(VisibleType::GONE);

    auto extensionMenuContext = extensionMenu_->GetRenderContext();
    auto id = GetId();

    auto button = BuildMoreOrBackButton(id, false);
    button->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
    button->MountToParent(extensionMenu_);

    std::vector<OptionParam> params;
    int32_t itemNum = 0;
    for (auto item : menuOptionItems) {
        if (itemNum >= index) {
            auto callback = [overlayId = id, func = std::move(item.action)]() {
                auto pipeline = PipelineContext::GetCurrentContext();
                CHECK_NULL_VOID(pipeline);
                auto overlayManager = pipeline->GetSelectOverlayManager();
                CHECK_NULL_VOID(overlayManager);

                auto selectOverlay = overlayManager->GetSelectOverlayNode(overlayId);
                auto pattern = selectOverlay->GetPattern<SelectOverlayPattern>();
                auto selectInfo = pattern->GetSelectInfo();
                func(selectInfo);
                overlayManager->DestroySelectOverlay(overlayId);
            };
            params.emplace_back(item.content.value_or("null"), item.icon.value_or(" "), callback);
        }
        itemNum++;
    }
    auto menuWrapper =
        MenuView::Create(std::move(params), -1, "ExtensionMenu", MenuType::SELECT_OVERLAY_EXTENSION_MENU);
    CHECK_NULL_VOID(menuWrapper);
    auto menu = DynamicCast<FrameNode>(menuWrapper->GetChildAtIndex(0));
    CHECK_NULL_VOID(menu);
    menuWrapper->RemoveChild(menu);
    menuWrapper.Reset();

    // set click position to menu
    auto props = menu->GetLayoutProperty<MenuLayoutProperty>();
    auto context = menu->GetRenderContext();
    CHECK_NULL_VOID(props);
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto textOverlayTheme = pipeline->GetTheme<TextOverlayTheme>();
    CHECK_NULL_VOID(textOverlayTheme);
    auto selectTheme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(selectTheme);
    auto selectmargin = static_cast<float>(selectTheme->GetOutPadding().ConvertToPx());
    selectmargin = 0.0f;
    auto offsetY = textOverlayTheme->GetMenuToolbarHeight().ConvertToPx();
    props->UpdateMenuOffset(
        OffsetF(-selectmargin, offsetY + MORE_MENU_INTERVAL.ConvertToPx() - selectmargin) + GetPageOffset());
    context->UpdateBackShadow(ShadowConfig::NoneShadow);
    menu->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    ElementRegister::GetInstance()->AddUINode(menu);
    menu->MountToParent(extensionMenu_);

    extensionMenuContext->UpdateOpacity(0.0);
    extensionMenuContext->UpdateTransformTranslate({ 0.0f, MORE_MENU_TRANSLATE.ConvertToPx(), 0.0f });

    extensionMenu_->MarkModifyDone();
}

void SelectOverlayNode::CreateToolBar()
{
    auto info = GetPattern<SelectOverlayPattern>()->GetSelectOverlayInfo();
    selectMenu_ = FrameNode::GetOrCreateFrameNode("SelectMenu", ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<LinearLayoutPattern>(false); });
    selectMenu_->GetLayoutProperty<LinearLayoutProperty>()->UpdateMainAxisAlign(FlexAlign::FLEX_END);
    selectMenu_->GetRenderContext()->SetClipToFrame(true);
    selectMenu_->GetLayoutProperty()->UpdateMeasureType(MeasureType::MATCH_CONTENT);

    // Increase the node to realize the animation effect of font transparency and offset.
    selectMenuInner_ =
        FrameNode::GetOrCreateFrameNode("SelectMenuInner", ElementRegister::GetInstance()->MakeUniqueId(),
            []() { return AceType::MakeRefPtr<LinearLayoutPattern>(false); });
    selectMenuInner_->GetLayoutProperty<LinearLayoutProperty>()->UpdateMainAxisAlign(FlexAlign::FLEX_END);
    selectMenuInner_->GetLayoutProperty()->UpdateMeasureType(MeasureType::MATCH_CONTENT);

    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto textOverlayTheme = pipeline->GetTheme<TextOverlayTheme>();
    CHECK_NULL_VOID(textOverlayTheme);
    selectMenu_->GetRenderContext()->UpdateBackgroundColor(textOverlayTheme->GetMenuBackgroundColor());
    selectMenuInner_->GetRenderContext()->UpdateOpacity(1.0);
    selectMenuInner_->GetRenderContext()->UpdateTransformTranslate({ 0.0f, 0.0f, 0.0f });

    const auto& border = textOverlayTheme->GetMenuBorder();
    auto borderWidth = Dimension(border.Left().GetWidth().ConvertToPx());
    selectMenu_->GetLayoutProperty()->UpdateBorderWidth({ borderWidth, borderWidth, borderWidth, borderWidth });
    auto borderRadius = textOverlayTheme->GetMenuToolbarHeight() / 2.0f;
    selectMenu_->GetRenderContext()->UpdateBorderRadius({ borderRadius, borderRadius, borderRadius, borderRadius });
    auto borderColor = border.Left().GetColor();
    selectMenu_->GetRenderContext()->UpdateBorderColor({ borderColor, borderColor, borderColor, borderColor });
    auto borderStyle = border.Left().GetBorderStyle();
    selectMenu_->GetRenderContext()->UpdateBorderStyle({ borderStyle, borderStyle, borderStyle, borderStyle });

    const auto& padding = textOverlayTheme->GetMenuPadding();
    auto left = CalcLength(padding.Left().ConvertToPx());
    auto right = CalcLength(padding.Right().ConvertToPx());
    auto top = CalcLength(padding.Top().ConvertToPx());
    auto bottom = CalcLength(padding.Bottom().ConvertToPx());
    selectMenuInner_->GetLayoutProperty()->UpdatePadding({ left, right, top, bottom });

    selectMenuInner_->GetLayoutProperty()->UpdateUserDefinedIdealSize(
        { std::nullopt, CalcLength(textOverlayTheme->GetMenuToolbarHeight()) });

    if (info->menuInfo.menuIsShow) {
        selectMenu_->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    } else {
        selectMenu_->GetLayoutProperty()->UpdateVisibility(VisibleType::GONE);
    }

    selectMenuInner_->MountToParent(selectMenu_);
    selectMenuInner_->GetOrCreateGestureEventHub()->MarkResponseRegion(true);

    selectMenu_->GetRenderContext()->UpdateBackShadow(ShadowConfig::DefaultShadowM);
    selectMenu_->MountToParent(Claim(this));
    selectMenu_->GetOrCreateGestureEventHub()->MarkResponseRegion(true);
    selectMenu_->MarkModifyDone();
}

void SelectOverlayNode::GetDefaultButtonAndMenuWidth(float& defaultOptionWidth, float& fontWidth, float& maxWidth)
{
    MeasureContext content;
    content.textContent = Localization::GetInstance()->GetEntryLetters(BUTTON_COPY);
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto textOverlayTheme = pipeline->GetTheme<TextOverlayTheme>();
    CHECK_NULL_VOID(textOverlayTheme);
    auto selectOverlayMaxWidth = textOverlayTheme->GetSelectOverlayMaxWidth().ConvertToPx();

    const auto& menuPadding = textOverlayTheme->GetMenuPadding();

    maxWidth = selectOverlayMaxWidth - menuPadding.Left().ConvertToPx() - menuPadding.Right().ConvertToPx() -
               textOverlayTheme->GetMoreButtonHeight().ConvertToPx();

    auto textStyle = textOverlayTheme->GetMenuButtonTextStyle();
    content.fontSize = textStyle.GetFontSize();
#ifdef ENABLE_ROSEN_BACKEND
    auto size = RosenRenderCustomPaint::MeasureTextSizeInner(content);
#else
    auto size = Size(0.0, 0.0);
#endif
    fontWidth = size.Width();
    const auto& buttonPadding = textOverlayTheme->GetMenuButtonPadding();
    defaultOptionWidth = fontWidth + buttonPadding.Left().ConvertToPx() + buttonPadding.Right().ConvertToPx();
}

void SelectOverlayNode::UpdateToolBar(bool menuItemChanged)
{
    auto info = GetPattern<SelectOverlayPattern>()->GetSelectOverlayInfo();
    if (menuItemChanged) {
        selectMenuInner_->Clean();

        float defaultOptionWidth = 0.0f;
        float fontWidth = 0.0f;
        float maxWidth = 0.0f;
        float allocatedSize = 0.0f;

        GetDefaultButtonAndMenuWidth(defaultOptionWidth, fontWidth, maxWidth);

        if (info->menuInfo.showCut) {
            auto button = BuildButton(
                Localization::GetInstance()->GetEntryLetters(BUTTON_CUT), info->menuCallback.onCut, GetId(), fontWidth);
            button->MountToParent(selectMenuInner_);
            allocatedSize += defaultOptionWidth;
        }
        if (info->menuInfo.showCopy) {
            auto button = BuildButton(Localization::GetInstance()->GetEntryLetters(BUTTON_COPY),
                info->menuCallback.onCopy, GetId(), fontWidth);
            button->MountToParent(selectMenuInner_);
            allocatedSize += defaultOptionWidth;
        }
        if (info->menuInfo.showPaste) {
            auto button = BuildButton(Localization::GetInstance()->GetEntryLetters(BUTTON_PASTE),
                info->menuCallback.onPaste, GetId(), fontWidth);
            button->MountToParent(selectMenuInner_);
            allocatedSize += defaultOptionWidth;
        }
        if (info->menuInfo.showCopyAll) {
            auto button = BuildButton(Localization::GetInstance()->GetEntryLetters(BUTTON_COPY_ALL),
                info->menuCallback.onSelectAll, GetId(), fontWidth, true);
            button->MountToParent(selectMenuInner_);
            allocatedSize += defaultOptionWidth;
        }
        auto itemNum = -1;
        auto extensionOptionStartIndex = -1;
        if (!info->menuOptionItems.empty()) {
            for (auto item : info->menuOptionItems) {
                itemNum++;
                float extensionOptionWidth = 0.0f;
                auto button = BuildButton(item.content.value_or("null"), item.action, GetId(), extensionOptionWidth);
                allocatedSize += extensionOptionWidth;
                if (allocatedSize > maxWidth) {
                    button.Reset();
                    extensionOptionStartIndex = itemNum;
                    break;
                }
                button->MountToParent(selectMenuInner_);
            }
        }
        if (extensionOptionStartIndex != -1) {
            auto backButton = BuildMoreOrBackButton(GetId(), true);
            backButton->MountToParent(selectMenuInner_);
            CreateExtensionToolBar(info->menuOptionItems, extensionOptionStartIndex);
        }
    }
    if (info->menuInfo.menuDisable) {
        selectMenu_->GetLayoutProperty()->UpdateVisibility(VisibleType::GONE);
    } else if (info->menuInfo.menuIsShow) {
        selectMenu_->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
    } else {
        selectMenu_->GetLayoutProperty()->UpdateVisibility(VisibleType::GONE);
    }
    selectMenu_->MarkModifyDone();
    MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
}

RefPtr<FrameNode> SelectOverlayNode::CreateMenuNode(const std::shared_ptr<SelectOverlayInfo>& info)
{
    std::vector<OptionParam> params = GetOptionsParams(info);
    auto menuWrapper = MenuView::Create(std::move(params), -1);
    CHECK_NULL_RETURN(menuWrapper, nullptr);
    auto menu = DynamicCast<FrameNode>(menuWrapper->GetChildAtIndex(0));
    menuWrapper->RemoveChild(menu);
    menuWrapper.Reset();
    // set click position to menu
    CHECK_NULL_RETURN(menu, nullptr);
    auto props = menu->GetLayoutProperty<MenuLayoutProperty>();
    CHECK_NULL_RETURN(props, nullptr);
    props->UpdateMenuOffset(info->rightClickOffset + GetPageOffset());

    auto menuPattern = menu->GetPattern<MenuPattern>();
    CHECK_NULL_RETURN(menuPattern, nullptr);
    auto options = menuPattern->GetOptions();
    SetOptionsAction(info, options);

    menu->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
    ElementRegister::GetInstance()->AddUINode(menu);

    return menu;
}

} // namespace OHOS::Ace::NG
