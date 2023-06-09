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

#include "core/components_ng/pattern/window_scene/scene/host/host_window_node.h"

#include "base/utils/utils.h"
#include "core/components_ng/pattern/window_scene/scene/host/host_window_pattern.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "pointer_event.h"

namespace OHOS::Ace::NG {
namespace {
const std::unordered_map<SourceType, int32_t> SOURCE_TYPE_MAP = {
    { SourceType::TOUCH, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN },
    { SourceType::TOUCH_PAD, MMI::PointerEvent::SOURCE_TYPE_TOUCHPAD },
    { SourceType::MOUSE, MMI::PointerEvent::SOURCE_TYPE_MOUSE },
};

const std::unordered_map<TouchType, int32_t> TOUCH_TYPE_MAP = {
    { TouchType::CANCEL, MMI::PointerEvent::POINTER_ACTION_CANCEL },
    { TouchType::DOWN, MMI::PointerEvent::POINTER_ACTION_DOWN },
    { TouchType::MOVE, MMI::PointerEvent::POINTER_ACTION_MOVE },
    { TouchType::UP, MMI::PointerEvent::POINTER_ACTION_UP },
};

std::shared_ptr<MMI::PointerEvent> ConvertPointerEvent(const OffsetF offsetF, const TouchEvent& point)
{
    std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();

    OHOS::MMI::PointerEvent::PointerItem item;
    item.SetWindowX(static_cast<int32_t>(point.x - offsetF.GetX()));
    item.SetWindowY(static_cast<int32_t>(point.y - offsetF.GetY()));
    item.SetDisplayX(static_cast<int32_t>(point.screenX));
    item.SetDisplayY(static_cast<int32_t>(point.screenY));
    item.SetPointerId(point.id);
    pointerEvent->AddPointerItem(item);

    int32_t sourceType = MMI::PointerEvent::SOURCE_TYPE_UNKNOWN;
    auto sourceTypeIter = SOURCE_TYPE_MAP.find(point.sourceType);
    if (sourceTypeIter != SOURCE_TYPE_MAP.end()) {
        sourceType = sourceTypeIter->second;
    }
    pointerEvent->SetSourceType(sourceType);

    int32_t pointerAction = OHOS::MMI::PointerEvent::POINTER_ACTION_UNKNOWN;
    auto pointerActionIter = TOUCH_TYPE_MAP.find(point.type);
    if (pointerActionIter != TOUCH_TYPE_MAP.end()) {
        pointerAction = pointerActionIter->second;
    }
    pointerEvent->SetPointerAction(pointerAction);
    pointerEvent->SetPointerId(point.id);
    return pointerEvent;
}
}

HitTestResult HostWindowNode::TouchTest(const PointF& globalPoint, const PointF& parentLocalPoint,
    const TouchRestrict& touchRestrict, TouchTestResult& result, int32_t touchId)
{
    const auto& rect = GetGeometryNode()->GetFrameRect();
    if (!rect.IsInRegion(parentLocalPoint)) {
        return HitTestResult::OUT_OF_REGION;
    }

    auto context = GetContext();
    CHECK_NULL_RETURN(context, HitTestResult::BUBBLING);

    DispatchPointerEvent(touchRestrict.touchEvent);
    auto callback = [weak = WeakClaim(this)] (const TouchEvent& point) {
        auto hostWindowNode = weak.Upgrade();
        CHECK_NULL_VOID(hostWindowNode);
        hostWindowNode->DispatchPointerEvent(point);
    };
    context->AddUIExtensionCallback(callback);
    return HitTestResult::BUBBLING;
}

void HostWindowNode::DispatchPointerEvent(const TouchEvent& point) const
{
    auto pattern = GetPattern<HostWindowPattern>();
    CHECK_NULL_VOID(pattern);
    auto selfGlobalOffset = GetTransformRelativeOffset();
    auto pointerEvent = ConvertPointerEvent(selfGlobalOffset, point);
    pattern->DispatchPointerEvent(pointerEvent);
}

RefPtr<HostWindowNode> HostWindowNode::GetOrCreateHostWindowNode(
    const std::string& tag, int32_t nodeId, const std::function<RefPtr<Pattern>(void)>& patternCreator)
{
    auto hostWindowNode = ElementRegister::GetInstance()->GetSpecificItemById<HostWindowNode>(nodeId);
    if (hostWindowNode) {
        if (hostWindowNode->GetTag() == tag) {
            return hostWindowNode;
        }
        ElementRegister::GetInstance()->RemoveItemSilently(nodeId);
        auto parent = hostWindowNode->GetParent();
        if (parent) {
            parent->RemoveChild(hostWindowNode);
        }
    }

    auto pattern = patternCreator ? patternCreator() : AceType::MakeRefPtr<Pattern>();
    hostWindowNode = AceType::MakeRefPtr<HostWindowNode>(tag, nodeId, pattern, false);
    hostWindowNode->InitializePatternAndContext();
    ElementRegister::GetInstance()->AddUINode(hostWindowNode);
    return hostWindowNode;
}
} // namespace OHOS::Ace::NG