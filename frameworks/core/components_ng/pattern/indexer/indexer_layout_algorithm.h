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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_INDEXER_INDEXER_LAYOUT_ALGORITHM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_INDEXER_INDEXER_LAYOUT_ALGORITHM_H

#include "base/memory/referenced.h"
#include "core/components_ng/layout/layout_algorithm.h"
#include "core/components_ng/layout/layout_wrapper.h"
#include "core/components_ng/pattern/indexer/indexer_layout_property.h"

namespace OHOS::Ace::NG {
class PipelineContext;

class ACE_EXPORT IndexerLayoutAlgorithm : public LayoutAlgorithm {
    DECLARE_ACE_TYPE(IndexerLayoutAlgorithm, LayoutAlgorithm);

public:
    IndexerLayoutAlgorithm(int32_t itemCount_) : itemCount_(itemCount_) {}

    float GetItemSizeRender() const
    {
        return itemSizeRender_;
    }

    void Measure(LayoutWrapper* layoutWrapper) override;

    void Layout(LayoutWrapper* layoutWrapper) override;

private:
    void InitializeIndexer(LayoutWrapper* layoutWrapper);

    std::vector<std::string> arrayValue_;
    uint32_t popupSize_ = 0;
    int32_t itemCount_ = 0;
    float itemSize_ = 0.0f;
    float itemWidth_ = 0.0f;
    float itemSizeRender_ = 0.0f;
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_INDEXER_INDEXER_LAYOUT_ALGORITHM_H
