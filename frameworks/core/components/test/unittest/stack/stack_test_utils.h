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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_STACK_STACK_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_STACK_STACK_TEST_UTILS_H

#include "core/components/common/layout/constants.h"
#include "core/components/positioned/positioned_component.h"
#include "core/components/positioned/render_positioned.h"
#include "core/components/stack/render_stack.h"
#include "core/components/stack/stack_component.h"

namespace OHOS::Ace {

class MockRenderStack : public RenderStack {
    DECLARE_ACE_TYPE(MockRenderStack, RenderStack);

public:
    MockRenderStack() = default;
    ~MockRenderStack() override = default;

    Offset GetNonPositionedChildOffsetTest(const Size& childSize)
    {
        return RenderStack::GetNonPositionedChildOffset(childSize);
    }
};

class StackTestUtils {
public:
    static RefPtr<RenderStack> CreateRenderStack(const Alignment& alignment, Overflow overflow,
        StackFit stackFit, HitTestMode hitTestMode = HitTestMode::HTMDEFAULT);
    static RefPtr<RenderPositioned> CreateRenderPositioned(RefPtr<PositionedComponent> positioned);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_STACK_STACK_TEST_UTILS_H
