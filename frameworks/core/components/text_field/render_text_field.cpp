/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components/text_field/render_text_field.h"

#include "base/geometry/dimension.h"
#include "base/i18n/localization.h"
#include "base/json/json_util.h"
#include "base/log/dump_log.h"
#include "base/log/log_wrapper.h"
#include "base/mousestyle/mouse_style.h"
#include "base/subwindow/subwindow_manager.h"
#include "base/utils/string_utils.h"
#include "base/utils/utils.h"
#include "core/animation/curve_animation.h"
#include "core/common/clipboard/clipboard_proxy.h"
#include "core/common/container_scope.h"
#include "core/common/font_manager.h"
#include "core/common/text_field_manager.h"
#include "core/components/stack/stack_element.h"
#include "core/components/text/text_utils.h"
#include "core/components/text_overlay/text_overlay_component.h"
#include "core/components/text_overlay/text_overlay_element.h"
#include "core/components_v2/inspector/utils.h"
#include "core/event/ace_event_helper.h"
#include "core/event/mouse_event.h"

#if defined(ENABLE_STANDARD_INPUT)
#include "core/components/text_field/on_text_changed_listener_impl.h"
#endif

namespace OHOS::Ace {
namespace {

constexpr uint32_t TWINKLING_INTERVAL_MS = 500;
// Tick count indicate how long should the naked character should be displayed while obscure_ == true.
constexpr uint32_t OBSCURE_SHOW_TICKS = 3;
constexpr double HANDLE_HOT_ZONE = 10.0;

constexpr char16_t OBSCURING_CHARACTER = u'•';
constexpr char16_t OBSCURING_CHARACTER_FOR_AR = u'*';

constexpr int32_t DEFAULT_SELECT_INDEX = 0;
constexpr int32_t SHOW_HANDLE_DURATION = 250;
constexpr int32_t DOUBLE_CLICK_FINGERS = 1;
constexpr int32_t DOUBLE_CLICK_COUNTS = 2;
constexpr double FIFTY_PERCENT = 0.5;

constexpr Dimension OFFSET_FOCUS = 4.0_vp;
constexpr Dimension DEFLATE_RADIUS_FOCUS = 3.0_vp;

// Whether the system is Mac or not determines which key code is selected.
#if defined(MAC_PLATFORM)
#define KEY_META_OR_CTRL_LEFT KeyCode::KEY_META_LEFT
#define KEY_META_OR_CTRL_RIGHT KeyCode::KEY_META_RIGHT
#else
#define KEY_META_OR_CTRL_LEFT KeyCode::KEY_CTRL_LEFT
#define KEY_META_OR_CTRL_RIGHT KeyCode::KEY_CTRL_RIGHT
#endif

} // namespace

#if defined(ENABLE_STANDARD_INPUT)
void RenderTextField::UpdateConfiguration()
{
    MiscServices::Configuration configuration;
    configuration.SetEnterKeyType(static_cast<MiscServices::EnterKeyType>((int32_t)action_));
    configuration.SetTextInputType(static_cast<MiscServices::TextInputType>((int32_t)keyboard_));
    MiscServices::InputMethodController::GetInstance()->OnConfigurationChange(configuration);
}
#endif

RenderTextField::RenderTextField()
    : twinklingInterval(TWINKLING_INTERVAL_MS), controller_(AceType::MakeRefPtr<TextEditController>())
{}

RenderTextField::~RenderTextField()
{
    LOGI("Destruction text field.");
    if (controller_) {
        controller_->Clear();
        controller_->RemoveObserver(WeakClaim(this));
    }
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        return;
    }
    PopTextOverlay();
    pipelineContext->RemoveFontNode(AceType::WeakClaim(this));
    auto fontManager = pipelineContext->GetFontManager();
    if (fontManager) {
        fontManager->UnRegisterCallback(AceType::WeakClaim(this));
        fontManager->RemoveVariationNode(WeakClaim(this));
    }

    // If soft keyboard is still exist, close it.
    if (HasConnection()) {
#if defined(ENABLE_STANDARD_INPUT)
        LOGI("Destruction text field, close input method.");
        MiscServices::InputMethodController::GetInstance()->Close();
#else
        connection_->Close(GetInstanceId());
        connection_ = nullptr;
#endif
    }
}

void RenderTextField::Update(const RefPtr<Component>& component)
{
    const RefPtr<TextFieldComponent> textField = AceType::DynamicCast<TextFieldComponent>(component);
    if (!textField) {
        return;
    }

    // Clear children to avoid children increase.
    ClearChildren();

    if (textField->IsTextLengthLimited()) {
        maxLength_ = textField->GetMaxLength();
    }

    copyOption_ = textField->GetCopyOption();
    selection_ = textField->GetSelection();
    placeholder_ = textField->GetPlaceholder();
    inputFilter_ = textField->GetInputFilter();
    inactivePlaceholderColor_ = textField->GetPlaceholderColor();
    focusPlaceholderColor_ = textField->GetFocusPlaceholderColor();
    focusBgColor_ = textField->GetFocusBgColor();
    focusTextColor_ = textField->GetFocusTextColor();
    selectedColor_ = textField->GetSelectedColor();
    pressColor_ = textField->GetPressColor();
    hoverColor_ = textField->GetHoverColor();
    hoverAnimationType_ = textField->GetHoverAnimationType();
    decoration_ = textField->GetDecoration();
    inactiveBgColor_ = textField->GetBgColor();
    if (decoration_ && (decoration_->GetImage() || decoration_->GetGradient().IsValid())) {
        inactiveBgColor_ = Color::TRANSPARENT;
        focusBgColor_ = Color::TRANSPARENT;
    }
    originBorder_ = textField->GetOriginBorder();
    if (style_ != textField->GetTextStyle()) {
        ResetStatus();
    }
    style_ = textField->GetTextStyle();
    placeHoldStyle_ = textField->GetPlaceHoldStyle();
    editingStyle_ = textField->GetEditingStyle();
    fontSize_ = style_.GetFontSize();
    errorTextStyle_ = textField->GetErrorTextStyle();
    errorSpacingInDimension_ = textField->GetErrorSpacing();
    errorIsInner_ = textField->GetErrorIsInner();
    errorBorderWidth_ = textField->GetErrorBorderWidth();
    errorBorderColor_ = textField->GetErrorBorderColor();
    needFade_ = textField->NeedFade();
    inactiveTextColor_ = style_.GetTextColor();
    maxLines_ = textField->GetTextMaxLines();
    onTextChangeEvent_ = AceAsyncEvent<void(const std::string&)>::Create(textField->GetOnTextChange(), context_);
    onError_ = textField->GetOnError();
    onValueChangeEvent_ = textField->GetOnTextChange().GetUiStrFunction();
    if (textField->GetOnChange()) {
        onChange_ = *textField->GetOnChange();
    }
    if (textField->GetOnEditChanged()) {
        onEditChanged_ = *textField->GetOnEditChanged();
    }
    if (textField->GetOnSubmit()) {
        onSubmit_ = *textField->GetOnSubmit();
    }
    if (textField->GetOnClick()) {
        onClick_ = *textField->GetOnClick();
    }
    onSelectChangeEvent_ = AceAsyncEvent<void(const std::string&)>::Create(textField->GetOnSelectChange(), context_);
    onFinishInputEvent_ = AceAsyncEvent<void(const std::string&)>::Create(textField->GetOnFinishInput(), context_);
    onTapEvent_ = AceAsyncEvent<void()>::Create(textField->GetOnTap(), context_);
    catchMode_ = textField->GetOnTap().IsEmpty() || textField->GetOnTap().GetCatchMode();
    static const int32_t bubbleModeVersion = 6;
    auto pipeline = context_.Upgrade();
    if (!catchMode_) {
        if (pipeline && pipeline->GetMinPlatformVersion() >= bubbleModeVersion) {
            catchMode_ = false;
        } else {
            catchMode_ = true;
        }
    }
    onLongPressEvent_ = AceAsyncEvent<void()>::Create(textField->GetOnLongPress(), context_);
    textAlign_ = textField->GetTextAlign();
    textDirection_ = textField->GetTextDirection();
    realTextDirection_ = textDirection_;
    showCursor_ = textField->ShowCursor();
    UpdateObscure(textField);
    enabled_ = textField->IsEnabled();
    widthReserved_ = textField->GetWidthReserved();
    blockRightShade_ = textField->GetBlockRightShade();
    isVisible_ = textField->IsVisible();
    showPasswordIcon_ = textField->ShowPasswordIcon();
    if (textField->HasSetResetToStart() && textField->GetUpdateType() == UpdateType::ALL) {
        resetToStart_ = textField->GetResetToStart();
    }
    if (keyboard_ != TextInputType::UNSPECIFIED && keyboard_ != textField->GetTextInputType()) {
        CloseKeyboard();
    }
    if (keyboard_ != textField->GetTextInputType()) {
        auto context = context_.Upgrade();
        if (context && context->GetIsDeclarative()) {
            ClearEditingValue();
        } else {
            if (keyboard_ == TextInputType::VISIBLE_PASSWORD) {
                ClearEditingValue();
            }
        }
        keyboard_ = textField->GetTextInputType();
    }

    if (action_ != TextInputAction::UNSPECIFIED && action_ != textField->GetAction()) {
        auto context = context_.Upgrade();
        if (context && context->GetIsDeclarative()) {
            CloseKeyboard();
        }
    }
    if (action_ != textField->GetAction()) {
        action_ = textField->GetAction();
    }

    actionLabel_ = textField->GetActionLabel();
    height_ = textField->GetHeight();
    if (textField->IsCursorColorSet()) {
        cursorColorIsSet_ = true;
        cursorColor_ = textField->GetCursorColor();
    }
    cursorRadius_ = textField->GetCursorRadius();
    textFieldController_ = textField->GetTextFieldController();
    if (textFieldController_) {
        auto weak = AceType::WeakClaim(this);
        textFieldController_->SetCaretPosition([weak](int32_t caretPosition) {
            auto textField = weak.Upgrade();
            if (textField) {
                textField->UpdateSelection(caretPosition);
                textField->cursorPositionType_ = CursorPositionType::NORMAL;
                textField->MarkNeedLayout();
            }
        });
    }
    if (textField->GetTextEditController() && controller_ != textField->GetTextEditController()) {
        if (controller_) {
            controller_->RemoveObserver(WeakClaim(this));
        }
        controller_ = textField->GetTextEditController();
    }
    if (controller_) {
        controller_->RemoveObserver(WeakClaim(this));
        controller_->AddObserver(WeakClaim(this));
        controller_->SetHint(placeholder_);
        if (textField->IsValueUpdated()) {
            if (textField->GetValue() != GetEditingValue().text) {
                PopTextOverlay();
            }
            controller_->SetText(textField->GetValue(), false);
        }
    }
    // maybe change text and selection
    ApplyRestoreInfo();
    extend_ = textField->IsExtend();
    softKeyboardEnabled_ = textField->IsSoftKeyboardEnabled();
    text_ = textField->GetValue();
    showEllipsis_ = textField->ShowEllipsis();
    auto context = context_.Upgrade();
    if (!clipboard_ && context) {
        clipboard_ = ClipboardProxy::GetInstance()->GetClipboard(context->GetTaskExecutor());
    }

    if ((style_.IsAllowScale() || style_.GetFontSize().Unit() == DimensionUnit::FP) && context) {
        context->AddFontNode(AceType::WeakClaim(this));
    }

    showCounter_ = textField->ShowCounter();
    countTextStyle_ = textField->GetCountTextStyle();
    overCountStyle_ = textField->GetOverCountStyle();
    countTextStyleOuter_ = textField->GetCountTextStyleOuter();
    overCountStyleOuter_ = textField->GetOverCountStyleOuter();

    inputOptions_ = textField->GetInputOptions();
    onOptionsClick_ = textField->GetOnOptionsClick();
    onTranslate_ = textField->GetOnTranslate();
    onShare_ = textField->GetOnShare();
    onSearch_ = textField->GetOnSearch();
    inputStyle_ = textField->GetInputStyle();
    if (textField->IsSetFocusOnTouch()) {
        isFocusOnTouch_ = textField->IsFocusOnTouch();
    }
#if defined(ENABLE_STANDARD_INPUT)
    UpdateConfiguration();
#endif
    SetCallback(textField);
    UpdateFormatters();
    UpdateFocusStyles();
    UpdateIcon(textField);
    RegisterFontCallbacks();
    MarkNeedLayout();
    UpdateAccessibilityAttr();
}

