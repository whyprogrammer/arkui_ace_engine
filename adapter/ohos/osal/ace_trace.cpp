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

#include "base/log/ace_trace.h"

#include "hitrace_meter.h"

#include "base/log/log.h"
#include "base/utils/macros.h"
#include "base/utils/system_properties.h"

namespace OHOS::Ace {

bool AceTraceEnabled()
{
    return true;
}

void AceTraceBegin(const char* name)
{
    if (name == nullptr) {
        return;
    }
    std::string nameStr(name);
    StartTrace(HITRACE_TAG_ACE, nameStr);
}

void AceTraceEnd()
{
    FinishTrace(HITRACE_TAG_ACE);
}

} // namespace OHOS::Ace
