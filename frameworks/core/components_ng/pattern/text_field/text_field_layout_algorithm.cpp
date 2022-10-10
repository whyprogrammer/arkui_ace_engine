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

#include "core/components_ng/pattern/text_field/text_field_layout_algorithm.h"

#include <algorithm>
#include <optional>
#include <unicode/uchar.h>

#include "base/geometry/axis.h"
#include "base/geometry/dimension.h"
#include "base/geometry/ng/size_t.h"
#include "base/i18n/localization.h"
#include "base/memory/referenced.h"
#include "base/utils/utils.h"
#include "core/components/font/constants_converter.h"
#include "core/components/text/text_theme.h"
#include "core/components/theme/theme_manager.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/text/text_layout_property.h"
#include "core/components_ng/pattern/text_field/text_field_layout_property.h"
#include "core/components_ng/pattern/text_field/text_field_pattern.h"
#include "core/components_ng/property/measure_utils.h"
#include "core/components_ng/render/drawing_prop_convertor.h"
#include "core/components_ng/render/font_collection.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {

void TextFieldLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    const auto& layoutConstraint = layoutWrapper->GetLayoutProperty()->GetLayoutConstraint();
    const auto& padding = layoutWrapper->GetLayoutProperty()->CreatePaddingAndBorder();
    OptionalSizeF frameSize =
        CreateIdealSize(layoutConstraint.value(), Axis::HORIZONTAL, MeasureType::MATCH_PARENT_MAIN_AXIS);
    const auto& content = layoutWrapper->GetGeometryNode()->GetContent();
    float contentHeight = 0.0f;
    if (content) {
        auto contentSize = content->GetRect().GetSize();
        AddPaddingToSize(padding, contentSize);
        contentHeight = contentSize.Height();
    }
    if (!frameSize.Height().has_value()) {
        frameSize.SetHeight(std::max(GetTextFieldDefaultHeight(), contentHeight));
    }
    layoutWrapper->GetGeometryNode()->SetFrameSize(frameSize.ConvertToSizeT());
}

std::optional<SizeF> TextFieldLayoutAlgorithm::MeasureContent(
    const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper)
{
    auto frameNode = layoutWrapper->GetHostNode();
    CHECK_NULL_RETURN(frameNode, std::nullopt);
    auto pipeline = frameNode->GetContext();
    CHECK_NULL_RETURN(pipeline, std::nullopt);
    auto textFieldLayoutProperty = DynamicCast<TextFieldLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_RETURN(textFieldLayoutProperty, std::nullopt);
    auto textFieldTheme = pipeline->GetTheme<TextFieldTheme>();
    CHECK_NULL_RETURN(textFieldTheme, std::nullopt);
    auto pattern = frameNode->GetPattern<TextFieldPattern>();
    TextStyle textStyle;
    if (textFieldLayoutProperty->HasValue()) {
        UpdateTextStyle(textFieldLayoutProperty, textFieldTheme, textStyle);
    } else {
        UpdatePlaceholderTextStyle(textFieldLayoutProperty, textFieldTheme, textStyle);
    }
    auto textContent = textFieldLayoutProperty->GetValueValue(textFieldLayoutProperty->GetPlaceholderValue(""));
    CreateParagraph(textStyle, textContent);

    float imageSize = 0.0f;
    auto showPasswordIcon = textFieldLayoutProperty->GetShowPasswordIcon().value_or(false);
    imageSize = showPasswordIcon ? GetTextFieldDefaultImageHeight() : 0.0f;
    if (contentConstraint.selfIdealSize.Height()) {
        imageSize = std::min(imageSize, contentConstraint.selfIdealSize.Height().value());
    }

    if (textStyle.GetMaxLines() == 1) {
        // for text input case, need to measure in one line without constraint.
        paragraph_->Layout(Infinity<float>());
    } else {
        paragraph_->Layout(contentConstraint.maxSize.Width() - imageSize);
    }

    auto paragraphNewWidth = static_cast<float>(paragraph_->GetMaxIntrinsicWidth());
    if (!NearEqual(paragraphNewWidth, paragraph_->GetMaxWidth())) {
        paragraph_->Layout(std::ceil(paragraphNewWidth));
    }

    switch (pattern->GetTextModified()) {
        // TODO: need to move these function out of measure process.
        case TextModifiedType::INPUT_METHOD:
            UpdateCaretPositionByTextEdit(layoutWrapper);
            break;
        case TextModifiedType::TOUCH_OR_KEY:
            UpdateCaretPositionByTouchOffset(layoutWrapper);
            break;
        default:
            break;
    }
    auto height = std::min(static_cast<float>(paragraph_->GetHeight()), contentConstraint.maxSize.Height());
    // check password image size.
    if (!showPasswordIcon) {
        textRect_.SetSize(SizeF(contentConstraint.maxSize.Width(), height));
        imageRect_.SetSize(SizeF());
        return SizeF(contentConstraint.maxSize.Width(), height);
    }

    height = std::min(static_cast<float>(paragraph_->GetHeight()), contentConstraint.maxSize.Height());
    textRect_.SetSize(SizeF(contentConstraint.maxSize.Width() - imageSize, static_cast<float>(height)));
    imageRect_.SetSize(SizeF(imageSize, imageSize));
    return SizeF(contentConstraint.maxSize.Width(), std::max(imageSize, height));
}

void TextFieldLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    // update child position.
    auto size = layoutWrapper->GetGeometryNode()->GetFrameSize();
    const auto& padding = layoutWrapper->GetLayoutProperty()->CreatePaddingAndBorder();
    MinusPaddingToSize(padding, size);
    auto left = padding.left.value_or(0);
    auto top = padding.top.value_or(0);
    auto paddingOffset = OffsetF(left, top);
    auto align = Alignment::CENTER;
    if (layoutWrapper->GetLayoutProperty()->GetPositionProperty()) {
        align = layoutWrapper->GetLayoutProperty()->GetPositionProperty()->GetAlignment().value_or(align);
    }
    // Update content position.
    const auto& content = layoutWrapper->GetGeometryNode()->GetContent();
    CHECK_NULL_VOID(content);
    auto contentSize = content->GetRect().GetSize();
    auto contentOffset = Alignment::GetAlignPosition(size, contentSize, align) + paddingOffset;
    content->SetOffset(contentOffset);
    // update text rect.
    auto textOffset = Alignment::GetAlignPosition(contentSize, textRect_.GetSize(), Alignment::CENTER_LEFT);
    textRect_.SetOffset(textOffset);
    // update image rect.
    if (!imageRect_.IsEmpty()) {
        auto imageOffset = Alignment::GetAlignPosition(contentSize, imageRect_.GetSize(), Alignment::CENTER_RIGHT);
        imageRect_.SetOffset(imageOffset);
    }
}

void TextFieldLayoutAlgorithm::UpdateTextStyle(
    const RefPtr<TextFieldLayoutProperty>& layoutProperty, const RefPtr<TextFieldTheme>& theme, TextStyle& textStyle)
{
    const std::vector<std::string> defaultFontFamily = { "sans-serif" };
    textStyle.SetFontFamilies(layoutProperty->GetFontFamilyValue(defaultFontFamily));
    textStyle.SetFontSize(layoutProperty->GetFontSizeValue(theme ? theme->GetFontSize() : textStyle.GetFontSize()));
    textStyle.SetFontWeight(
        layoutProperty->GetFontWeightValue(theme ? theme->GetFontWeight() : textStyle.GetFontWeight()));
    textStyle.SetTextColor(layoutProperty->GetTextColorValue(theme ? theme->GetTextColor() : textStyle.GetTextColor()));
    if (layoutProperty->GetMaxLines()) {
        textStyle.SetMaxLines(layoutProperty->GetMaxLines().value());
    }
    if (layoutProperty->HasItalicFontStyle()) {
        textStyle.SetFontStyle(layoutProperty->GetItalicFontStyle().value());
    }
    if (layoutProperty->HasTextAlign()) {
        textStyle.SetTextAlign(layoutProperty->GetTextAlign().value());
    }
}

