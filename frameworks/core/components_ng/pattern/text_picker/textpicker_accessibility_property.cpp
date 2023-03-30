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

#include "core/components_ng/pattern/text_picker/textpicker_accessibility_property.h"

#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/pattern/text_picker/textpicker_column_pattern.h"
#include "core/components_ng/pattern/text_picker/textpicker_pattern.h"

namespace OHOS::Ace::NG {
constexpr int32_t MIDDLE_OF_COUNTS = 2;
std::string TextPickerAccessibilityProperty::GetText() const
{
    auto frameNode = host_.Upgrade();
    CHECK_NULL_RETURN(frameNode, "");
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, "");
    auto columnNode = textPickerPattern->GetColumnNode();
    CHECK_NULL_RETURN(columnNode, "");
    auto textPickerColumnPattern = columnNode->GetPattern<TextPickerColumnPattern>();
    CHECK_NULL_RETURN(textPickerColumnPattern, "");
    return textPickerColumnPattern->GetOption(textPickerColumnPattern->GetCurrentIndex());
}

int32_t TextPickerAccessibilityProperty::GetCurrentIndex() const
{
    auto frameNode = host_.Upgrade();
    CHECK_NULL_RETURN(frameNode, -1);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, -1);
    auto columnNode = textPickerPattern->GetColumnNode();
    CHECK_NULL_RETURN(columnNode, -1);
    auto textPickerColumnPattern = columnNode->GetPattern<TextPickerColumnPattern>();
    CHECK_NULL_RETURN(textPickerColumnPattern, -1);
    return textPickerColumnPattern->GetCurrentIndex();
}

int32_t TextPickerAccessibilityProperty::GetBeginIndex() const
{
    auto frameNode = host_.Upgrade();
    CHECK_NULL_RETURN(frameNode, -1);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, -1);
    auto columnNode = textPickerPattern->GetColumnNode();
    CHECK_NULL_RETURN(columnNode, -1);
    auto textPickerColumnPattern = columnNode->GetPattern<TextPickerColumnPattern>();
    CHECK_NULL_RETURN(textPickerColumnPattern, -1);
    auto middleIndex = textPickerColumnPattern->GetShowOptionCount() / MIDDLE_OF_COUNTS;
    auto currentIndex = textPickerColumnPattern->GetCurrentIndex();
    auto totalIndex = textPickerColumnPattern->GetOptionCount();
    if (totalIndex == 0) {
        return 0;
    }

    if (textPickerColumnPattern->NotLoopOptions()) {
        return 0;
    }

    return (currentIndex + totalIndex - middleIndex) % totalIndex;
}

int32_t TextPickerAccessibilityProperty::GetEndIndex() const
{
    auto frameNode = host_.Upgrade();
    CHECK_NULL_RETURN(frameNode, -1);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, -1);
    auto columnNode = textPickerPattern->GetColumnNode();
    CHECK_NULL_RETURN(columnNode, -1);
    auto textPickerColumnPattern = columnNode->GetPattern<TextPickerColumnPattern>();
    CHECK_NULL_RETURN(textPickerColumnPattern, -1);
    auto middleIndex = textPickerColumnPattern->GetShowOptionCount() / MIDDLE_OF_COUNTS;
    auto currentIndex = textPickerColumnPattern->GetCurrentIndex();
    auto totalIndex = textPickerColumnPattern->GetOptionCount();
    if (totalIndex == 0) {
        return 0;
    }

    if (textPickerColumnPattern->NotLoopOptions()) {
        return totalIndex - 1;
    }
    return (currentIndex + middleIndex) % totalIndex;
}

AccessibilityValue TextPickerAccessibilityProperty::GetAccessibilityValue() const
{
    AccessibilityValue result;
    auto frameNode = host_.Upgrade();
    CHECK_NULL_RETURN(frameNode, result);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, result);
    auto columnNode = textPickerPattern->GetColumnNode();
    CHECK_NULL_RETURN(columnNode, result);
    auto textPickerColumnPattern = columnNode->GetPattern<TextPickerColumnPattern>();
    CHECK_NULL_RETURN(textPickerColumnPattern, result);

    auto totalIndex = textPickerColumnPattern->GetOptionCount();
    if (totalIndex == 0) {
        return result;
    }
    result.min = 0;
    result.max = totalIndex - 1;
    result.current = textPickerColumnPattern->GetCurrentIndex();
    return result;
}

int32_t TextPickerAccessibilityProperty::GetCollectionItemCounts() const
{
    auto frameNode = host_.Upgrade();
    CHECK_NULL_RETURN(frameNode, -1);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, -1);
    auto columnNode = textPickerPattern->GetColumnNode();
    CHECK_NULL_RETURN(columnNode, -1);
    auto textPickerColumnPattern = columnNode->GetPattern<TextPickerColumnPattern>();
    CHECK_NULL_RETURN(textPickerColumnPattern, -1);
    return textPickerColumnPattern->GetOptionCount();
}

bool TextPickerAccessibilityProperty::IsScrollable() const
{
    auto frameNode = host_.Upgrade();
    CHECK_NULL_RETURN(frameNode, false);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, false);
    auto columnNode = textPickerPattern->GetColumnNode();
    CHECK_NULL_RETURN(columnNode, false);
    auto textPickerColumnPattern = columnNode->GetPattern<TextPickerColumnPattern>();
    CHECK_NULL_RETURN(textPickerColumnPattern, false);
    return textPickerColumnPattern->GetOptionCount() > 1;
}

void TextPickerAccessibilityProperty::SetSpecificSupportAction()
{
    auto frameNode = host_.Upgrade();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    auto columnNode = textPickerPattern->GetColumnNode();
    CHECK_NULL_VOID(columnNode);
    auto textPickerColumnPattern = columnNode->GetPattern<TextPickerColumnPattern>();
    CHECK_NULL_VOID(textPickerColumnPattern);
    if (IsScrollable()) {
        if (textPickerColumnPattern->NotLoopOptions()) {
            if (GetCurrentIndex() > GetBeginIndex()) {
                AddSupportAction(AceAction::ACTION_SCROLL_BACKWARD);
            }
            if (GetCurrentIndex() < GetEndIndex()) {
                AddSupportAction(AceAction::ACTION_SCROLL_FORWARD);
            }
        } else {
            AddSupportAction(AceAction::ACTION_SCROLL_FORWARD);
            AddSupportAction(AceAction::ACTION_SCROLL_BACKWARD);
        }
    }
}
} // namespace OHOS::Ace::NG
