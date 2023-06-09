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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_PANEL_COMPOSED_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_PANEL_COMPOSED_ELEMENT_H

#include "base/utils/utils.h"
#include "core/components/panel/sliding_panel_element.h"
#include "core/components_v2/inspector/inspector_composed_element.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT PanelComposedElement : public InspectorComposedElement {
    DECLARE_ACE_TYPE(PanelComposedElement, InspectorComposedElement)

public:
    explicit PanelComposedElement(const ComposeId& id) : InspectorComposedElement(id) {}
    ~PanelComposedElement() override = default;

    void Dump() override;
    std::unique_ptr<JsonValue> ToJsonObject() const override;
    std::string GetPanelType() const;
    std::string GetPanelMode() const;
    std::string GetDragBar() const;
    std::string GetFullHeight() const;
    std::string GetHalfHeight() const;
    std::string GetMiniHeight() const;
    std::string GetShow() const;
    std::string GetBackgroundMask() const;

    AceType::IdType GetTargetTypeId() const override
    {
        return SlidingPanelElement::TypeId();
    }

    RefPtr<Element> GetRenderElement() const override
    {
        auto slidingPanelElement = GetContentElement<SlidingPanelElement>(SlidingPanelElement::TypeId());
        CHECK_NULL_RETURN(slidingPanelElement, nullptr);
        return GetInspectorComposedElementParent(slidingPanelElement);
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INSPECTOR_PANEL_COMPOSED_ELEMENT_H