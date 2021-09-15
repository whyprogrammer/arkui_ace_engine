/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <unordered_map>

#include "core/components_v2/inspector/blank_composed_element.h"

#include "base/log/dump_log.h"

namespace OHOS::Ace::V2 {

namespace {

const std::unordered_map<std::string, std::function<std::string(const BlankComposedElement&)>> CREATE_JSON_MAP {
    { "min", [](const BlankComposedElement& inspector) { return inspector.GetMin(); } }
};

}

void BlankComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("blank_composed_element"));
    DumpLog::GetInstance().AddDesc(
        std::string("min: ").append(GetMin()));
}

std::unique_ptr<JsonValue> BlankComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string BlankComposedElement::GetMin() const
{
    auto renderFlexItem = GetRenderFlexItem();
    auto min = renderFlexItem ? renderFlexItem->GetFlexBasis() : 0.0;
    return std::to_string(min);
}

RefPtr<RenderFlexItem> BlankComposedElement::GetRenderFlexItem() const
{
    auto node = GetInspectorNode(FlexItemElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderFlexItem>(node);
    }
    return nullptr;
}

} // namespace OHOS::Ace::V2