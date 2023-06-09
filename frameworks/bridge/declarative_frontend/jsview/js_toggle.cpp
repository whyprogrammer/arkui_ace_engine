/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "frameworks/bridge/declarative_frontend/jsview/js_toggle.h"

#include <cstddef>
#include <string>

#include "base/log/ace_scoring_log.h"
#include "bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "bridge/declarative_frontend/jsview/models/toggle_model_impl.h"
#include "core/common/container.h"
#include "core/components/common/properties/color.h"
#include "core/components/toggle/toggle_theme.h"
#include "core/components_ng/pattern/button/toggle_button_model_ng.h"
#include "core/components_ng/pattern/toggle/toggle_model_ng.h"

namespace OHOS::Ace {

std::unique_ptr<ToggleModel> ToggleModel::instance_ = nullptr;
std::mutex ToggleModel::mutex_;

ToggleModel* ToggleModel::GetInstance()
{
    if (!instance_) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!instance_) {
#ifdef NG_BUILD
            instance_.reset(new NG::ToggleModelNG());
#else
            if (Container::IsCurrentUseNewPipeline()) {
                instance_.reset(new NG::ToggleModelNG());
            } else {
                instance_.reset(new Framework::ToggleModelImpl());
            }
#endif
        }
    }
    return instance_.get();
}

} // namespace OHOS::Ace

namespace OHOS::Ace::Framework {

void JSToggle::JSBind(BindingTarget globalObj)
{
    JSClass<JSToggle>::Declare("Toggle");
    JSClass<JSToggle>::StaticMethod("create", &JSToggle::Create);
    JSClass<JSToggle>::StaticMethod("onChange", &JSToggle::OnChange);
    JSClass<JSToggle>::StaticMethod("selectedColor", &JSToggle::SelectedColor);
    JSClass<JSToggle>::StaticMethod("width", &JSToggle::JsWidth);
    JSClass<JSToggle>::StaticMethod("height", &JSToggle::JsHeight);
    JSClass<JSToggle>::StaticMethod("size", &JSToggle::JsSize);
    JSClass<JSToggle>::StaticMethod("padding", &JSToggle::JsPadding);
    JSClass<JSToggle>::StaticMethod("switchPointColor", &JSToggle::SwitchPointColor);
    JSClass<JSToggle>::StaticMethod("backgroundColor", &JSToggle::SetBackgroundColor);

    JSClass<JSToggle>::StaticMethod("onTouch", &JSInteractableView::JsOnTouch);
    JSClass<JSToggle>::StaticMethod("onClick", &JSInteractableView::JsOnClick);
    JSClass<JSToggle>::StaticMethod("onHover", &JSInteractableView::JsOnHover);
    JSClass<JSToggle>::StaticMethod("onKeyEvent", &JSInteractableView::JsOnKey);
    JSClass<JSToggle>::StaticMethod("onDeleteEvent", &JSInteractableView::JsOnDelete);
    JSClass<JSToggle>::StaticMethod("onAppear", &JSInteractableView::JsOnAppear);
    JSClass<JSToggle>::StaticMethod("onDisAppear", &JSInteractableView::JsOnDisAppear);
    JSClass<JSToggle>::Inherit<JSViewAbstract>();
    JSClass<JSToggle>::Bind(globalObj);
}

void ParseToggleIsOnObject(const JSCallbackInfo& info, const JSRef<JSVal>& changeEventVal)
{
    CHECK_NULL_VOID(changeEventVal->IsFunction());

    auto jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(changeEventVal));
    auto onChangeEvent = [execCtx = info.GetExecutionContext(), func = std::move(jsFunc)](bool isOn) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("Toggle.onChangeEvent");
        auto newJSVal = JSRef<JSVal>::Make(ToJSValue(isOn));
        func->ExecuteJS(1, &newJSVal);
    };
    ToggleModel::GetInstance()->OnChangeEvent(std::move(onChangeEvent));
}

void JSToggle::Create(const JSCallbackInfo& info)
{
    if (info.Length() < 1 || !info[0]->IsObject()) {
        LOGE("toggle create error, info is non-valid");
        return;
    }

    auto paramObject = JSRef<JSObject>::Cast(info[0]);
    auto type = paramObject->GetProperty("type");
    int32_t toggleTypeInt = 1;
    if (type->IsNumber()) {
        toggleTypeInt = type->ToNumber<int32_t>();
    }
    if (toggleTypeInt < 0 || toggleTypeInt > 2) {
        toggleTypeInt = 1;
    }
    auto tempIsOn = paramObject->GetProperty("isOn");
    bool isOn = false;
    JSRef<JSVal> changeEventVal;
    if (tempIsOn->IsObject()) {
        JSRef<JSObject> isOnObj = JSRef<JSObject>::Cast(tempIsOn);
        changeEventVal = isOnObj->GetProperty("changeEvent");
        auto isOnProperty = isOnObj->GetProperty("value");
        isOn = isOnProperty->IsBoolean() ? isOnProperty->ToBoolean() : false;
    } else {
        isOn = tempIsOn->IsBoolean() ? tempIsOn->ToBoolean() : false;
    }
    
    ToggleModel::GetInstance()->Create(NG::ToggleType(toggleTypeInt), isOn);
    if (!changeEventVal->IsUndefined() && changeEventVal->IsFunction()) {
        ParseToggleIsOnObject(info, changeEventVal);
    }
}

void JSToggle::JsWidth(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsWidth(info[0]);
}

void JSToggle::JsWidth(const JSRef<JSVal>& jsValue)
{
    CalcDimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return;
    }

    if (Container::IsCurrentUseNewPipeline()) {
        JSViewAbstract::JsWidth(jsValue);
        return;
    }

    ToggleModel::GetInstance()->SetWidth(value);
}