void TextFieldLayoutAlgorithm::UpdatePlaceholderTextStyle(
    const RefPtr<TextFieldLayoutProperty>& layoutProperty, const RefPtr<TextFieldTheme>& theme, TextStyle& textStyle)
{
    const std::vector<std::string> defaultFontFamily = { "sans-serif" };
    textStyle.SetFontFamilies(layoutProperty->GetFontFamilyValue(defaultFontFamily));
    textStyle.SetFontSize(
        layoutProperty->GetPlaceholderFontSizeValue(theme ? theme->GetFontSize() : textStyle.GetFontSize()));
    textStyle.SetFontWeight(
        layoutProperty->GetPlaceholderFontWeightValue(theme ? theme->GetFontWeight() : textStyle.GetFontWeight()));
    textStyle.SetTextColor(
        layoutProperty->GetPlaceholderTextColorValue(theme ? theme->GetPlaceholderColor() : textStyle.GetTextColor()));
    if (layoutProperty->HasPlaceholderMaxLines()) {
        textStyle.SetMaxLines(layoutProperty->GetPlaceholderMaxLines().value());
    }
    if (layoutProperty->HasPlaceholderItalicFontStyle()) {
        textStyle.SetFontStyle(layoutProperty->GetPlaceholderItalicFontStyle().value());
    }
    if (layoutProperty->HasPlaceholderTextAlign()) {
        textStyle.SetTextAlign(layoutProperty->GetPlaceholderTextAlign().value());
    }
}

void TextFieldLayoutAlgorithm::CreateParagraph(const TextStyle& textStyle, std::string content)
{
    RSParagraphStyle paraStyle;
    paraStyle.textDirection_ = ToRSTextDirection(GetTextDirection(content));
    paraStyle.textAlign_ = ToRSTextAlign(textStyle.GetTextAlign());
    paraStyle.maxLines_ = textStyle.GetMaxLines();
    paraStyle.locale_ = Localization::GetInstance()->GetFontLocale();
    paraStyle.wordBreakType_ = ToRSWordBreakType(textStyle.GetWordBreak());
    if (textStyle.GetTextOverflow() == TextOverflow::ELLIPSIS) {
        paraStyle.ellipsis_ = RSParagraphStyle::ELLIPSIS;
    }

    auto builder = RSParagraphBuilder::CreateRosenBuilder(paraStyle, RSFontCollection::GetInstance(false));
    builder->PushStyle(ToRSTextStyle(PipelineContext::GetCurrentContext(), textStyle));
    StringUtils::TransformStrCase(content, static_cast<int32_t>(textStyle.GetTextCase()));
    builder->AddText(StringUtils::Str8ToStr16(content));
    builder->Pop();

    auto paragraph = builder->Build();
    paragraph_.reset(paragraph.release());
}

TextDirection TextFieldLayoutAlgorithm::GetTextDirection(const std::string& content)
{
    TextDirection textDirection = TextDirection::LTR;
    auto showingTextForWString = StringUtils::ToWstring(content);
    for (const auto& charOfShowingText : showingTextForWString) {
        if (u_charDirection(charOfShowingText) == UCharDirection::U_LEFT_TO_RIGHT) {
            textDirection = TextDirection::LTR;
        } else if (u_charDirection(charOfShowingText) == UCharDirection::U_RIGHT_TO_LEFT) {
            textDirection = TextDirection::RTL;
        } else if (u_charDirection(charOfShowingText) == UCharDirection::U_RIGHT_TO_LEFT_ARABIC) {
            textDirection = TextDirection::RTL;
        }
    }
    return textDirection;
}

const std::shared_ptr<RSParagraph>& TextFieldLayoutAlgorithm::GetParagraph()
{
    return paragraph_;
}

float TextFieldLayoutAlgorithm::GetTextFieldDefaultHeight()
{
    const auto defaultHeight = 40.0_vp;
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, defaultHeight.ConvertToPx());
    auto textFieldTheme = pipeline->GetTheme<TextFieldTheme>();
    CHECK_NULL_RETURN(textFieldTheme, defaultHeight.ConvertToPx());
    auto height = textFieldTheme->GetHeight();
    return static_cast<float>(height.ConvertToPx());
}

float TextFieldLayoutAlgorithm::GetTextFieldDefaultImageHeight()
{
    const auto defaultHeight = 40.0_vp;
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, defaultHeight.ConvertToPx());
    auto textFieldTheme = pipeline->GetTheme<TextFieldTheme>();
    CHECK_NULL_RETURN(textFieldTheme, defaultHeight.ConvertToPx());
    auto height = textFieldTheme->GetIconHotZoneSize();
    return static_cast<float>(height.ConvertToPx());
}

