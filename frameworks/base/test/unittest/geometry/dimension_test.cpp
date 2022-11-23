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

#include "gtest/gtest.h"

#include <memory>
#include "base/geometry/dimension.h"
#include "core/pipeline_ng/test/mock/mock_pipeline_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {
const double DEFAULT_DOUBLE = 1.0;
const double ZERO_DOUBLE = 0.0;

const Dimension DIMENSION_PX(DEFAULT_DOUBLE, DimensionUnit::PX);
const Dimension DIMENSION_VP(DEFAULT_DOUBLE, DimensionUnit::VP);
const Dimension DIMENSION_FP(DEFAULT_DOUBLE, DimensionUnit::FP);
const Dimension DIMENSION_LPX(DEFAULT_DOUBLE, DimensionUnit::LPX);
const Dimension DIMENSION_PCT(DEFAULT_DOUBLE, DimensionUnit::PERCENT);
const Dimension DIMENSION_AUTO(DEFAULT_DOUBLE, DimensionUnit::AUTO);
const Dimension DIMENSION_CALC(DEFAULT_DOUBLE, DimensionUnit::CALC);

const std::string DIMENSION_PX_STR = StringUtils::DoubleToString(DEFAULT_DOUBLE).append("px");
const std::string DIMENSION_VP_STR = StringUtils::DoubleToString(DEFAULT_DOUBLE).append("vp");
const std::string DIMENSION_FP_STR = StringUtils::DoubleToString(DEFAULT_DOUBLE).append("fp");
const std::string DIMENSION_LPX_STR = StringUtils::DoubleToString(DEFAULT_DOUBLE).append("lpx");
const std::string DIMENSION_PCT_STR = StringUtils::DoubleToString(DEFAULT_DOUBLE * 100).append("%");
const std::string DIMENSION_AUTO_STR = StringUtils::DoubleToString(DEFAULT_DOUBLE).append("auto");
}

class DimensionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DimensionTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "DimensionTest SetUpTestCase";
}

void DimensionTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "DimensionTest TearDownTestCase";
}

void DimensionTest::SetUp()
{
    GTEST_LOG_(INFO) << "DimensionTest SetUp";
}

void DimensionTest::TearDown()
{
    GTEST_LOG_(INFO) << "DimensionTest TearDown";
}

/**
 * @tc.name: DimensionTest001
 * @tc.desc: Test the function ConvertToVp of the class Dimension.
 * @tc.type: FUNC
 */
HWTEST_F(DimensionTest, DimensionTest001, TestSize.Level1)
{
    /**
     * @tc.steps1: initialize parameters.
     */
    MockPipelineBase::SetUp();

    /**
     * @tc.steps2: Test the function ConvertToVp of the class Dimension.
     * @tc.expected: The return values are equal to DEFAULT_DOUBLE or ZERO_DOUBLE
     */
    EXPECT_DOUBLE_EQ(DIMENSION_PX.ConvertToVp(), DEFAULT_DOUBLE);
    EXPECT_DOUBLE_EQ(DIMENSION_VP.ConvertToVp(), DEFAULT_DOUBLE);
    EXPECT_DOUBLE_EQ(DIMENSION_FP.ConvertToVp(), DEFAULT_DOUBLE);
    EXPECT_DOUBLE_EQ(DIMENSION_LPX.ConvertToVp(), DEFAULT_DOUBLE);
    EXPECT_DOUBLE_EQ(DIMENSION_PCT.ConvertToVp(), ZERO_DOUBLE);
    EXPECT_DOUBLE_EQ(DIMENSION_AUTO.ConvertToVp(), ZERO_DOUBLE);
    EXPECT_DOUBLE_EQ(DIMENSION_CALC.ConvertToVp(), ZERO_DOUBLE);
}

/**
 * @tc.name: DimensionTest002
 * @tc.desc: Test the function ConvertToPx of the class Dimension.
 * @tc.type: FUNC
 */
