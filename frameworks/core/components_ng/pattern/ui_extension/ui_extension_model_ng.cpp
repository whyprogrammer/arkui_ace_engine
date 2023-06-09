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

#include "core/components_ng/pattern/ui_extension/ui_extension_model_ng.h"

#include "core/components/common/layout/constants.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_node.h"
#include "core/components_ng/pattern/ui_extension/ui_extension_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline/pipeline_base.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
void UIExtensionModelNG::Create(const std::string& bundleName, const std::string& abilityName)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto frameNode = UIExtensionNode::GetOrCreateUIExtensionNode(V2::UI_EXTENSION_COMPONENT_ETS_TAG, nodeId,
        [bundleName, abilityName]() { return AceType::MakeRefPtr<UIExtensionPattern>(bundleName, abilityName); });
    stack->Push(frameNode);
    auto pipeline = AceType::DynamicCast<PipelineContext>(PipelineBase::GetCurrentContext());
    CHECK_NULL_VOID(pipeline);
    pipeline->AddWindowStateChangedCallback(nodeId);
}

void UIExtensionModelNG::SetWant(const std::string& want)
{
    ACE_UPDATE_PAINT_PROPERTY(UIExtensionPaintProperty, Want, want);
}

void UIExtensionModelNG::SetOnConnect(std::function<void()>&& onConnect) {}

void UIExtensionModelNG::SetOnDisconnect(std::function<void()>&& onDisconnect) {}

void UIExtensionModelNG::SetOnError(std::function<void()>&& onError) {}

void UIExtensionModelNG::SetOnCall(std::function<void()>&& onCall) {}

void UIExtensionModelNG::SetOnResult(std::function<void()>&& onResult) {}
} // namespace OHOS::Ace::NG
