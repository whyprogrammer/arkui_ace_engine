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

#include "core/components_ng/pattern/text_picker/textpicker_column_pattern.h"

#include <cstdint>

#include "base/geometry/dimension.h"
#include "base/geometry/ng/size_t.h"
#include "base/utils/utils.h"
#include "core/components/picker/picker_theme.h"
#include "core/components_ng/pattern/image/image_layout_property.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/pattern/text_picker/textpicker_event_hub.h"
#include "core/components_ng/pattern/text_picker/textpicker_layout_property.h"
#include "core/components_ng/pattern/text_picker/textpicker_pattern.h"
#include "core/components_ng/pattern/text_picker/toss_animation_controller.h"
#include "core/pipeline_ng/ui_task_scheduler.h"

namespace OHOS::Ace::NG {
namespace {
const uint32_t OPTION_COUNT_PHONE_LANDSCAPE = 3;
const Dimension FONT_SIZE = Dimension(2.0);
const int32_t ANIMATION_ZERO_TO_OUTER = 200; // 200ms for animation that from zero to outer.
const int32_t ANIMATION_OUTER_TO_ZERO = 150; // 150ms for animation that from outer to zero.
const Dimension FOCUS_SIZE = Dimension(1.0);
const int32_t MIDDLE_CHILD_INDEX = 2;
const float MOVE_DISTANCE = 5.0f;
constexpr int32_t HOVER_ANIMATION_DURATION = 40;
constexpr size_t MIXTURE_CHILD_COUNT = 2;
const Dimension ICON_SIZE = 24.0_vp;
const Dimension ICON_TEXT_SPACE = 8.0_vp;
} // namespace

void TextPickerColumnPattern::OnAttachToFrameNode()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);

    auto context = host->GetContext();
    CHECK_NULL_VOID(context);
    auto pickerTheme = context->GetTheme<PickerTheme>();
    CHECK_NULL_VOID(pickerTheme);
    auto hub = host->GetEventHub<EventHub>();
    CHECK_NULL_VOID(hub);
    auto gestureHub = hub->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    tossAnimationController_->SetPipelineContext(context);
    tossAnimationController_->SetColumn(AceType::WeakClaim(this));
    jumpInterval_ = pickerTheme->GetJumpInterval().ConvertToPx();
    CreateAnimation();
    InitPanEvent(gestureHub);
    host->GetRenderContext()->SetClipToFrame(true);
}

bool TextPickerColumnPattern::OnDirtyLayoutWrapperSwap(
    const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config)
{
    CHECK_NULL_RETURN_NOLOG(config.frameSizeChange, false);
    CHECK_NULL_RETURN(dirty, false);
    return true;
}

void TextPickerColumnPattern::OnModifyDone()
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<PickerTheme>();
    pressColor_ = theme->GetPressColor();
    hoverColor_ = theme->GetHoverColor();
    InitMouseAndPressEvent();
}

void TextPickerColumnPattern::OnAroundButtonClick(RefPtr<EventParam> param)
{
    int32_t step = param->itemIndex - MIDDLE_CHILD_INDEX;
    if (step != 0) {
        InnerHandleScroll(step, false);
    }
}


void TextPickerColumnPattern::OnMiddleButtonTouchDown(RefPtr<EventParam> param)
{
    PlayPressAnimation(pressColor_);
}

void TextPickerColumnPattern::OnMiddleButtonTouchMove(RefPtr<EventParam> param)
{
    PlayPressAnimation(Color::TRANSPARENT);
}

void TextPickerColumnPattern::OnMiddleButtonTouchUp(RefPtr<EventParam> param)
{
    PlayPressAnimation(Color::TRANSPARENT);
}

RefPtr<TouchEventImpl> TextPickerColumnPattern::CreateItemTouchEventListener(RefPtr<EventParam> param)
{
    auto itemCallback = [param, weak = WeakClaim(this)](const TouchEventInfo& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        if (info.GetTouches().front().GetTouchType() == TouchType::DOWN) {
            pattern->SetLocalDownDistance(info.GetTouches().front().GetLocalLocation().GetDistance());
            pattern->OnMiddleButtonTouchDown(param);
        }
        if (info.GetTouches().front().GetTouchType() == TouchType::UP) {
            pattern->OnMiddleButtonTouchUp(param);
            pattern->SetLocalDownDistance(0.0f);
        }
        if (info.GetTouches().front().GetTouchType() == TouchType::MOVE) {
            if (std::abs(info.GetTouches().front().GetLocalLocation().GetDistance() - pattern->GetLocalDownDistance()) >
                MOVE_DISTANCE) {
                pattern->OnMiddleButtonTouchMove(param);
            }
        }
    };
    auto listener = MakeRefPtr<TouchEventImpl>(std::move(itemCallback));
    return listener;
}

RefPtr<ClickEvent> TextPickerColumnPattern::CreateItemClickEventListener(RefPtr<EventParam> param)
{
    auto clickEventHandler = [param, weak = WeakClaim(this)](const GestureEvent& /* info */) {
        auto pattern = weak.Upgrade();
        pattern->OnAroundButtonClick(param);
    };

    auto listener = AceType::MakeRefPtr<NG::ClickEvent>(clickEventHandler);
    return listener;
}

RefPtr<InputEvent> TextPickerColumnPattern::CreateMouseHoverEventListener(RefPtr<EventParam> param)
{
    auto mouseTask = [weak = WeakClaim(this)](bool isHover) {
        auto pattern = weak.Upgrade();
        if (pattern) {
            pattern->HandleMouseEvent(isHover);
        }
    };
    auto hoverEventListener = MakeRefPtr<InputEvent>(std::move(mouseTask));
    return hoverEventListener;
}

