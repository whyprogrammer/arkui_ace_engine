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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_BASE_VIEW_STACK_PROCESSOR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_BASE_VIEW_STACK_PROCESSOR_H

#include <memory>
#include <stack>
#include <unordered_map>
#include <vector>

#include "base/memory/referenced.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/layout/layout_property.h"
#include "core/components_ng/pattern/custom/custom_node.h"
#include "core/components_ng/pattern/tabs/tab_bar_pattern.h"
#include "core/gestures/gesture_processor.h"
#include "core/pipeline/base/render_context.h"

#define ACE_UPDATE_LAYOUT_PROPERTY(target, name, value)                         \
    do {                                                                        \
        auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode(); \
        CHECK_NULL_VOID(frameNode);                                             \
        auto cast##target = frameNode->GetLayoutProperty<target>();             \
        if (cast##target) {                                                     \
            cast##target->Update##name(value);                                  \
        }                                                                       \
    } while (false)

#define ACE_UPDATE_PAINT_PROPERTY(target, name, value)                          \
    do {                                                                        \
        auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode(); \
        CHECK_NULL_VOID(frameNode);                                             \
        auto cast##target = frameNode->GetPaintProperty<target>();              \
        if (cast##target) {                                                     \
            cast##target->Update##name(value);                                  \
        }                                                                       \
    } while (false)

#define ACE_UPDATE_RENDER_CONTEXT(name, value)                                  \
    do {                                                                        \
        auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode(); \
        CHECK_NULL_VOID(frameNode);                                             \
        auto target = frameNode->GetRenderContext();                            \
        if (target) {                                                           \
            target->Update##name(value);                                        \
        }                                                                       \
    } while (false)

namespace OHOS::Ace::NG {
class ACE_EXPORT ViewStackProcessor final {
public:
    friend class ScopedViewStackProcessor;

    static ViewStackProcessor* GetInstance();
    ~ViewStackProcessor() = default;

    template<typename Pattern>
    RefPtr<Pattern> GetMainFrameNodePattern() const
    {
        auto frameNode = GetMainFrameNode();
        if (!frameNode) {
            return nullptr;
        }
        return AceType::DynamicCast<Pattern>(frameNode->GetPattern());
    }

    template<typename EventHubType>
    RefPtr<EventHubType> GetMainFrameNodeEventHub() const
    {
        auto frameNode = GetMainFrameNode();
        if (!frameNode) {
            return nullptr;
        }
        return frameNode->GetEventHub<EventHubType>();
    }

    RefPtr<GestureEventHub> GetMainFrameNodeGestureEventHub() const
    {
        auto frameNode = GetMainFrameNode();
        if (!frameNode) {
            return nullptr;
        }
        return frameNode->GetOrCreateGestureEventHub();
    }

    RefPtr<InputEventHub> GetMainFrameNodeInputEventHub() const
    {
        auto frameNode = GetMainFrameNode();
        if (!frameNode) {
            return nullptr;
        }
        return frameNode->GetOrCreateInputEventHub();
    }

    RefPtr<FocusHub> GetMainFrameNodeFocusHub() const
    {
        auto frameNode = GetMainFrameNode();
        if (!frameNode) {
            return nullptr;
        }
        return frameNode->GetOrCreateFocusHub();
    }

    RefPtr<FrameNode> GetMainFrameNode() const;

    // Get main component include composed component created by js view.
    RefPtr<UINode> GetMainElementNode() const;

    // create wrappingComponentsMap and the component to map and then Push
    // the map to the render component stack.
    void Push(const RefPtr<UINode>& element, bool isCustomView = false);

    void PushTabBar(const TabBarParam& tabBarParam);
    const TabBarParam& PopTabBar() const;

    // Wrap the components map for the stack top and then pop the stack.
    // Add the wrapped component has child of the new stack top's main component.
    void Pop();

    // pop the last container
    void PopContainer();

    // End of Render function, create component tree and flush modify task.
    RefPtr<UINode> Finish();

