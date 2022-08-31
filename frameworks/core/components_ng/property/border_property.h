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

#ifndef FOUNDATION_ACE_FRAMEWORKS_COMPONENTS_NG_PROPERTIES_BORDER_PROPERTY_H
#define FOUNDATION_ACE_FRAMEWORKS_COMPONENTS_NG_PROPERTIES_BORDER_PROPERTY_H

#include <optional>

#include "base/geometry/dimension.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"

namespace OHOS::Ace::NG {

template<typename T>
struct BorderRadiusPropertyT {
    std::optional<T> radiusTopLeft;
    std::optional<T> radiusTopRight;
    std::optional<T> radiusBottomLeft;
    std::optional<T> radiusBottomRight;

    void SetRadius(const T& borderRadius)
    {
        radiusTopLeft = borderRadius;
        radiusTopRight = borderRadius;
        radiusBottomLeft = borderRadius;
        radiusBottomRight = borderRadius;
    }

    bool operator==(const BorderRadiusPropertyT& value) const
    {
        return (radiusTopLeft == value.radiusTopLeft) && (radiusTopRight == value.radiusTopRight) &&
               (radiusBottomLeft == value.radiusBottomLeft) && (radiusBottomRight == value.radiusBottomRight);
    }

    bool UpdateWithCheck(const BorderRadiusPropertyT& value)
    {
        bool isModified = false;
        if (value.radiusTopLeft.has_value() && (radiusTopLeft != value.radiusTopLeft)) {
            radiusTopLeft = value.radiusTopLeft;
            isModified = true;
        }
        if (value.radiusTopRight.has_value() && (radiusTopRight != value.radiusTopRight)) {
            radiusTopRight = value.radiusTopRight;
            isModified = true;
        }
        if (value.radiusBottomLeft.has_value() && (radiusBottomLeft != value.radiusBottomLeft)) {
            radiusBottomLeft = value.radiusBottomLeft;
            isModified = true;
        }
        if (value.radiusBottomRight.has_value() && (radiusBottomRight != value.radiusBottomRight)) {
            radiusBottomRight = value.radiusBottomRight;
            isModified = true;
        }
        return isModified;
    }

    std::string ToString() const
    {
        std::string str;
        str.append("radiusTopLeft: [").append(radiusTopLeft.has_value() ? radiusTopLeft->ToString() : "NA").append("]");
        str.append("radiusTopRight: [")
            .append(radiusTopRight.has_value() ? radiusTopRight->ToString() : "NA")
            .append("]");
        str.append("radiusBottomLeft: [")
            .append(radiusBottomLeft.has_value() ? radiusBottomLeft->ToString() : "NA")
            .append("]");
        str.append("radiusBottomRight: [")
            .append(radiusBottomRight.has_value() ? radiusBottomRight->ToString() : "NA")
            .append("]");
        return str;
    }
};

template<>
struct BorderRadiusPropertyT<float> {
    std::optional<float> radiusTopLeft;
    std::optional<float> radiusTopRight;
    std::optional<float> radiusBottomLeft;
    std::optional<float> radiusBottomRight;

    bool operator==(const BorderRadiusPropertyT<float>& value) const
    {
        if (radiusTopLeft.has_value() ^ value.radiusTopLeft.has_value()) {
            return false;
        }
        if (!NearEqual(radiusTopLeft.value_or(0), value.radiusTopLeft.value_or(0))) {
            return false;
        }
        if (radiusTopRight.has_value() ^ value.radiusTopRight.has_value()) {
            return false;
        }
        if (!NearEqual(radiusTopRight.value_or(0), value.radiusTopRight.value_or(0))) {
            return false;
        }
        if (radiusBottomLeft.has_value() ^ value.radiusBottomLeft.has_value()) {
            return false;
        }
        if (!NearEqual(radiusBottomLeft.value_or(0), value.radiusBottomLeft.value_or(0))) {
            return false;
        }
        if (radiusBottomRight.has_value() ^ value.radiusBottomRight.has_value()) {
            return false;
        }
        if (!NearEqual(radiusBottomRight.value_or(0), value.radiusBottomRight.value_or(0))) {
            return false;
        }
        return true;
    }

    std::string ToString() const
    {
        std::string str;
        str.append("radiusTopLeft: [")
            .append(radiusTopLeft.has_value() ? std::to_string(radiusTopLeft.value()) : "NA")
            .append("]");
        str.append("radiusTopRight: [")
            .append(radiusTopRight.has_value() ? std::to_string(radiusTopRight.value()) : "NA")
            .append("]");
        str.append("radiusBottomLeft: [")
            .append(radiusBottomLeft.has_value() ? std::to_string(radiusBottomLeft.value()) : "NA")
            .append("]");
        str.append("radiusBottomRight: [")
            .append(radiusBottomRight.has_value() ? std::to_string(radiusBottomRight.value()) : "NA")
            .append("]");
        return str;
    }
};

template<typename T>
struct BorderColorPropertyT {
    std::optional<T> leftColor;
    std::optional<T> rightColor;
    std::optional<T> topColor;
    std::optional<T> bottomColor;

    void SetColor(const T& borderColor)
    {
        leftColor = borderColor;
        rightColor = borderColor;
        topColor = borderColor;
        bottomColor = borderColor;
    }

    bool operator==(const BorderColorPropertyT& value) const
    {
        return (leftColor == value.leftColor) && (rightColor == value.rightColor) && (topColor == value.topColor) &&
               (bottomColor == value.bottomColor);
    }

