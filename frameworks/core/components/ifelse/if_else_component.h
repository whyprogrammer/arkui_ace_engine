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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_COMPONENTS_IF_ELSE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_COMPONENTS_IF_ELSE_COMPONENT_H

#include "base/utils/macros.h"
#include "core/pipeline/base/multi_composed_component.h"

namespace OHOS::Ace {

class ACE_EXPORT IfElseComponent : public MultiComposedComponent {
    DECLARE_ACE_TYPE(IfElseComponent, MultiComposedComponent);

public:
    IfElseComponent(const ComposeId& id, const std::string& name) : MultiComposedComponent(id, name) {}
    ~IfElseComponent() override = default;

    RefPtr<Element> CreateElement() override;
    int32_t GetBranchId();
    void SetBranchId(int32_t branchId);

private:
    int32_t branchId_ = -1;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_COMPONENTS_IF_ELSE_COMPONENT_H
