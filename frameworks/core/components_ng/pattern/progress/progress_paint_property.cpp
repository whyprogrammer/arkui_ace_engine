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

#include "core/components_ng/pattern/progress/progress_paint_property.h"

#include "base/geometry/dimension.h"
#include "core/common/container.h"
#include "core/components/progress/progress_theme.h"
#include "core/components_ng/pattern/progress/progress_date.h"
#include "core/components_v2/inspector/utils.h"
#include "core/pipeline/pipeline_base.h"

namespace OHOS::Ace::NG {
constexpr float PROGRSS_MAX_VALUE = 100.f;

void ProgressPaintProperty::ToJsonValue(std::unique_ptr<JsonValue>& json) const
{
    PaintProperty::ToJsonValue(json);

    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto progressTheme = pipeline->GetTheme<ProgressTheme>();
    CHECK_NULL_VOID(progressTheme);

    json->Put("constructor", ProgressOptions().c_str());
    json->Put("total", std::to_string(GetMaxValue().value_or(PROGRSS_MAX_VALUE)).c_str());
    json->Put("value", std::to_string(GetValue().value_or(0.f)).c_str());
    json->Put("scaleCount", std::to_string(GetScaleCount().value_or(progressTheme->GetScaleNumber())).c_str());
    json->Put("scaleWidth", (GetScaleWidth().value_or(progressTheme->GetScaleWidth()).ToString()).c_str());
    json->Put("color", (GetColor().value_or(progressTheme->GetTrackSelectedColor())).ColorToString().c_str());
    if (GetProgressType().value_or(ProgressType::LINEAR) == ProgressType::CAPSULE) {
        json->Put("backgroundColor",
            (GetBackgroundColor().value_or(progressTheme->GetCapsuleBgColor())).ColorToString().c_str());
    } else {
        json->Put("backgroundColor",
            (GetBackgroundColor().value_or(progressTheme->GetTrackBgColor())).ColorToString().c_str());
    }
    json->Put(
        "capsuleBorderColor", (GetBorderColor().value_or(progressTheme->GetBorderColor())).ColorToString().c_str());
    ToJsonValueForCapsule(json);
}

std::string ProgressPaintProperty::ProgressOptions() const
{
    auto jsonValue = JsonUtil::Create(true);
    jsonValue->Put("value", std::to_string(GetValue().value_or(0.f)).c_str());
    jsonValue->Put("total", std::to_string(GetMaxValue().value_or(PROGRSS_MAX_VALUE)).c_str());
    jsonValue->Put("type",
        ProgressTypeUtils::ConvertProgressTypeToString(GetProgressType().value_or(ProgressType::LINEAR)).c_str());
    return jsonValue->ToString();
}

void ProgressPaintProperty::ToJsonValueForCapsule(std::unique_ptr<JsonValue>& json) const
{
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto progressTheme = pipeline->GetTheme<ProgressTheme>();
    CHECK_NULL_VOID(progressTheme);
    auto capsuleStyle = JsonUtil::Create(false);
    auto fontStyle = JsonUtil::Create(false);
    auto font = JsonUtil::Create(false);
    capsuleStyle->Put(
        "borderWidth", (GetBorderWidth().value_or(progressTheme->GetBorderWidth())).ToString().c_str());
    capsuleStyle->Put(
        "borderColor", (GetBorderColor().value_or(progressTheme->GetBorderColor())).ColorToString().c_str());
    capsuleStyle->Put(
        "fontColor", (GetTextColor().value_or(progressTheme->GetTextColor())).ColorToString().c_str());
    capsuleStyle->Put(
        "content", (GetText().value_or("")).c_str());
    capsuleStyle->Put(
        "enableScanEffect", (GetEnableScanEffect().value_or(false)) ? "true" : "false");
    font->Put(
        "size", (GetTextSize().value_or(progressTheme->GetTextSize())).ToString().c_str());
    font->Put("style", GetItalicFontStyle().value_or(Ace::FontStyle::NORMAL) == Ace::FontStyle::NORMAL
        ? "FontStyle.Normal" : "FontStyle.Italic");
    font->Put("weight", V2::ConvertWrapFontWeightToStirng(GetFontWeight().value_or(FontWeight::NORMAL)).c_str());
    std::vector<std::string>  defaultFamily = { "Sans" };
    std::vector<std::string> fontFamilyVector = GetFontFamily().value_or(defaultFamily);
    if (fontFamilyVector.empty()) {
        fontFamilyVector = defaultFamily;
    }
    std::string fontFamily = fontFamilyVector.at(0);
    for (uint32_t i = 1; i < fontFamilyVector.size(); ++i) {
        fontFamily += ',' + fontFamilyVector.at(i);
    }
    font->Put("family", fontFamily.c_str());
    capsuleStyle->Put("font", font);
    json->Put("capsuleStyle", capsuleStyle);
}
} // namespace OHOS::Ace::NG
