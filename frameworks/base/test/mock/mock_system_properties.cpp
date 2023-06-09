/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "base/log/ace_performance_check.h"
#include "base/utils/system_properties.h"
#include "bridge/common/utils/engine_helper.h"

namespace OHOS::Ace {
namespace {
constexpr int32_t PAGE_NODES = 1000;
constexpr int32_t PAGE_DEPTH = 30;
constexpr int32_t NODE_CHILDREN = 100;
constexpr int32_t FUNCTION_TIMEOUT = 15;
constexpr int32_t VSYNC_TIMEOUT = 500;
constexpr int32_t NODE_TIMEOUT = 15;
constexpr int32_t FOREACH_ITEMS = 50;
constexpr int32_t FLEX_LAYOUTS = 8;
} // namespace

DeviceType SystemProperties::deviceType_ = DeviceType::UNKNOWN;
DeviceOrientation SystemProperties::orientation_ { DeviceOrientation::PORTRAIT };
bool SystemProperties::isHookModeEnabled_ = false;
bool SystemProperties::rosenBackendEnabled_ = true;
bool SystemProperties::windowAnimationEnabled_ = true;
double SystemProperties::resolution_ = 0.0;
constexpr float defaultAnimationScale = 1.0f;
bool SystemProperties::extSurfaceEnabled_ = false;
uint32_t SystemProperties::dumpFrameCount_ = 0;
PerformancePtr SystemProperties::performanceProps_ = nullptr;

// =================================================================================
// resolve compile error temporarily and wait
// for unittest cases to be integrated and modified
ScopedDelegate::ScopedDelegate(Framework::FrontendDelegate* delegate, int32_t id)
    : delegate_(delegate), scope_(new ContainerScope(id))
{}

ScopedDelegate::~ScopedDelegate()
{
    delete scope_;
    scope_ = nullptr;
}

std::pair<int32_t, int32_t> EngineHelper::GetPositionOnJsCode()
{
    return { 0, 0 };
}

ScopedDelegate EngineHelper::GetCurrentDelegate()
{
    return { nullptr, 0 };
}

void AceScopedPerformanceCheck::RecordFlexLayoutsCount(const CheckNodeMap& itemMap) {}

AceScopedPerformanceCheck::AceScopedPerformanceCheck(const std::string& /* name */) {}

AceScopedPerformanceCheck::~AceScopedPerformanceCheck() {}

void AceScopedPerformanceCheck::InsertNodeTimeout(int64_t sec, int32_t row, int32_t col, const std::string& tag) {}

void AceScopedPerformanceCheck::RecordPageNodeCountAndDepth(
    int32_t pageNodeCount, int32_t pageDepth, const CheckNodeMap& nodeMap)
{}

void AceScopedPerformanceCheck::RecordForEachItemsCount(const CheckNodeMap& itemMap) {}

// =================================================================================

float SystemProperties::GetFontWeightScale()
{
    // Default value of font weight scale is 1.0.
    return 1.0f;
}

DeviceType SystemProperties::GetDeviceType()
{
    return DeviceType::PHONE;
}

bool SystemProperties::GetDebugEnabled()
{
    return false;
}

float SystemProperties::GetAnimationScale()
{
    return defaultAnimationScale;
}

bool SystemProperties::GetIsUseMemoryMonitor()
{
    return false;
}

void SystemProperties::InitPerformanceParameters() {}

bool SystemProperties::IsPerformanceCheckEnabled()
{
    return SystemProperties::performanceProps_ != nullptr;
}

int32_t SystemProperties::GetPerformanceParameterWithType(PerformanceParameterType type)
{
    int32_t result = 0;
    switch (type) {
        case PerformanceParameterType::PAGE_NODES:
            result = PAGE_NODES;
            break;
        case PerformanceParameterType::PAGE_DEPTH:
            result = PAGE_DEPTH;
            break;
        case PerformanceParameterType::NODE_CHILDREN:
            result = NODE_CHILDREN;
            break;
        case PerformanceParameterType::FUNCTION_TIMEOUT:
            result = FUNCTION_TIMEOUT;
            break;
        case PerformanceParameterType::VSYNC_TIMEOUT:
            result = VSYNC_TIMEOUT;
            break;
        case PerformanceParameterType::NODE_TIMEOUT:
            result = NODE_TIMEOUT;
            break;
        case PerformanceParameterType::FOREACH_ITEMS:
            result = FOREACH_ITEMS;
            break;
        case PerformanceParameterType::FLEX_LAYOUTS:
            result = FLEX_LAYOUTS;
            break;
        default:
            result = -1;
    }
    return result;
}
} // namespace OHOS::Ace
