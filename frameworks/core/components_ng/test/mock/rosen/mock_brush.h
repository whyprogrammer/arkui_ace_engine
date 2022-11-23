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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_TEST_MOCK_BRUSH_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_TEST_MOCK_BRUSH_H

#include "gmock/gmock.h"
#include "testing_brush.h"
#include "testing_color.h"

namespace OHOS::Ace::Testing {
class MockBrush : public TestingBrush {
public:
    MockBrush() = default;
    ~MockBrush() = default;

    MOCK_METHOD1(SetAntiAlias, void(bool aa));
    MOCK_METHOD1(SetWidth, void(scalar width));
    MOCK_METHOD1(SetColor, void(const TestingColor& c));
    MOCK_METHOD1(SetColor, void(int c));
    MOCK_METHOD1(SetShaderEffect, void(std::shared_ptr<TestingShaderEffect> e));
    MOCK_METHOD1(SetFilter, void(const TestingFilter filter));
};
} // namespace OHOS::Ace::Testing
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_TEST_MOCK_BRUSH_H