void JSToggle::JsHeight(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    JsHeight(info[0]);
}

void JSToggle::JsHeight(const JSRef<JSVal>& jsValue)
{
    CalcDimension value;
    if (!ParseJsDimensionVp(jsValue, value)) {
        return;
    }

    if (Container::IsCurrentUseNewPipeline()) {
        JSViewAbstract::JsHeight(jsValue);
        return;
    }
    ToggleModel::GetInstance()->SetHeight(value);
}

void JSToggle::JsSize(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }

    if (!info[0]->IsObject()) {
        LOGE("arg is not Object or String.");
        return;
    }

    JSRef<JSObject> sizeObj = JSRef<JSObject>::Cast(info[0]);
    JsWidth(sizeObj->GetProperty("width"));
    JsHeight(sizeObj->GetProperty("height"));
}

void JSToggle::OnChange(const JSCallbackInfo& args)
{
    if (args.Length() < 1 || !args[0]->IsFunction()) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 argument.");
        return;
    }
    auto jsFunc = AceType::MakeRefPtr<JsFunction>(JSRef<JSObject>(), JSRef<JSFunc>::Cast(args[0]));
    auto onChange = [execCtx = args.GetExecutionContext(), func = std::move(jsFunc)](bool isOn) {
        JAVASCRIPT_EXECUTION_SCOPE_WITH_CHECK(execCtx);
        ACE_SCORING_EVENT("Toggle.onChange");
        auto newJSVal = JSRef<JSVal>::Make(ToJSValue(isOn));
        func->ExecuteJS(1, &newJSVal);
    };
    ToggleModel::GetInstance()->OnChange(std::move(onChange));
    args.ReturnSelf();
}

void JSToggle::SelectedColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    Color color;
    std::optional<Color> selectedColor;
    if (ParseJsColor(info[0], color)) {
        selectedColor = color;
    }

    ToggleModel::GetInstance()->SetSelectedColor(selectedColor);
}

void JSToggle::SwitchPointColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    Color color;
    if (!ParseJsColor(info[0], color)) {
        auto theme = GetTheme<SwitchTheme>();
        if (theme) {
            color = theme->GetPointColor();
        }
    }

    ToggleModel::GetInstance()->SetSwitchPointColor(color);
}

void JSToggle::JsPadding(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The arg is wrong, it is supposed to have atleast 1 arguments");
        return;
    }
    if (!info[0]->IsString() && !info[0]->IsNumber() && !info[0]->IsObject()) {
        LOGE("arg is not a string, number or object.");
        return;
    }
    if (Container::IsCurrentUseNewPipeline() || ToggleModel::GetInstance()->IsToggle()) {
        JSViewAbstract::JsPadding(info);
        return;
    }

    if (info[0]->IsObject()) {
        auto argsPtrItem = JsonUtil::ParseJsonString(info[0]->ToString());
        if (!argsPtrItem || argsPtrItem->IsNull()) {
            LOGE("Js Parse object failed. argsPtr is null. %s", info[0]->ToString().c_str());
            return;
        }
        if (argsPtrItem->Contains("top") || argsPtrItem->Contains("bottom") || argsPtrItem->Contains("left") ||
            argsPtrItem->Contains("right")) {
            CalcDimension topDimen = CalcDimension(0.0, DimensionUnit::VP);
            CalcDimension leftDimen = CalcDimension(0.0, DimensionUnit::VP);
            CalcDimension rightDimen = CalcDimension(0.0, DimensionUnit::VP);
            CalcDimension bottomDimen = CalcDimension(0.0, DimensionUnit::VP);
            ParseJsonDimensionVp(argsPtrItem->GetValue("top"), topDimen);
            ParseJsonDimensionVp(argsPtrItem->GetValue("left"), leftDimen);
            ParseJsonDimensionVp(argsPtrItem->GetValue("right"), rightDimen);
            ParseJsonDimensionVp(argsPtrItem->GetValue("bottom"), bottomDimen);
            if (leftDimen == 0.0_vp) {
                leftDimen = rightDimen;
            }
            if (topDimen == 0.0_vp) {
                topDimen = bottomDimen;
            }
            if (leftDimen == 0.0_vp) {
                leftDimen = topDimen;
            }
            NG::PaddingPropertyF padding;
            padding.left = leftDimen.ConvertToPx();
            padding.right = rightDimen.ConvertToPx();
            padding.top = topDimen.ConvertToPx();
            padding.bottom = bottomDimen.ConvertToPx();
            ToggleModel::GetInstance()->SetPadding(padding);
            return;
        }
    }
    CalcDimension length;
    if (!JSViewAbstract::ParseJsDimensionVp(info[0], length)) {
        return;
    }
    NG::PaddingPropertyF padding;
    padding.left = length.ConvertToPx();
    padding.right = length.ConvertToPx();
    padding.top = length.ConvertToPx();
    padding.bottom = length.ConvertToPx();
    ToggleModel::GetInstance()->SetPadding(padding);
}

void JSToggle::SetBackgroundColor(const JSCallbackInfo& info)
{
    if (info.Length() < 1) {
        LOGE("The argv is wrong, it is supposed to have at least 1 argument");
        return;
    }
    Color backgroundColor;
    if (!ParseJsColor(info[0], backgroundColor)) {
        return;
    }

    if (!Container::IsCurrentUseNewPipeline()) {
        JSViewAbstract::JsBackgroundColor(info);
        return;
    }
    ToggleModel::GetInstance()->SetBackgroundColor(backgroundColor);
}

} // namespace OHOS::Ace::Framework