void RenderTextField::SetCallback(const RefPtr<TextFieldComponent>& textField)
{
    if (textField->GetOnCopy()) {
        onCopy_ = *textField->GetOnCopy();
    }
    if (textField->GetOnCut()) {
        onCut_ = *textField->GetOnCut();
    }
    if (textField->GetOnPaste()) {
        onPaste_ = *textField->GetOnPaste();
    }
}

void RenderTextField::OnPaintFinish()
{
    UpdateFocusAnimation();
    UpdateOverlay();
    InitAccessibilityEventListener();
    UpdateAccessibilityPosition();
    auto layoutParamChanged = lastLayoutParam_.value() == GetLayoutParam();
    if (layoutParamChanged) {
        lastLayoutParam_ = std::make_optional(GetLayoutParam());
    }
    bool needNotifyChangeEvent = !isValueFromFront_ || layoutParamChanged;
    // If height or lines is changed, make needNotifyChangeEvent_ true to notify change event.
    if (needNotifyChangeEvent && (!NearEqual(textHeight_, textHeightLast_) || textLines_ != textLinesLast_)) {
        needNotifyChangeEvent_ = true;
        textHeightLast_ = textHeight_;
        textLinesLast_ = textLines_;
    }
    if (needNotifyChangeEvent_ && (onTextChangeEvent_ || onValueChangeEvent_ || onChange_)) {
        needNotifyChangeEvent_ = false;
        if (onValueChangeEvent_) {
            onValueChangeEvent_(GetEditingValue().text);
        }
        if (onTextChangeEvent_) {
            auto jsonResult = JsonUtil::Create(true);
            jsonResult->Put("text", GetEditingValue().text.c_str());
            jsonResult->Put("value", GetEditingValue().text.c_str());
            jsonResult->Put("lines", textLines_);
            jsonResult->Put("height", textHeight_);
            onTextChangeEvent_(std::string(R"("change",)").append(jsonResult->ToString()));
        }
    }
}

void RenderTextField::PerformLayout()
{
    if (!lastLayoutParam_.has_value()) {
        lastLayoutParam_ = std::make_optional(GetLayoutParam());
    }

    if (GetEditingValue().text.empty()) {
        cursorPositionType_ = CursorPositionType::END;
    }

    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        const auto& currentText = controller_->GetValue().text;
        showPlaceholder_ = currentText.empty();
        if (showPlaceholder_) {
            SetTextStyle(placeHoldStyle_);
        } else {
            SetTextStyle(editingStyle_);
        }
    }

    auto pipelineContext = GetContext().Upgrade();
    if ((style_.IsAllowScale() || style_.GetFontSize().Unit() == DimensionUnit::FP) && pipelineContext &&
        !NearEqual(fontScale_, pipelineContext->GetFontScale())) {
        fontScale_ = pipelineContext->GetFontScale();
        style_.SetFontSize(fontSize_ * fontScale_);
    }

    iconSize_ = NormalizeToPx(iconSizeInDimension_);
    iconHotZoneSize_ = NormalizeToPx(iconHotZoneSizeInDimension_);
    errorSpacing_ = NormalizeToPx(errorSpacingInDimension_);
    if (!GetChildren().empty()) {
        auto innerLayout = GetLayoutParam();
        innerLayout.SetMinSize(Size());
        const auto& child = GetChildren().front();
        child->Layout(innerLayout);
    }
    ApplyAspectRatio();
    SetLayoutSize(GetLayoutParam().Constrain(Measure()));
    UpdateFocusAnimation();

    LayoutParam layoutParam = GetLayoutParam();
    layoutParam.SetMinSize(Size());
    if (iconImage_) {
        iconImage_->Layout(layoutParam);
    }
    if (renderShowIcon_) {
        renderShowIcon_->Layout(layoutParam);
    }
    if (renderHideIcon_) {
        renderHideIcon_->Layout(layoutParam);
    }

    HandleDeviceOrientationChange();
}

bool RenderTextField::HandleMouseEvent(const MouseEvent& event)
{
    if (event.button == MouseButton::LEFT_BUTTON) {
        if (event.action == MouseAction::PRESS) {
            UpdateStartSelection(DEFAULT_SELECT_INDEX, event.GetOffset(), true, false);
            AddOutOfRectCallbackToContext();
        } else if (event.action == MouseAction::MOVE) {
            int32_t start = GetEditingValue().selection.baseOffset;
            int32_t end = GetCursorPositionForClick(event.GetOffset());
            UpdateSelection(start, end);
            MarkNeedRender();
        } else {
            LOGD("on left button release");
        }
    }

    if (event.button == MouseButton::RIGHT_BUTTON && event.action == MouseAction::PRESS) {
        Offset rightClickOffset = event.GetOffset();
        ShowTextOverlay(rightClickOffset, false, true);
        AddOutOfRectCallbackToContext();
    }

    return false;
}

void RenderTextField::HandleMouseHoverEvent(MouseState mouseState)
{
    auto pipeline = context_.Upgrade();
    if (!pipeline) {
        return;
    }
    int32_t windowId = pipeline->GetWindowId();
    auto mouseStyle = MouseStyle::CreateMouseStyle();
    MouseFormat defaultStyle = MouseFormat::DEFAULT;
    MouseFormat textCursorStyle = MouseFormat::TEXT_CURSOR;
    if (mouseState == MouseState::HOVER) {
        mouseStyle->SetPointerStyle(windowId, textCursorStyle);
    } else {
        mouseStyle->SetPointerStyle(windowId, defaultStyle);
    }
}

void RenderTextField::OnTouchTestHit(
    const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result)
{
    if (!enabled_) {
        return;
    }
    if (!clickRecognizer_) {
        clickRecognizer_ = AceType::MakeRefPtr<ClickRecognizer>();
        clickRecognizer_->SetUseCatchMode(catchMode_);
        auto weak = WeakClaim(this);
        clickRecognizer_->SetOnClick([weak](const ClickInfo& info) {
            auto client = weak.Upgrade();
            if (client) {
                client->OnClick(info);
            }
        });
        clickRecognizer_->SetPriority(GesturePriority::Low);
    }
    clickRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(clickRecognizer_);

    if (!doubleClickRecognizer_) {
        doubleClickRecognizer_ =
            AceType::MakeRefPtr<ClickRecognizer>(context_, DOUBLE_CLICK_FINGERS, DOUBLE_CLICK_COUNTS);
        doubleClickRecognizer_->SetUseCatchMode(catchMode_);
        auto weak = WeakClaim(this);
        doubleClickRecognizer_->SetOnClick([weak](const ClickInfo& info) {
            auto client = weak.Upgrade();
            if (client) {
                client->OnDoubleClick(info);
            }
        });
        doubleClickRecognizer_->SetPriority(GesturePriority::High);
    }
    doubleClickRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(doubleClickRecognizer_);

    if (!longPressRecognizer_) {
        longPressRecognizer_ = AceType::MakeRefPtr<LongPressRecognizer>(context_);
        auto weak = WeakClaim(this);
        longPressRecognizer_->SetOnLongPress([weak = WeakClaim(this)](const LongPressInfo& info) {
            auto client = weak.Upgrade();
            if (client) {
                client->OnLongPress(info);
            }
        });
        longPressRecognizer_->SetPriority(GesturePriority::High);
    }
    longPressRecognizer_->SetCoordinateOffset(coordinateOffset);
    longPressRecognizer_->SetTouchRestrict(touchRestrict);
    result.emplace_back(longPressRecognizer_);

    if (!rawRecognizer_) {
        rawRecognizer_ = AceType::MakeRefPtr<RawRecognizer>();
        auto weak = WeakClaim(this);
        rawRecognizer_->SetOnTouchDown([weak = WeakClaim(this)](const TouchEventInfo& info) {
            auto textField = weak.Upgrade();
            if (textField) {
                textField->StartPressAnimation(true);
            }
        });

        rawRecognizer_->SetOnTouchUp([weak = WeakClaim(this)](const TouchEventInfo& info) {
            auto textField = weak.Upgrade();
            if (textField) {
                textField->StartPressAnimation(false);
                textField->OnTapCallback();
            }
        });

        rawRecognizer_->SetOnTouchCancel([weak = WeakClaim(this)](const TouchEventInfo& info) {
            auto textField = weak.Upgrade();
            if (textField) {
                textField->StartPressAnimation(false);
            }
        });
    }
    rawRecognizer_->SetTouchRestrict(touchRestrict);
    rawRecognizer_->SetCoordinateOffset(coordinateOffset);
    result.emplace_back(rawRecognizer_);
}

void RenderTextField::StartPressAnimation(bool pressDown)
{
    if (!pressController_) {
        pressController_ = AceType::MakeRefPtr<Animator>(context_);
    }
    if (pressController_->IsRunning()) {
        pressController_->Stop();
    }
    if (hoverController_ && hoverController_->IsRunning()) {
        hoverController_->Stop();
    }
    pressController_->ClearInterpolators();
    RefPtr<KeyframeAnimation<Color>> animation = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    if (pressDown) {
        CreateMouseAnimation(animation, GetEventEffectColor(), pressColor_);
    } else {
        CreateMouseAnimation(animation, GetEventEffectColor(), Color::TRANSPARENT);
    }
    pressController_->AddInterpolator(animation);
    pressController_->SetDuration(PRESS_DURATION);
    pressController_->SetFillMode(FillMode::FORWARDS);
    pressController_->Forward();
}

void RenderTextField::StartHoverAnimation(bool isHovered)
{
    if (pressController_ && pressController_->IsRunning()) {
        return;
    }
    if (!hoverController_) {
        hoverController_ = AceType::MakeRefPtr<Animator>(context_);
    }
    if (hoverController_->IsRunning()) {
        hoverController_->Stop();
    }
    hoverController_->ClearInterpolators();
    RefPtr<KeyframeAnimation<Color>> animation = AceType::MakeRefPtr<KeyframeAnimation<Color>>();
    if (isHovered) {
        CreateMouseAnimation(animation, GetEventEffectColor(), hoverColor_);
    } else {
        CreateMouseAnimation(animation, GetEventEffectColor(), Color::TRANSPARENT);
    }
    hoverController_->AddInterpolator(animation);
    hoverController_->SetDuration(HOVER_DURATION);
    hoverController_->SetFillMode(FillMode::FORWARDS);
    hoverController_->Forward();
}

void RenderTextField::AnimateMouseHoverEnter()
{
    StartHoverAnimation(true);
}

void RenderTextField::AnimateMouseHoverExit()
{
    StartHoverAnimation(false);
}

