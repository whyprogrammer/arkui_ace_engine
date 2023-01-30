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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_PANEL_PANEL_PATTERN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_PANEL_PANEL_PATTERN_H

#include <optional>
#include "base/geometry/axis.h"
#include "base/memory/referenced.h"
#include "core/components/common/layout/constants.h"
#include "core/components/panel/panel_component.h"
#include "core/components_ng/event/event_hub.h"
#include "core/components_ng/pattern/panel/drag_bar_pattern.h"
#include "core/components_ng/pattern/panel/sliding_panel_event_hub.h"
#include "core/components_ng/pattern/panel/sliding_panel_layout_algorithm.h"
#include "core/components_ng/pattern/panel/sliding_panel_layout_property.h"
#include "core/components_ng/pattern/pattern.h"

namespace OHOS::Ace::NG {

class SlidingPanelPattern : public Pattern {
    DECLARE_ACE_TYPE(SlidingPanelPattern, Pattern);

public:
    SlidingPanelPattern() = default;
    ~SlidingPanelPattern() override = default;

    bool IsAtomicNode() const override
    {
        return false;
    }

    RefPtr<LayoutProperty> CreateLayoutProperty() override
    {
        return MakeRefPtr<SlidingPanelLayoutProperty>();
    }

    RefPtr<LayoutAlgorithm> CreateLayoutAlgorithm() override
    {
        auto layoutAlgorithm = MakeRefPtr<SlidingPanelLayoutAlgorithm>();
        layoutAlgorithm->SetCurrentOffset(currentOffset_);
        layoutAlgorithm->SetIsFirstLayout(isFirstLayout_);
        return layoutAlgorithm;
    }

    RefPtr<EventHub> CreateEventHub() override
    {
        return MakeRefPtr<SlidingPanelEventHub>();
    }

    void OnAnimationStop();
    void UpdateCurrentOffset(float offset);
    void UpdateCurrentOffsetOnAnimate(float currentOffset);
    void MarkDirtyNode(PropertyChangeFlag extraFlag);
    void ToJsonValue(std::unique_ptr<JsonValue>& json) const override;

private:
    void OnModifyDone() override;
    void OnAttachToFrameNode() override;
    bool OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config) override;

    // Init pan recognizer to move items when drag update, play translate animation when drag end.
    void InitPanEvent(const RefPtr<GestureEventHub>& gestureHub);
    void Update();
    // Init LayoutProperties
    void InitializeLayoutProps();

    void FireSizeChangeEvent();
    void FireHeightChangeEvent();
    void HandleDragStart(const Offset& startPoint);
    void HandleDragUpdate(const GestureEvent& info);
    void HandleDragEnd(float dragVelocity);
    void CalculateModeTypeMini(float dragLen, float dragVelocity);
    void CalculateModeTypeFold(float dragLen, float dragVelocity);
    void CalculateModeTypeTemp(float dragLen, float dragVelocity);
    void AnimateTo(float targetLocation, PanelMode mode);
    void AppendBlankHeightAnimation(float targetLocation, PanelMode mode);
    int32_t GetAnimationDuration(float delta, float dragRange) const;
    void CheckHeightValidity();
    void CheckPanelModeAndType();
    void FirstLayout();
    void IsShowChanged(bool isShow);
    void HeightDynamicUpdate();
    void SetDragBarCallBack();

    PanelType GetPanelType() const;
    PanelMode GetPanelMode() const;
    RefPtr<FrameNode> GetDragBarNode();
    void FireChangeEvent() const;

    RefPtr<PanEvent> panEvent_;
    RefPtr<Animator> animator_;
    std::unordered_map<PanelMode, double> defaultBlankHeights_;
    
    bool isAnimating_ = false;
    bool isFirstLayout_ = true;

    PanelMode mode_ = PanelMode::FULL;
    PanelMode previousMode_ = PanelMode::HALF;
    PanelType type_ = PanelType::FOLDABLE_BAR;
    float fullHalfBoundary_ = 0.0f;
    float halfMiniBoundary_ = 0.0f;
    float fullMiniBoundary_ = 0.0f;

    Dimension fullHeight_;
    Dimension halfHeight_;
    Dimension miniHeight_;

    float minBlankHeight_ = 0.0;
    float currentOffset_ = 0.0f;
    float dragStartCurrentOffset_ = 0.0f;

    std::optional<bool> isShow_;
    bool isDrag_ = false;

    ACE_DISALLOW_COPY_AND_MOVE(SlidingPanelPattern);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_PANEL_PANEL_PATTERN_H
