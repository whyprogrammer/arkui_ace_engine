/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_DATEPICKER_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_DATEPICKER_H

#include "bridge/declarative_frontend/jsview/js_interactable_view.h"
#include "bridge/declarative_frontend/jsview/js_view_abstract.h"
#include "core/components/picker/picker_base_component.h"
#include "core/components_ng/pattern/picker/datepicker_event_hub.h"
#include "core/components_ng/pattern/picker/picker_type_define.h"
#include "frameworks/bridge/declarative_frontend/jsview/dialog/js_alert_dialog.h"

namespace OHOS::Ace::Framework {
class JSDatePicker : public JSViewAbstract {
public:
    static void Create(const JSCallbackInfo& info);

    static void JSBind(BindingTarget globalObj);
    static void SetLunar(bool isLunar);
    static void OnChange(const JSCallbackInfo& info);
    static void PickerBackgroundColor(const JSCallbackInfo& info);
    static void SetDisappearTextStyle(const JSCallbackInfo& info);
    static void SetTextStyle(const JSCallbackInfo& info);
    static void SetSelectedTextStyle(const JSCallbackInfo& info);
    static void ParseTextStyle(const JSRef<JSObject>& paramObj, NG::PickerTextStyle& textStyle);
    static void ParseTextProperties(const JSRef<JSObject>& paramObj, NG::PickerTextProperties& result);
    // keep compatible, need remove after
    static void UseMilitaryTime(bool isUseMilitaryTime);

private:
    static void CreateDatePicker(const JSCallbackInfo& info, const JSRef<JSObject>& paramObj);
    // keep compatible, need remove after
    static void CreateTimePicker(const JSCallbackInfo& info, const JSRef<JSObject>& paramObj);
    static PickerDate ParseDate(const JSRef<JSVal>& dateVal);
    static void SetDefaultAttributes();
    // keep compatible, need remove after
    static PickerTime ParseTime(const JSRef<JSVal>& timeVal);
};

class JSDatePickerDialog : JSAlertDialog {
public:
    static void JSBind(BindingTarget globalObj);
    static void Show(const JSCallbackInfo& info);
    static void DatePickerDialogShow(const JSRef<JSObject>& paramObj,
        const std::map<std::string, NG::DialogEvent>& dialogEvent,
        const std::map<std::string, NG::DialogGestureEvent>& dialogCancelEvent);

private:
    static void CreateDatePicker(RefPtr<Component>& component, const JSRef<JSObject>& paramObj);
    // keep compatible, need remove after
    static void CreateTimePicker(RefPtr<Component>& component, const JSRef<JSObject>& paramObj);
    static PickerDate ParseDate(const JSRef<JSVal>& dateVal);
    // keep compatible, need remove after
    static PickerTime ParseTime(const JSRef<JSVal>& timeVal);
};

class JSTimePicker : public JSViewAbstract {
public:
    static void Create(const JSCallbackInfo& info);

    static void JSBind(BindingTarget globalObj);
    static void OnChange(const JSCallbackInfo& info);
    static void UseMilitaryTime(bool isUseMilitaryTime);
    static void PickerBackgroundColor(const JSCallbackInfo& info);

    static void SetDisappearTextStyle(const JSCallbackInfo& info);
    static void SetTextStyle(const JSCallbackInfo& info);
    static void SetSelectedTextStyle(const JSCallbackInfo& info);
private:
    static void CreateTimePicker(const JSCallbackInfo& info, const JSRef<JSObject>& paramObj);
    static void SetDefaultAttributes();
    static PickerTime ParseTime(const JSRef<JSVal>& timeVal);
};

class JSTimePickerDialog {
public:
    static void JSBind(BindingTarget globalObj);
    static void Show(const JSCallbackInfo& info);
    static void TimePickerDialogShow(const JSRef<JSObject>& paramObj,
        const std::map<std::string, NG::DialogEvent>& dialogEvent,
        const std::map<std::string, NG::DialogGestureEvent>& dialogCancelEvent);

private:
    static void CreateTimePicker(RefPtr<Component>& component, const JSRef<JSObject>& paramObj);
    static PickerTime ParseTime(const JSRef<JSVal>& timeVal);
    static PickerDate ParseDate(const JSRef<JSVal>& dateVal);
};
} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_JS_DATEPICKER_H
