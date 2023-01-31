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

#include "core/components_ng/pattern/window_scene/scene/host/host_window_extension_model.h"

#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_v2/inspector/inspector_constants.h"

namespace OHOS::Ace::NG {

void HostWindowExtensionModel::Create(const sptr<Rosen::Session>& session)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::HOST_WINDOW_EXTENSION_ETS_TAG, nodeId,
        [&session]() { return AceType::MakeRefPtr<HostWindowExtension>(session); });
    stack->Push(frameNode);
    frameNode->GetPattern<HostWindowExtension>()->InitContent();
}

} // namespace OHOS::Ace::NG
