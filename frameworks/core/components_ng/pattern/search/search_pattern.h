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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SEARCH_SEARCH_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SEARCH_SEARCH_PATTERN_H

#include "base/memory/referenced.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/search/search_event_hub.h"
#include "core/components_ng/pattern/search/search_layout_algorithm.h"
#include "core/components_ng/pattern/search/search_layout_property.h"
#include "core/components_ng/pattern/search/search_paint_method.h"
#include "core/components_ng/pattern/search/search_paint_property.h"

namespace OHOS::Ace::NG {

class SearchPattern : public Pattern {
    DECLARE_ACE_TYPE(SearchPattern, Pattern);

public:
    SearchPattern() = default;
    ~SearchPattern() override = default;

    bool IsAtomicNode() const override
    {
        return false;
    }

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<SearchLayoutProperty>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        return MakeRefPtr<SearchLayoutAlgorithm>();
    }

    RefPtr<PaintProperty> CreatePaintProperty() override
    {
        return MakeRefPtr<SearchPaintProperty>();
    }

    RefPtr<NodePaintMethod> CreateNodePaintMethod() override
    {
        auto paintMethod = MakeRefPtr<SearchPaintMethod>(buttonSize_);
        paintMethod->SetButtonSize(buttonSize_);
        return paintMethod;
    }

    RefPtr<EventHub> CreateEventHub() override
    {
        return MakeRefPtr<SearchEventHub>();
    }

    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& /*config*/) override
    {
        auto buttonLayoutWrapper = dirty->GetOrCreateChildByIndex(2);
        CHECK_NULL_RETURN(buttonLayoutWrapper, true);
        auto buttonGeometryNode = buttonLayoutWrapper->GetGeometryNode();
        CHECK_NULL_RETURN(buttonGeometryNode, true);
        buttonSize_ = buttonGeometryNode->GetFrameSize();
        return true;
    }

private:
    void OnModifyDone() override;
    void OnClickButtonAndImage();

    SizeF buttonSize_;
    RefPtr<ClickEvent> imageClickListener_;
    RefPtr<ClickEvent> buttonClickListener_;
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_SEARCH_SEARCH_PATTERN_H