void TextPickerColumnPattern::InitMouseAndPressEvent()
{
    if (touchEventInit) {
        return;
    }
    
    auto host = GetHost();
    CHECK_NULL_VOID(host);

    auto childSize = static_cast<int32_t>(host->GetChildren().size());

    for (int i = 0; i < childSize; i++) {
        RefPtr<FrameNode> childNode = DynamicCast<FrameNode>(host->GetChildAtIndex(i));
        RefPtr<EventParam> param = MakeRefPtr<EventParam>();
        param->instance = childNode;
        param->itemIndex = i;
        param->itemTotalCounts = childSize;
        
        auto eventHub = childNode->GetEventHub<EventHub>();
        CHECK_NULL_VOID(eventHub);

        if (i != MIDDLE_CHILD_INDEX) {
            RefPtr<ClickEvent> clickListener = CreateItemClickEventListener(param);
            CHECK_NULL_VOID(clickListener);
            auto gesture = eventHub->GetOrCreateGestureEventHub();
            CHECK_NULL_VOID(gesture);
            gesture->AddClickEvent(clickListener);
        } else {
            auto inputHub = eventHub->GetOrCreateInputEventHub();
            CHECK_NULL_VOID(inputHub);
            RefPtr<InputEvent> hoverEventListener = CreateMouseHoverEventListener(param);
            CHECK_NULL_VOID(hoverEventListener);
            inputHub->AddOnHoverEvent(hoverEventListener);

            RefPtr<TouchEventImpl> itemListener = CreateItemTouchEventListener(param);
            CHECK_NULL_VOID(itemListener);
            auto gesture = eventHub->GetOrCreateGestureEventHub();
            CHECK_NULL_VOID(gesture);
            gesture->AddTouchEvent(itemListener);
        }
    }

    touchEventInit = true;
}

void TextPickerColumnPattern::HandleMouseEvent(bool isHover)
{
    if (isHover) {
        PlayPressAnimation(hoverColor_);
    } else {
        PlayPressAnimation(Color::TRANSPARENT);
    }
}

void TextPickerColumnPattern::SetButtonBackgroundColor(const Color& pressColor)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto stack = host->GetParent();
    CHECK_NULL_VOID(stack);
    auto buttonNode = DynamicCast<FrameNode>(stack->GetFirstChild());
    auto renderContext = buttonNode->GetRenderContext();
    renderContext->UpdateBackgroundColor(pressColor);
    buttonNode->MarkModifyDone();
    buttonNode->MarkDirtyNode();
}

void TextPickerColumnPattern::PlayPressAnimation(const Color& pressColor)
{
    AnimationOption option = AnimationOption();
    option.SetDuration(HOVER_ANIMATION_DURATION);
    option.SetFillMode(FillMode::FORWARDS);
    AnimationUtils::Animate(option, [weak = AceType::WeakClaim(this), pressColor]() {
        auto picker = weak.Upgrade();
        if (picker) {
            picker->SetButtonBackgroundColor(pressColor);
        }
    });
}

uint32_t TextPickerColumnPattern::GetShowOptionCount() const
{
    auto context = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(context, 0);
    auto pickerTheme = context->GetTheme<PickerTheme>();
    CHECK_NULL_RETURN(pickerTheme, 0);
    auto showCount = pickerTheme->GetShowOptionCount();
    if (SystemProperties::GetDeviceType() == DeviceType::PHONE &&
        SystemProperties::GetDeviceOrientation() == DeviceOrientation::LANDSCAPE) {
        showCount = OPTION_COUNT_PHONE_LANDSCAPE;
    }
    return showCount;
}

void TextPickerColumnPattern::FlushCurrentOptions(bool isDown, bool isUpateTextContentOnly, bool isDirectlyClear)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto stackNode = DynamicCast<FrameNode>(host->GetParent());
    CHECK_NULL_VOID(stackNode);
    auto parentNode = DynamicCast<FrameNode>(stackNode->GetParent());
    CHECK_NULL_VOID(parentNode);
    auto textPickerLayoutProperty = parentNode->GetLayoutProperty<TextPickerLayoutProperty>();
    CHECK_NULL_VOID(textPickerLayoutProperty);

    if (!isUpateTextContentOnly) {
        animationProperties_.clear();
    }
    if (columnkind_ == TEXT) {
        FlushCurrentTextOptions(textPickerLayoutProperty, isUpateTextContentOnly, isDirectlyClear);
    } else if (columnkind_ == ICON) {
        FlushCurrentImageOptions();
    } else if (columnkind_ == MIXTURE) {
        FlushCurrentMixtureOptions(textPickerLayoutProperty, isUpateTextContentOnly);
    }
    if (isUpateTextContentOnly) {
        FlushAnimationTextProperties(isDown);
    }
    if (isIndexChanged_) {
        HandleEventCallback(true);
    }
}

void TextPickerColumnPattern::ClearCurrentTextOptions(const RefPtr<TextPickerLayoutProperty>& textPickerLayoutProperty,
    bool isUpateTextContentOnly, bool isDirectlyClear)
{
    if (isDirectlyClear) {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto child = host->GetChildren();
        for (auto iter = child.begin(); iter != child.end(); iter++) {
            auto textNode = DynamicCast<FrameNode>(*iter);
            CHECK_NULL_VOID(textNode);
            auto textPattern = textNode->GetPattern<TextPattern>();
            CHECK_NULL_VOID(textPattern);
            auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
            CHECK_NULL_VOID(textLayoutProperty);
            textLayoutProperty->UpdateContent("");
            textNode->GetRenderContext()->SetClipToFrame(true);
            textNode->MarkModifyDone();
            textNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        }
        selectedIndex_ = 0;
    }
}

