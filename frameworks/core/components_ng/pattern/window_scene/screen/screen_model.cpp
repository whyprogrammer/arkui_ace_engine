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

#include "core/components_ng/pattern/window_scene/screen/screen_model.h"

#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_v2/inspector/inspector_constants.h"

namespace OHOS::Ace::NG {

void ScreenModel::Create(const sptr<Rosen::ScreenSession>& screenSession)
{
    auto stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto frameNode = FrameNode::GetOrCreateFrameNode(V2::SCREEN_ETS_TAG, nodeId,
        [&screenSession]() { return AceType::MakeRefPtr<ScreenPattern>(screenSession); });
    stack->Push(frameNode);
}

void ScreenModel::SetBrightness(float brightness)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto screenPattern = frameNode->GetPattern<ScreenPattern>();
    CHECK_NULL_VOID(screenPattern);
    screenPattern->SetBrightness(brightness);
}

void ScreenModel::SetRotation(float degree)
{
    // ACE_UPDATE_LAYOUT_PROPERTY(ScreenLayoutProperty, Rotation, degree);

    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto screenPattern = frameNode->GetPattern<ScreenPattern>();
    CHECK_NULL_VOID(screenPattern);
    screenPattern->SetRotation(degree);
}

} // namespace OHOS::Ace::NG
