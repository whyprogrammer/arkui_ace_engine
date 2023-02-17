/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_HYPERLINK_HYPERLINK_MODEL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_HYPERLINK_HYPERLINK_MODEL_H

#include "base/utils/macros.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"

namespace OHOS::Ace {

class ACE_EXPORT HyperlinkModel {
public:
    static HyperlinkModel* GetInstance();
    virtual ~HyperlinkModel() = default;

    virtual void Create(const std::string& address, const std::string& summary) = 0;
    virtual void Pop() = 0;
    virtual void SetColor(const Color& value) = 0;

private:
    static std::unique_ptr<HyperlinkModel> instance_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_HYPERLINK_HYPERLINK_MODEL_H