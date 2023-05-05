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

#include "bridge/declarative_frontend/jsview/js_progress.h"

#include "bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "bridge/declarative_frontend/jsview/models/progress_model_impl.h"
#include "core/components/common/properties/color.h"
#include "core/components/progress/progress_theme.h"
#include "core/components/text/text_theme.h"
#include "core/components_ng/pattern/progress/progress_model.h"
#include "core/components_ng/pattern/progress/progress_model_ng.h"

namespace OHOS::Ace {

std::unique_ptr<ProgressModel> ProgressModel::instance_ = nullptr;
std::mutex ProgressModel::mutex_;
ProgressType g_progressType = ProgressType::LINEAR;

ProgressModel* ProgressModel::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
#ifdef NG_BUILD
            instance_.reset(new NG::ProgressModelNG());
#else
            if (Container::IsCurrentUseNewPipeline()) {
                instance_.reset(new NG::ProgressModelNG());
            } else {
                instance_.reset(new Framework::ProgressModelImpl());
            }
#endif
        }
    }
    return instance_.get();
}

} // namespace OHOS::Ace

namespace OHOS::Ace::Framework {

void JSProgress::Create(const JSCallbackInfo& info)
{
    if (info.Length() != 1 || !info[0]->IsObject()) {
        LOGE("create progress fail beacase the param is invalid");
        return;
    }
    auto paramObject = JSRef<JSObject>::Cast(info[0]);

    auto value = 0;
    auto jsValue = paramObject->GetProperty("value");
    if (jsValue->IsNumber()) {
        value = jsValue->ToNumber<double>();
    } else {
        LOGE("create progress fail because the value is not number");
    }

    auto total = 100;
    auto jsTotal = paramObject->GetProperty("total");
    if (jsTotal->IsNumber() && jsTotal->ToNumber<int>() > 0) {
        total = jsTotal->ToNumber<int>();
    } else {
        LOGE("create progress fail because the total is not value or total is less than zero");
    }

    if (value > total) {
        LOGE("value is lager than total , set value euqals total");
        value = total;
    } else if (value < 0) {
        LOGE("value is s less than zero, set value euqals zero");
        value = 0;
    }

    auto jsStyle = paramObject->GetProperty("type");
    if (jsStyle->IsNull() || jsStyle->IsUndefined()) {
        jsStyle = paramObject->GetProperty("style");
    }

    auto progressStyle = static_cast<ProgressStyle>(jsStyle->ToNumber<int32_t>());
    if (progressStyle == ProgressStyle::Eclipse) {
        g_progressType = ProgressType::MOON;
    } else if (progressStyle == ProgressStyle::Ring) {
        g_progressType = ProgressType::RING;
    } else if (progressStyle == ProgressStyle::ScaleRing) {
        g_progressType = ProgressType::SCALE;
    } else if (progressStyle == ProgressStyle::Capsule) {
        g_progressType = ProgressType::CAPSULE;
    }

    ProgressModel::GetInstance()->Create(0.0, value, 0.0, total, static_cast<NG::ProgressType>(g_progressType));
}

void JSProgress::JSBind(BindingTarget globalObj)
{
    JSClass<JSProgress>::Declare("Progress");
    MethodOptions opt = MethodOptions::NONE;

    JSClass<JSProgress>::StaticMethod("create", &JSProgress::Create, opt);
    JSClass<JSProgress>::StaticMethod("value", &JSProgress::SetValue, opt);
    JSClass<JSProgress>::StaticMethod("color", &JSProgress::SetColor, opt);
    JSClass<JSProgress>::StaticMethod("circularStyle", &JSProgress::SetCircularStyle, opt);
    JSClass<JSProgress>::StaticMethod("cricularStyle", &JSProgress::SetCircularStyle, opt);
    JSClass<JSProgress>::StaticMethod("style", &JSProgress::SetCircularStyle, opt);
    JSClass<JSProgress>::StaticMethod("backgroundColor", &JSProgress::JsBackgroundColor, opt);
    JSClass<JSProgress>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSProgress>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSProgress>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSProgress>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSProgress>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSProgress>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSProgress>::StaticMethod("borderColor", &JSProgress::JsBorderColor, opt);
    JSClass<JSProgress>::Inherit<JSViewAbstract>();
    JSClass<JSProgress>::Bind(globalObj);
}

void JSProgress::SetValue(double value)
{
    if (std::isnan(value)) {
        return;
    }

    if (value < 0) {
        LOGE("value is leses than zero , set value euqals zero");
        value = 0;
    }

    ProgressModel::GetInstance()->SetValue(value);
}

void JSProgress::SetColor(const JSCallbackInfo& info)
{
    Color colorVal;
    if (!ParseJsColor(info[0], colorVal)) {
        RefPtr<ProgressTheme> theme = GetTheme<ProgressTheme>();
        if (g_progressType == ProgressType::CAPSULE) {
            colorVal = theme->GetCapsuleSelectColor();
        } else {
            colorVal = theme->GetTrackSelectedColor();
        }
    }

    ProgressModel::GetInstance()->SetColor(colorVal);
}

void JSProgress::SetCircularStyle(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    if (g_progressType == ProgressType::CAPSULE) {
        JsSetCapsuleStyle(info);
        return;
    }

    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    RefPtr<ProgressTheme> theme = GetTheme<ProgressTheme>();

    CalcDimension strokeWidthDimension;
    auto jsStrokeWidth = paramObject->GetProperty("strokeWidth");
    if (!ParseJsDimensionVp(jsStrokeWidth, strokeWidthDimension)) {
        LOGI("circular Style error. now use default strokeWidth");
        strokeWidthDimension = theme->GetTrackThickness();
    }

    if (strokeWidthDimension.Value() <= 0.0 || strokeWidthDimension.Unit() == DimensionUnit::PERCENT) {
        strokeWidthDimension = theme->GetTrackThickness();
    }

    ProgressModel::GetInstance()->SetStrokeWidth(strokeWidthDimension);

    auto jsScaleCount = paramObject->GetProperty("scaleCount");
    auto scaleCount = jsScaleCount->IsNumber() ? jsScaleCount->ToNumber<int32_t>() : theme->GetScaleNumber();
    if (scaleCount > 1.0) {
        ProgressModel::GetInstance()->SetScaleCount(scaleCount);
    } else {
        ProgressModel::GetInstance()->SetScaleCount(theme->GetScaleNumber());
    }

    CalcDimension scaleWidthDimension;
    auto jsScaleWidth = paramObject->GetProperty("scaleWidth");
    if (!ParseJsDimensionVp(jsScaleWidth, scaleWidthDimension)) {
        LOGI("circular Style error. now use default scaleWidth");
        scaleWidthDimension = theme->GetScaleWidth();
    }

    if ((scaleWidthDimension.Value() <= 0.0) || (scaleWidthDimension.Value() > strokeWidthDimension.Value()) ||
        scaleWidthDimension.Unit() == DimensionUnit::PERCENT) {
        scaleWidthDimension = theme->GetScaleWidth();
    }

    ProgressModel::GetInstance()->SetScaleWidth(scaleWidthDimension);
}

void JSProgress::JsBackgroundColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    Color colorVal;
    if (!ParseJsColor(info[0], colorVal)) {
        RefPtr<ProgressTheme> theme = GetTheme<ProgressTheme>();
        if (g_progressType == ProgressType::CAPSULE) {
            colorVal = theme->GetCapsuleBgColor();
        } else {
            colorVal = theme->GetTrackBgColor();
        }
    }

