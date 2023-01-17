/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "core/components_ng/image_provider/svg_image_object.h"

#include "core/components_ng/image_provider/adapter/skia_image_data.h"
#include "core/components_ng/image_provider/image_loading_context.h"
#include "core/components_ng/render/adapter/svg_canvas_image.h"

namespace OHOS::Ace::NG {

RefPtr<SvgImageObject> SvgImageObject::Create(const ImageSourceInfo& src, const RefPtr<ImageData>& data)
{
    auto obj = AceType::MakeRefPtr<SvgImageObject>(src, SizeF());
    if (!obj->MakeSvgDom(data, src.GetFillColor())) {
        return nullptr;
    };
    return obj;
}

const RefPtr<SvgDomBase>& SvgImageObject::GetSVGDom() const
{
    return svgDomBase_;
}

void SvgImageObject::MakeCanvasImage(
    const RefPtr<ImageLoadingContext>& ctx, const SizeF& /*resizeTarget*/, bool /*forceResize*/, bool /*syncLoad*/)
{
    CHECK_NULL_VOID(GetSVGDom());
    // just set svgDom to canvasImage
    auto canvasImage = MakeRefPtr<SvgCanvasImage>(GetSVGDom());
    ctx->SuccessCallback(canvasImage);
}

bool SvgImageObject::MakeSvgDom(const RefPtr<ImageData>& data, const std::optional<Color>& svgFillColor)
{
    auto skiaImageData = DynamicCast<SkiaImageData>(data);
    CHECK_NULL_RETURN(skiaImageData, false);
    // update SVGSkiaDom
    svgDomBase_ = skiaImageData->MakeSvgDom(svgFillColor);
    CHECK_NULL_RETURN(svgDomBase_, false);
    // get ImageSize
    SetImageSize(svgDomBase_->GetContainerSize());
    return true;
}

} // namespace OHOS::Ace::NG