void RenderTextField::OnClick(const ClickInfo& clickInfo)
{
    // Handle click on password icon when password icon is valid, switch between show and hide icon.
    Point clickPoint = Point(clickInfo.GetLocalLocation().GetX(), clickInfo.GetLocalLocation().GetY());
    if (showPasswordIcon_ && passwordIconRect_.IsInRegion(clickPoint)) {
        obscure_ = !obscure_;
        passwordRecord_ = obscure_;
        PopTextOverlay();
        MarkNeedLayout();
        return;
    }

    isValueFromRemote_ = false;
    auto globalPosition = clickInfo.GetGlobalLocation();
    auto globalOffset = GetGlobalOffset();

    if (SearchAction(globalPosition, globalOffset)) {
        return;
    }
    if (!onTapCallbackResult_) {
        return;
    }
    if (onTapEvent_) {
        onTapEvent_();
    }
    if (onClick_) {
        onClick_(clickInfo);
    }
    CursorMoveOnClick(globalPosition);
    ShowError("", false);
    UpdateStartSelection(DEFAULT_SELECT_INDEX, globalPosition, true, false);
    if (clickInfo.GetSourceDevice() == SourceType::MOUSE) {
        StartTwinkling();
    } else {
        StartTwinkling();
        ShowTextOverlay(globalPosition, true);
    }
    auto context = GetContext().Upgrade();
    if (context) {
        context->SetClickPosition(GetGlobalOffset() + Size(0, GetLayoutSize().Height()));
    }
    AddOutOfRectCallbackToContext();
}

void RenderTextField::OnTapCallback()
{
    if (isFocusOnTouch_ && tapCallback_) {
        onTapCallbackResult_ = tapCallback_();
    }
}

void RenderTextField::OnEditChange(bool isInEditStatus)
{
    CHECK_NULL_VOID(onEditChanged_);
    if (isInEditStatus && !isInEditStatus_) {
        isInEditStatus_ = true;
        onEditChanged_(true);
    } else if (!isInEditStatus && isInEditStatus_) {
        isInEditStatus_ = false;
        onEditChanged_(false);
    }
}

void RenderTextField::AddOutOfRectCallbackToContext()
{
    auto context = GetContext().Upgrade();
    CHECK_NULL_VOID(context);
    OutOfRectTouchCallback outRectCallback = [weak = WeakClaim(this)]() {
        auto render = weak.Upgrade();
        if (render) {
            if (render->isOverlayShowed_) {
                render->PopTextOverlay();
            }
            render->StopTwinkling();
            render->CloseKeyboard();
            render->OnEditChange(false);
        }
    };
    OutOfRectGetRectCallback getRectCallback = [weak = WeakClaim(this)](std::vector<Rect>& resRectList) {
        auto render = weak.Upgrade();
        if (render) {
            render->GetFieldAndOverlayTouchRect(resRectList);
        }
    };
    context->AddRectCallback(getRectCallback, outRectCallback, outRectCallback);
}

void RenderTextField::GetFieldAndOverlayTouchRect(std::vector<Rect>& resRectList)
{
    auto context = GetContext().Upgrade();
    CHECK_NULL_VOID(context);
    resRectList.clear();
    auto fieldTouchRectList = GetTouchRectList();
    for (auto& rect : fieldTouchRectList) {
        rect.SetOffset(GetGlobalOffset());
    }
    resRectList.insert(resRectList.end(), fieldTouchRectList.begin(), fieldTouchRectList.end());
    auto textOverlayManager = context->GetTextOverlayManager();
    if (textOverlayManager) {
        auto overlayTouchRectList = textOverlayManager->GetTextOverlayRect();
        resRectList.insert(resRectList.end(), overlayTouchRectList.begin(), overlayTouchRectList.end());
    }
}

bool RenderTextField::SearchAction(const Offset& globalPosition, const Offset& globalOffset)
{
    double widthReserved = NormalizeToPx(widthReserved_);
    if (widthReserved > 0) {
        if (textDirection_ == TextDirection::RTL) {
            if ((globalPosition.GetX() - globalOffset.GetX()) < widthReserved) {
                controller_->SetText("");
                return true;
            } else if ((globalPosition.GetX() - globalOffset.GetX()) > (GetLayoutSize().Width() - iconHotZoneSize_) &&
                       iconImage_ && action_ == TextInputAction::SEARCH) {
                PerformAction(action_, true);
                return true;
            }
        } else {
            if ((globalPosition.GetX() - globalOffset.GetX()) >= (GetLayoutSize().Width() - widthReserved)) {
                controller_->SetText("");
                return true;
            } else if ((globalPosition.GetX() - globalOffset.GetX()) < iconHotZoneSize_ && iconImage_ &&
                       action_ == TextInputAction::SEARCH) {
                PerformAction(action_, true);
                return true;
            }
        }
    }
    return false;
}

void RenderTextField::OnDoubleClick(const ClickInfo& clickInfo)
{
    auto clickPosition = GetCursorPositionForClick(clickInfo.GetGlobalLocation());
    auto selection = TextUtils::GetRangeOfSameType(GetEditingValue().text, clickPosition - 1);
    UpdateSelection(selection.GetStart(), selection.GetEnd());
    LOGI("text field accept double click, position: %{public}d, selection: %{public}s", clickPosition,
        selection.ToString().c_str());
    MarkNeedRender();
    AddOutOfRectCallbackToContext();
}

void RenderTextField::OnLongPress(const LongPressInfo& longPressInfo)
{
    if (isFocusOnTouch_ && tapCallback_ && !isOverlayShowed_) {
        if (!tapCallback_()) {
            return;
        }
    }

    if (onLongPressEvent_) {
        onLongPressEvent_();
    }

    ShowError("", false);

    if (longPressInfo.GetSourceDevice() == SourceType::MOUSE) {
        return;
    }

    Offset longPressPosition = longPressInfo.GetGlobalLocation();
    bool isTextEnd =
        (static_cast<size_t>(GetCursorPositionForClick(longPressPosition)) == GetEditingValue().GetWideText().length());
    bool singleHandle = isTextEnd || GetEditingValue().text.empty();
    bool isPassword = (keyboard_ == TextInputType::VISIBLE_PASSWORD);
    UpdateStartSelection(DEFAULT_SELECT_INDEX, longPressPosition, singleHandle || isPassword, true);
    ShowTextOverlay(longPressPosition, false);
    AddOutOfRectCallbackToContext();
}

void RenderTextField::ShowTextOverlay(const Offset& showOffset, bool isSingleHandle, bool isUsingMouse)
{
    if (!isVisible_) {
        return;
    }

    if (!IsSelectiveDevice()) {
        StartTwinkling();
        return;
    }

    isSingleHandle_ = isSingleHandle;

    auto selStart = GetEditingValue().selection.GetStart();
    auto selEnd = GetEditingValue().selection.GetEnd();

    Offset startHandleOffset = GetHandleOffset(selStart);
    Offset endHandleOffset = isSingleHandle ? startHandleOffset : GetHandleOffset(selEnd);

    if (isOverlayShowed_ && updateHandlePosition_) {
        Rect caretStart;
        bool visible = GetCaretRect(selStart, caretStart) ? IsVisible(caretStart + textOffsetForShowCaret_) : false;
        OverlayShowOption option { .showMenu = isOverlayShowed_,
            .showStartHandle = visible,
            .showEndHandle = visible,
            .isSingleHandle = isSingleHandle,
            .updateOverlayType = isSingleHandle ? UpdateOverlayType::CLICK : UpdateOverlayType::LONG_PRESS,
            .startHandleOffset = startHandleOffset,
            .endHandleOffset = endHandleOffset };
        if (!isSingleHandle_ || startHandleOffset != endHandleOffset) {
            isOverlayFocus_ = true;
        }
        updateHandlePosition_(option);

        // When the textOverlay is showed, restart the animation
        if (!animator_) {
            LOGE("Show textOverlay error, animator is nullptr");
            return;
        }
        if (!animator_->IsStopped()) {
            animator_->Stop();
        }
        animator_->Play();
        return;
    }

    // Pop text overlay before push.
    PopTextOverlay();

    textOverlay_ =
        AceType::MakeRefPtr<TextOverlayComponent>(GetThemeManager(), context_.Upgrade()->GetAccessibilityManager());
    textOverlay_->SetWeakTextField(WeakClaim(this));
    textOverlay_->SetIsSingleHandle(isSingleHandle || (keyboard_ == TextInputType::VISIBLE_PASSWORD));
    textOverlay_->SetLineHeight(selectHeight_);
    textOverlay_->SetClipRect(
        innerRect_ + Size(HANDLE_HOT_ZONE, HANDLE_HOT_ZONE) + GetOffsetToPage() - Offset(HANDLE_HOT_ZONE / 2.0, 0.0));
    textOverlay_->SetTextDirection(textDirection_);
    textOverlay_->SetRealTextDirection(existStrongDirectionLetter_ ? realTextDirection_ : TextDirection::LTR);
    textOverlay_->SetIsPassword(keyboard_ == TextInputType::VISIBLE_PASSWORD);
    textOverlay_->SetStartHandleOffset(startHandleOffset);
    textOverlay_->SetEndHandleOffset(endHandleOffset);
    textOverlay_->SetImageFill(imageFill_);
    textOverlay_->SetOptions(inputOptions_);
    textOverlay_->SetOptionsClickMarker(onOptionsClick_);
    textOverlay_->SetTranslateButtonMarker(onTranslate_);
    textOverlay_->SetShareButtonMarker(onShare_);
    textOverlay_->SetSearchButtonMarker(onSearch_);
    textOverlay_->SetContext(context_);
    textOverlay_->SetIsUsingMouse(isUsingMouse);
    // Add the Animation
    InitAnimation();

    if (!isSingleHandle_ || startHandleOffset != endHandleOffset) {
        isOverlayFocus_ = true;
    }
    RegisterCallbacksToOverlay();
}

void RenderTextField::InitAnimation()
{
    if (!textOverlay_) {
        LOGE("InitAnimation error, textOverlay is nullptr");
        return;
    }

    // Get the handleDiameter in theme, textoverlay is not nullptr
    double initHandleDiameter = textOverlay_->GetHandleDiameter().Value();
    double initHandleDiameterInner = textOverlay_->GetHandleDiameterInner().Value();

    // Add the animation for handleDiameter
    auto diameterAnimation = AceType::MakeRefPtr<CurveAnimation<double>>(
        initHandleDiameter * FIFTY_PERCENT, initHandleDiameter, Curves::ELASTICS);
    diameterAnimation->AddListener([text = AceType::WeakClaim(this)](double value) {
        auto textField = text.Upgrade();
        if (textField && textField->updateHandleDiameter_) {
            textField->updateHandleDiameter_(value);
        }
    });

    // Add the animation for handleDiameterinner
    auto diameterInnerAnimation = AceType::MakeRefPtr<CurveAnimation<double>>(
        initHandleDiameterInner * FIFTY_PERCENT, initHandleDiameterInner, Curves::ELASTICS);
    diameterInnerAnimation->AddListener([text = AceType::WeakClaim(this)](double value) {
        auto textField = text.Upgrade();
        if (textField && textField->updateHandleDiameterInner_) {
            textField->updateHandleDiameterInner_(value);
        }
    });

    // Add the animation
    LOGD("Add animation to animator");
    animator_ = AceType::MakeRefPtr<Animator>(context_);
    animator_->AddInterpolator(diameterAnimation);
    animator_->AddInterpolator(diameterInnerAnimation);
    animator_->SetDuration(SHOW_HANDLE_DURATION);
    animator_->Play();
}

