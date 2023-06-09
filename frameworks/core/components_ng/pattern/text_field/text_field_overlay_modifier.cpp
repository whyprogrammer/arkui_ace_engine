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

#include "core/components_ng/pattern/text_field/text_field_overlay_modifier.h"

#include "base/utils/utils.h"
#include "core/components_ng/base/modifier.h"
#include "core/components_ng/pattern/scroll/inner/scroll_bar_painter.h"
#include "core/components_ng/pattern/text_field/text_field_pattern.h"
#include "core/components_ng/render/drawing.h"
#include "core/components_ng/render/drawing_prop_convertor.h"
#include "core/components_ng/render/image_painter.h"

namespace OHOS::Ace::NG {
TextFieldOverlayModifier::TextFieldOverlayModifier(const WeakPtr<OHOS::Ace::NG::Pattern>& pattern,
    WeakPtr<ScrollBar>&& scrollBar, WeakPtr<ScrollEdgeEffect>&& edgeEffect)
    : pattern_(pattern), scrollBar_(scrollBar), edgeEffect_(edgeEffect)
{
    cursorColor_ = AceType::MakeRefPtr<AnimatablePropertyColor>(LinearColor(Color()));
    cursorWidth_ = AceType::MakeRefPtr<AnimatablePropertyFloat>(static_cast<float>(CURSOR_WIDTH.ConvertToPx()));
    selectedColor_ = AceType::MakeRefPtr<AnimatablePropertyColor>(LinearColor(Color()));
    cursorVisible_ = AceType::MakeRefPtr<PropertyBool>(false);
    contentSize_ = AceType::MakeRefPtr<PropertySizeF>(SizeF());
    contentOffset_ = AceType::MakeRefPtr<PropertyOffsetF>(OffsetF());
    auto textFieldPattern = DynamicCast<TextFieldPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(textFieldPattern);
    cursorOffsetX_ = AceType::MakeRefPtr<AnimatablePropertyFloat>(textFieldPattern->GetCaretOffsetX());
    frameSize_ = AceType::MakeRefPtr<PropertySizeF>(SizeF());
    currentOffset_ = AceType::MakeRefPtr<PropertyFloat>(0.0f);
    isSelectedAreaRedraw_ = AceType::MakeRefPtr<PropertyBool>(false);
    underlineWidth_ = AceType::MakeRefPtr<PropertyFloat>(0.0f);
    underlineColor_ = AceType::MakeRefPtr<PropertyColor>(Color());
    showCounter_ = AceType::MakeRefPtr<PropertyBool>(false);

    AttachProperty(cursorColor_);
    AttachProperty(cursorWidth_);
    AttachProperty(selectedColor_);
    AttachProperty(cursorVisible_);
    AttachProperty(contentSize_);
    AttachProperty(contentOffset_);
    AttachProperty(cursorOffsetX_);
    AttachProperty(frameSize_);
    AttachProperty(currentOffset_);
    AttachProperty(isSelectedAreaRedraw_);
    AttachProperty(underlineWidth_);
    AttachProperty(underlineColor_);
    AttachProperty(showCounter_);
}

void TextFieldOverlayModifier::onDraw(DrawingContext& context)
{
    PaintCursor(context);
    PaintSelection(context);
    PaintScrollBar(context.canvas);
    PaintEdgeEffect(frameSize_->Get(), context.canvas);
    PaintUnderline(context.canvas);
}

void TextFieldOverlayModifier::PaintUnderline(RSCanvas& canvas) const
{
    auto textFieldPattern = DynamicCast<TextFieldPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(textFieldPattern);
    if (!textFieldPattern->GetShowUnderLine()) {
        return;
    }
    auto textRect = textFieldPattern->GetContentRect();
    auto textFrameRect = textFieldPattern->GetFrameRect();
    Point leftPoint, rightPoint;
    leftPoint.SetX(textRect.Left());
    leftPoint.SetY(textFrameRect.Bottom() - textFrameRect.Top());
    rightPoint.SetX(textRect.Right());
    rightPoint.SetY(textFrameRect.Bottom() - textFrameRect.Top());
    RSPen pen;
    pen.SetColor(ToRSColor(underlineColor_->Get()));
    pen.SetWidth(underlineWidth_->Get());
    pen.SetAntiAlias(true);
    canvas.AttachPen(pen);
    canvas.DrawLine(
        ToRSPoint(PointF(leftPoint.GetX(), leftPoint.GetY())), ToRSPoint(PointF(rightPoint.GetX(), rightPoint.GetY())));
    canvas.DetachPen();
}

void TextFieldOverlayModifier::PaintSelection(DrawingContext& context) const
{
    auto canvas = context.canvas;
    auto textFieldPattern = DynamicCast<TextFieldPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(textFieldPattern);
    if (!textFieldPattern->InSelectMode()) {
        return;
    }
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto themeManager = pipelineContext->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto theme = themeManager->GetTheme<TextFieldTheme>();
    RSBrush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(ToRSColor(selectedColor_->Get()));
    canvas.AttachBrush(brush);
    auto paintOffset = contentOffset_->Get() - OffsetF(0.0f, textFieldPattern->GetBaseLineOffset());
    auto textBoxes = textFieldPattern->GetTextBoxes();
    auto textRect = textFieldPattern->GetTextRect();
    bool isTextArea = textFieldPattern->IsTextArea();
    if (inputStyle_ == InputStyle::DEFAULT) {
        float clipRectHeight = 0.0f;
        if (showCounter_->Get() && textFieldPattern->GetCounterParagraph()) {
            clipRectHeight = paintOffset.GetY() + contentSize_->Get().Height() - textFieldPattern->GetCountHeight();
        } else {
            clipRectHeight = paintOffset.GetY() + contentSize_->Get().Height();
        }
        RSRect clipInnerRect(paintOffset.GetX(), paintOffset.GetY(), paintOffset.GetX() + contentSize_->Get().Width(),
            clipRectHeight);
        canvas.ClipRect(clipInnerRect, RSClipOp::INTERSECT);
        // for default style, selection height is equal to the content height
        for (const auto& textBox : textBoxes) {
            canvas.DrawRect(
                RSRect(textBox.rect_.GetLeft() + (isTextArea ? contentOffset_->Get().GetX() : textRect.GetX()),
                    textBox.rect_.GetTop() + (isTextArea ? textRect.GetY() : contentOffset_->Get().GetY()),
                    textBox.rect_.GetRight() + (isTextArea ? contentOffset_->Get().GetX() : textRect.GetX()),
                    textBox.rect_.GetBottom() + (isTextArea ? textRect.GetY() : contentOffset_->Get().GetY())));
        }
    } else {
        if (!textBoxes.empty()) {
            auto theOnlyBox = *textBoxes.begin();
            // for inline style, selection height is equal to the frame height
            canvas.DrawRect(RSRect(theOnlyBox.rect_.GetLeft() + textRect.GetX(), 0.0f,
                theOnlyBox.rect_.GetRight() + textRect.GetX(), textFieldPattern->GetFrameRect().Height()));
        }
    }

    canvas.Restore();
}

void TextFieldOverlayModifier::PaintCursor(DrawingContext& context) const
{
    auto canvas = context.canvas;
    auto textFieldPattern = DynamicCast<TextFieldPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(textFieldPattern);
    if (!cursorVisible_->Get() || textFieldPattern->GetSelectMode() == SelectionMode::SELECT_ALL) {
        return;
    }
    auto paragraph = textFieldPattern->GetParagraph();
    CHECK_NULL_VOID(paragraph);
    canvas.Save();
    RSBrush brush;
    brush.SetAntiAlias(true);
    brush.SetColor(ToRSColor(cursorColor_->Get()));
    canvas.AttachBrush(brush);
    auto paintOffset = contentOffset_->Get() - OffsetF(0.0f, textFieldPattern->GetBaseLineOffset());
    float clipRectHeight = 0.0;
    if (showCounter_->Get() && textFieldPattern->GetCounterParagraph()) {
        clipRectHeight = paintOffset.GetY() + contentSize_->Get().Height() - textFieldPattern->GetCountHeight();
    } else {
        clipRectHeight = paintOffset.GetY() + contentSize_->Get().Height();
    }
    RSRect clipInnerRect(paintOffset.GetX(), paintOffset.GetY(),
        // add extra clip space for cases such as auto width
        paintOffset.GetX() + contentSize_->Get().Width() + cursorWidth_->Get() * 2.0f, clipRectHeight);
    if (!textFieldPattern->GetShowUnderLine()) {
        canvas.ClipRect(clipInnerRect, RSClipOp::INTERSECT);
    }
    auto caretRect = textFieldPattern->GetCaretRect();
    canvas.DrawRect(RSRect(cursorOffsetX_->Get(), caretRect.GetY(),
        cursorOffsetX_->Get() + static_cast<float>(cursorWidth_->Get()), caretRect.GetY() + caretRect.Height()));
        canvas.DetachBrush();
}

void TextFieldOverlayModifier::PaintScrollBar(RSCanvas& canvas)
{
    auto textFieldPattern = DynamicCast<TextFieldPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(textFieldPattern);
    auto scrollBar = scrollBar_.Upgrade();
    CHECK_NULL_VOID_NOLOG(scrollBar);
    textFieldPattern->CheckScrollable();
    if (scrollBar->NeedPaint() && textFieldPattern->IsScrollable()) {
        ScrollBarPainter::PaintRectBar(canvas, scrollBar);
    }
}

void TextFieldOverlayModifier::PaintEdgeEffect(const SizeF& frameSize, RSCanvas& canvas)
{
    auto edgeEffect = edgeEffect_.Upgrade();
    CHECK_NULL_VOID_NOLOG(edgeEffect);
    edgeEffect->Paint(canvas, frameSize, { 0.0f, 0.0f });
}

void TextFieldOverlayModifier::SetCursorColor(Color& value)
{
    cursorColor_->Set(LinearColor(value));
}

void TextFieldOverlayModifier::SetCursorWidth(float value)
{
    cursorWidth_->Set(value);
}

void TextFieldOverlayModifier::SetSelectedBackGroundColor(Color& value)
{
    selectedColor_->Set(LinearColor(value));
}

void TextFieldOverlayModifier::SetCursorVisible(bool value)
{
    cursorVisible_->Set(value);
}

void TextFieldOverlayModifier::SetContentSize(SizeF& value)
{
    contentSize_->Set(value);
}

void TextFieldOverlayModifier::SetContentOffset(OffsetF& value)
{
    contentOffset_->Set(value);
}

void TextFieldOverlayModifier::SetCursorOffsetX(float value)
{
    cursorOffsetX_->Set(value);
}

void TextFieldOverlayModifier::SetInputStyle(InputStyle& value)
{
    inputStyle_ = value;
}

void TextFieldOverlayModifier::SetFrameSize(const SizeF& value)
{
    frameSize_->Set(value);
}

void TextFieldOverlayModifier::SetCurrentOffset(float value)
{
    currentOffset_->Set(value);
}

void TextFieldOverlayModifier::SetSelectedAreaRedraw(bool value)
{
    isSelectedAreaRedraw_->Set(value);
}

void TextFieldOverlayModifier::SetUnderlineWidth(float value)
{
    underlineWidth_->Set(value);
}

void TextFieldOverlayModifier::SetUnderlineColor(const Color& value)
{
    underlineColor_->Set(value);
}

void TextFieldOverlayModifier::SetShowCounter(bool value)
{
    showCounter_->Set(value);
}
} // namespace OHOS::Ace::NG
