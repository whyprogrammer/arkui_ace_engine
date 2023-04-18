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

#ifndef NEW_SKIA
#include "flutter_render_water_flow.h"
#endif
#include "rosen_render_water_flow.h"

namespace OHOS::Ace::V2 {
RefPtr<RenderNode> RenderWaterFlow::Create()
{
    if (SystemProperties::GetRosenBackendEnabled()) {
#ifdef ENABLE_ROSEN_BACKEND
        return AceType::MakeRefPtr<RosenRenderWaterFlow>();
#else
        return nullptr;
#endif
    } else {
#ifndef NEW_SKIA
        return AceType::MakeRefPtr<FlutterRenderWaterFlow>();
#else
        return nullptr;
#endif
    }
}
} // namespace OHOS::Ace::V2