    ProgressModel::GetInstance()->SetBackgroundColor(colorVal);
}

void JSProgress::JsBorderColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JSViewAbstract::JsBorderColor(info);
}

void JSProgress::JsSetCapsuleStyle(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    RefPtr<ProgressTheme> theme = GetTheme<ProgressTheme>();

    auto jsBorderWidth = paramObject->GetProperty("borderWidth");
    CalcDimension borderWidth;
    if (!ParseJsDimensionVp(jsBorderWidth, borderWidth)) {
        LOGI("JsProgress set Inner BorderWidth is mull");
        borderWidth = theme->GetBorderWidth();
    }
    if (LessNotEqual(borderWidth.Value(), 0.0)) {
        LOGI("JsProgress set Inner BorderWidth is to small");
        borderWidth = theme->GetBorderWidth();
    }
    ProgressModel::GetInstance()->SetBorderWidth(borderWidth);

    auto jsBorderColor = paramObject->GetProperty("borderColor");
    Color colorVal;
    if (!ParseJsColor(jsBorderColor, colorVal)) {
        colorVal = theme->GetBorderColor();
    }
    ProgressModel::GetInstance()->SetBorderColor(colorVal);

    auto jsSweepingEffect = paramObject->GetProperty("enableScanEffect");
    bool sweepingEffect;
    if (!ParseJsBool(jsSweepingEffect, sweepingEffect)) {
        sweepingEffect = false;
    }
    ProgressModel::GetInstance()->SetSweepingEffect(sweepingEffect);

    auto jsContext = paramObject->GetProperty("content");
    std::string text;
    if (jsContext->IsUndefined() || jsContext->IsNull() || (!ParseJsString(jsContext, text))) {
        ProgressModel::GetInstance()->SetText(std::nullopt);
    } else {
        ProgressModel::GetInstance()->SetText(text);
    }

    JsSetFontStyle(info);
}

