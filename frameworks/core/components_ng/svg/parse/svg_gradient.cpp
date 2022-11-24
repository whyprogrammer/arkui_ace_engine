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

#include "frameworks/core/components_ng/svg/parse/svg_gradient.h"

#include "base/utils/utils.h"
#include "frameworks/core/components_ng/svg/parse/svg_stop.h"

namespace OHOS::Ace::NG {

SvgGradient::SvgGradient(GradientType gradientType) : SvgNode()
{
    gradientDeclaration_ = AceType::MakeRefPtr<SvgGradientDeclaration>();
    gradientDeclaration_->Init();
    gradientDeclaration_->InitializeStyle();
    gradientDeclaration_->SetGradientType(gradientType);
    InitNoneFlag();
}

RefPtr<SvgNode> SvgGradient::CreateLinearGradient()
{
    return AceType::MakeRefPtr<SvgGradient>(GradientType::LINEAR);
}

RefPtr<SvgNode> SvgGradient::CreateRadialGradient()
{
    return AceType::MakeRefPtr<SvgGradient>(GradientType::RADIAL);
}

void SvgGradient::SetAttr(const std::string& name, const std::string& value)
{
    gradientDeclaration_->SetSpecializedAttr(std::make_pair(name, value));
}

void SvgGradient::OnAppendChild(const RefPtr<SvgNode>& child)
{
    auto svgStop = AceType::DynamicCast<SvgStop>(child);
    CHECK_NULL_VOID(svgStop);
    gradientDeclaration_->AddGradientColor(svgStop->GetGradientColor());
}

const Gradient& SvgGradient::GetGradient() const
{
    return gradientDeclaration_->GetGradient();
}

} // namespace OHOS::Ace::NG
