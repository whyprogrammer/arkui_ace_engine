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

#ifndef FOUNDATION_ACE_INTERFACE_INNERKITS_JS_LAYERED_ICON_H
#define FOUNDATION_ACE_INTERFACE_INNERKITS_JS_LAYERED_ICON_H

#define ACE_FORCE_EXPORT __attribute__((visibility("default")))

#ifndef ACE_EXPORT
#ifndef HIDDEN_SYMBOL
#define ACE_EXPORT ACE_FORCE_EXPORT
#else
#define ACE_EXPORT
#endif
#endif

#include <optional>
#include <string>

#include "foundation/arkui/ace_engine/interfaces/inner_api/form_render/include/form_renderer_hilog.h"
#include "image_converter.h"
#include "resource_manager.h"

#include "base/memory/ace_type.h"

namespace OHOS::Ace::Napi {

class ACE_EXPORT DrawableDescriptor {
public:
    enum class DrawableType {
        BASE,
        LAYERED,
    };
    DrawableDescriptor() = default;
    explicit DrawableDescriptor(std::shared_ptr<Media::PixelMap> pixelMap) : pixelMap_(std::move(pixelMap)) {};
    DrawableDescriptor(std::unique_ptr<uint8_t[]> mediaData, size_t len)
        : mediaData_(std::move(mediaData)), len_(len) {};
    virtual ~DrawableDescriptor() = default;
    virtual std::shared_ptr<Media::PixelMap> GetPixelMap();

private:
    bool GetPixelMapFromBuffer();

    std::unique_ptr<uint8_t[]> mediaData_;
    size_t len_ = 0;
    std::optional<std::shared_ptr<Media::PixelMap>> pixelMap_;
};

class ACE_EXPORT LayeredDrawableDescriptor : public DrawableDescriptor {
public:
    LayeredDrawableDescriptor(
        std::unique_ptr<uint8_t[]> jsonBuf, size_t len, std::shared_ptr<Global::Resource::ResourceManager>& resourceMgr)
        : jsonBuf_(std::move(jsonBuf)), len_(len), resourceMgr_(std::move(resourceMgr)) {};
    ~LayeredDrawableDescriptor() override = default;
    std::unique_ptr<DrawableDescriptor> GetForeground();
    std::unique_ptr<DrawableDescriptor> GetBackground();
    std::unique_ptr<DrawableDescriptor> GetMask();
    std::shared_ptr<Media::PixelMap> GetPixelMap() override;

private:
    friend class ImageConverter;
    std::unique_ptr<Media::ImageSource> CreateImageSource(const char* item, uint32_t& errorCode);
    bool GetPixelMapFromJsonBuf(bool isBackground);
    bool GetDefaultMask();
    bool GetMaskByName(const std::string& name);
    bool CreatePixelMap();

    std::unique_ptr<uint8_t[]> jsonBuf_;
    size_t len_ = 0;
    std::shared_ptr<Global::Resource::ResourceManager> resourceMgr_;
    std::optional<std::shared_ptr<Media::PixelMap>> foreground_;
    std::optional<std::shared_ptr<Media::PixelMap>> background_;
    std::optional<std::shared_ptr<Media::PixelMap>> mask_;
    std::optional<std::shared_ptr<Media::PixelMap>> layeredPixelMap_;
};

class DrawableDescriptorFactory {
public:
    static std::unique_ptr<DrawableDescriptor> Create(int32_t id,
        std::shared_ptr<Global::Resource::ResourceManager>& resourceMgr, Global::Resource::RState& state,
        DrawableDescriptor::DrawableType& drawableType, uint32_t density)
    {
        std::string type;
        size_t len;
        std::unique_ptr<uint8_t[]> jsonBuf;
        state = resourceMgr->GetDrawableInfoById(id, type, len, jsonBuf, density);
        if (state != Global::Resource::SUCCESS) {
            HILOG_ERROR("Failed to get drawable info from resmgr");
            return nullptr;
        }
        if (type == "json") {
            HILOG_DEBUG("Create LayeredDrawableDescriptor object");
            drawableType = DrawableDescriptor::DrawableType::LAYERED;
            return std::make_unique<LayeredDrawableDescriptor>(std::move(jsonBuf), len, resourceMgr);
        }
        if (type == "png" || type == "jpg" || type == "bmp" || type == "svg" || type == "gif") {
            HILOG_DEBUG("Create DrawableDescriptor object");
            drawableType = DrawableDescriptor::DrawableType::BASE;
            return std::make_unique<DrawableDescriptor>(std::move(jsonBuf), len);
        }
        HILOG_ERROR("unknow resource type: %{public}s", type.c_str());
        return nullptr;
    }

    static std::unique_ptr<DrawableDescriptor> Create(const char* name,
        std::shared_ptr<Global::Resource::ResourceManager>& resourceMgr, Global::Resource::RState& state,
        DrawableDescriptor::DrawableType& drawableType, uint32_t density)
    {
        std::string type;
        size_t len;
        std::unique_ptr<uint8_t[]> jsonBuf;
        state = resourceMgr->GetDrawableInfoByName(name, type, len, jsonBuf, density);
        if (state != Global::Resource::SUCCESS) {
            HILOG_ERROR("Failed to get drawable info from resmgr");
            return nullptr;
        }
        if (type == "json") {
            HILOG_DEBUG("Create LayeredDrawableDescriptor object");
            drawableType = DrawableDescriptor::DrawableType::LAYERED;
            return std::make_unique<LayeredDrawableDescriptor>(std::move(jsonBuf), len, resourceMgr);
        }
        if (type == "png" || type == "jpg" || type == "bmp" || type == "svg" || type == "gif") {
            HILOG_DEBUG("Create DrawableDescriptor object");
            drawableType = DrawableDescriptor::DrawableType::BASE;
            return std::make_unique<DrawableDescriptor>(std::move(jsonBuf), len);
        }
        HILOG_ERROR("unknow resource type: %{public}s", type.c_str());
        return nullptr;
    }
};
} // namespace OHOS::Ace::Napi
#endif // #define FOUNDATION_ACE_INTERFACE_INNERKITS_JS_LAYERED_ICON_H