void TextFieldLayoutAlgorithm::UpdateCaretPositionByTextEdit(LayoutWrapper* layoutWrapper)
{
    auto host = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(host);
    auto pattern = host->GetPattern<TextFieldPattern>();
    CHECK_NULL_VOID(pattern);
    auto value = pattern->GetEditingValue();
    auto selection = value.selection;
    // scene of inserting text to tail
    if (selection.IsValid() && (selection.GetStart() == selection.GetEnd() &&
                                   selection.GetEnd() == static_cast<int32_t>(value.GetWideText().length()))) {
        pattern->SetCaretOffsetX(static_cast<float>(paragraph_->GetLongestLine()));
        return;
    }
    // scene of insert in the middle
    // after text insert, selection will update both base and extend offset to the same value
    CalcCaretByPosition(pattern, selection.baseOffset);
}

void TextFieldLayoutAlgorithm::UpdateCaretPositionByTouchOffset(LayoutWrapper* layoutWrapper)
{
    auto host = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(host);
    auto pattern = host->GetPattern<TextFieldPattern>();
    CHECK_NULL_VOID(pattern);
    auto offset = pattern->GetLastTouchOffset() - Offset(pattern->GetBasicPadding(), 0.0f);
    auto value = pattern->GetEditingValue();
    // update text editing value for touch out of boundary edge case
    if (pattern->GetTextDirection() == TextDirection::LTR &&
        GreatOrEqual(offset.GetX(), paragraph_->GetLongestLine())) {
        value.MoveToPosition(static_cast<int32_t>(value.GetWideText().length()));
        pattern->SetCaretOffsetX(static_cast<float>(paragraph_->GetLongestLine()));
        pattern->SetEditingValue(value);
        return;
    }
    auto position = ConvertTouchOffsetToCaretPosition(offset);
    UpdatePositionOfTextEditingValue(position, layoutWrapper);

    CalcCaretByPosition(pattern, position);
}

void TextFieldLayoutAlgorithm::CalcCaretByPosition(const RefPtr<Pattern>& pattern, int32_t position)
{
    auto textFieldPattern = DynamicCast<TextFieldPattern>(pattern);
    CHECK_NULL_VOID(textFieldPattern);
    CaretMetrics downStreamMetrics;
    if (!ComputeOffsetForCaretDownstream(textFieldPattern->GetEditingValue(), position, downStreamMetrics)) {
        textFieldPattern->SetCaretOffsetX(static_cast<float>(paragraph_->GetLongestLine()));
        LOGW("Get caret offset failed, set it to text tail");
        return;
    }
    textFieldPattern->SetCaretOffsetX(static_cast<float>(downStreamMetrics.offset.GetX()));
}

void TextFieldLayoutAlgorithm::UpdatePositionOfTextEditingValue(int32_t position, LayoutWrapper* layoutWrapper)
{
    auto host = layoutWrapper->GetHostNode();
    CHECK_NULL_VOID(host);
    auto pattern = host->GetPattern<TextFieldPattern>();
    CHECK_NULL_VOID(pattern);
    auto value = pattern->GetEditingValue();
    // TextEditingValue::MoveToPosition will handle edge values itself
    value.MoveToPosition(position);
    pattern->SetEditingValue(value);
}

bool TextFieldLayoutAlgorithm::ComputeOffsetForCaretDownstream(
    const TextEditingValue& textEditingValue, int32_t extent, CaretMetrics& result) const
{
    if (!paragraph_ || static_cast<size_t>(extent) >= textEditingValue.GetWideText().length()) {
        return false;
    }

    result.Reset();
    const int32_t graphemeClusterLength = 1;
    const int32_t next = extent + graphemeClusterLength;
    auto boxes = paragraph_->GetRectsForRange(
        extent, next, RSTypographyProperties::RectHeightStyle::MAX, RSTypographyProperties::RectWidthStyle::TIGHT);
    if (boxes.empty()) {
        LOGW("Box empty");
        return false;
    }

    const auto& textBox = *boxes.begin();
    // Caret is within width of the downstream glyphs.
    float caretStart = textBox.rect_.GetLeft();
    float offsetX = std::min(caretStart, static_cast<float>(paragraph_->GetLongestLine()));
    result.offset.SetX(offsetX);
    result.offset.SetY(textBox.rect_.GetTop());
    result.height = textBox.rect_.GetHeight();
    return true;
}

int32_t TextFieldLayoutAlgorithm::ConvertTouchOffsetToCaretPosition(const Offset& localOffset)
{
    if (!paragraph_) {
        LOGW("Paragraph is empty");
        return 0;
    }
    return static_cast<int32_t>(
        paragraph_->GetGlyphPositionAtCoordinateWithCluster(localOffset.GetX(), localOffset.GetY()).pos_);
}

} // namespace OHOS::Ace::NG
