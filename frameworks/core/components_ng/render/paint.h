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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_RENDER_PAINT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_RENDER_PAINT_H

#include "base/geometry/matrix3.h"
#include "base/geometry/ng/point_t.h"
#include "base/geometry/ng/rect_t.h"
#include "base/geometry/rrect.h"
#include "base/memory/ace_type.h"
#include "core/components/common/properties/clip_path.h"
#include "core/components/common/properties/color.h"

#define PI_NUM 3.1415926f

namespace OHOS::Ace::NG {

enum class FilterQuality {
    NONE = 0,
    LOW,
    MEDIUM,
    HIGH,
};

enum class PaintStyle {
    FILL,
    STROKE,
    STROKE_AND_FILL,
};

// Paint is interface for drawing styles.
class Paint : public virtual AceType {
    DECLARE_ACE_TYPE(NG::Paint, AceType)

public:
    static RefPtr<Paint> Create();

    virtual void Reset() = 0;

    virtual void EnableAntiAlias() = 0;
    virtual void SetFilterQuality(FilterQuality quality) = 0;

    virtual void SetStyle(PaintStyle style) = 0;
    virtual void SetColor(const Color& color) = 0;
    virtual void SetStrokeWidth(float width) = 0;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_RENDER_PAINT_H
