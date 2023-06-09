/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_ANIMATION_GEOMETRY_TRANSITION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_ANIMATION_GEOMETRY_TRANSITION_H

#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/geometry/ng/rect_t.h"

namespace OHOS::Ace::NG {
class FrameNode;
class LayoutWrapper;

class GeometryTransition : public AceType {
    DECLARE_ACE_TYPE(GeometryTransition, AceType);

public:
    GeometryTransition(const WeakPtr<FrameNode>& frameNode);
    ~GeometryTransition() override = default;

    bool IsNodeInAndActive(const WeakPtr<FrameNode>& frameNode) const;
    bool IsRunning() const;
    bool IsInAndOutEmpty() const;
    std::string ToString() const;
    void Build(const WeakPtr<FrameNode>& frameNode, bool isNodeIn);
    bool Update(const WeakPtr<FrameNode>& which, const WeakPtr<FrameNode>& value);
    void WillLayout(const RefPtr<LayoutWrapper>& layoutWrapper);
    void DidLayout(const WeakPtr<FrameNode>& frameNode);

    static void OnLayout(bool layoutStarted)
    {
        layoutStarted_ = layoutStarted;
    }

private:
    enum class State {
        IDLE,
        ACTIVE,
        IDENTITY,
    };

    bool IsInAndOutValid() const;
    bool IsNodeInAndIdentity(const WeakPtr<FrameNode>& frameNode) const;
    bool IsNodeOutAndActive(const WeakPtr<FrameNode>& frameNode) const;
    void SwapInAndOut(bool condition);
    std::pair<RefPtr<FrameNode>, RefPtr<FrameNode>> GetMatchedPair(bool isNodeIn) const;
    void ModifyLayoutConstraint(const RefPtr<LayoutWrapper>& layoutWrapper, bool isNodeIn);
    void SyncGeometry(bool isNodeIn);

    WeakPtr<FrameNode> inNode_;
    WeakPtr<FrameNode> outNode_;
    State state_ = State::IDLE;
    bool hasInAnim_ = false;
    bool hasOutAnim_ = false;
    bool buildDuringLayout_ = false;
    bool isInNodeLayoutModified_ = false;
    SizeF size_;
    OffsetF outNodePos_;
    OffsetF outNodeParentPos_;
    static bool layoutStarted_;

    ACE_DISALLOW_COPY_AND_MOVE(GeometryTransition);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_ANIMATION_GEOMETRY_TRANSITION_H