void TextPickerColumnPattern::FlushCurrentTextOptions(const RefPtr<TextPickerLayoutProperty>& textPickerLayoutProperty,
    bool isUpateTextContentOnly, bool isDirectlyClear)
{
    ClearCurrentTextOptions(textPickerLayoutProperty, isUpateTextContentOnly, isDirectlyClear);
    uint32_t totalOptionCount = GetOptionCount();
    if (totalOptionCount == 0) {
        return;
    }
    uint32_t currentIndex = GetCurrentIndex();
    currentIndex = currentIndex % totalOptionCount;
    uint32_t showCount = GetShowOptionCount();
    auto middleIndex = showCount / 2; // the center option is selected.
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto child = host->GetChildren();
    auto iter = child.begin();
    if (child.size() != showCount) {
        return;
    }
    for (uint32_t index = 0; index < showCount; index++) {
        uint32_t optionIndex = (totalOptionCount + currentIndex + index - middleIndex) % totalOptionCount;
        RangeContent optionValue = options_[optionIndex];
        int32_t diffIndex = static_cast<int32_t>(index) - static_cast<int32_t>(middleIndex);
        int32_t virtualIndex = static_cast<int32_t>(currentIndex) + diffIndex;
        bool virtualIndexValidate = virtualIndex >= 0 && virtualIndex < static_cast<int32_t>(totalOptionCount);
        auto textNode = DynamicCast<FrameNode>(*iter);
        CHECK_NULL_VOID(textNode);
        auto textPattern = textNode->GetPattern<TextPattern>();
        CHECK_NULL_VOID(textPattern);
        auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
        CHECK_NULL_VOID(textLayoutProperty);
        if (!isUpateTextContentOnly) {
            UpdatePickerTextProperties(textLayoutProperty, textPickerLayoutProperty,
                index, middleIndex, showCount);
        }
        if (NotLoopOptions() && !virtualIndexValidate) {
            textLayoutProperty->UpdateContent("");
        } else {
            textLayoutProperty->UpdateContent(optionValue.text_);
            textLayoutProperty->UpdateTextAlign(TextAlign::CENTER);
        }
        textNode->GetRenderContext()->SetClipToFrame(true);
        textNode->MarkModifyDone();
        textNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        iter++;
    }
    selectedIndex_ = currentIndex;
}

void TextPickerColumnPattern::FlushCurrentImageOptions()
{
    uint32_t totalOptionCount = GetOptionCount();
    if (totalOptionCount == 0) {
        return;
    }
    uint32_t currentIndex = GetCurrentIndex();
    currentIndex = currentIndex % totalOptionCount;
    uint32_t showCount = GetShowOptionCount();
    auto middleIndex = showCount / 2; // the center option is selected.
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto child = host->GetChildren();
    auto iter = child.begin();
    if (child.size() != showCount) {
        return;
    }
    for (uint32_t index = 0; index < showCount; index++) {
        uint32_t optionIndex = (totalOptionCount + currentIndex + index - middleIndex) % totalOptionCount;
        RangeContent optionValue = options_[optionIndex];
        int32_t diffIndex = static_cast<int32_t>(index) - static_cast<int32_t>(middleIndex);
        int32_t virtualIndex = static_cast<int32_t>(currentIndex) + diffIndex;
        bool virtualIndexValidate = virtualIndex >= 0 && virtualIndex < static_cast<int32_t>(totalOptionCount);
        auto rangeNode = DynamicCast<FrameNode>(*iter);
        CHECK_NULL_VOID(rangeNode);
        auto iconNode = DynamicCast<FrameNode>(rangeNode->GetFirstChild());
        CHECK_NULL_VOID(iconNode);
        auto iconPattern = iconNode->GetPattern<ImagePattern>();
        CHECK_NULL_VOID(iconPattern);
        auto iconLayoutProperty = iconPattern->GetLayoutProperty<ImageLayoutProperty>();
        CHECK_NULL_VOID(iconLayoutProperty);
        CalcSize idealSize = { CalcSize(CalcLength(ICON_SIZE), CalcLength(ICON_SIZE)) };
        MeasureProperty layoutConstraint;
        layoutConstraint.selfIdealSize = idealSize;
        iconLayoutProperty->UpdateCalcLayoutProperty(layoutConstraint);
        if (NotLoopOptions() && !virtualIndexValidate) {
            iconLayoutProperty->UpdateVisibility(VisibleType::INVISIBLE);
        } else {
            iconLayoutProperty->UpdateVisibility(VisibleType::VISIBLE);
            iconLayoutProperty->UpdateImageSourceInfo(ImageSourceInfo(optionValue.icon_));
        }
        iconNode->MarkModifyDone();
        iconNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);

        rangeNode->GetRenderContext()->SetClipToFrame(true);
        rangeNode->MarkModifyDone();
        rangeNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        iter++;
    }
    selectedIndex_ = currentIndex;
}