void RenderTextField::RegisterCallbacksToOverlay()
{
    if (!textOverlay_) {
        return;
    }
    textOverlay_->SetOnCut([weak = AceType::WeakClaim(this)] {
        auto textfield = weak.Upgrade();
        if (textfield) {
            textfield->HandleOnCut();
        }
    });

    textOverlay_->SetOnCopy([weak = AceType::WeakClaim(this)] {
        auto textfield = weak.Upgrade();
        if (textfield) {
            textfield->HandleOnCopy();
        }
    });

    textOverlay_->SetOnCopyAll(
        [weak = AceType::WeakClaim(this)](const std::function<void(const Offset&, const Offset&)>& callback) {
            auto textfield = weak.Upgrade();
            if (textfield) {
                textfield->HandleOnCopyAll(callback);
            }
        });

    textOverlay_->SetOnStartHandleMove(
        [weak = AceType::WeakClaim(this)](int32_t end, const Offset& startHandleOffset,
            const std::function<void(const Offset&)>& startCallback, bool isSingleHandle) {
            auto textfield = weak.Upgrade();
            if (textfield) {
                textfield->HandleOnStartHandleMove(end, startHandleOffset, startCallback, isSingleHandle);
            }
        });

    textOverlay_->SetOnEndHandleMove([weak = AceType::WeakClaim(this)](int32_t start, const Offset& endHandleOffset,
                                         const std::function<void(const Offset&)>& endCallback) {
        auto textfield = weak.Upgrade();
        if (textfield) {
            textfield->HandleOnEndHandleMove(start, endHandleOffset, endCallback);
        }
    });

    auto callback = [weakTextField = WeakClaim(this)](const std::string& data) {
        auto textfield = weakTextField.Upgrade();
        if (textfield) {
            auto textOverlay = textfield->textOverlay_;
            if (textOverlay && !data.empty()) {
                textOverlay->SetOnPaste([weakTextField] {
                    auto textfield = weakTextField.Upgrade();
                    if (textfield) {
                        textfield->HandleOnPaste();
                    }
                });
            }
            textfield->PushTextOverlayToStack();
            textfield->UpdateOverlay();
        }
    };
    if (clipboard_) {
        clipboard_->GetData(callback);
    }

    auto onFocusChange = [weak = WeakClaim(this)](bool isFocus, bool needCloseKeyboard) {
        auto textField = weak.Upgrade();
        if (textField) {
            textField->OnOverlayFocusChange(isFocus, needCloseKeyboard);
        }
    };
    textOverlay_->SetOnFocusChange(onFocusChange);
}

void RenderTextField::PushTextOverlayToStack()
{
    if (!textOverlay_) {
        LOGE("TextOverlay is null");
        return;
    }

    hasTextOverlayPushed_ = true;
    auto lastStack = GetLastStack();
    if (!lastStack) {
        LOGE("LastStack is null");
        return;
    }
    isOverlayShowed_ = true;
    lastStack->PushComponent(textOverlay_, false);
    stackElement_ = WeakClaim(RawPtr(lastStack));
    MarkNeedRender();
}

void RenderTextField::PopTextOverlay()
{
    const auto& stackElement = stackElement_.Upgrade();
    if (stackElement) {
        stackElement->PopTextOverlay();
    }
    isOverlayShowed_ = false;
}

RefPtr<RenderSlidingPanel> RenderTextField::GetSlidingPanelAncest()
{
    auto parent = GetParent().Upgrade();
    while (parent) {
        auto renderSlidingPanel = AceType::DynamicCast<RenderSlidingPanel>(parent);
        if (renderSlidingPanel) {
            return renderSlidingPanel;
        }
        parent = parent->GetParent().Upgrade();
    }
    return nullptr;
}

void RenderTextField::ResetSlidingPanelParentHeight()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("ResetSlidingPanelParentHeight: Context is null");
        return;
    }
    auto manager = context->GetTextFieldManager();
    if (manager && AceType::InstanceOf<TextFieldManager>(manager)) {
        auto textFieldManager = AceType::DynamicCast<TextFieldManager>(manager);
        textFieldManager->ResetSlidingPanelParentHeight();
    }
}

void RenderTextField::ResetOnFocusForTextFieldManager()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("ResetOnFocusForTextFieldManager: Context is null");
        return;
    }
    auto manager = context->GetTextFieldManager();
    if (manager && AceType::InstanceOf<TextFieldManager>(manager)) {
        auto textFieldManager = AceType::DynamicCast<TextFieldManager>(manager);
        textFieldManager->ClearOnFocusTextField();
    }
}

bool RenderTextField::RequestKeyboard(bool isFocusViewChanged, bool needStartTwinkling)
{
    if (!enabled_) {
        LOGD("TextField is not enabled.");
        return false;
    }

    instanceId_ = ContainerScope::CurrentId();

    if (softKeyboardEnabled_) {
        LOGI("Request open soft keyboard");
#if defined(ENABLE_STANDARD_INPUT)
        if (textChangeListener_ == nullptr) {
            textChangeListener_ = new OnTextChangedListenerImpl(WeakClaim(this));
        }
        auto inputMethod = MiscServices::InputMethodController::GetInstance();
        if (!inputMethod) {
            LOGE("Request open soft keyboard failed because input method is null.");
            return false;
        }
        auto context = context_.Upgrade();
        if (context) {
            LOGI("RequestKeyboard set calling window id is : %{public}d", context->GetWindowId());
            inputMethod->SetCallingWindow(context->GetWindowId());
        }
        inputMethod->Attach(textChangeListener_);
#else
        if (!HasConnection()) {
            AttachIme();
            if (!HasConnection()) {
                LOGE("Get TextInput connection error");
                return false;
            }
            connection_->SetEditingState(GetEditingValue(), GetInstanceId());
        }
        connection_->Show(isFocusViewChanged, GetInstanceId());
#endif
    }
    auto context = context_.Upgrade();
    if (context) {
        auto manager = context->GetTextFieldManager();
        if (manager && AceType::InstanceOf<TextFieldManager>(manager)) {
            auto textFieldManager = AceType::DynamicCast<TextFieldManager>(manager);
            textFieldManager->SetOnFocusTextField(WeakClaim(this));
        }
    }
    if (keyboard_ != TextInputType::MULTILINE) {
        resetToStart_ = false;
        MarkNeedLayout();
    }
    if (needStartTwinkling) {
        StartTwinkling();
    }
    return true;
}

bool RenderTextField::CloseKeyboard(bool forceClose)
{
    if (!isOverlayShowed_ || !isOverlayFocus_ || forceClose) {
        if (!textFieldController_) {
            StopTwinkling();
        }
        LOGI("Request close soft keyboard");
#if defined(ENABLE_STANDARD_INPUT)
        auto inputMethod = MiscServices::InputMethodController::GetInstance();
        if (!inputMethod) {
            LOGE("Request close soft keyboard failed because input method is null.");
            return false;
        }
        inputMethod->HideTextInput();
#else
        if (HasConnection()) {
            connection_->Close(GetInstanceId());
            connection_ = nullptr;
        }
#endif

        if (onKeyboardClose_) {
            onKeyboardClose_(forceClose);
            onKeyboardClose_ = nullptr;
            UpdateSelection(GetEditingValue().selection.GetEnd());
            MarkNeedLayout();
        }
        ResetSlidingPanelParentHeight();
        if (keyboard_ != TextInputType::MULTILINE && keyboard_ != TextInputType::VISIBLE_PASSWORD) {
            resetToStart_ = true;
            MarkNeedLayout();
        }
        return true;
    }
    return false;
}

void RenderTextField::AttachIme()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGW("No context exists, failed to request keyboard.");
        return;
    }

    TextInputConfiguration config;
    config.type = keyboard_;
    config.action = action_;
    config.actionLabel = actionLabel_;
    config.obscureText = obscure_;
    LOGD("Request keyboard configuration: type=%{private}d action=%{private}d actionLabel=%{private}s "
         "obscureText=%{private}d",
        keyboard_, action_, actionLabel_.c_str(), obscure_);
    connection_ =
        TextInputProxy::GetInstance().Attach(WeakClaim(this), config, context->GetTaskExecutor(), GetInstanceId());
}

void RenderTextField::StartTwinkling()
{
    // Ignore the result because all ops are called on this same thread (ACE UI).
    // The only reason failed is that the task has finished.
    cursorTwinklingTask_.Cancel();

    // Show cursor right now.
    cursorVisibility_ = true;
    // Does not matter call more than one times.
    MarkNeedRender();

    ScheduleCursorTwinkling();
}

void RenderTextField::StopTwinkling()
{
    obscureTickPendings_ = 0;
    cursorTwinklingTask_.Cancel();

    if (cursorVisibility_) {
        // Repaint only if cursor is visible for now.
        cursorVisibility_ = false;
        MarkNeedRender();
    }
}

const TextEditingValue& RenderTextField::GetEditingValue() const
{
    return controller_->GetValue();
}

const TextEditingValue& RenderTextField::GetPreEditingValue() const
{
    return controller_->GetPreValue();
}

double RenderTextField::GetEditingBoxY() const
{
    return GetGlobalOffset().GetY() + height_.Value();
}

double RenderTextField::GetEditingBoxTopY() const
{
    return GetGlobalOffset().GetY();
}

bool RenderTextField::GetEditingBoxModel() const
{
    bool isDeclarative = false;
    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        isDeclarative = true;
    }
    return isDeclarative;
}

void RenderTextField::SetEditingValue(TextEditingValue&& newValue, bool needFireChangeEvent, bool isClearRecords)
{
    if (newValue.text != GetEditingValue().text && needFireChangeEvent) {
        needNotifyChangeEvent_ = true;
        operationRecords_.push_back(newValue);
        if (isClearRecords) {
            inverseOperationRecords_.clear();
        }
    }
    ChangeCounterStyle(newValue);
    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        if (GetEditingValue().text.empty()) {
            Dimension fontSize_ = placeHoldStyle_.GetFontSize();
            if (fontSize_.Value() <= 0) {
                Dimension fontSize_ { 14, DimensionUnit::FP };
                placeHoldStyle_.SetFontSize(fontSize_);
            }
            SetTextStyle(placeHoldStyle_);
        }
    }
    controller_->SetValue(newValue, needFireChangeEvent);
    UpdateAccessibilityAttr();
}

void RenderTextField::SetEditingValue(const std::string& text)
{
    auto newValue = GetEditingValue();
    newValue.text = text;
    SetEditingValue(std::move(newValue));
}

void RenderTextField::ClearEditingValue()
{
    TextEditingValue emptyValue;
    SetEditingValue(std::move(emptyValue));
}

std::u16string RenderTextField::GetTextForDisplay(const std::string& text) const
{
    std::u16string txtContent = StringUtils::Str8ToStr16(text);
    auto len = txtContent.length();
    if (!obscure_ || len == 0 || (obscureTickPendings_ > 0 && len == 1)) {
        return txtContent;
    }

    std::u16string obscured;
    if (Localization::GetInstance()->GetLanguage() == "ar") { // ar is the abbreviation of Arabic.
        obscured = std::u16string(len, OBSCURING_CHARACTER_FOR_AR);
    } else {
        obscured = std::u16string(len, OBSCURING_CHARACTER);
    }
    int32_t posBeforeCursor = GetEditingValue().selection.extentOffset - 1;
    if (obscureTickPendings_ > 0 && posBeforeCursor >= 0 && static_cast<size_t>(posBeforeCursor) < obscured.length()) {
        // Let the last commit character naked.
        obscured[posBeforeCursor] = txtContent[posBeforeCursor];
    }

    return obscured;
}

