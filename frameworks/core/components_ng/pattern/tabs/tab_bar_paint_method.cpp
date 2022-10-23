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

#include "core/components_ng/pattern/tabs/tab_bar_paint_method.h"

#include "core/components_ng/pattern/tabs/tab_bar_paint_property.h"

#include "base/geometry/dimension_rect.h"
#include "core/components_ng/render/drawing_prop_convertor.h"

namespace OHOS::Ace::NG {
namespace {

void PaintIndicator(RSCanvas& canvas, PaintWrapper* paintWrapper)
{
    auto paintProperty = AceType::DynamicCast<TabBarPaintProperty>(paintWrapper->GetPaintProperty());
    if (!paintProperty->GetIndicator()) {
        return;
    }

    RectF indicator = paintProperty->GetIndicatorValue();
    indicator.SetHeight(10);

    // draw line
    RSBrush brush;
    brush.SetColor(ToRSColor(Color(0xFF254ff7)));
    brush.SetBlendMode(RSBlendMode::SRC_OVER);
    canvas.AttachBrush(brush);
    canvas.DrawRect(ToRSRect(indicator));
}

} // namespace

CanvasDrawFunction TabBarPaintMethod::GetForegroundDrawFunction(PaintWrapper* paintWrapper)
{
    auto paintFunc = [paintWrapper](RSCanvas& canvas) { PaintIndicator(canvas, paintWrapper); };

    return paintFunc;
}

} // namespace OHOS::Ace::NG
