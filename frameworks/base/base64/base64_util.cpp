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

#include "base/base64/base64_util.h"

#include "include/core/SkData.h"
#include "include/utils/SkBase64.h"

namespace OHOS::Ace {

bool Base64Util::Decode(const std::string& src, std::string& dst)
{
#if defined(NEW_SKIA) || defined(FLUTTER_2_5)
    size_t outputLen = 0;
    SkBase64::Error error = SkBase64::Decode(src.data(), src.size(), nullptr, &outputLen);
    if (error != SkBase64::Error::kNoError) {
        return false;
    }

    sk_sp<SkData> resData = SkData::MakeUninitialized(outputLen);
    void* output = resData->writable_data();
    error = SkBase64::Decode(src.data(), src.size(), output, &outputLen);
    if (error != SkBase64::Error::kNoError) {
        return false;
    }
    dst.assign(static_cast<const char*>(resData->data()), resData->size());
    return true;
#else
    SkBase64 base64Decoder;
    SkBase64::Error error = base64Decoder.decode(src.data(), src.size());
    if (error != SkBase64::kNoError) {
        return false;
    }
    auto base64Data = base64Decoder.getData();
    if (base64Data == nullptr) {
        return false;
    }
    dst.assign(base64Data, base64Decoder.getDataSize());
    // in SkBase64, the fData is not deleted after decoded.
    delete[] base64Data;
    base64Data = nullptr;
    return true;
#endif
}

} // namespace OHOS::Ace
