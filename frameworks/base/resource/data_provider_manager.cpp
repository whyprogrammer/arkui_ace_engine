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

#include "base/resource/data_provider_manager.h"

#include <shared_mutex>
#include <sys/stat.h>
#include <unistd.h>

#include "base/log/log.h"
#include "base/utils/utils.h"

namespace OHOS::Ace {

std::unique_ptr<DataProviderRes> DataProviderManager::GetDataProviderResFromUri(const std::string& uriStr)
{
    LOGD("DataProviderManager::GetDataProviderResFromUri start uri: %{private}s", uriStr.c_str());
    if (platformImpl_) {
        return platformImpl_(uriStr);
    }
    return nullptr;
}

void* DataProviderManagerStandard::GetDataProviderThumbnailResFromUri(const std::string& uriStr)
{
    InitHelper();
    std::shared_lock lock(helperMutex_);
    if (!helper_) {
        LOGE("data ability helper is null when try query thumbnail resource, uri: %{private}s", uriStr.c_str());
        return nullptr;
    }
    return helper_->QueryThumbnailResFromDataAbility(uriStr);
}

std::unique_ptr<DataProviderRes> DataProviderManagerStandard::GetDataProviderResFromUri(const std::string& uriStr)
{
    LOGD("DataProviderManagerStandard::GetDataProviderResFromUri start uri: %{private}s", uriStr.c_str());
    InitHelper();
    std::shared_lock lock(helperMutex_);
    if (!helper_) {
        LOGE("data ability helper is null");
        return nullptr;
    }
    auto fd = helper_->OpenFile(uriStr, "r");
    if (fd == -1) {
        LOGE("file descriptor is not valid");
        return nullptr;
    }

    // get size of file.
    struct stat statBuf;
    auto statRes = fstat(fd, &statBuf);
    if (statRes != 0) {
        LOGE("get stat fail");
        close(fd);
        return nullptr;
    }
    auto size = statBuf.st_size;

    // read file content.
    std::vector<uint8_t> buffer(size);
    auto readRes = read(fd, buffer.data(), size);
    close(fd);
    if (readRes == -1) {
        LOGE("read file fail");
        return nullptr;
    }

    auto result = std::make_unique<DataProviderRes>(std::move(buffer));
    return result;
}

void DataProviderManagerStandard::InitHelper()
{
    {
        std::shared_lock lock(helperMutex_);
        if (helper_) {
            return;
        }
    }
    // creating helper_, need exclusive access
    std::unique_lock lock(helperMutex_);
    if (!helper_ && dataAbilityHelperImpl_) {
        helper_ = dataAbilityHelperImpl_();
    }
}

int32_t DataProviderManagerStandard::GetDataProviderFile(const std::string& uriStr, const std::string& mode)
{
    InitHelper();
    std::shared_lock lock(helperMutex_);
    CHECK_NULL_RETURN(helper_, -1);
    return helper_->OpenFile(uriStr, mode);
}

} // namespace OHOS::Ace