HWTEST_F(DimensionTest, DimensionTest002, TestSize.Level1)
{
    /**
     * @tc.steps1: initialize parameters.
     */
    MockPipelineBase::SetUp();

    /**
     * @tc.steps2: Test the function ConvertToPx of the class Dimension.
     * @tc.expected: The return values are equal to DEFAULT_DOUBLE or ZERO_DOUBLE.
     */
    EXPECT_DOUBLE_EQ(DIMENSION_PX.ConvertToPx(), DEFAULT_DOUBLE);
    EXPECT_DOUBLE_EQ(DIMENSION_VP.ConvertToPx(), DEFAULT_DOUBLE);
    EXPECT_DOUBLE_EQ(DIMENSION_FP.ConvertToPx(), DEFAULT_DOUBLE);
    EXPECT_DOUBLE_EQ(DIMENSION_LPX.ConvertToPx(), DEFAULT_DOUBLE);
    EXPECT_DOUBLE_EQ(DIMENSION_PCT.ConvertToPx(), ZERO_DOUBLE);
    EXPECT_DOUBLE_EQ(DIMENSION_AUTO.ConvertToPx(), ZERO_DOUBLE);
    EXPECT_DOUBLE_EQ(DIMENSION_CALC.ConvertToPx(), ZERO_DOUBLE);
}

/**
 * @tc.name: DimensionTest003
 * @tc.desc: Test the function ToString of the class Dimension.
 * @tc.type: FUNC
 */
HWTEST_F(DimensionTest, DimensionTest003, TestSize.Level1)
{
    /**
     * @tc.steps1: Test the function ToString of the class Dimension.
     * @tc.expected: The return values are equal to DIMENSION_STR of PX, VP, FP, LPX, PCT and AUTO.
     */
    EXPECT_EQ(DIMENSION_PX.ToString(), DIMENSION_PX_STR);
    EXPECT_EQ(DIMENSION_VP.ToString(), DIMENSION_VP_STR);
    EXPECT_EQ(DIMENSION_FP.ToString(), DIMENSION_FP_STR);
    EXPECT_EQ(DIMENSION_LPX.ToString(), DIMENSION_LPX_STR);
    EXPECT_EQ(DIMENSION_PCT.ToString(), DIMENSION_PCT_STR);
    EXPECT_EQ(DIMENSION_AUTO.ToString(), DIMENSION_AUTO_STR);
}

/**
 * @tc.name: DimensionTest004
 * @tc.desc: Test the function NormalizeToPx of the class Dimension with -DEFAULT_DOUBLE.
 * @tc.type: FUNC
 */
HWTEST_F(DimensionTest, DimensionTest004, TestSize.Level1)
{
    /**
     * @tc.steps1: Test the function NormalizeToPx of the class Dimension with DIMENSION_PX.
     * @tc.expected: The return values are equal to DEFAULT_DOUBLE.
     */
    double result = 0;
    EXPECT_TRUE(DIMENSION_PX.NormalizeToPx(
        -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, result));
    EXPECT_DOUBLE_EQ(result, DEFAULT_DOUBLE);

    /**
     * @tc.steps2: Test the function NormalizeToPx of the class Dimension with DIMENSION_VP.
     * @tc.expected: The return values are equal to ZERO_DOUBLE.
     */
    result = 0;
    EXPECT_FALSE(DIMENSION_VP.NormalizeToPx(
        -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, result));
    EXPECT_DOUBLE_EQ(result, ZERO_DOUBLE);

    /**
     * @tc.steps3: Test the function NormalizeToPx of the class Dimension with DIMENSION_FP.
     * @tc.expected: The return values are equal to ZERO_DOUBLE.
     */
    result = 0;
    EXPECT_FALSE(DIMENSION_FP.NormalizeToPx(
        -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, result));
    EXPECT_DOUBLE_EQ(result, ZERO_DOUBLE);

    /**
     * @tc.steps4: Test the function NormalizeToPx of the class Dimension with DIMENSION_LPX.
     * @tc.expected: The return values are equal to ZERO_DOUBLE.
     */
    result = 0;
    EXPECT_FALSE(DIMENSION_LPX.NormalizeToPx(
        -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, result));
    EXPECT_DOUBLE_EQ(result, ZERO_DOUBLE);

    /**
     * @tc.steps5: Test the function NormalizeToPx of the class Dimension with DIMENSION_PCT.
     * @tc.expected: The return values are equal to ZERO_DOUBLE.
     */
    result = 0;
    EXPECT_FALSE(DIMENSION_PCT.NormalizeToPx(
        -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, result));
    EXPECT_DOUBLE_EQ(result, ZERO_DOUBLE);

    /**
     * @tc.steps6: Test the function NormalizeToPx of the class Dimension with DIMENSION_AUTO.
     * @tc.expected: The return values are equal to ZERO_DOUBLE.
     */
    result = 0;
    EXPECT_FALSE(DIMENSION_AUTO.NormalizeToPx(
        -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, result));
    EXPECT_DOUBLE_EQ(result, ZERO_DOUBLE);

    /**
     * @tc.steps7: Test the function NormalizeToPx of the class Dimension with DIMENSION_CALC.
     * @tc.expected: The return values are equal to ZERO_DOUBLE.
     */
    result = 0;
    EXPECT_FALSE(DIMENSION_CALC.NormalizeToPx(
        -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, -DEFAULT_DOUBLE, result));
    EXPECT_DOUBLE_EQ(result, ZERO_DOUBLE);
}