void TextPickerColumnPattern::FlushCurrentMixtureOptions(
    const RefPtr<TextPickerLayoutProperty>& textPickerLayoutProperty,
    bool isUpateTextContentOnly)
{
    uint32_t totalOptionCount = GetOptionCount();
    if (totalOptionCount == 0) {
        return;
    }
    uint32_t currentIndex = GetCurrentIndex();
    currentIndex = currentIndex % totalOptionCount;
    uint32_t showCount = GetShowOptionCount();
    auto middleIndex = showCount / 2; // the center option is selected.
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto child = host->GetChildren();
    auto iter = child.begin();
    if (child.size() != showCount) {
        return;
    }
    for (uint32_t index = 0; index < showCount; index++) {
        uint32_t optionIndex = (totalOptionCount + currentIndex + index - middleIndex) % totalOptionCount;
        RangeContent optionValue = options_[optionIndex];
        int32_t diffIndex = static_cast<int32_t>(index) - static_cast<int32_t>(middleIndex);
        int32_t virtualIndex = static_cast<int32_t>(currentIndex) + diffIndex;
        bool virtualIndexValidate = virtualIndex >= 0 && virtualIndex < static_cast<int32_t>(totalOptionCount);
        auto linearLayoutNode = DynamicCast<FrameNode>(*iter);
        CHECK_NULL_VOID(linearLayoutNode);
        auto children = linearLayoutNode->GetChildren();
        if (children.size() != MIXTURE_CHILD_COUNT) {
            LOGE("children number is wrong.");
            continue;
        }
        auto iconNode = DynamicCast<FrameNode>(linearLayoutNode->GetFirstChild());
        auto iconPattern = iconNode->GetPattern<ImagePattern>();
        CHECK_NULL_VOID(iconPattern);
        auto iconLayoutProperty = iconPattern->GetLayoutProperty<ImageLayoutProperty>();
        CHECK_NULL_VOID(iconLayoutProperty);
        CalcSize idealSize = { CalcSize(CalcLength(ICON_SIZE), CalcLength(ICON_SIZE)) };
        MeasureProperty layoutConstraint;
        layoutConstraint.selfIdealSize = idealSize;
        iconLayoutProperty->UpdateCalcLayoutProperty(layoutConstraint);
        MarginProperty margin;
        margin.right = CalcLength(ICON_TEXT_SPACE);
        iconLayoutProperty->UpdateMargin(margin);

        auto textNode = DynamicCast<FrameNode>(linearLayoutNode->GetLastChild());
        auto textPattern = textNode->GetPattern<TextPattern>();
        CHECK_NULL_VOID(textPattern);
        auto textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
        CHECK_NULL_VOID(textLayoutProperty);
        if (!isUpateTextContentOnly) {
            UpdatePickerTextProperties(textLayoutProperty, textPickerLayoutProperty,
                index, middleIndex, showCount);
        }
        if (NotLoopOptions() && !virtualIndexValidate) {
            iconLayoutProperty->UpdateVisibility(VisibleType::INVISIBLE);
            textLayoutProperty->UpdateContent("");
        } else {
            textLayoutProperty->UpdateContent(optionValue.text_);
            iconLayoutProperty->UpdateVisibility(VisibleType::VISIBLE);
            iconLayoutProperty->UpdateImageSourceInfo(ImageSourceInfo(optionValue.icon_));
        }
        iconNode->MarkModifyDone();
        iconNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        textNode->MarkModifyDone();
        textNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);

        linearLayoutNode->GetRenderContext()->SetClipToFrame(true);
        linearLayoutNode->MarkModifyDone();
        linearLayoutNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        iter++;
    }
    selectedIndex_ = currentIndex;
}

void TextPickerColumnPattern::FlushAnimationTextProperties(bool isDown)
{
    if (!animationProperties_.size()) {
        return;
    }
    if (isDown) {
        for (size_t i = 0; i < animationProperties_.size(); i++) {
            if (i > 0) {
                animationProperties_[i - 1].upFontSize = animationProperties_[i].upFontSize;
                animationProperties_[i - 1].fontSize = animationProperties_[i].fontSize;
                animationProperties_[i - 1].downFontSize = animationProperties_[i].downFontSize;

                animationProperties_[i - 1].upColor = animationProperties_[i].upColor;
                animationProperties_[i - 1].currentColor = animationProperties_[i].currentColor;
                animationProperties_[i - 1].downColor = animationProperties_[i].downColor;
            }
            if (i == (animationProperties_.size() - 1)) {
                animationProperties_[i].upFontSize = animationProperties_[i].fontSize;
                animationProperties_[i].fontSize = animationProperties_[i].fontSize * 0.5;
                animationProperties_[i].downFontSize = Dimension();

                animationProperties_[i].upColor = animationProperties_[i].currentColor;
                auto colorEvaluator = AceType::MakeRefPtr<LinearEvaluator<Color>>();
                animationProperties_[i].currentColor =
                    colorEvaluator->Evaluate(Color(), animationProperties_[i].currentColor, 0.5);
                animationProperties_[i].downColor = Color();
            }
        }
    } else {
        for (size_t i = animationProperties_.size() - 1;; i--) {
            if (i == 0) {
                animationProperties_[i].upFontSize = Dimension();
                animationProperties_[i].downFontSize = animationProperties_[i].fontSize;
                animationProperties_[i].fontSize = animationProperties_[i].fontSize * 0.5;

                animationProperties_[i].upColor = Color();
                animationProperties_[i].downColor = animationProperties_[i].currentColor;
                auto colorEvaluator = AceType::MakeRefPtr<LinearEvaluator<Color>>();
                animationProperties_[i].currentColor =
                    colorEvaluator->Evaluate(Color(), animationProperties_[i].currentColor, 0.5);
                break;
            } else {
                animationProperties_[i].upFontSize = animationProperties_[i - 1].upFontSize;
                animationProperties_[i].fontSize = animationProperties_[i - 1].fontSize;
                animationProperties_[i].downFontSize = animationProperties_[i - 1].downFontSize;

                animationProperties_[i].upColor = animationProperties_[i - 1].upColor;
                animationProperties_[i].currentColor = animationProperties_[i - 1].currentColor;
                animationProperties_[i].downColor = animationProperties_[i - 1].downColor;
            }
        }
    }
}

