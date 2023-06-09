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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_COMPONENTS_IF_ELSE_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_COMPONENTS_IF_ELSE_ELEMENT_H

#include <string>

#include "base/utils/macros.h"
#include "frameworks/core/pipeline/base/multi_composed_element.h"

namespace OHOS::Ace {

class ACE_EXPORT IfElseElement : public MultiComposedElement {
    DECLARE_ACE_TYPE(IfElseElement, MultiComposedElement);

public:
    explicit IfElseElement(const ComposeId& id) : MultiComposedElement(id) {}
    ~IfElseElement() override = default;

    bool CanUpdate(const RefPtr<Component>& newComponent) override;
    void Update() override;

    // added for partial update
    int32_t GetBranchId()
    {
        return branchId_;
    }

    static void ComponentToElementLocalizedUpdate(
        const RefPtr<Component>& component, RefPtr<Element>& element);
        
private:
    int32_t branchId_ = -1;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_COMPONENTS_IF_ELSE_ELEMENT_H