void RenderTextField::UpdateObscure(const RefPtr<TextFieldComponent>& textField)
{
    auto context = context_.Upgrade();
    if (context && context->GetIsDeclarative()) {
        if (!passwordRecord_) {
            if (keyboard_ != textField->GetTextInputType()) {
                passwordRecord_ = true;
                obscure_ = textField->NeedObscure();
            } else {
                obscure_ = !textField->NeedObscure();
            }
        } else {
            obscure_ = textField->NeedObscure();
        }
    } else {
        obscure_ = textField->NeedObscure();
    }
}

void RenderTextField::UpdateFormatters()
{
    textInputFormatters_.clear();

    if (maxLength_ < std::numeric_limits<uint32_t>::max()) {
        textInputFormatters_.emplace_back(std::make_unique<LengthLimitingFormatter>(maxLength_));
    }

    if (maxLines_ == 1) {
        textInputFormatters_.emplace_back(std::make_unique<SingleLineFormatter>());
    }

    switch (keyboard_) {
        case TextInputType::NUMBER: {
            textInputFormatters_.emplace_back(std::make_unique<NumberFormatter>());
            break;
        }
        case TextInputType::PHONE: {
            textInputFormatters_.emplace_back(std::make_unique<PhoneNumberFormatter>());
            break;
        }
        case TextInputType::EMAIL_ADDRESS: {
            textInputFormatters_.emplace_back(std::make_unique<EmailFormatter>());
            break;
        }
        case TextInputType::URL: {
            textInputFormatters_.emplace_back(std::make_unique<UriFormatter>());
            break;
        }
        default: {
            // No need limit.
        }
    }

    TextEditingValue temp = GetEditingValue();
    for (const auto& formatter : textInputFormatters_) {
        if (formatter) {
            formatter->Format(GetEditingValue(), temp);
        }
    }
    SetEditingValue(std::move(temp));
}

std::wstring WstringSearch(std::wstring wideText, const std::wregex& regex)
{
    std::wstring result;
    std::wsmatch matchResults;
    while (std::regex_search(wideText, matchResults, regex)) {
        for (auto&& mr : matchResults) {
            result.append(mr);
        }
        wideText = matchResults.suffix();
    }
    return result;
}

void RenderTextField::EditingValueFilter(TextEditingValue& result)
{
    for (const auto& formatter : textInputFormatters_) {
        if (formatter) {
            formatter->Format(GetEditingValue(), result);
        }
    }

    if (inputFilter_.empty() || result.text.empty()) {
        return;
    }

    std::wregex filterRegex(StringUtils::ToWstring(inputFilter_));

    auto wideText = result.GetWideText();
    auto wideTextError = std::regex_replace(wideText, filterRegex, L"");
    if (!wideTextError.empty() && onError_) {
        onError_(StringUtils::ToString(wideTextError));
    }

    auto start = result.selection.GetStart();
    auto end = result.selection.GetEnd();
    if ((start <= 0) && (end <= 0)) {
        result.text = StringUtils::ToString(WstringSearch(wideText, filterRegex));
    } else {
        std::wstring wstrBeforeSelection;
        if ((start > 0) && (static_cast<size_t>(start) <= wideText.length())) {
            wstrBeforeSelection = wideText.substr(0, start);
            wstrBeforeSelection = WstringSearch(wstrBeforeSelection, filterRegex);
        }
        std::wstring wstrInSelection;
        if (start < end) {
            wstrInSelection = wideText.substr(start, end - start);
            wstrInSelection = WstringSearch(wstrInSelection, filterRegex);
        }
        std::wstring wstrAfterSelection;
        if (end >= start && end <= static_cast<int32_t>(wideText.length())) {
            size_t lenLeft = wideText.length() - static_cast<size_t>(end);
            wstrAfterSelection = wideText.substr(end, lenLeft);
            wstrAfterSelection = WstringSearch(wstrAfterSelection, filterRegex);
        }

        result.text = StringUtils::ToString(wstrBeforeSelection + wstrInSelection + wstrAfterSelection);
        if (result.selection.baseOffset > result.selection.extentOffset) {
            result.selection.Update(static_cast<int32_t>(wstrBeforeSelection.length() + wstrInSelection.length()),
                static_cast<int32_t>(wstrBeforeSelection.length()));
        } else {
            result.selection.Update(static_cast<int32_t>(wstrBeforeSelection.length()),
                static_cast<int32_t>(wstrBeforeSelection.length() + wstrInSelection.length()));
        }
    }
}

void RenderTextField::UpdateEditingValue(const std::shared_ptr<TextEditingValue>& value, bool needFireChangeEvent)
{
    ContainerScope scope(instanceId_);
    if (!value) {
        LOGE("the value is nullptr");
        return;
    }

    lastKnownRemoteEditingValue_ = value;
    lastKnownRemoteEditingValue_->hint = placeholder_;
    TextEditingValue valueNeedToUpdate = *value;
    if (cursorPositionType_ != CursorPositionType::END ||
        (valueNeedToUpdate.selection.baseOffset == valueNeedToUpdate.selection.extentOffset &&
            valueNeedToUpdate.selection.baseOffset != static_cast<int32_t>(valueNeedToUpdate.GetWideText().length()))) {
        cursorPositionType_ = CursorPositionType::NORMAL;
        isValueFromRemote_ = true;
    }
    auto valueBeforeUpdate = GetEditingValue();

    ChangeCounterStyle(valueNeedToUpdate);

    EditingValueFilter(valueNeedToUpdate);

    if (obscure_ && (valueNeedToUpdate.text.length() == valueBeforeUpdate.text.length() + 1)) {
        // Reset pending.
        obscureTickPendings_ = OBSCURE_SHOW_TICKS;
    }

    if (valueNeedToUpdate.text != valueBeforeUpdate.text && needFireChangeEvent) {
        needNotifyChangeEvent_ = true;
    }

    SetEditingValue(std::move(valueNeedToUpdate), needFireChangeEvent);
    UpdateRemoteEditingIfNeeded(needFireChangeEvent);

    MarkNeedLayout();

    // If input or delete text when overlay is showing, pop overlay from stack.
    if (valueNeedToUpdate.text != valueBeforeUpdate.text) {
        if (onValueChange_) {
            onValueChange_();
        }
        if (onChange_) {
            onChange_(GetEditingValue().text);
        }
    }
}

void RenderTextField::PerformDefaultAction()
{
    PerformAction(action_);
}

void RenderTextField::PerformAction(TextInputAction action, bool forceCloseKeyboard)
{
    LOGD("PerformAction  %{public}d", static_cast<int32_t>(action));
    ContainerScope scope(instanceId_);
    if (action == TextInputAction::NEXT && moveNextFocusEvent_) {
        moveNextFocusEvent_();
    } else {
        CloseKeyboard(forceCloseKeyboard);
    }
    if (onFinishInputEvent_) {
        auto jsonResult = JsonUtil::Create(true);
        jsonResult->Put("value", static_cast<int32_t>(action));
        onFinishInputEvent_(std::string(R"("enterkeyclick",)").append(jsonResult->ToString()));
    }
    if (onSubmitEvent_ && controller_) {
        onSubmitEvent_(controller_->GetValue().text);
    }
    if (onSubmit_) {
        onSubmit_(static_cast<int32_t>(action));
    }
}

Size RenderTextField::Measure()
{
    return Size();
}

void RenderTextField::ScheduleCursorTwinkling()
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGW("No context exists.");
        return;
    }

    if (!context->GetTaskExecutor()) {
        LOGW("context has no task executor.");
        return;
    }

    auto weak = WeakClaim(this);
    cursorTwinklingTask_.Reset([weak] {
        auto client = weak.Upgrade();
        if (client) {
            client->OnCursorTwinkling();
        }
    });
    auto taskExecutor = context->GetTaskExecutor();
    if (taskExecutor) {
        taskExecutor->PostDelayedTask(cursorTwinklingTask_, TaskExecutor::TaskType::UI, twinklingInterval);
    } else {
        LOGE("the task executor is nullptr");
    }
}

void RenderTextField::OnCursorTwinkling()
{
    // When glyph changes from visible to invisible, layout is needed.
    obscureTickPendings_ == 1 ? MarkNeedLayout() : MarkNeedRender();
    if (obscureTickPendings_ > 0) {
        --obscureTickPendings_;
    }
    cursorVisibility_ = !cursorVisibility_;
    ScheduleCursorTwinkling();
}

bool RenderTextField::OnKeyEvent(const KeyEvent& event)
{
    if (!enabled_) {
        return false;
    }

    // If back or escape is clicked and overlay is showing, pop overlay firstly.
    if (event.action == KeyAction::UP && (event.code == KeyCode::KEY_BACK || event.code == KeyCode::KEY_ESCAPE)) {
        if (isOverlayShowed_) {
            PopTextOverlay();
            return false;
        }
    }
    if (event.action == KeyAction::UP &&
        ((event.code == KeyCode::KEY_SHIFT_LEFT || event.code == KeyCode::KEY_SHIFT_RIGHT) ||
            (event.code == KEY_META_OR_CTRL_LEFT || event.code == KEY_META_OR_CTRL_RIGHT))) {
        return HandleKeyEvent(event);
    }

    if (event.action == KeyAction::DOWN) {
        cursorPositionType_ = CursorPositionType::NONE;
        if (KeyCode::TV_CONTROL_UP <= event.code && event.code <= KeyCode::TV_CONTROL_RIGHT &&
            (event.IsKey({ KeyCode::KEY_SHIFT_LEFT, event.code }) ||
                event.IsKey({ KeyCode::KEY_SHIFT_RIGHT, event.code }))) {
            HandleOnSelect(event.code);
            return true;
        }
        if (event.code == KeyCode::TV_CONTROL_LEFT) {
            CursorMoveLeft();
            obscureTickPendings_ = 0;
            return true;
        }
        if (event.code == KeyCode::TV_CONTROL_RIGHT) {
            CursorMoveRight();
            obscureTickPendings_ = 0;
            return true;
        }
        if (event.code == KeyCode::TV_CONTROL_UP) {
            CursorMoveUp();
            obscureTickPendings_ = 0;
            return true;
        }
        if (event.code == KeyCode::TV_CONTROL_DOWN) {
            CursorMoveDown();
            obscureTickPendings_ = 0;
            return true;
        }
        if (event.code == KeyCode::KEY_FORWARD_DEL) {
            int32_t startPos = GetEditingValue().selection.GetStart();
            int32_t endPos = GetEditingValue().selection.GetEnd();
            Delete(startPos, startPos == endPos ? startPos - 1 : endPos);
            return true;
        }
        if (event.code == KeyCode::KEY_DEL) {
            int32_t startPos = GetEditingValue().selection.GetStart();
            int32_t endPos = GetEditingValue().selection.GetEnd();
            Delete(startPos, startPos == endPos ? startPos + 1 : endPos);
            return true;
        }
    }
    return HandleKeyEvent(event);
}

void RenderTextField::UpdateFocusStyles()
{
    if (hasFocus_) {
        style_.SetTextColor(focusTextColor_);
        placeholderColor_ = focusPlaceholderColor_;
        if (decoration_) {
            decoration_->SetBackgroundColor(focusBgColor_);
        }
    } else {
        style_.SetTextColor(inactiveTextColor_);
        placeholderColor_ = inactivePlaceholderColor_;
        if (decoration_) {
            decoration_->SetBackgroundColor(inactiveBgColor_);
        }
    }
}