void TextPickerColumnPattern::UpdateDisappearTextProperties(const RefPtr<PickerTheme>& pickerTheme,
    const RefPtr<TextLayoutProperty>& textLayoutProperty,
    const RefPtr<TextPickerLayoutProperty>& textPickerLayoutProperty)
{
    auto normalOptionSize = pickerTheme->GetOptionStyle(false, false).GetFontSize();
    if (textPickerLayoutProperty->HasDisappearColor()) {
        textLayoutProperty->UpdateTextColor(textPickerLayoutProperty->GetDisappearColor().value());
    }
    if (textPickerLayoutProperty->HasDisappearFontSize()) {
        textLayoutProperty->UpdateFontSize(textPickerLayoutProperty->GetDisappearFontSize().value());
    } else {
        textLayoutProperty->UpdateAdaptMaxFontSize(normalOptionSize);
        textLayoutProperty->UpdateAdaptMinFontSize(
            pickerTheme->GetOptionStyle(false, false).GetAdaptMinFontSize());
    }
    if (textPickerLayoutProperty->HasDisappearWeight()) {
        textLayoutProperty->UpdateFontWeight(textPickerLayoutProperty->GetDisappearWeight().value());
    }
}

void TextPickerColumnPattern::UpdateCandidateTextProperties(const RefPtr<PickerTheme>& pickerTheme,
    const RefPtr<TextLayoutProperty>& textLayoutProperty,
    const RefPtr<TextPickerLayoutProperty>& textPickerLayoutProperty)
{
    auto focusOptionSize = pickerTheme->GetOptionStyle(false, false).GetFontSize() + FONT_SIZE;
    if (textPickerLayoutProperty->HasColor()) {
        textLayoutProperty->UpdateTextColor(textPickerLayoutProperty->GetColor().value());
    }
    if (textPickerLayoutProperty->HasFontSize()) {
        textLayoutProperty->UpdateFontSize(textPickerLayoutProperty->GetFontSize().value());
    } else {
        textLayoutProperty->UpdateAdaptMaxFontSize(focusOptionSize);
        textLayoutProperty->UpdateAdaptMinFontSize(
            pickerTheme->GetOptionStyle(true, false).GetAdaptMinFontSize() - FOCUS_SIZE);
    }
    if (textPickerLayoutProperty->HasWeight()) {
        textLayoutProperty->UpdateFontWeight(textPickerLayoutProperty->GetWeight().value());
    }
}

void TextPickerColumnPattern::UpdateSelectedTextProperties(const RefPtr<PickerTheme>& pickerTheme,
    const RefPtr<TextLayoutProperty>& textLayoutProperty,
    const RefPtr<TextPickerLayoutProperty>& textPickerLayoutProperty)
{
    auto selectedOptionSize = pickerTheme->GetOptionStyle(true, false).GetFontSize();
    Color themeSelectedColor = pickerTheme->GetOptionStyle(true, false).GetTextColor();
    Color selectedColor = textPickerLayoutProperty->GetSelectedColor().value_or(themeSelectedColor);
    textLayoutProperty->UpdateTextColor(selectedColor);
    FontWeight themeFontWeight = pickerTheme->GetOptionStyle(true, false).GetFontWeight();
    FontWeight selectedFontWeight = textPickerLayoutProperty->GetSelectedWeight().value_or(themeFontWeight);
    textLayoutProperty->UpdateFontWeight(selectedFontWeight);
    if (textPickerLayoutProperty->HasSelectedFontSize()) {
        textLayoutProperty->UpdateFontSize(textPickerLayoutProperty->GetSelectedFontSize().value());
    } else {
        textLayoutProperty->UpdateAdaptMaxFontSize(selectedOptionSize);
        textLayoutProperty->UpdateAdaptMinFontSize(pickerTheme->GetOptionStyle(true, false).GetAdaptMinFontSize());
    }
}

void TextPickerColumnPattern::AddAnimationTextProperties(uint32_t currentIndex,
    const RefPtr<TextLayoutProperty>& textLayoutProperty)
{
    TextProperties properties;
    if (textLayoutProperty->HasFontSize()) {
        properties.fontSize = Dimension(textLayoutProperty->GetFontSize().value().ConvertToPx());
    }
    if (textLayoutProperty->HasTextColor()) {
        properties.currentColor = textLayoutProperty->GetTextColor().value();
    }
    if (currentIndex > 0) {
        properties.upFontSize = animationProperties_[currentIndex - 1].fontSize;
        animationProperties_[currentIndex - 1].downFontSize = properties.fontSize;

        properties.upColor = animationProperties_[currentIndex - 1].currentColor;
        animationProperties_[currentIndex - 1].downColor = properties.currentColor;
    }
    animationProperties_.emplace_back(properties);
}

void TextPickerColumnPattern::UpdatePickerTextProperties(const RefPtr<TextLayoutProperty>& textLayoutProperty,
    const RefPtr<TextPickerLayoutProperty>& textPickerLayoutProperty,
    uint32_t currentIndex, uint32_t middleIndex, uint32_t showCount)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = host->GetContext();
    CHECK_NULL_VOID(context);
    auto pickerTheme = context->GetTheme<PickerTheme>();
    CHECK_NULL_VOID(pickerTheme);
    if (currentIndex < middleIndex) {
        if (currentIndex == 0) {
            UpdateDisappearTextProperties(pickerTheme, textLayoutProperty, textPickerLayoutProperty);
        } else {
            UpdateCandidateTextProperties(pickerTheme, textLayoutProperty, textPickerLayoutProperty);
        }
        textLayoutProperty->UpdateAlignment(Alignment::TOP_CENTER);
    }
    if (currentIndex == middleIndex) {
        UpdateSelectedTextProperties(pickerTheme, textLayoutProperty, textPickerLayoutProperty);
        textLayoutProperty->UpdateAlignment(Alignment::CENTER);
    }
    if (currentIndex > middleIndex) {
        if (currentIndex == showCount - 1) {
            UpdateDisappearTextProperties(pickerTheme, textLayoutProperty, textPickerLayoutProperty);
        } else {
            UpdateCandidateTextProperties(pickerTheme, textLayoutProperty, textPickerLayoutProperty);
        }
        textLayoutProperty->UpdateAlignment(Alignment::BOTTOM_CENTER);
    }
    textLayoutProperty->UpdateMaxLines(1);
    AddAnimationTextProperties(currentIndex, textLayoutProperty);
}

