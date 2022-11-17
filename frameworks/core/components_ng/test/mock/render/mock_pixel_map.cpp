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
#include "core/components_ng/test/mock/render/mock_pixel_map.h"

#include "base/memory/ace_type.h"

namespace OHOS::Ace {
RefPtr<PixelMap> PixelMap::CreatePixelMap(void* rawPtr)
{
    return AceType::MakeRefPtr<MockPixelMap>();
}

int32_t MockPixelMap::GetWidth() const
{
    return 1;
}

int32_t MockPixelMap::GetHeight() const
{
    return 1;
}
} // namespace OHOS::Ace