void RenderTextField::UpdateFocusAnimation()
{
    if (hasFocus_) {
        auto context = context_.Upgrade();
        if (!context) {
            return;
        }
        Offset offset;
        Size size;
        Radius deflateRadius;
        if (IsSelectiveDevice()) {
            double focusOffset = NormalizeToPx(OFFSET_FOCUS);
            offset = Offset(focusOffset, focusOffset);
            size = Size(focusOffset * 2.0, focusOffset * 2.0);
            deflateRadius = Radius(DEFLATE_RADIUS_FOCUS, DEFLATE_RADIUS_FOCUS);
        }
        RRect rrect = RRect::MakeRect(
            Rect(GetPosition() + offset, GetLayoutSize() - ComputeDeflateSizeOfErrorAndCountText() - size));
        if (decoration_) {
            const auto& border = decoration_->GetBorder();
            rrect.SetCorner({ border.TopLeftRadius() - deflateRadius, border.TopRightRadius() - deflateRadius,
                border.BottomRightRadius() - deflateRadius, border.BottomLeftRadius() - deflateRadius });
        }
        context->ShowFocusAnimation(rrect, focusBgColor_, GetGlobalOffset() + offset);
    }
}

void RenderTextField::UpdateIcon(const RefPtr<TextFieldComponent>& textField)
{
    if (!textField) {
        return;
    }
    iconSizeInDimension_ = textField->GetIconSize();
    iconHotZoneSizeInDimension_ = textField->GetIconHotZoneSize();
    UpdatePasswordIcon(textField);

    double widthReserved = NormalizeToPx(widthReserved_);
    if (textField->GetIconImage() == iconSrc_ && textField->GetImageFill() == imageFill_ && widthReserved <= 0.0) {
        return;
    }
    imageFill_ = textField->GetImageFill();
    iconSrc_ = textField->GetIconImage();
    if (!iconSrc_.empty() || widthReserved > 0.0) {
        RefPtr<ImageComponent> imageComponent;
        if (iconSrc_.empty() && widthReserved > 0.0) {
            imageComponent = AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::SEARCH_SVG);
        } else {
            imageComponent = AceType::MakeRefPtr<ImageComponent>(iconSrc_);
            imageComponent->SetImageFill(imageFill_);
        }
        imageComponent->SetWidth(textField->GetIconSize());
        imageComponent->SetHeight(textField->GetIconSize());
        if (textDirection_ == TextDirection::RTL) {
            imageComponent->SetMatchTextDirection(true);
            imageComponent->SetTextDirection(TextDirection::RTL);
        }

        iconImage_ = AceType::DynamicCast<RenderImage>(imageComponent->CreateRenderNode());
        if (!iconImage_) {
            return;
        }
        iconImage_->Attach(GetContext());
        iconImage_->SetDirectPaint(true);
        iconImage_->Update(imageComponent);
        AddChild(iconImage_);
    }
}

void RenderTextField::UpdatePasswordIcon(const RefPtr<TextFieldComponent>& textField)
{
    if (!IsSelectiveDevice()) {
        return;
    }
    if (!showPasswordIcon_) {
        renderShowIcon_.Reset();
        renderHideIcon_.Reset();
        return;
    }

    showIconSrc_ = textField->GetShowIconImage();
    hideIconSrc_ = textField->GetHideIconImage();

    // update show icon.
    RefPtr<ImageComponent> showImage;
    if (showIconSrc_.empty()) {
        showImage = AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::SHOW_PASSWORD_SVG);
    } else {
        showImage = AceType::MakeRefPtr<ImageComponent>(showIconSrc_);
    }
    showImage->SetWidth(textField->GetIconSize());
    showImage->SetHeight(textField->GetIconSize());

    renderShowIcon_ = AceType::DynamicCast<RenderImage>(showImage->CreateRenderNode());
    if (!renderShowIcon_) {
        return;
    }
    renderShowIcon_->Attach(GetContext());
    renderShowIcon_->SetDirectPaint(true);
    renderShowIcon_->Update(showImage);
    AddChild(renderShowIcon_);

    // update hide icon.
    RefPtr<ImageComponent> hideImage;
    if (hideIconSrc_.empty()) {
        hideImage = AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::HIDE_PASSWORD_SVG);
    } else {
        hideImage = AceType::MakeRefPtr<ImageComponent>(hideIconSrc_);
    }
    hideImage->SetWidth(textField->GetIconSize());
    hideImage->SetHeight(textField->GetIconSize());

    renderHideIcon_ = AceType::DynamicCast<RenderImage>(hideImage->CreateRenderNode());
    if (!renderHideIcon_) {
        return;
    }
    renderHideIcon_->Attach(GetContext());
    renderHideIcon_->SetDirectPaint(true);
    renderHideIcon_->Update(hideImage);
    AddChild(renderHideIcon_);
}

void RenderTextField::UpdateOverlay()
{
    // When textfield PerformLayout, update overlay.
    if (isOverlayShowed_ && updateHandlePosition_) {
        auto selStart = GetEditingValue().selection.GetStart();
        auto selEnd = GetEditingValue().selection.GetEnd();
        Rect caretStart;
        Rect caretEnd;
        bool startHandleVisible =
            GetCaretRect(selStart, caretStart) ? IsVisible(caretStart + textOffsetForShowCaret_) : false;
        bool endHandleVisible =
            (selStart == selEnd)
                ? startHandleVisible
                : (GetCaretRect(selEnd, caretEnd) ? IsVisible(caretEnd + textOffsetForShowCaret_) : false);

        OverlayShowOption option { .showMenu = isOverlayShowed_,
            .showStartHandle = startHandleVisible,
            .showEndHandle = endHandleVisible,
            .isSingleHandle = isSingleHandle_,
            .updateOverlayType = UpdateOverlayType::SCROLL,
            .startHandleOffset = GetPositionForExtend(selStart, isSingleHandle_),
            .endHandleOffset = GetPositionForExtend(selEnd, isSingleHandle_) };
        updateHandlePosition_(option);
        if (onClipRectChanged_) {
            onClipRectChanged_(innerRect_ + Size(HANDLE_HOT_ZONE, HANDLE_HOT_ZONE) + GetOffsetToPage() -
                               Offset(HANDLE_HOT_ZONE / 2.0, 0.0));
        }
    }
}

void RenderTextField::RegisterFontCallbacks()
{
    // Register callback for fonts.
    auto pipelineContext = context_.Upgrade();
    if (!pipelineContext) {
        return;
    }
    auto callback = [textfield = AceType::WeakClaim(this)] {
        auto refPtr = textfield.Upgrade();
        if (refPtr) {
            refPtr->isCallbackCalled_ = true;
            refPtr->MarkNeedLayout();
        }
    };
    auto fontManager = pipelineContext->GetFontManager();
    if (fontManager) {
        for (const auto& familyName : style_.GetFontFamilies()) {
            fontManager->RegisterCallback(AceType::WeakClaim(this), familyName, callback);
        }
        fontManager->AddVariationNode(WeakClaim(this));
    }
}

void RenderTextField::OnStatusChanged(OHOS::Ace::RenderStatus renderStatus)
{
    hasFocus_ = renderStatus == RenderStatus::FOCUS;
    UpdateFocusStyles();
    MarkNeedLayout();

    if (!hasFocus_) {
        auto context = context_.Upgrade();
        if (!context) {
            return;
        }
        // Don't call cancel focus animation when next frame comes because then focus is switched, next node will
        // show focus immediately, we shouldn't cancel focus animation that time.
        context->CancelFocusAnimation();
    }
}

void RenderTextField::OnValueChanged(bool needFireChangeEvent, bool needFireSelectChangeEvent)
{
    isValueFromFront_ = !needFireChangeEvent;
    TextEditingValue temp = GetEditingValue();
    EditingValueFilter(temp);
    if (cursorPositionType_ == CursorPositionType::NORMAL && temp.selection.GetStart() == temp.selection.GetEnd()) {
        temp.selection.Update(AdjustCursorAndSelection(temp.selection.GetEnd()));
    }
    FireSelectChangeIfNeeded(temp, needFireSelectChangeEvent);
    SetEditingValue(std::move(temp), needFireChangeEvent);
    UpdateRemoteEditingIfNeeded(needFireChangeEvent);
    MarkNeedLayout();
}

void RenderTextField::FireSelectChangeIfNeeded(const TextEditingValue& newValue, bool needFireSelectChangeEvent) const
{
    if (needFireSelectChangeEvent && onSelectChangeEvent_ && newValue.selection != GetPreEditingValue().selection) {
        auto jsonResult = JsonUtil::Create(true);
        jsonResult->Put("start", newValue.selection.GetStart());
        jsonResult->Put("end", newValue.selection.GetEnd());
        onSelectChangeEvent_(std::string(R"("selectchange",)").append(jsonResult->ToString()));
    }
}

void RenderTextField::CursorMoveLeft(CursorMoveSkip skip)
{
    if (skip != CursorMoveSkip::CHARACTER) {
        // Not support yet.
        LOGE("move skip not support character yet");
        return;
    }
    isValueFromRemote_ = false;
    auto value = GetEditingValue();
    value.MoveLeft();
    SetEditingValue(std::move(value));
    cursorPositionType_ = CursorPositionType::NONE;
    MarkNeedLayout();
}

void RenderTextField::CursorMoveRight(CursorMoveSkip skip)
{
    if (skip != CursorMoveSkip::CHARACTER) {
        // Not support yet.
        LOGE("move skip not support character yet");
        return;
    }
    isValueFromRemote_ = false;
    auto value = GetEditingValue();
    value.MoveRight();
    SetEditingValue(std::move(value));
    cursorPositionType_ = CursorPositionType::NONE;
    MarkNeedLayout();
}

void RenderTextField::CursorMoveUp()
{
    if (keyboard_ != TextInputType::MULTILINE) {
        return;
    }
    isValueFromRemote_ = false;
    auto value = GetEditingValue();
    value.MoveToPosition(GetCursorPositionForMoveUp());
    SetEditingValue(std::move(value));
    cursorPositionType_ = CursorPositionType::NONE;
    MarkNeedLayout();
}

void RenderTextField::CursorMoveDown()
{
    if (keyboard_ != TextInputType::MULTILINE) {
        return;
    }
    isValueFromRemote_ = false;
    auto value = GetEditingValue();
    value.MoveToPosition(GetCursorPositionForMoveDown());
    SetEditingValue(std::move(value));
    cursorPositionType_ = CursorPositionType::NONE;
    MarkNeedLayout();
}

void RenderTextField::CursorMoveOnClick(const Offset& offset)
{
    auto value = GetEditingValue();
    auto position = GetCursorPositionForClick(offset);
    value.MoveToPosition(position);
    SetEditingValue(std::move(value));

    if (!GetEditingValue().text.empty() && position == GetEditingValue().selection.GetEnd()) {
        OnValueChanged(true, false);
    }
}

void RenderTextField::UpdateSelection(int32_t both)
{
    UpdateSelection(both, both);
}

void RenderTextField::UpdateSelection(int32_t start, int32_t end)
{
    auto value = GetEditingValue();
    value.UpdateSelection(start, end);
    SetEditingValue(std::move(value));
    auto refPtr = accessibilityNode_.Upgrade();
    if (refPtr) {
        refPtr->SetTextSelectionStart(start);
        refPtr->SetTextSelectionEnd(end);
    }
}

