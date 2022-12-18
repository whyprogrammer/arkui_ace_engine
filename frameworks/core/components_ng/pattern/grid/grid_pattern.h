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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_GRID_GRID_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_GRID_GRID_PATTERN_H

#include <memory>

#include "base/geometry/axis.h"
#include "base/memory/referenced.h"
#include "core/components_ng/pattern/grid/grid_event_hub.h"
#include "core/components_ng/pattern/grid/grid_layout_info.h"
#include "core/components_ng/pattern/grid/grid_layout_property.h"
#include "core/components_ng/pattern/grid/grid_paint_method.h"
#include "core/components_ng/pattern/grid/grid_paint_property.h"
#include "core/components_ng/pattern/grid/grid_position_controller.h"
#include "core/components_ng/pattern/pattern.h"
#include "core/components_ng/pattern/scroll_bar/proxy/scroll_bar_proxy.h"

namespace OHOS::Ace::NG {
class GridScrollBar;
class ACE_EXPORT GridPattern : public Pattern {
    DECLARE_ACE_TYPE(GridPattern, Pattern);

public:
    GridPattern() = default;
    ~GridPattern() override;

    bool IsAtomicNode() const override
    {
        return false;
    }

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<GridLayoutProperty>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override;

    RefPtr<PaintProperty> CreatePaintProperty() override
    {
        return MakeRefPtr<GridPaintProperty>();
    }

    RefPtr<NodePaintMethod> CreateNodePaintMethod() override;

    void SetMultiSelectable(bool multiSelectable)
    {
        multiSelectable_ = multiSelectable;
    }

    bool MultiSelectable() const
    {
        return multiSelectable_;
    }

    void SetSupportAnimation(bool supportAnimation)
    {
        supportAnimation_ = supportAnimation;
    }

    bool SupportAnimation() const
    {
        return supportAnimation_;
    }

    FocusPattern GetFocusPattern() const override
    {
        return { FocusType::SCOPE, true };
    }

    ScopeFocusAlgorithm GetScopeFocusAlgorithm() override
    {
        auto property = GetLayoutProperty<GridLayoutProperty>();
        if (!property) {
            return ScopeFocusAlgorithm();
        }
        return ScopeFocusAlgorithm(property->IsVertical(), true, ScopeType::OTHERS,
            [wp = WeakClaim(this)](
                FocusStep step, const WeakPtr<FocusHub>& currFocusNode, WeakPtr<FocusHub>& nextFocusNode) {
                auto grid = wp.Upgrade();
                if (grid) {
                    nextFocusNode = grid->GetNextFocusNode(step, currFocusNode);
                }
            });
    }

    RefPtr<EventHub> CreateEventHub() override
    {
        return MakeRefPtr<GridEventHub>();
    }

    bool UsResRegion() override
    {
        return false;
    }

    GridLayoutInfo GetGridLayoutInfo() const
    {
        return gridLayoutInfo_;
    }

    void ResetGridLayoutInfo()
    {
        gridLayoutInfo_ = GridLayoutInfo();
    }

    void ToJsonValue(std::unique_ptr<JsonValue>& json) const override;

    void OnMouseSelectAll();

    bool UpdateScrollPosition(float offset);

    void SetPositionController(const RefPtr<ScrollController>& controller);

    void SetScrollBarProxy(const RefPtr<NG::ScrollBarProxy>& scrollBarProxy);
    float GetScrollableDistance() const;
    float GetCurrentPosition() const;

    void ScrollPage(bool reverse);

    bool UpdateStartIndex(uint32_t index);

    bool AnimateTo(float position, float duration, const RefPtr<Curve>& curve);

    bool OnScrollCallback(float offset, int32_t source);

private:
    void OnAttachToFrameNode() override;
    void OnModifyDone() override;
    float GetMainContentSize() const;
    void AddScrollEvent();
    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config) override;
    WeakPtr<FocusHub> GetNextFocusNode(FocusStep step, const WeakPtr<FocusHub>& currentFocusNode);
    void InitOnKeyEvent(const RefPtr<FocusHub>& focusHub);
    bool OnKeyEvent(const KeyEvent& event);
    bool HandleDirectionKey(KeyCode code);
    void InitMouseEvent();
    void HandleMouseEventWithoutKeyboard(const MouseInfo& info);
    void ClearMultiSelect();
    void ClearSelectedZone();
    RectF ComputeSelectedZone(const OffsetF& startOffset, const OffsetF& endOffset);
    void MultiSelectWithoutKeyboard(const RectF& selectedZone);
    void UpdateScrollerAnimation(float offset);

    GridLayoutInfo gridLayoutInfo_;
    RefPtr<ScrollableEvent> scrollableEvent_;
    RefPtr<GridPositionController> positionController_;
    RefPtr<Animator> animator_;
    float animatorOffset_ = 0.0f;

    bool multiSelectable_ = false;
    bool supportAnimation_ = false;
    bool isConfigScrollable_ = false;
    bool isMouseEventInit_ = false;

    OffsetF mouseStartOffset_;
    OffsetF mouseEndOffset_;

    friend GridScrollBar;
    GridScrollBar* scrollBar_ = nullptr;

    ACE_DISALLOW_COPY_AND_MOVE(GridPattern);
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_GRID_GRID_PATTERN_H