void TextPickerColumnPattern::TextPropertiesLinearAnimation(const RefPtr<TextLayoutProperty>& textLayoutProperty,
    uint32_t index, uint32_t showCount, bool isDown, double scale)
{
    if (index >= animationProperties_.size()) {
        LOGE("Animation Properties vactor is break.");
        return;
    }
    if ((!index && isDown) || ((index == (showCount - 1)) && !isDown)) {
        return;
    }
    Dimension startFontSize = animationProperties_[index].fontSize;
    Color startColor = animationProperties_[index].currentColor;
    Dimension endFontSize;
    Color endColor;
    if (!isDown) {
        endFontSize = animationProperties_[index].downFontSize;
        endColor = animationProperties_[index].downColor;
    } else {
        endFontSize = animationProperties_[index].upFontSize;
        endColor = animationProperties_[index].upColor;
    }
    Dimension updateSize = LinearFontSize(startFontSize, endFontSize, scale);
    textLayoutProperty->UpdateFontSize(updateSize);
    auto colorEvaluator = AceType::MakeRefPtr<LinearEvaluator<Color>>();
    Color updateColor = colorEvaluator->Evaluate(startColor, endColor, scale);
    textLayoutProperty->UpdateTextColor(updateColor);
}

void TextPickerColumnPattern::UpdateTextPropertiesLinear(bool isDown, double scale)
{
    if (columnkind_ == ICON) {
        return;
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    uint32_t showCount = GetShowOptionCount();
    auto child = host->GetChildren();
    auto iter = child.begin();
    if (child.size() != showCount) {
        return;
    }
    for (uint32_t index = 0; index < showCount; index++) {
        auto rangeNode = DynamicCast<FrameNode>(*iter);
        CHECK_NULL_VOID(rangeNode);
        RefPtr<TextLayoutProperty> textLayoutProperty;
        if (columnkind_ == TEXT) {
            auto textPattern = rangeNode->GetPattern<TextPattern>();
            CHECK_NULL_VOID(textPattern);
            textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
            CHECK_NULL_VOID(textLayoutProperty);
            TextPropertiesLinearAnimation(textLayoutProperty, index, showCount, isDown, scale);
        } else if (columnkind_ == MIXTURE) {
            auto children = rangeNode->GetChildren();
            if (children.size() != MIXTURE_CHILD_COUNT) {
                LOGE("children number is wrong.");
                continue;
            }
            auto textNode = DynamicCast<FrameNode>(rangeNode->GetLastChild());
            auto textPattern = textNode->GetPattern<TextPattern>();
            textLayoutProperty = textPattern->GetLayoutProperty<TextLayoutProperty>();
            CHECK_NULL_VOID(textLayoutProperty);
            TextPropertiesLinearAnimation(textLayoutProperty, index, showCount, isDown, scale);
            textNode->MarkModifyDone();
            textNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        }
        rangeNode->MarkModifyDone();
        rangeNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        iter++;
    }
}

Dimension TextPickerColumnPattern::LinearFontSize(const Dimension& startFontSize,
    const Dimension& endFontSize, double percent)
{
    return startFontSize + (endFontSize - startFontSize) * percent;
}

void TextPickerColumnPattern::InitPanEvent(const RefPtr<GestureEventHub>& gestureHub)
{
    CHECK_NULL_VOID_NOLOG(!panEvent_);
    auto actionStartTask = [weak = WeakClaim(this)](const GestureEvent& event) {
        LOGI("Pan event start");
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID_NOLOG(pattern);
        pattern->HandleDragStart(event);
    };
    auto actionUpdateTask = [weak = WeakClaim(this)](const GestureEvent& event) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID_NOLOG(pattern);
        pattern->HandleDragMove(event);
    };
    auto actionEndTask = [weak = WeakClaim(this)](const GestureEvent& info) {
        LOGI("Pan event end mainVelocity: %{public}lf", info.GetMainVelocity());
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID_NOLOG(pattern);
        if (info.GetInputEventType() == InputEventType::AXIS) {
            return;
        }
        pattern->HandleDragEnd();
    };
    auto actionCancelTask = [weak = WeakClaim(this)]() {
        LOGI("Pan event cancel");
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID_NOLOG(pattern);
        pattern->HandleDragEnd();
    };
    PanDirection panDirection;
    panDirection.type = PanDirection::VERTICAL;
    panEvent_ = MakeRefPtr<PanEvent>(
        std::move(actionStartTask), std::move(actionUpdateTask), std::move(actionEndTask), std::move(actionCancelTask));
    gestureHub->AddPanEvent(panEvent_, panDirection, DEFAULT_PAN_FINGER, DEFAULT_PAN_DISTANCE);
}

