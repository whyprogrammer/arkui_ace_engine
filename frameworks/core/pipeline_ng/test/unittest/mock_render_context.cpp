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

#include "base/memory/referenced.h"
#include "core/components_ng/render/render_context.h"
#include "core/components_ng/pattern/container_modal/container_modal_view.h"
#include "core/components_ng/manager/full_screen/full_screen_manager.h"

namespace OHOS::Ace::NG {
class MockRenderContext : public RenderContext {
public:
    RefPtr<Canvas> GetCanvas() override
    {
        return nullptr;
    }
    void Restore() {}
};

RefPtr<RenderContext> RenderContext::Create()
{
    return AceType::MakeRefPtr<MockRenderContext>();
}
} // namespace OHOS::Ace::NG
