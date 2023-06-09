/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_DRAW_DELEGATE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_DRAW_DELEGATE_H

#include "base/geometry/rect.h"
#include "core/pipeline/layers/layer.h"

namespace OHOS::Rosen {
class RSNode;
}

namespace OHOS::Ace {

namespace NG {
class RenderContext;
}

class DrawDelegate {
public:
    using DoDrawFrame = std::function<void(RefPtr<Flutter::Layer>&, const Rect&)>;
    using DoDrawRSFrame = std::function<void(std::shared_ptr<Rosen::RSNode>&, const Rect&)>;
    using DoDrawRSFrameByRenderContext = std::function<void(RefPtr<NG::RenderContext>&)>;
    using DoDrawLastFrame = std::function<void(const Rect&)>;

    DrawDelegate() = default;
    ~DrawDelegate() = default;

    void DrawFrame(RefPtr<Flutter::Layer>& rootLayer, const Rect& dirty)
    {
        if (doDrawFrameCallback_) {
            doDrawFrameCallback_(rootLayer, dirty);
        }
    }

    void DrawRSFrame(std::shared_ptr<Rosen::RSNode>& node, const Rect& dirty)
    {
        if (doDrawRSFrameCallback_) {
            doDrawRSFrameCallback_(node, dirty);
        }
    }

    void DrawRSFrame(RefPtr<NG::RenderContext>& renderContext)
    {
        if (doDrawRSFrameByRenderContextCallback_) {
            doDrawRSFrameByRenderContextCallback_(renderContext);
        }
    }

    void DrawLastFrame(const Rect& dirty)
    {
        if (doDrawLastFrameCallback_) {
            doDrawLastFrameCallback_(dirty);
        }
    }

    void SetDrawFrameCallback(DoDrawFrame&& doFrameCallback)
    {
        doDrawFrameCallback_ = doFrameCallback;
    }

    void SetDrawRSFrameCallback(DoDrawRSFrame&& doRSFrameCallback)
    {
        doDrawRSFrameCallback_ = doRSFrameCallback;
    }

    void SetDrawRSFrameByRenderContextCallback(DoDrawRSFrameByRenderContext&& doRSFrameByRenderContextCallback)
    {
        doDrawRSFrameByRenderContextCallback_ = doRSFrameByRenderContextCallback;
    }

    void SetDrawFrameRepeatCallback(DoDrawLastFrame&& doFrameCallback)
    {
        doDrawLastFrameCallback_ = doFrameCallback;
    }

private:
    DoDrawFrame doDrawFrameCallback_;
    DoDrawRSFrame doDrawRSFrameCallback_;
    DoDrawRSFrameByRenderContext doDrawRSFrameByRenderContextCallback_;
    DoDrawLastFrame doDrawLastFrameCallback_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_DRAW_DELEGATE_H
