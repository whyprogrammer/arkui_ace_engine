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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_TEST_MOCK_CLIPBOARD_MOCK_CLIPBOARD_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_TEST_MOCK_CLIPBOARD_MOCK_CLIPBOARD_H

#include "gmock/gmock-function-mocker.h"
#include "gmock/gmock.h"

#include "core/common/clipboard/clipboard.h"

namespace OHOS::Ace {
class MockClipBoard : public Clipboard {
    DECLARE_ACE_TYPE(MockClipBoard, Clipboard);

public:
    explicit MockClipBoard(const RefPtr<TaskExecutor>& taskExecutor) : Clipboard(taskExecutor) {}
    ~MockClipBoard() override = default;

    void HasData(const std::function<void(bool hasData)>& callback) override;
    void SetData(const std::string& data, CopyOptions copyOption, bool isDragData) override;
    void GetData(const std::function<void(const std::string&)>& callback, bool syncMode) override;

    MOCK_METHOD2(SetPixelMapData, void(const RefPtr<PixelMap>& pixmap, CopyOptions copyOption));
    MOCK_METHOD2(GetPixelMapData, void(const std::function<void(const RefPtr<PixelMap>&)>& callback, bool syncMode));
    MOCK_METHOD0(Clear, void());
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_TEST_MOCK_CLIPBOARD_MOCK_CLIPBOARD_H
