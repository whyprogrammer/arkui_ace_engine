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

#include "base/i18n/localization.h"

namespace OHOS::Ace {
struct LocaleProxy final {};
Localization::~Localization() = default;

std::string Localization::GetFontLocale()
{
    return fontLocale_;
}

std::shared_ptr<Localization> Localization::GetInstance()
{
    static auto instance = std::make_shared<Localization>();
    return instance;
}

std::string Localization::FormatDuration(uint32_t duration, const std::string& format)
{
    return "08:00:00";
}

const std::string Localization::FormatDateTime(DateTime dateTime, const std::string& format)
{
    return "08:00:00";
}

std::string Localization::GetEntryLetters(const std::string& lettersIndex)
{
    return "";
}
} // namespace OHOS::Ace