RefPtr<TextPickerLayoutProperty> TextPickerColumnPattern::GetParentLayout() const
{
    auto host = GetHost();
    auto stackNode = DynamicCast<FrameNode>(host->GetParent());
    auto parentNode = DynamicCast<FrameNode>(stackNode->GetParent());

    auto property = parentNode->GetLayoutProperty<TextPickerLayoutProperty>();
    return property;
}


void TextPickerColumnPattern::HandleDragStart(const GestureEvent& event)
{
    CHECK_NULL_VOID_NOLOG(GetHost());
    CHECK_NULL_VOID_NOLOG(GetToss());
    auto toss = GetToss();
    yOffset_ = event.GetGlobalPoint().GetY();
    toss->SetStart(yOffset_);
    yLast_ = yOffset_;
    pressed_ = true;
    auto frameNode = GetHost();
    CHECK_NULL_VOID(frameNode);
    frameNode->OnAccessibilityEvent(AccessibilityEventType::SCROLL_START);
}

void TextPickerColumnPattern::HandleDragMove(const GestureEvent& event)
{
    if (event.GetInputEventType() == InputEventType::AXIS) {
        int32_t step = LessNotEqual(event.GetDelta().GetY(), 0.0) ? 1 : -1;
        InnerHandleScroll(step);
        
        return;
    }

    CHECK_NULL_VOID_NOLOG(pressed_);
    CHECK_NULL_VOID_NOLOG(GetHost());
    CHECK_NULL_VOID_NOLOG(GetToss());
    auto toss = GetToss();
    double offsetY = event.GetGlobalPoint().GetY();
    if (NearEqual(offsetY, yLast_, 1.0)) { // if changing less than 1.0, no need to handle
        return;
    }

    toss->SetEnd(offsetY);
    UpdateColumnChildPosition(offsetY, true);
}

void TextPickerColumnPattern::HandleDragEnd()
{
    pressed_ = false;
    CHECK_NULL_VOID_NOLOG(GetHost());
    CHECK_NULL_VOID_NOLOG(GetToss());
    auto toss = GetToss();
    auto frameNode = GetHost();
    CHECK_NULL_VOID(frameNode);
    if (!NotLoopOptions() && toss->Play()) {
        frameNode->OnAccessibilityEvent(AccessibilityEventType::SCROLL_END);
        return;
    }
    yOffset_ = 0.0;
    yLast_ = 0.0;
    if (!animationCreated_) {
        ScrollOption(0.0);
        return;
    }
    auto curve = CreateAnimation(scrollDelta_, 0.0);
    fromController_->ClearInterpolators();
    fromController_->AddInterpolator(curve);
    fromController_->Play();
    frameNode->OnAccessibilityEvent(AccessibilityEventType::SCROLL_END);
}

void TextPickerColumnPattern::CreateAnimation()
{
    CHECK_NULL_VOID_NOLOG(!animationCreated_);
    toController_ = CREATE_ANIMATOR(PipelineContext::GetCurrentContext());
    toController_->SetDuration(ANIMATION_ZERO_TO_OUTER); // 200ms for animation that from zero to outer.
    auto weak = AceType::WeakClaim(this);
    toController_->AddStopListener([weak]() {
        auto column = weak.Upgrade();
        if (column) {
            column->HandleCurveStopped();
        } else {
            LOGE("timepicker column is null.");
        }
    });
    fromBottomCurve_ = CreateAnimation(jumpInterval_, 0.0);
    fromTopCurve_ = CreateAnimation(0.0 - jumpInterval_, 0.0);
    fromController_ = CREATE_ANIMATOR(PipelineContext::GetCurrentContext());
    fromController_->SetDuration(ANIMATION_OUTER_TO_ZERO);
    animationCreated_ = true;
}

RefPtr<CurveAnimation<double>> TextPickerColumnPattern::CreateAnimation(double from, double to)
{
    auto weak = AceType::WeakClaim(this);
    auto curve = AceType::MakeRefPtr<CurveAnimation<double>>(from, to, Curves::FRICTION);
    curve->AddListener(Animation<double>::ValueCallback([weak](double value) {
        auto column = weak.Upgrade();
        CHECK_NULL_VOID(column);
        column->ScrollOption(value);
    }));
    return curve;
}

void TextPickerColumnPattern::HandleCurveStopped()
{
    CHECK_NULL_VOID_NOLOG(animationCreated_);
    if (NearZero(scrollDelta_)) {
        return;
    }
    ScrollOption(0.0 - scrollDelta_);
    int32_t step = GreatNotEqual(scrollDelta_, 0.0) ? 1 : -1;
    InnerHandleScroll(step);
    fromController_->ClearInterpolators();
    if (LessNotEqual(scrollDelta_, 0.0)) {
        fromController_->AddInterpolator(fromTopCurve_);
    } else {
        fromController_->AddInterpolator(fromBottomCurve_);
    }
    fromController_->Play();
}

void TextPickerColumnPattern::ScrollOption(double delta, bool isJump)
{
    UpdateScrollDelta(delta);
    double oldDelta = scrollDelta_;
    scrollDelta_ = delta;

    if ((isJump_ && LessOrEqual(delta * oldDelta, 0.0)) || NearZero(delta) || (isJump_ && isJump)) {
        isJump_ = false;
        FlushCurrentOptions();
        return;
    }
    if (isJump) {
        isJump_ = true;
    }
    if (NearZero(jumpInterval_)) {
        return;
    }
    double scale = 0.0;
    if (!isJump_) {
        scale = fabs(delta) / (jumpInterval_ * 2);
        UpdateTextPropertiesLinear(LessNotEqual(delta, 0.0), scale);
    } else {
        scale = ((2 * jumpInterval_) - fabs(delta))  / (jumpInterval_ * 2);
        UpdateTextPropertiesLinear(!LessNotEqual(delta, 0.0), scale);
    }
}

