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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_COUNTER_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_COUNTER_COMPOSED_ELEMENT_H

#include "base/utils/utils.h"
#include "core/components/counter/counter_element.h"
#include "core/components_v2/inspector/inspector_composed_element.h"

namespace OHOS::Ace::V2 {
class ACE_EXPORT CounterComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(CounterComposedElement, InspectorComposedElement)

public:
    explicit CounterComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~CounterComposedElement() override = default;

    std::string GetWidth() const override;
    std::string GetHeight() const override;
    RefPtr<Element> GetRenderElement() const override
    {
        auto counterElement = GetContentElement<CounterElement>(CounterElement::TypeId());
        CHECK_NULL_RETURN(counterElement, nullptr);
        return GetInspectorComposedElementParent(counterElement);
    }
};
}

#endif