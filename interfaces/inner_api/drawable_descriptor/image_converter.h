/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#ifndef FOUNDATION_ACE_INTERFACE_INNERKITS_IMAGE_CONVERTER_H
#define FOUNDATION_ACE_INTERFACE_INNERKITS_IMAGE_CONVERTER_H

#include "include/core/SkBitmap.h"
#include "include/core/SkImageInfo.h"
#ifdef NEW_SKIA
#include "include/core/SkSamplingOptions.h"
#endif

#include "pixel_map.h"
#include "image_source.h"

namespace OHOS::Ace::Napi {
class ImageConverter {
public:
    std::unique_ptr<Media::ImageSource> CreateImageSource(const char* item, uint32_t& errorCode);
    static SkColorType PixelFormatToSkColorType(Media::PixelFormat pixelFormat);
    static SkAlphaType AlphaTypeToSkAlphaType(Media::AlphaType alphaType);
    static std::shared_ptr<SkBitmap> PixelMapToBitmap(
        const std::shared_ptr<Media::PixelMap>& pixelMap);
    static std::shared_ptr<Media::PixelMap> BitmapToPixelMap(
        const std::shared_ptr<SkBitmap>& bitMap, Media::InitializationOptions& opts);
};
} // namespace OHOS::Ace::Napi
#endif // #define FOUNDATION_ACE_INTERFACE_INNERKITS_IMAGE_CONVERTER_H

