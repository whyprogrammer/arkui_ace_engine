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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_BUTTON_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_BUTTON_ELEMENT_H

#include "core/components/button/button_component.h"
#include "core/components/button/render_button.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

class ButtonElement : public ComponentGroupElement, public FocusNode {
    DECLARE_ACE_TYPE(ButtonElement, ComponentGroupElement, FocusNode);

public:
    ButtonElement() = default;
    ~ButtonElement() override = default;

    void Apply(const RefPtr<Element>& child) override;
    void Update() override;
    void LocalizedUpdate() override;

private:
    void OnFocus() override;
    void OnClick() override;
    bool OnClick(const KeyEvent& event) override;
    void OnBlur() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    void OnChange();

    RefPtr<RenderButton> button_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BUTTON_BUTTON_ELEMENT_H