/**
 * @tc.name: DimensionTest005
 * @tc.desc: Test the function NormalizeToPx of the class Dimension with DEFAULT_DOUBLE.
 * @tc.type: FUNC
 */
HWTEST_F(DimensionTest, DimensionTest005, TestSize.Level1)
{
    /**
     * @tc.steps1: Test the function NormalizeToPx of the class Dimension with DIMENSION_PX.
     * @tc.expected: The return values are equal to DEFAULT_DOUBLE.
     */
    double result = 0;
    EXPECT_TRUE(DIMENSION_PX.NormalizeToPx(DEFAULT_DOUBLE, DEFAULT_DOUBLE, DEFAULT_DOUBLE, DEFAULT_DOUBLE, result));
    EXPECT_DOUBLE_EQ(result, DEFAULT_DOUBLE);

    /**
     * @tc.steps2: Test the function NormalizeToPx of the class Dimension with DIMENSION_VP.
     * @tc.expected: The return values are equal to DEFAULT_DOUBLE.
     */
    result = 0;
    EXPECT_TRUE(DIMENSION_VP.NormalizeToPx(DEFAULT_DOUBLE, DEFAULT_DOUBLE, DEFAULT_DOUBLE, DEFAULT_DOUBLE, result));
    EXPECT_DOUBLE_EQ(result, DEFAULT_DOUBLE);

    /**
     * @tc.steps3: Test the function NormalizeToPx of the class Dimension with DIMENSION_FP.
     * @tc.expected: The return values are equal to DEFAULT_DOUBLE.
     */
    result = 0;
    EXPECT_TRUE(DIMENSION_FP.NormalizeToPx(DEFAULT_DOUBLE, DEFAULT_DOUBLE, DEFAULT_DOUBLE, DEFAULT_DOUBLE, result));
    EXPECT_DOUBLE_EQ(result, DEFAULT_DOUBLE);

    /**
     * @tc.steps4: Test the function NormalizeToPx of the class Dimension with DIMENSION_LPX.
     * @tc.expected: The return values are equal to DEFAULT_DOUBLE.
     */
    result = 0;
    EXPECT_TRUE(DIMENSION_LPX.NormalizeToPx(DEFAULT_DOUBLE, DEFAULT_DOUBLE, DEFAULT_DOUBLE, DEFAULT_DOUBLE, result));
    EXPECT_DOUBLE_EQ(result, DEFAULT_DOUBLE);

    /**
     * @tc.steps5: Test the function NormalizeToPx of the class Dimension with DIMENSION_PCT.
     * @tc.expected: The return values are equal to DEFAULT_DOUBLE.
     */
    result = 0;
    EXPECT_TRUE(DIMENSION_PCT.NormalizeToPx(DEFAULT_DOUBLE, DEFAULT_DOUBLE, DEFAULT_DOUBLE, DEFAULT_DOUBLE, result));
    EXPECT_DOUBLE_EQ(result, DEFAULT_DOUBLE);

    /**
     * @tc.steps6: Test the function NormalizeToPx of the class Dimension with DIMENSION_AUTO.
     * @tc.expected: The return values are equal to ZERO_DOUBLE.
     */
    result = 0;
    EXPECT_FALSE(DIMENSION_AUTO.NormalizeToPx(DEFAULT_DOUBLE, DEFAULT_DOUBLE, DEFAULT_DOUBLE, DEFAULT_DOUBLE, result));
    EXPECT_DOUBLE_EQ(result, ZERO_DOUBLE);

    /**
     * @tc.steps7: Test the function NormalizeToPx of the class Dimension with DIMENSION_CALC.
     * @tc.expected: The return values are equal to ZERO_DOUBLE.
     */
    result = 0;
    EXPECT_FALSE(DIMENSION_CALC.NormalizeToPx(DEFAULT_DOUBLE, DEFAULT_DOUBLE, DEFAULT_DOUBLE, DEFAULT_DOUBLE, result));
    EXPECT_DOUBLE_EQ(result, ZERO_DOUBLE);
}
} // namespace OHOS::Ace