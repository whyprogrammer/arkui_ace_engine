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

#ifndef FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_MODELS_SCROLL_MODEL_IMPL_H
#define FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_MODELS_SCROLL_MODEL_IMPL_H

#include "core/components_ng/pattern/scroll/scroll_model.h"

namespace OHOS::Ace::Framework {

class ScrollModelImpl : public ScrollModel {
public:
    ScrollModelImpl() = default;
    ~ScrollModelImpl() override = default;

    void Create() override;
    RefPtr<ScrollControllerBase> GetOrCreateController() override;
    void SetAxis(Axis axis) override;
    void SetOnScrollBegin(NG::ScrollBeginEvent&& event) override;
    void SetOnScroll(NG::OnScrollEvent&& event) override;
    void SetOnScrollEdge(NG::ScrollEdgeEvent&& event) override;
    void SetOnScrollEnd(NG::ScrollEndEvent&& event) override;
    void SetScrollBarProxy(const RefPtr<ScrollBarProxy>& proxy) override;
    void InitScrollBar(const RefPtr<ScrollBarTheme>& theme, const std::pair<bool, Color>& color,
        const std::pair<bool, Dimension>& width, EdgeEffect effect) override;
    void SetDisplayMode(DisplayMode displayMode) override;
    void SetScrollBarWidth(const Dimension& dimension) override;
    void SetScrollBarColor(const Color& color) override;
    void SetEdgeEffect(EdgeEffect edgeEffect) override;
};

} // namespace OHOS::Ace::Framework
#endif // FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_JS_VIEW_MODELS_SCROLL_MODEL_IMPL_H
