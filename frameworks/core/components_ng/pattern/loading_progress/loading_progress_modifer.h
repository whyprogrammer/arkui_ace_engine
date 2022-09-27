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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LOADING_PROGRESS_LOADING_PROGRESS_MODIFIER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LOADING_PROGRESS_LOADING_PROGRESS_MODIFIER_H

#include "core/common/container.h"
#include "core/components/common/properties/color.h"
#include "core/components/progress/progress_theme.h"
#include "core/components_ng/base/modifier.h"
#include "core/components_ng/property/property.h"
#include "core/components_ng/render/drawing.h"

namespace OHOS::Ace::NG {

const Dimension RING_RADIUS = 10.5_vp;
const Dimension ORBIT_RADIUS = 17.0_vp;
const Dimension RING_WIDTH = 2.8_vp;
const Dimension COMET_WIDTH = 6.0_vp;
const float RING_MOVEMENT = 0.06f;
const float FULL_COUNT = 100.0f;
const float COUNT = 50.0f;
const float HALF = 0.5f;

class LoadingProgressModifier : public ContentModifierFloat {
    DECLARE_ACE_TYPE(LoadingProgressModifier, ContentModifierFloat);

public:
    LoadingProgressModifier();
    ~LoadingProgressModifier() override = default;
    void onDraw(DrawingContext& context, const float& date) override
    {
        auto container = Container::Current();
        CHECK_NULL_VOID(container);
        auto pipeline = container->GetPipelineContext();
        CHECK_NULL_VOID(pipeline);
        auto themeManager = pipeline->GetThemeManager();
        CHECK_NULL_VOID(themeManager);
        auto parogressTheme = themeManager->GetTheme<ProgressTheme>();
        float scale_ = 1.0;
        scale_ = std::min((context.width / (ORBIT_RADIUS.ConvertToPx() + COMET_WIDTH.ConvertToPx())),
                     (context.height /
                        (RING_RADIUS.ConvertToPx() * (1 + RING_MOVEMENT) + RING_WIDTH.ConvertToPx() * HALF))) *
                 HALF;
        if (date > COUNT) {
            DrawRing(context, date, scale_);
            DrawOrbit(context, date, scale_);
        } else {
            DrawOrbit(context, date, scale_);
            DrawRing(context, date, scale_);
        }
    }

    void DrawRing(DrawingContext& canvas, float date, float scale_) const;
    void DrawOrbit(DrawingContext& canvas, float date, float scale_) const;
    void SetColor(Color color)
    {
        color_ = color;
    };

private:
    Color color_ = Color::BLUE;

    ACE_DISALLOW_COPY_AND_MOVE(LoadingProgressModifier);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_LOADING_PROGRESS_LOADING_PROGRESS_MODIFIER_H
