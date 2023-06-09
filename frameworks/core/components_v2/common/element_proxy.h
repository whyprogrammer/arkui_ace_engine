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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_COMMON_ELEMENT_PROXY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_COMMON_ELEMENT_PROXY_H

#include <limits>
#include <list>

#include "base/memory/referenced.h"
#include "base/utils/noncopyable.h"
#include "core/common/layout_inspector.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/element_register.h"

namespace OHOS::Ace::V2 {

class ElementProxyHost;

class ElementProxy : public virtual AceType {
    DECLARE_ACE_TYPE(ElementProxy, AceType);

public:
    static constexpr size_t INVALID_INDEX = std::numeric_limits<size_t>::max();

    static RefPtr<ElementProxy> Create(const WeakPtr<ElementProxyHost>& host, const RefPtr<Component>& component);

    explicit ElementProxy(const WeakPtr<ElementProxyHost>& host) : Referenced(false), host_(host) {}
    ~ElementProxy() override = default;

    virtual void Update(const RefPtr<Component>& component, size_t startIndex) = 0;
    virtual void UpdateIndex(size_t startIndex) = 0;
    virtual RefPtr<Component> GetComponentByIndex(size_t index) = 0;
    virtual RefPtr<Element> GetElementByIndex(size_t index) = 0;
    virtual void ReleaseElementByIndex(size_t index) = 0;
    virtual void ReleaseElementById(const ComposeId& composeId) = 0;
    virtual void RefreshActiveComposeIds() = 0;

    size_t RenderCount() const
    {
        return count_;
    }

    const ComposeId& GetId() const
    {
        return composedId_;
    }

    bool IndexInRange(size_t index) const
    {
        return (index >= startIndex_) && (index < startIndex_ + count_);
    }

    virtual void Dump(const std::string& prefix) const;

    int32_t GetElementId()
    {
        return elmtId_;
    }
    void SetElementId(int32_t id)
    {
        elmtId_ = id;
    }

    /**
     * method needs to be called after localized update
     * to the proxied Element
     */
    virtual void LocalizedUpdate(
        const RefPtr<Component>& newComponent, const RefPtr<Component>& outmostWrappingComponent)
    {}

protected:
    void AddSelfToElementRegistry();
    void RemoveSelfFromElementRegistry();

    WeakPtr<ElementProxyHost> host_;

    ComposeId composedId_;
    size_t startIndex_ = INVALID_INDEX;
    size_t count_ = 0;

private:
    ElementIdType elmtId_ = ElementRegister::UndefinedElementId;

    ACE_DISALLOW_COPY_AND_MOVE(ElementProxy);
};

class ElementProxyHost : virtual public AceType {
    DECLARE_ACE_TYPE(ElementProxyHost, AceType);

public:
    size_t TotalCount() const;

    void UpdateChildren(const std::list<RefPtr<Component>>& components);
    virtual void UpdateIndex();

    RefPtr<Component> GetComponentByIndex(size_t index);
    RefPtr<Element> GetElementByIndex(size_t index);
    void ReleaseElementByIndex(size_t index);
    void ReleaseElementById(const ComposeId& id);
    void AddComposeId(const ComposeId& id);
    void AddActiveComposeId(ComposeId& id);
    void ReleaseRedundantComposeIds();
    void DumpProxy();

    virtual RefPtr<Element> OnUpdateElement(const RefPtr<Element>& element, const RefPtr<Component>& component) = 0;
    virtual RefPtr<Component> OnMakeEmptyComponent() = 0;
    virtual void OnDataSourceUpdated(size_t startIndex)
    {
#if !defined(PREVIEW)
    LayoutInspector::SupportInspector();
#endif
    }
    virtual size_t GetReloadedCheckNum();

private:
    RefPtr<ElementProxy> proxy_;
    std::set<ComposeId> composeIds_;
    std::set<ComposeId> activeComposeIds_;
};

class ACE_EXPORT ForEachElementLookup {
public:
    static std::list<std::string> GetIdArray(int32_t elmtId);
};

} // namespace OHOS::Ace::V2


#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_COMMON_ELEMENT_PROXY_H