void TextPickerColumnPattern::UpdateScrollDelta(double delta)
{
    SetCurrentOffset(delta);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
}

void TextPickerColumnPattern::UpdateToss(double offsetY)
{
    UpdateColumnChildPosition(offsetY);
}

void TextPickerColumnPattern::TossStoped()
{
    yOffset_ = 0.0;
    yLast_ = 0.0;
    ScrollOption(0.0);
}

std::string TextPickerColumnPattern::GetSelectedObject(bool isColumnChange, int32_t status) const
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, "");
    auto value = GetOption(GetSelected());
    auto index = GetSelected();
    if (isColumnChange) {
        value = GetCurrentText();
        index = GetCurrentIndex();
    }

    auto context = host->GetContext();
    CHECK_NULL_RETURN(context, "");

    if (context->GetIsDeclarative()) {
        return std::string("{\"value\":") + "\"" + value + "\"" + ",\"index\":" + std::to_string(index) +
               ",\"status\":" + std::to_string(status) + "}";
    } else {
        return std::string("{\"newValue\":") + "\"" + value + "\"" + ",\"newSelected\":" + std::to_string(index) +
               ",\"status\":" + std::to_string(status) + "}";
    }
}

void TextPickerColumnPattern::UpdateColumnChildPosition(double offsetY, bool isUpatePropertiesOnly)
{
    yLast_ = offsetY;
    double dragDelta = yLast_ - yOffset_;
    if (!CanMove(LessNotEqual(dragDelta, 0))) {
        return;
    }

    auto context = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(context);
    auto pickerTheme = context->GetTheme<PickerTheme>();
    CHECK_NULL_VOID(pickerTheme);
    jumpInterval_ = pickerTheme->GetJumpInterval().ConvertToPx();
    // the abs of drag delta is less than jump interval.
    if (LessNotEqual(0.0 - jumpInterval_, dragDelta) && LessNotEqual(dragDelta, jumpInterval_)) {
        ScrollOption(dragDelta);
        return;
    }

    int32_t step = LessNotEqual(dragDelta, 0.0) ? 1 : -1;
    InnerHandleScroll(step, isUpatePropertiesOnly);

    double jumpDelta = (LessNotEqual(dragDelta, 0.0) ? jumpInterval_ : 0.0 - jumpInterval_);
    ScrollOption(jumpDelta, true);
    yOffset_ = offsetY - jumpDelta;
}

bool TextPickerColumnPattern::CanMove(bool isDown) const
{
    if (!NotLoopOptions()) {
        return true;
    }
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    int totalOptionCount = static_cast<int>(GetOptionCount());
    int currentIndex = static_cast<int>(GetCurrentIndex());
    int nextVirtualIndex = isDown ? currentIndex + 1 : currentIndex - 1;
    return nextVirtualIndex >= 0 && nextVirtualIndex < totalOptionCount;
}

bool TextPickerColumnPattern::NotLoopOptions() const
{
    RefPtr<TextPickerLayoutProperty> layout = GetParentLayout();
    bool canLoop = layout->GetCanLoop().value();
    return !canLoop;
}

bool TextPickerColumnPattern::InnerHandleScroll(int32_t step, bool isUpatePropertiesOnly)
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto totalOptionCount = GetOptionCount();

    CHECK_NULL_RETURN(host, false);
    if (totalOptionCount == 0) {
        return false;
    }

    int32_t currentIndex = GetCurrentIndex();
    RefPtr<TextPickerLayoutProperty> layout = GetParentLayout();
    CHECK_NULL_RETURN(host, false);

    bool canLoop = layout->GetCanLoop().value_or(true);
    if (!canLoop) {
        // scroll down
        if (step > 0) {
            currentIndex = (currentIndex + step) > (totalOptionCount - 1) ?
                totalOptionCount - 1 : currentIndex + step;
             
        // scroll up
        } else if (step < 0) {
            currentIndex = currentIndex + step < 0 ? 0 : currentIndex + step;
        }
    } else {
        currentIndex = (totalOptionCount + currentIndex + step) % totalOptionCount;
    }

    SetCurrentIndex(currentIndex);
    bool isDown = step > 0 ? true : false;
    HandleChangeCallback(isDown, true);
    FlushCurrentOptions(isDown, isUpatePropertiesOnly);
    
    return true;
}

bool TextPickerColumnPattern::OnKeyEvent(const KeyEvent& event)
{
    if (event.action != KeyAction::DOWN) {
        return false;
    }
    if (event.code == KeyCode::KEY_DPAD_UP || event.code == KeyCode::KEY_DPAD_DOWN ||
        event.code == KeyCode::KEY_DPAD_LEFT || event.code == KeyCode::KEY_DPAD_RIGHT) {
        HandleDirectionKey(event.code);
        return true;
    }
    return false;
}

bool TextPickerColumnPattern::HandleDirectionKey(KeyCode code)
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto currernIndex = GetCurrentIndex();
    auto totalOptionCount = GetOptionCount();
    if (totalOptionCount == 0) {
        return false;
    }
    if (code == KeyCode::KEY_DPAD_UP) {
        SetCurrentIndex((totalOptionCount + currernIndex - 1) % totalOptionCount);
        FlushCurrentOptions();
        host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        return true;
    }
    if (code == KeyCode::KEY_DPAD_DOWN) {
        SetCurrentIndex((totalOptionCount + currernIndex + 1) % totalOptionCount);
        FlushCurrentOptions();
        host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        return true;
    }
    return false;
}
} // namespace OHOS::Ace::NG