    std::string ToString() const
    {
        std::string str;
        str.append("leftColor: [").append(leftColor.has_value() ? leftColor->ToString() : "NA").append("]");
        str.append("rightColor: [").append(rightColor.has_value() ? rightColor->ToString() : "NA").append("]");
        str.append("topColor: [").append(topColor.has_value() ? topColor->ToString() : "NA").append("]");
        str.append("bottomColor: [").append(bottomColor.has_value() ? bottomColor->ToString() : "NA").append("]");
        return str;
    }
};

template<typename T>
struct BorderWidthPropertyT {
    std::optional<T> leftDimen;
    std::optional<T> rightDimen;
    std::optional<T> topDimen;
    std::optional<T> bottomDimen;

    void SetBorderWidth(const T& borderWidth)
    {
        leftDimen = borderWidth;
        rightDimen = borderWidth;
        topDimen = borderWidth;
        bottomDimen = borderWidth;
    }

    bool operator==(const BorderWidthPropertyT& value) const
    {
        return (leftDimen == value.leftDimen) && (rightDimen == value.rightDimen) && (topDimen == value.topDimen) &&
               (bottomDimen == value.bottomDimen);
    }

    bool UpdateWithCheck(const BorderWidthPropertyT& value)
    {
        bool isModified = false;
        if (value.leftDimen.has_value() && (leftDimen != value.leftDimen)) {
            leftDimen = value.leftDimen;
            isModified = true;
        }
        if (value.rightDimen.has_value() && (rightDimen != value.rightDimen)) {
            rightDimen = value.rightDimen;
            isModified = true;
        }
        if (value.topDimen.has_value() && (topDimen != value.topDimen)) {
            topDimen = value.topDimen;
            isModified = true;
        }
        if (value.bottomDimen.has_value() && (bottomDimen != value.bottomDimen)) {
            bottomDimen = value.bottomDimen;
            isModified = true;
        }
        return isModified;
    }

    std::string ToString() const
    {
        std::string str;
        str.append("leftDimen: [").append(leftDimen.has_value() ? leftDimen->ToString() : "NA").append("]");
        str.append("rightDimen: [").append(rightDimen.has_value() ? rightDimen->ToString() : "NA").append("]");
        str.append("topDimen: [").append(topDimen.has_value() ? topDimen->ToString() : "NA").append("]");
        str.append("bottomDimen: [").append(bottomDimen.has_value() ? bottomDimen->ToString() : "NA").append("]");
        return str;
    }
};

template<>
struct BorderWidthPropertyT<float> {
    std::optional<float> leftDimen;
    std::optional<float> rightDimen;
    std::optional<float> topDimen;
    std::optional<float> bottomDimen;

    bool operator==(const BorderWidthPropertyT<float>& value) const
    {
        if (leftDimen.has_value() ^ value.leftDimen.has_value()) {
            return false;
        }
        if (!NearEqual(leftDimen.value_or(0), value.leftDimen.value_or(0))) {
            return false;
        }
        if (rightDimen.has_value() ^ value.rightDimen.has_value()) {
            return false;
        }
        if (!NearEqual(rightDimen.value_or(0), value.rightDimen.value_or(0))) {
            return false;
        }
        if (topDimen.has_value() ^ value.topDimen.has_value()) {
            return false;
        }
        if (!NearEqual(topDimen.value_or(0), value.topDimen.value_or(0))) {
            return false;
        }
        if (bottomDimen.has_value() ^ value.bottomDimen.has_value()) {
            return false;
        }
        if (!NearEqual(bottomDimen.value_or(0), value.bottomDimen.value_or(0))) {
            return false;
        }
        return true;
    }

    std::string ToString() const
    {
        std::string str;
        str.append("leftDimen: [").append(leftDimen.has_value() ? std::to_string(leftDimen.value()) : "NA").append("]");
        str.append("rightDimen: [")
            .append(rightDimen.has_value() ? std::to_string(rightDimen.value()) : "NA")
            .append("]");
        str.append("topDimen: [").append(topDimen.has_value() ? std::to_string(topDimen.value()) : "NA").append("]");
        str.append("bottomDimen: [")
            .append(bottomDimen.has_value() ? std::to_string(bottomDimen.value()) : "NA")
            .append("]");
        return str;
    }
};

template<typename T>
struct BorderStylePropertyT {
    std::optional<T> styleLeft;
    std::optional<T> styleRight;
    std::optional<T> styleTop;
    std::optional<T> styleBottom;

    void SetBorderStyle(const T& borderStyle)
    {
        styleLeft = borderStyle;
        styleRight = borderStyle;
        styleTop = borderStyle;
        styleBottom = borderStyle;
    }

    bool operator==(const BorderStylePropertyT& value) const
    {
        return (styleLeft == value.styleLeft) && (styleRight == value.styleRight) && (styleTop == value.styleTop) &&
               (styleBottom == value.styleBottom);
    }

    std::string ToString() const
    {
        std::string str;
        str.append("styleLeft: [").append(styleLeft.has_value() ? styleLeft->ToString() : "NA").append("]");
        str.append("styleRight: [").append(styleRight.has_value() ? styleRight->ToString() : "NA").append("]");
        str.append("styleTop: [").append(styleTop.has_value() ? styleTop->ToString() : "NA").append("]");
        str.append("styleBottom: [").append(styleBottom.has_value() ? styleBottom->ToString() : "NA").append("]");
        return str;
    }
};

using BorderRadiusPropertyF = BorderRadiusPropertyT<float>;
using BorderRadiusProperty = BorderRadiusPropertyT<Dimension>;
using BorderColorProperty = BorderColorPropertyT<Color>;
using BorderWidthPropertyF = BorderWidthPropertyT<float>;
using BorderWidthProperty = BorderWidthPropertyT<Dimension>;
using BorderStyleProperty = BorderStylePropertyT<BorderStyle>;

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_COMPONENTS_NG_PROPERTIES_BORDER_PROPERTY_H