    // Set key to be used for next node on the stack
    void PushKey(const std::string& key);

    // Returns a key for the node if it has been pushed to the stack. Default is ""
    std::string GetKey();

    // Takes care of the viewId wrt to foreach
    std::string ProcessViewId(const std::string& viewId);

    // Clear the key pushed to the stack
    void PopKey();

    void SetVisualState(VisualState state)
    {
        visualState_ = state;
    }

    std::optional<VisualState> GetVisualState() const
    {
        return visualState_;
    }

    bool IsVisualStateSet()
    {
        return visualState_.has_value();
    }

    void ClearVisualState()
    {
        visualState_.reset();
    }

    void ClearStack()
    {
        auto emptyStack = std::stack<RefPtr<UINode>>();
        elementsStack_.swap(emptyStack);
    }

    RefPtr<GestureProcessor> GetOrCreateGestureProcessor()
    {
        if (!gestureStack_) {
            gestureStack_ = AceType::MakeRefPtr<GestureProcessor>();
        }
        return gestureStack_;
    }

    void ResetGestureProcessor()
    {
        return gestureStack_.Reset();
    }

    /**
     * when nesting observeComponentCreation functions, such as in the case of
     * If, and the if branch creates a Text etc that requires an implicit pop
     * this function is needed after executing the inner observeComponentCreation
     * and before read ViewStackProcessor.GetTopMostElementId(); on the outer one
     */
    void ImplicitPopBeforeContinue();

    // End of Rerender function, flush modifier task.
    void FlushRerenderTask();

    /**
     * start 'get' access recording
     * account all get access to given node id
     * next node creation will claim the given node id
     * see ClaimNodeId()
     */
    void StartGetAccessRecordingFor(int32_t elmtId)
    {
        accountGetAccessToNodeId_ = elmtId;
        reservedNodeId_ = elmtId;
    }

    int32_t ClaimNodeId()
    {
        const auto result = reservedNodeId_;
        reservedNodeId_ = ElementRegister::UndefinedElementId;
        return result;
    }

    /**
     * get the elmtId to which all get access should be accounted
     * ElementRegister::UndefinedElementId; means no get access recording enabled
     */
    ElementIdType GetNodeIdToAccountFor() const
    {
        return accountGetAccessToNodeId_;
    }
    void SetNodeIdToAccountFor(ElementIdType elmtId)
    {
        accountGetAccessToNodeId_ = elmtId;
    }

    /**
     * inverse of StartGetAccessRecordingFor
     */
    void StopGetAccessRecording()
    {
        accountGetAccessToNodeId_ = ElementRegister::UndefinedElementId;
        reservedNodeId_ = ElementRegister::UndefinedElementId;
    }

private:
    ViewStackProcessor();

    bool ShouldPopImmediately();

    // Singleton instance
    static thread_local std::unique_ptr<ViewStackProcessor> instance;

    // render component stack
    std::stack<RefPtr<UINode>> elementsStack_;

    RefPtr<GestureProcessor> gestureStack_;

    std::string viewKey_;
    std::stack<size_t> keyStack_;

    std::stack<int32_t> parentIdStack_;

    std::optional<VisualState> visualState_ = std::nullopt;

    // elmtId reserved for next component creation
    ElementIdType reservedNodeId_ = ElementRegister::UndefinedElementId;

    // elmtId to account get access to
    ElementIdType accountGetAccessToNodeId_ = ElementRegister::UndefinedElementId;

    TabBarParam tabBarParam_;

    ACE_DISALLOW_COPY_AND_MOVE(ViewStackProcessor);
};

class ACE_EXPORT ScopedViewStackProcessor final {
public:
    ScopedViewStackProcessor();
    ~ScopedViewStackProcessor();

private:
    std::unique_ptr<ViewStackProcessor> instance_;

    ACE_DISALLOW_COPY_AND_MOVE(ScopedViewStackProcessor);
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_BASE_VIEW_STACK_PROCESSOR_H