void RenderTextField::UpdateRemoteEditing(bool needFireChangeEvent)
{
#if defined(ENABLE_STANDARD_INPUT)
    auto value = GetEditingValue();
    MiscServices::InputMethodController::GetInstance()->OnSelectionChange(
        StringUtils::Str8ToStr16(value.text), value.selection.GetStart(), value.selection.GetEnd());
#else
    if (!HasConnection()) {
        return;
    }
    connection_->SetEditingState(GetEditingValue(), GetInstanceId(), needFireChangeEvent);
#endif
}

void RenderTextField::UpdateRemoteEditingIfNeeded(bool needFireChangeEvent)
{
    if (!enabled_) {
        return;
    }
#if defined(ENABLE_STANDARD_INPUT)
    UpdateRemoteEditing(needFireChangeEvent);
#else
    if (!lastKnownRemoteEditingValue_ || GetEditingValue() != *lastKnownRemoteEditingValue_) {
        lastKnownRemoteEditingValue_ = std::make_shared<TextEditingValue>(GetEditingValue());
        UpdateRemoteEditing(needFireChangeEvent);
    }
#endif
}

void RenderTextField::ShowError(const std::string& errorText, bool resetToStart)
{
    errorText_ = errorText;
    if (!errorText.empty()) {
        auto refPtr = accessibilityNode_.Upgrade();
        if (refPtr) {
            refPtr->SetErrorText(errorText);
        }
    }

    if (!errorText.empty()) {
        ChangeBorderToErrorStyle();
    } else {
        if (decoration_) {
            decoration_->SetBorder(originBorder_);
        }
    }
    MarkNeedLayout();
}

void RenderTextField::SetOnValueChange(const std::function<void()>& onValueChange)
{
    onValueChange_ = onValueChange;
}

const std::function<void()>& RenderTextField::GetOnValueChange() const
{
    return onValueChange_;
}

void RenderTextField::SetOnKeyboardClose(const std::function<void(bool)>& onKeyboardClose)
{
    onKeyboardClose_ = onKeyboardClose;
}

void RenderTextField::SetOnClipRectChanged(const std::function<void(const Rect&)>& onClipRectChanged)
{
    onClipRectChanged_ = onClipRectChanged;
}

void RenderTextField::SetUpdateHandlePosition(const std::function<void(const OverlayShowOption&)>& updateHandlePosition)
{
    updateHandlePosition_ = updateHandlePosition;
}

void RenderTextField::SetUpdateHandleDiameter(const std::function<void(const double&)>& updateHandleDiameter)
{
    updateHandleDiameter_ = updateHandleDiameter;
}

void RenderTextField::SetUpdateHandleDiameterInner(const std::function<void(const double&)>& updateHandleDiameterInner)
{
    updateHandleDiameterInner_ = updateHandleDiameterInner;
}

void RenderTextField::SetIsOverlayShowed(bool isOverlayShowed, bool needStartTwinkling)
{
    isOverlayShowed_ = isOverlayShowed;
    // When pop overlay, reset selection and clear selected style.
    if (GetEditingValue().selection.GetStart() != GetEditingValue().selection.GetEnd()) {
        UpdateSelection(GetEditingValue().selection.GetEnd());
    }
    if (!isOverlayShowed_ && hasFocus_ && needStartTwinkling) {
        StartTwinkling();
    }
}

void RenderTextField::HandleOnSelect(KeyCode keyCode, CursorMoveSkip skip)
{
    if (skip != CursorMoveSkip::CHARACTER) {
        // Not support yet.
        LOGE("move skip not support character yet");
        return;
    }

    isValueFromRemote_ = false;
    auto value = GetEditingValue();
    int32_t startPos = value.selection.GetStart();
    int32_t endPos = value.selection.GetEnd();
    static bool isForwardSelect;
    switch (keyCode) {
        case KeyCode::KEY_DPAD_LEFT:
            if (startPos == endPos) {
                isForwardSelect = true;
            }
            if (isForwardSelect) {
                value.UpdateSelection(startPos - 1, endPos);
            } else {
                value.UpdateSelection(startPos, endPos - 1);
            }
            break;
        case KeyCode::KEY_DPAD_RIGHT:
            if (startPos == endPos) {
                isForwardSelect = false;
            }
            if (isForwardSelect) {
                value.UpdateSelection(startPos + 1, endPos);
            } else {
                value.UpdateSelection(startPos, endPos + 1);
            }
            break;
        default:
            LOGI("Currently only left and right selections are supported.");
            return;
    }

    SetEditingValue(std::move(value));
    MarkNeedLayout();
}

void RenderTextField::HandleOnRevoke()
{
    if (operationRecords_.empty()) {
        return;
    }
    inverseOperationRecords_.push_back(GetEditingValue());
    operationRecords_.pop_back();
    auto value = operationRecords_.back();
    operationRecords_.pop_back();
    isValueFromRemote_ = false;
    SetEditingValue(std::move(value), true, false);
    cursorPositionType_ = CursorPositionType::NONE;
    MarkNeedLayout();
    if (onChange_) {
        onChange_(GetEditingValue().text);
    }
}

void RenderTextField::HandleOnInverseRevoke()
{
    if (inverseOperationRecords_.empty()) {
        return;
    }
    auto value = inverseOperationRecords_.back();
    inverseOperationRecords_.pop_back();
    isValueFromRemote_ = false;
    SetEditingValue(std::move(value), true, false);
    cursorPositionType_ = CursorPositionType::NONE;
    MarkNeedLayout();
    if (onChange_) {
        onChange_(GetEditingValue().text);
    }
}

void RenderTextField::HandleOnCut()
{
    if (!clipboard_) {
        return;
    }
    if (GetEditingValue().GetSelectedText().empty()) {
        LOGD("copy value is empty");
        return;
    }
    if (copyOption_ != CopyOptions::None) {
        LOGD("copy value is %{private}s", GetEditingValue().GetSelectedText().c_str());
        clipboard_->SetData(GetEditingValue().GetSelectedText(), copyOption_);
    }
    if (onCut_) {
        onCut_(GetEditingValue().GetSelectedText());
    }
    auto value = GetEditingValue();
    value.text = value.GetBeforeSelection() + value.GetAfterSelection();
    value.UpdateSelection(GetEditingValue().selection.GetStart());
    SetEditingValue(std::move(value));
    if (onChange_) {
        onChange_(GetEditingValue().text);
    }
}

void RenderTextField::HandleOnCopy()
{
    if (!clipboard_) {
        return;
    }
    if (GetEditingValue().GetSelectedText().empty()) {
        LOGW("copy value is empty");
        return;
    }
    if (copyOption_ != CopyOptions::None) {
        LOGD("copy value is %{private}s", GetEditingValue().GetSelectedText().c_str());
        clipboard_->SetData(GetEditingValue().GetSelectedText(), copyOption_);
    }
    if (onCopy_) {
        onCopy_(GetEditingValue().GetSelectedText());
    }
    UpdateSelection(GetEditingValue().selection.GetEnd());
}

void RenderTextField::HandleOnPaste()
{
    if (!clipboard_) {
        return;
    }
    auto textSelection = GetEditingValue().selection;
    auto pasteCallback = [weak = WeakClaim(this), textSelection](const std::string& data) {
        if (data.empty()) {
            LOGW("paste value is empty");
            return;
        }
        LOGD("paste value is %{private}s", data.c_str());
        auto textfield = weak.Upgrade();
        if (textfield) {
            auto value = textfield->GetEditingValue();
            value.selection = textSelection;
            value.text = value.GetBeforeSelection() + data + value.GetAfterSelection();
            value.UpdateSelection(textSelection.GetStart() + StringUtils::Str8ToStr16(data).length());
            textfield->SetEditingValue(std::move(value));
            if (textfield->onPaste_) {
                textfield->onPaste_(data);
            }
            if (textfield->onChange_) {
                textfield->onChange_(textfield->GetEditingValue().text);
            }
        }
    };
    clipboard_->GetData(pasteCallback);
}

void RenderTextField::HandleOnCopyAll(const std::function<void(const Offset&, const Offset&)>& callback)
{
    isSingleHandle_ = false;
    cursorPositionType_ = CursorPositionType::NORMAL;
    auto textSize = GetEditingValue().GetWideText().length();
    if (textSize == 0) {
        isSingleHandle_ = true;
    }
    UpdateSelection(0, textSize);
    if (callback) {
        callback(GetPositionForExtend(0, isSingleHandle_),
            GetPositionForExtend(GetEditingValue().GetWideText().length(), isSingleHandle_));
    }
}

void RenderTextField::HandleOnStartHandleMove(int32_t end, const Offset& startHandleOffset,
    const std::function<void(const Offset&)>& startCallback, bool isSingleHandle)
{
    Offset realOffset = startHandleOffset;
    if (startCallback) {
        UpdateStartSelection(end, realOffset, isSingleHandle, false);
        startCallback(GetHandleOffset(GetEditingValue().selection.GetStart()));
    }
}

void RenderTextField::HandleOnEndHandleMove(
    int32_t start, const Offset& endHandleOffset, const std::function<void(const Offset&)>& endCallback)
{
    Offset realOffset = endHandleOffset;
    if (endCallback) {
        UpdateEndSelection(start, realOffset);
        endCallback(GetHandleOffset(GetEditingValue().selection.GetEnd()));
    }
}

RefPtr<StackElement> RenderTextField::GetLastStack() const
{
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Context is nullptr");
        return nullptr;
    }
    return context->GetLastStack();
}

bool RenderTextField::HandleKeyEvent(const KeyEvent& event)
{
    std::string appendElement;
    if (event.action == KeyAction::DOWN) {
        if (event.code == KeyCode::KEY_ENTER || event.code == KeyCode::KEY_NUMPAD_ENTER ||
            event.code == KeyCode::KEY_DPAD_CENTER) {
            if (keyboard_ == TextInputType::MULTILINE) {
                appendElement = "\n";
            } else {
                // normal enter should trigger onSubmit
                PerformAction(action_, true);
            }
        } else if (event.IsNumberKey()) {
            appendElement = event.ConvertCodeToString();
        } else if (event.IsLetterKey()) {
            if (event.IsKey({ KEY_META_OR_CTRL_LEFT, KeyCode::KEY_SHIFT_LEFT, KeyCode::KEY_Z }) ||
                event.IsKey({ KEY_META_OR_CTRL_LEFT, KeyCode::KEY_SHIFT_RIGHT, KeyCode::KEY_Z }) ||
                event.IsKey({ KEY_META_OR_CTRL_RIGHT, KeyCode::KEY_SHIFT_LEFT, KeyCode::KEY_Z }) ||
                event.IsKey({ KEY_META_OR_CTRL_RIGHT, KeyCode::KEY_SHIFT_RIGHT, KeyCode::KEY_Z }) ||
                event.IsKey({ KEY_META_OR_CTRL_LEFT, KeyCode::KEY_Y }) ||
                event.IsKey({ KEY_META_OR_CTRL_RIGHT, KeyCode::KEY_Y })) {
                HandleOnInverseRevoke();
            } else if (event.IsKey({ KEY_META_OR_CTRL_LEFT, KeyCode::KEY_Z }) ||
                       event.IsKey({ KEY_META_OR_CTRL_RIGHT, KeyCode::KEY_Z })) {
                HandleOnRevoke();
            } else if (event.IsKey({ KEY_META_OR_CTRL_LEFT, KeyCode::KEY_A }) ||
                       event.IsKey({ KEY_META_OR_CTRL_RIGHT, KeyCode::KEY_A })) {
                HandleOnCopyAll(nullptr);
            } else if (event.IsKey({ KEY_META_OR_CTRL_LEFT, KeyCode::KEY_C }) ||
                       event.IsKey({ KEY_META_OR_CTRL_RIGHT, KeyCode::KEY_C })) {
                HandleOnCopy();
            } else if (event.IsKey({ KEY_META_OR_CTRL_LEFT, KeyCode::KEY_V }) ||
                       event.IsKey({ KEY_META_OR_CTRL_RIGHT, KeyCode::KEY_V })) {
                HandleOnPaste();
            } else if (event.IsKey({ KEY_META_OR_CTRL_LEFT, KeyCode::KEY_X }) ||
                       event.IsKey({ KEY_META_OR_CTRL_RIGHT, KeyCode::KEY_X })) {
                HandleOnCut();
            } else {
                appendElement = event.ConvertCodeToString();
            }
        }
        MarkNeedLayout();
    }
    if (appendElement.empty()) {
        return false;
    }

    auto editingValue = std::make_shared<TextEditingValue>();
    editingValue->text = GetEditingValue().GetBeforeSelection() + appendElement + GetEditingValue().GetAfterSelection();
    editingValue->UpdateSelection(
        std::max(GetEditingValue().selection.GetEnd(), 0) + StringUtils::Str8ToStr16(appendElement).length());
    UpdateEditingValue(editingValue);
    MarkNeedLayout();
    return true;
}

