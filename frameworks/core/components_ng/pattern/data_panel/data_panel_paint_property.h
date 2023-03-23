/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_DATA_PANEL_DATA_PANEL_PAINT_PROPERTY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_DATA_PANEL_DATA_PANEL_PAINT_PROPERTY_H

#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components/data_panel/data_panel_theme.h"
#include "core/components_ng/render/paint_property.h"
#include "core/components_ng/property/gradient_property.h"
#include "core/pipeline/pipeline_base.h"

namespace OHOS::Ace::NG {
struct DataPanelShadow {
    double radius = 5.0;
    double offsetX = 5.0;
    double offsetY = 5.0;
    std::vector<Gradient> colors;
    bool operator==(const DataPanelShadow& rhs) const
    {
        return radius == rhs.radius && offsetX == rhs.offsetX && offsetY == rhs.offsetY && colors == rhs.colors;
    }
};
class DataPanelPaintProperty : public PaintProperty {
    DECLARE_ACE_TYPE(DataPanelPaintProperty, PaintProperty)

public:
    DataPanelPaintProperty() = default;
    ~DataPanelPaintProperty() override = default;

    RefPtr<PaintProperty> Clone() const override
    {
        auto paintProperty = MakeRefPtr<DataPanelPaintProperty>();
        paintProperty->UpdatePaintProperty(this);
        paintProperty->propValues_ = CloneValues();
        paintProperty->propMax_ = CloneMax();
        paintProperty->propDataPanelType_ = CloneDataPanelType();
        paintProperty->propEffect_ = CloneEffect();

        paintProperty->propValueColors_ = CloneValueColors();
        paintProperty->propTrackBackground_ = CloneTrackBackground();
        paintProperty->propStrokeWidth_ = CloneStrokeWidth();
        paintProperty->propShadowOption_ = CloneShadowOption();
        return paintProperty;
    }

    void Reset() override
    {
        PaintProperty::Reset();
        ResetValues();
        ResetMax();
        ResetDataPanelType();
        ResetEffect();
        ResetValueColors();
        ResetTrackBackground();
        ResetStrokeWidth();
        ResetShadowOption();
    }

    void ToJsonValue(std::unique_ptr<JsonValue>& json) const override
    {
        PaintProperty::ToJsonValue(json);
        auto jsonDashArray = JsonUtil::CreateArray(true);
        for (size_t i = 0; i < propValues_.value().size(); ++i) {
            auto index = std::to_string(i);
            double value = propValues_.value()[i];
            jsonDashArray->Put(index.c_str(), value);
        }
        bool closeEffect = false;
        if (propEffect_.has_value()) {
            closeEffect = !propEffect_.value();
        }
        auto pipelineContext = PipelineBase::GetCurrentContext();
        CHECK_NULL_VOID(pipelineContext);
        auto theme = pipelineContext->GetTheme<DataPanelTheme>();
        json->Put("max", std::to_string(propMax_.value_or(100)).c_str());
        json->Put("closeEffect", closeEffect ? "true" : "false");
        json->Put("type", propDataPanelType_ == 1 ? "DataPanelType.Line" : "DataPanelType.Circle");
        json->Put("values", jsonDashArray);
        json->Put("trackBackgroundColor",
            GetTrackBackground().value_or(theme->GetBackgroundColor()).ColorToString().c_str());
        json->Put("strokeWidth", GetStrokeWidth().value_or(theme->GetThickness()).ToString().c_str());

        ToJsonValueColors(json);
        ToJsonTrackShadow(json);
    }

    void ToJsonValueColors(std::unique_ptr<JsonValue>& json) const
    {
        std::vector<Gradient> valueColors;
        if (propValueColors_.has_value()) {
            valueColors = propValueColors_.value();
        } else {
            auto pipelineContext = PipelineBase::GetCurrentContext();
            CHECK_NULL_VOID(pipelineContext);
            auto theme = pipelineContext->GetTheme<DataPanelTheme>();
            auto colors = theme->GetColorsArray();
            for (const auto& item : colors) {
                Gradient gradient;
                CreateGradient(item, gradient);
                valueColors.emplace_back(gradient);
            }
        }

        auto valueColorsJosnArray = JsonUtil::CreateArray(true);
        for (size_t i = 0; i < valueColors.size(); ++i) {
            Gradient gradientItem = valueColors[i];
            auto gradientItemJsonArray = JsonUtil::CreateArray(true);
            for (size_t index = 0; index < gradientItem.GetColors().size(); ++index) {
                auto gradientColor = gradientItem.GetColors()[index];
                auto gradientColorJosn = JsonUtil::Create(true);
                gradientColorJosn->Put("color", gradientColor.GetColor().ColorToString().c_str());
                gradientColorJosn->Put("offset", std::to_string(gradientColor.GetDimension().Value()).c_str());
                gradientItemJsonArray->Put(std::to_string(index).c_str(), gradientColorJosn);
            }
            valueColorsJosnArray->Put(std::to_string(i).c_str(), gradientItemJsonArray);
        }
        json->Put("valueColors", valueColorsJosnArray);
    }

