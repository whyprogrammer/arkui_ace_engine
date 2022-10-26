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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_RATING_RATING_PROPERTY_GROUP_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_RATING_RATING_PROPERTY_GROUP_H

#include "base/geometry/ng/size_t.h"
#include "base/resource/internal_resource.h"
#include "core/components_ng/property/property.h"
#include "core/image/image_source_info.h"

namespace OHOS::Ace::NG {
struct RatingPropertyGroup {
    ACE_DEFINE_PROPERTY_GROUP_ITEM(Indicator, bool);
    ACE_DEFINE_PROPERTY_GROUP_ITEM(Stars, int32_t);

    ACE_DEFINE_PROPERTY_GROUP_ITEM(ForegroundImageSourceInfo, ImageSourceInfo);
    ACE_DEFINE_PROPERTY_GROUP_ITEM(SecondaryImageSourceInfo, ImageSourceInfo);
    ACE_DEFINE_PROPERTY_GROUP_ITEM(BackgroundImageSourceInfo, ImageSourceInfo);
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_RATING_RATING_PROPERTY_GROUP_H