void RenderTextField::UpdateAccessibilityAttr()
{
    auto refPtr = accessibilityNode_.Upgrade();
    if (!refPtr) {
        LOGW("RenderTextField accessibilityNode is null.");
        return;
    }

    refPtr->SetHintText(placeholder_);
    refPtr->SetMaxTextLength(maxLength_);
    refPtr->SetEditable(enabled_);
    refPtr->SetClickableState(true);
    refPtr->SetLongClickableState(true);
    if (maxLines_ > 1) {
        refPtr->SetIsMultiLine(true);
    }
    if (controller_) {
        refPtr->SetText(controller_->GetText());
    }
    switch (keyboard_) {
        case TextInputType::TEXT:
            refPtr->SetTextInputType(AceTextCategory::INPUT_TYPE_TEXT);
            break;
        case TextInputType::NUMBER:
            refPtr->SetTextInputType(AceTextCategory::INPUT_TYPE_NUMBER);
            break;
        case TextInputType::DATETIME:
            refPtr->SetTextInputType(AceTextCategory::INPUT_TYPE_DATE);
            break;
        case TextInputType::EMAIL_ADDRESS:
            refPtr->SetTextInputType(AceTextCategory::INPUT_TYPE_EMAIL);
            break;
        case TextInputType::VISIBLE_PASSWORD:
            refPtr->SetTextInputType(AceTextCategory::INPUT_TYPE_PASSWORD);
            break;
        default:
            break;
    }
}

void RenderTextField::InitAccessibilityEventListener()
{
    const auto& accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    accessibilityNode->AddSupportAction(AceAction::ACTION_CLICK);
    accessibilityNode->SetActionClickImpl([weakPtr = WeakClaim(this)]() {
        const auto& textField = weakPtr.Upgrade();
        if (textField) {
            textField->OnClick(ClickInfo(0));
        }
    });

    accessibilityNode->AddSupportAction(AceAction::ACTION_LONG_CLICK);
    accessibilityNode->SetActionLongClickImpl([weakPtr = WeakClaim(this)]() {
        const auto& textField = weakPtr.Upgrade();
        if (textField) {
            textField->OnLongPress(LongPressInfo(0));
        }
    });

    accessibilityNode->AddSupportAction(AceAction::ACTION_SET_TEXT);
    accessibilityNode->SetActionSetTextImpl([weakPtr = WeakClaim(this)](const std::string& text) {
        const auto& textField = weakPtr.Upgrade();
        if (textField) {
            textField->SetEditingValue(text);
        }
    });
}

void RenderTextField::UpdateDirectionStatus()
{
    directionStatus_ = static_cast<DirectionStatus>(
        (static_cast<uint8_t>(textDirection_) << 1) | static_cast<uint8_t>(realTextDirection_));
}

void RenderTextField::UpdateStartSelection(int32_t end, const Offset& pos, bool isSingleHandle, bool isLongPress)
{
    int32_t extend = GetCursorPositionForClick(pos);
    if (isLongPress) {
        // Use custom selection if exist, otherwise select content near finger.
        if (selection_.IsValid()) {
            UpdateSelection(selection_.baseOffset, selection_.extentOffset);
        } else {
            int32_t extendEnd = extend + GetGraphemeClusterLength(extend, false);
            UpdateSelection(extend, extendEnd);
        }
        return;
    }
    if (isSingleHandle) {
        UpdateSelection(extend);
    } else {
        UpdateSelection(extend, end);
    }
}

void RenderTextField::UpdateEndSelection(int32_t start, const Offset& pos)
{
    int32_t extend = GetCursorPositionForClick(pos);
    UpdateSelection(start, extend);
}

Offset RenderTextField::GetPositionForExtend(int32_t extend, bool isSingleHandle)
{
    if (extend < 0) {
        extend = 0;
    }
    if (static_cast<size_t>(extend) > GetEditingValue().GetWideText().length()) {
        extend = static_cast<int32_t>(GetEditingValue().GetWideText().length());
    }
    return GetHandleOffset(extend);
}

int32_t RenderTextField::GetGraphemeClusterLength(int32_t extend, bool isPrefix) const
{
    auto text = GetTextForDisplay(GetEditingValue().text);
    char16_t aroundChar = 0;
    if (isPrefix) {
        if (static_cast<size_t>(extend) <= text.length()) {
            aroundChar = text[std::max(0, extend - 1)];
        }
    } else {
        if (static_cast<size_t>(extend) < (text.length())) {
            aroundChar = text[std::min(static_cast<int32_t>(text.length() - 1), extend)];
        }
    }
    return StringUtils::NotInUtf16Bmp(aroundChar) ? 2 : 1;
}

bool RenderTextField::ShowCounter() const
{
    return showCounter_ && maxLength_ < std::numeric_limits<uint32_t>::max();
}

void RenderTextField::ChangeCounterStyle(const TextEditingValue& value)
{
    if (!ShowCounter()) {
        return;
    }
    if (value.GetWideText().size() > maxLength_) {
        overCount_ = true;
        ChangeBorderToErrorStyle();
    } else if (value.GetWideText().size() < maxLength_) {
        overCount_ = false;
        if (decoration_) {
            decoration_->SetBorder(originBorder_);
        }
    }
}

void RenderTextField::ChangeBorderToErrorStyle()
{
    if (!decoration_) {
        decoration_ = AceType::MakeRefPtr<Decoration>();
    }
    const auto& border = decoration_->GetBorder();
    BorderEdge errorBorderEdge(errorBorderColor_, errorBorderWidth_, BorderStyle::SOLID);
    Border errorBorder;
    if (!border.Left().HasValue() && !border.Top().HasValue() && !border.Right().HasValue() &&
        border.Bottom().HasValue()) {
        // Change over count style for linear input.
        errorBorder = Border(BorderEdge(), BorderEdge(), BorderEdge(), errorBorderEdge);
    } else {
        errorBorder = Border(errorBorderEdge);
    }
    errorBorder.SetTopLeftRadius(decoration_->GetBorder().TopLeftRadius());
    errorBorder.SetTopRightRadius(decoration_->GetBorder().TopRightRadius());
    errorBorder.SetBottomLeftRadius(decoration_->GetBorder().BottomLeftRadius());
    errorBorder.SetBottomRightRadius(decoration_->GetBorder().BottomRightRadius());
    decoration_->SetBorder(errorBorder);
}

void RenderTextField::HandleDeviceOrientationChange()
{
    if (deviceOrientation_ != SystemProperties::GetDeviceOrientation()) {
        deviceOrientation_ = SystemProperties::GetDeviceOrientation();
        if (isOverlayShowed_) {
            onKeyboardClose_ = nullptr;
            PopTextOverlay();
            StartTwinkling();
        }
    }
}

void RenderTextField::OnHiddenChanged(bool hidden)
{
    if (hidden) {
        CloseKeyboard();
        PopTextOverlay();
    }
}

void RenderTextField::OnAppHide()
{
    RenderNode::OnAppHide();
    OnHiddenChanged(true);
}

void RenderTextField::OnOverlayFocusChange(bool isFocus, bool needCloseKeyboard)
{
    isOverlayFocus_ = isFocus;
    if (needCloseKeyboard && onOverlayFocusChange_) {
        onOverlayFocusChange_(isFocus);
    }
}

int32_t RenderTextField::GetInstanceId() const
{
    auto context = context_.Upgrade();
    if (context) {
        return context->GetInstanceId();
    }
    return 0;
}

void RenderTextField::Insert(const std::string& text)
{
    auto context = context_.Upgrade();
    if (context) {
        context->GetTaskExecutor()->PostTask(
            [weakPtr = WeakClaim(this), text] {
                const auto& textField = weakPtr.Upgrade();
                auto value = textField->GetEditingValue();
                auto textEditingValue = std::make_shared<TextEditingValue>();
                textEditingValue->text = value.GetBeforeSelection() + text + value.GetAfterSelection();
                textEditingValue->UpdateSelection(std::max(value.selection.GetStart(), 0) + text.length());
                textField->UpdateEditingValue(textEditingValue, true);
            },
            TaskExecutor::TaskType::UI);
    }
}

void RenderTextField::Delete(int32_t start, int32_t end)
{
    auto value = GetEditingValue();
    value.Delete(start, end);
    SetEditingValue(std::move(value));
    if (onValueChange_) {
        onValueChange_();
    }
    if (onChange_) {
        onChange_(GetEditingValue().text);
    }
}

std::string RenderTextField::ProvideRestoreInfo()
{
    auto value = GetEditingValue();
    auto jsonObj = JsonUtil::Create(true);
    if (controller_) {
        jsonObj->Put("text", controller_->GetText().c_str());
    } else {
        return "";
    }
    jsonObj->Put("position", value.selection.baseOffset);
    return jsonObj->ToString();
}

void RenderTextField::ApplyRestoreInfo()
{
    if (GetRestoreInfo().empty()) {
        return;
    }
    auto info = JsonUtil::ParseJsonString(GetRestoreInfo());
    if (!info->IsValid() || !info->IsObject()) {
        LOGW("RenderTextField:: restore info is invalid");
        return;
    }
    auto jsonPosition = info->GetValue("position");
    auto jsonText = info->GetValue("text");
    if (!jsonText->GetString().empty() && controller_) {
        controller_->SetText(jsonText->GetString());
        UpdateSelection(std::max(jsonPosition->GetInt(), 0));
        cursorPositionType_ = CursorPositionType::NORMAL;
    }
    SetRestoreInfo("");
}

void RenderTextField::ApplyAspectRatio()
{
    auto parent = GetParent().Upgrade();
    while (parent) {
        auto renderBox = DynamicCast<RenderBox>(parent);
        if (renderBox && !NearZero(renderBox->GetAspectRatio()) && GetLayoutParam().GetMaxSize().IsValid() &&
            !GetLayoutParam().GetMaxSize().IsInfinite()) {
            height_ = Dimension(GetLayoutParam().GetMaxSize().Height());
            break;
        }
        parent = parent->GetParent().Upgrade();
    }
}

void RenderTextField::Dump()
{
    DumpLog::GetInstance().AddDesc(std::string("CopyOptions: ").append(V2::ConvertWrapCopyOptionToString(copyOption_)));
}

} // namespace OHOS::Ace
