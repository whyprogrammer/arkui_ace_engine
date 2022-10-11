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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_MODELS_TEXT_FIELD_MODEL_IMPL_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_MODELS_TEXT_FIELD_MODEL_IMPL_H

#include <optional>
#include <string>

#include "frameworks/core/components_ng/pattern/text_field/text_field_model.h"

namespace OHOS::Ace::Framework {

class TextFieldModelImpl : public TextFieldModel {
public:
    RefPtr<TextFieldControllerBase> CreateTextInput(
        const std::optional<std::string>& placeholder, const std::optional<std::string>& value) override;

    RefPtr<TextFieldControllerBase> CreateTextArea(
        const std::optional<std::string>& placeholder, const std::optional<std::string>& value) override
    {
        return nullptr;
    }

    void SetType(TextInputType value) override;
    void SetPlaceholderColor(const Color& value) override;
    void SetPlaceholderFont(const Font& value) override;
    void SetEnterKeyType(TextInputAction value) override;
    void SetTextAlign(TextAlign value) override;
    void SetCaretColor(const Color& value) override;
    void SetMaxLength(uint32_t value) override;
    void SetMaxLines(uint32_t value) override;
    void SetFontSize(const Dimension& value) override;
    void SetFontWeight(FontWeight value) override;
    void SetTextColor(const Color& value) override;
    void SetFontStyle(FontStyle value) override;
    void SetFontFamily(const std::vector<std::string>& value) override;
    void SetInputFilter(const std::string& value, const std::function<void(const std::string&)>& onError) override;
    void SetInputStyle(InputStyle value) override;
    void SetShowPasswordIcon(bool value) override;
    void SetOnEditChanged(std::function<void(bool)>&& func) override;
    void SetOnSubmit(std::function<void(int32_t)>&& func) override;
    void SetOnChange(std::function<void(const std::string&)>&& func) override;
    void SetOnCopy(std::function<void(const std::string&)>&& func) override;
    void SetOnCut(std::function<void(const std::string&)>&& func) override;
    void SetOnPaste(std::function<void(const std::string&)>&& func) override;
    void SetCopyOption(CopyOptions copyOption) override;
    static void InitTextInputDefaultStyle();
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_MODELS_TEXT_FIELD_MODEL_IMPL_H
