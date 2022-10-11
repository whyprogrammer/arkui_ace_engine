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

#include "core/components_v2/grid_layout/grid_container_utils.h"

#include "core/components/common/layout/grid_system_manager.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::V2 {
namespace {

RefPtr<GridSizeInfo> ParseBreakpoints(const RefPtr<BreakPoints>& breakpoints)
{
    auto sizeInfo = AceType::MakeRefPtr<GridSizeInfo>();
    if (!breakpoints) {
        LOGI("user hasnt set breakpoint, use WindowSize and xs: 320vp, sm: 520vp, md: 840vp");
    } else {
        sizeInfo->Reset();
        for (const auto& breakpoint : breakpoints->breakpoints) {
            sizeInfo->sizeInfo.push_back(Framework::StringToDimension(breakpoint));
        }
    }
    return sizeInfo;
}

} // namespace

GridSizeType GridContainerUtils::ProcessGridSizeType(const RefPtr<V2::BreakPoints>& breakpoints, const Size& size)
{
    auto threshold = ParseBreakpoints(breakpoints);
    double windowWidth = 0.0;
    if (breakpoints->reference == BreakPointsReference::WindowSize) {
        windowWidth = GridSystemManager::GetInstance().GetScreenWidth();
    } else {
        windowWidth = size.Width();
    }
    auto context = NG::PipelineContext::GetCurrentContext();
    int index = 0;
    for (const auto& cur : threshold->sizeInfo) {
        if (context->NormalizeToPx(cur) > windowWidth) {
            break;
        }
        index++;
    }
    return static_cast<GridSizeType>(index);
}

GridSizeType GridContainerUtils::ProcessGridSizeType(
    const RefPtr<BreakPoints>& breakpoints, const Size& size, const RefPtr<PipelineContext>& pipeline)
{
    auto threshold = ParseBreakpoints(breakpoints);
    double windowWidth = 0.0;
    if (breakpoints->reference == BreakPointsReference::WindowSize) {
        windowWidth = GridSystemManager::GetInstance().GetScreenWidth();
    } else {
        windowWidth = size.Width();
    }
    int index = 0;
    for (const auto& cur : threshold->sizeInfo) {
        if (pipeline->NormalizeToPx(cur) > windowWidth) {
            break;
        }
        index++;
    }
    return static_cast<GridSizeType>(index);
}

std::pair<Dimension, Dimension> GridContainerUtils::ProcessGutter(GridSizeType sizeType, const Gutter& gutter)
{
    switch (sizeType) {
        case GridSizeType::XS:
            return std::pair<Dimension, Dimension>(gutter.xXs, gutter.yXs);
        case GridSizeType::SM:
            return std::pair<Dimension, Dimension>(gutter.xSm, gutter.ySm);
        case GridSizeType::MD:
            return std::pair<Dimension, Dimension>(gutter.xMd, gutter.yMd);
        case GridSizeType::LG:
            return std::pair<Dimension, Dimension>(gutter.xLg, gutter.yLg);
        case GridSizeType::XL:
            return std::pair<Dimension, Dimension>(gutter.xXl, gutter.yXl);
        case GridSizeType::XXL:
            return std::pair<Dimension, Dimension>(gutter.xXXl, gutter.yXXl);
        default:
            return std::pair<Dimension, Dimension>(gutter.xXs, gutter.yXs);
    }
}

std::pair<Dimension, Dimension> GridContainerUtils::ProcessGutter(GridSizeType sizeType, const RefPtr<Gutter>& gutter)
{
    return ProcessGutter(sizeType, *gutter);
}

int32_t GridContainerUtils::ProcessColumn(GridSizeType sizeType, const RefPtr<GridContainerSize>& columnNum)
{
    switch (sizeType) {
        case GridSizeType::XS:
            return columnNum->xs;
        case GridSizeType::SM:
            return columnNum->sm;
        case GridSizeType::MD:
            return columnNum->md;
        case GridSizeType::LG:
            return columnNum->lg;
        case GridSizeType::XL:
            return columnNum->xl;
        case GridSizeType::XXL:
            return columnNum->xxl;
        default:
            return columnNum->xs;
    }
}

double GridContainerUtils::ProcessColumnWidth(const std::pair<double, double>& gutter, int32_t columnNum, double width)
{
    auto xGutter = gutter.first;
    if (columnNum != 0) {
        return (width - (columnNum - 1) * xGutter) / columnNum;
    }
    return 0.0;
}

} // namespace OHOS::Ace::V2