    void ToJsonTrackShadow(std::unique_ptr<JsonValue>& json) const
    {
        auto pipelineContext = PipelineBase::GetCurrentContext();
        CHECK_NULL_VOID(pipelineContext);
        auto theme = pipelineContext->GetTheme<DataPanelTheme>();
        DataPanelShadow trackShadow;
        if (propShadowOption_.has_value()) {
            trackShadow.radius = propShadowOption_.value().radius;
            trackShadow.offsetX = propShadowOption_.value().offsetX;
            trackShadow.offsetY = propShadowOption_.value().offsetY;
            trackShadow.colors = propShadowOption_.value().colors;
        } else {
            trackShadow.radius = theme->GetTrackShadowRadius().ConvertToVp();
            trackShadow.offsetX = theme->GetTrackShadowOffsetX().ConvertToVp();
            trackShadow.offsetY = theme->GetTrackShadowOffsetY().ConvertToVp();
        }

        if (trackShadow.colors.size() == 0) {
            if (propValueColors_.has_value() && (propValueColors_.value().size() != 0)) {
                trackShadow.colors = propValueColors_.value();
            } else {
                auto colors = theme->GetColorsArray();
                for (const auto& item : colors) {
                    Gradient gradient;
                    CreateGradient(item, gradient);
                    trackShadow.colors.emplace_back(gradient);
                }
            }
        }
        auto shadowOptionJson = JsonUtil::Create(true);
        shadowOptionJson->Put("radius", std::to_string(trackShadow.radius).c_str());
        shadowOptionJson->Put("offsetX", std::to_string(trackShadow.offsetX).c_str());
        shadowOptionJson->Put("offsetY", std::to_string(trackShadow.offsetY).c_str());

        auto colorsJosnArray = JsonUtil::CreateArray(true);
        for (size_t i = 0; i < trackShadow.colors.size(); ++i) {
            Gradient gradientItem = trackShadow.colors[i];
            auto gradientItemJsonArray = JsonUtil::CreateArray(true);
            for (size_t index = 0; index < gradientItem.GetColors().size(); ++index) {
                auto gradientColor = gradientItem.GetColors()[index];
                auto gradientColorJosn = JsonUtil::Create(true);
                gradientColorJosn->Put("color", gradientColor.GetColor().ColorToString().c_str());
                gradientColorJosn->Put("offset", std::to_string(gradientColor.GetDimension().Value()).c_str());
                gradientItemJsonArray->Put(std::to_string(index).c_str(), gradientColorJosn);
            }
            colorsJosnArray->Put(std::to_string(i).c_str(), gradientItemJsonArray);
        }
        shadowOptionJson->Put("colors", colorsJosnArray);
        json->Put("trackShadow", shadowOptionJson);
    }

    void CreateGradient(const std::pair<Color, Color>& itemParam, Gradient& gradient) const
    {
        GradientColor gradientColorStart;
        gradientColorStart.SetColor(itemParam.first);
        gradientColorStart.SetDimension(Dimension(0.0));
        gradient.AddColor(gradientColorStart);
        GradientColor gradientColorEnd;
        gradientColorEnd.SetColor(itemParam.second);
        gradientColorEnd.SetDimension(Dimension(1.0));
        gradient.AddColor(gradientColorEnd);
    }

    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(Values, std::vector<double>, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(Max, double, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(DataPanelType, size_t, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(Effect, bool, PROPERTY_UPDATE_RENDER);

    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(ValueColors, std::vector<Gradient>, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(TrackBackground, Color, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(StrokeWidth, Dimension, PROPERTY_UPDATE_RENDER);
    ACE_DEFINE_PROPERTY_ITEM_WITHOUT_GROUP(ShadowOption, DataPanelShadow, PROPERTY_UPDATE_RENDER);

    ACE_DISALLOW_COPY_AND_MOVE(DataPanelPaintProperty);
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_DATA_PANEL_DATA_PANEL_PAINT_PROPERTY_H