void JSProgress::JsSetFontStyle(const JSCallbackInfo& info)
{
    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    RefPtr<ProgressTheme> theme = GetTheme<ProgressTheme>();
    RefPtr<TextTheme> textTheme = GetTheme<TextTheme>();
    auto jsFontColor = paramObject->GetProperty("fontColor");
    Color fontColorVal;
    if (!ParseJsColor(jsFontColor, fontColorVal)) {
        fontColorVal = theme->GetTextColor();
    }
    
    ProgressModel::GetInstance()->SetFontColor(fontColorVal);

    auto textStyle = paramObject->GetProperty("font");
    if (!textStyle->IsObject()) {
        LOGI("JsProgress textStyle is not object");
        JsSetFontDefault();
    } else {
        auto textObject = JSRef<JSObject>::Cast(textStyle);
        JsSetFont(textObject);
    }
}

void JSProgress::JsSetFontDefault()
{
    RefPtr<TextTheme> textTheme = GetTheme<TextTheme>();
    RefPtr<ProgressTheme> progressTheme = GetTheme<ProgressTheme>();
    ProgressModel::GetInstance()->SetFontSize(progressTheme->GetTextSize());
    ProgressModel::GetInstance()->SetFontFamily(textTheme->GetTextStyle().GetFontFamilies());
    ProgressModel::GetInstance()->SetFontWeight(textTheme->GetTextStyle().GetFontWeight());
    ProgressModel::GetInstance()->SetItalicFontStyle(textTheme->GetTextStyle().GetFontStyle());
}

void JSProgress::JsSetFont(const JSRef<JSObject>& textObject)
{
    RefPtr<ProgressTheme> theme = GetTheme<ProgressTheme>();
    CHECK_NULL_VOID(theme);
    RefPtr<TextTheme> textTheme = GetTheme<TextTheme>();
    CHECK_NULL_VOID(textTheme);
    auto size = textObject->GetProperty("size");
    CalcDimension fontSize;
    if (!ParseJsDimensionFp(size, fontSize)) {
        LOGI("JsProgress set fontSize is mull");
        fontSize = theme->GetTextSize();
    }
    if (LessNotEqual(fontSize.Value(), 0.0)) {
        LOGI("JsProgress set fontSize is to small");
        fontSize = theme->GetTextSize();
    }
    ProgressModel::GetInstance()->SetFontSize(fontSize);

    std::string weight;
    auto fontWeight = textObject->GetProperty("weight");
    if (!fontWeight->IsNull()) {
        if (fontWeight->IsNumber()) {
            weight = std::to_string(fontWeight->ToNumber<int32_t>());
        } else {
            ParseJsString(fontWeight, weight);
        }
        ProgressModel::GetInstance()->SetFontWeight(ConvertStrToFontWeight(weight));
    } else {
        ProgressModel::GetInstance()->SetFontWeight(textTheme->GetTextStyle().GetFontWeight());
    }

    auto family = textObject->GetProperty("family");
    if (!family->IsNull() && family->IsString()) {
        auto familyVal = family->ToString();
        ProgressModel::GetInstance()->SetFontFamily(ConvertStrToFontFamilies(familyVal));
    } else {
        ProgressModel::GetInstance()->SetFontFamily(textTheme->GetTextStyle().GetFontFamilies());
    }

    auto style = textObject->GetProperty("style");
    if (!style->IsNull() && style->IsNumber()) {
        auto styleVal = static_cast<FontStyle>(style->ToNumber<int32_t>());
        ProgressModel::GetInstance()->SetItalicFontStyle(styleVal);
    } else {
        ProgressModel::GetInstance()->SetItalicFontStyle(textTheme->GetTextStyle().GetFontStyle());
    }
}
} // namespace OHOS::Ace::Framework
