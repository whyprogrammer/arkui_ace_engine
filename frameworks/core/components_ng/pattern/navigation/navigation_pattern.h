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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_NAVIGATION_NAVIGATION_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_NAVIGATION_NAVIGATION_PATTERN_H

#include "base/memory/referenced.h"
#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/pattern/navigation/navigation_declaration.h"
#include "core/components_ng/pattern/navigation/navigation_event_hub.h"
#include "core/components_ng/pattern/navigation/navigation_group_node.h"
#include "core/components_ng/pattern/navigation/navigation_layout_algorithm.h"
#include "core/components_ng/pattern/navigation/navigation_layout_property.h"
#include "core/components_ng/pattern/navigation/navigation_stack.h"
#include "core/components_ng/pattern/navigation/title_bar_layout_property.h"
#include "core/components_ng/pattern/pattern.h"

namespace OHOS::Ace::NG {

using namespace Framework;
class NavigationPattern : public Pattern {
    DECLARE_ACE_TYPE(NavigationPattern, Pattern);

public:
    NavigationPattern() = default;
    ~NavigationPattern() override = default;

    bool IsAtomicNode() const override
    {
        return false;
    }

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<NavigationLayoutProperty>();
    }

    RefPtr<EventHub> CreateEventHub() override
    {
        return MakeRefPtr<NavigationEventHub>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        return MakeRefPtr<NavigationLayoutAlgorithm>();
    }

    void OnModifyDone() override;

    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config) override;

    void SetNavigationMode(NavigationMode navigationMode)
    {
        navigationMode_ = navigationMode;
    }

    FocusPattern GetFocusPattern() const override
    {
        return { FocusType::SCOPE, true };
    }

    void SetNavDestination(std::function<void(std::string)>&& builder)
    {
        builder_ = std::move(builder);
    }

    void SetNavigationStack(RefPtr<NavigationStack>&& navigationStack)
    {
        navigationStack_ = std::move(navigationStack);
    }

    const RefPtr<NavigationStack>& GetNavigationStack()
    {
        return navigationStack_;
    }

    void AddNavDestinationNode(const std::string& name, const RefPtr<UINode>& navDestinationNode)
    {
        navigationStack_->Add(name, navDestinationNode);
    }

    void AddNavDestinationNode(const std::string& name, const RefPtr<UINode>& navDestinationNode, NavRouteMode mode)
    {
        navigationStack_->Add(name, navDestinationNode, mode);
    }

    void AddNavDestinationNode(const std::string& name, RefPtr<UINode> navDestinationNode, NavRouteMode mode,
        const RefPtr<RouteInfo>& routeInfo)
    {
        navigationStack_->Add(name, navDestinationNode, mode, routeInfo);
    }

    RefPtr<UINode> GetNavDestinationNode(const std::string& name)
    {
        return navigationStack_->Get(name);
    }

    RefPtr<UINode> GetNavDestinationNode()
    {
        // get NavDestinationNode from the stack top
        return navigationStack_->Get();
    }

    RefPtr<UINode> GetPreNavDestination(const std::string& current)
    {
        return navigationStack_->GetPre(current);
    }

    const std::vector<std::pair<std::string, RefPtr<UINode>>>& GetAllNavDestinationNodes()
    {
        return navigationStack_->GetAllNavDestinationNodes();
    }

    std::pair<std::string, RefPtr<UINode>> GetTopNavPath()
    {
        if (navPathList_.empty()) {
            return std::make_pair("", nullptr);
        }
        int32_t top = navPathList_.size() - 1;
        return std::make_pair(navPathList_[top].first, navPathList_[top].second);
    }

    void RemoveIfNeeded(const std::string& name, const RefPtr<UINode>& navDestinationNode)
    {
        auto index = navigationStack_->FindIndex(name, navDestinationNode);
        // exit and not the top, need to be removed
        if (index != -1 && index != navPathList_.size() - 1) {
            navigationStack_->Remove(name, navDestinationNode);
        }
    }

private:
    RefPtr<RenderContext> GetTitleBarRenderContext();
    void DoAnimation(NavigationMode currentMode);
    bool CheckExistPreStack(const std::string& name);
    RefPtr<UINode> GetNodeAndRemoveByName(const std::string& name);
    RefPtr<UINode> GenerateUINodeByIndex(int32_t index);
    NavigationMode navigationMode_ = NavigationMode::AUTO;
    std::function<void(std::string)> builder_;
    RefPtr<NavigationStack> navigationStack_;
    NavPathList preNavPathList_;
    NavPathList navPathList_;
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_NAVIGATION_NAVIGATION_PATTERN_H
