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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_IMAGE_IMAGE_LAYOUT_ALGORITHM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_IMAGE_IMAGE_LAYOUT_ALGORITHM_H

#include "core/components_ng/layout/box_layout_algorithm.h"
#include "core/components_ng/layout/layout_wrapper.h"
#include "core/image/image_object.h"

namespace OHOS::Ace::NG {

class ACE_EXPORT ImageLayoutAlgorithm : public BoxLayoutAlgorithm {
    DECLARE_ACE_TYPE(ImageLayoutAlgorithm, BoxLayoutAlgorithm);

public:
    ImageLayoutAlgorithm(ImageObjSuccessCallback successCallback, UploadSuccessCallback uploadSuccessCallback,
        FailedCallback failedCallback, OnPostBackgroundTask onBackgroundTaskPostCallback)
        : successCallback_(std::move(successCallback)), uploadSuccessCallback_(std::move(uploadSuccessCallback)),
          failedCallback_(std::move(failedCallback)),
          onBackgroundTaskPostCallback_(std::move(onBackgroundTaskPostCallback))
    {}

    ~ImageLayoutAlgorithm() override = default;

    void OnReset() override
    {
        successCallback_ = nullptr;
        uploadSuccessCallback_ = nullptr;
        failedCallback_ = nullptr;
        onBackgroundTaskPostCallback_ = nullptr;
        imageObject_.Reset();
    }

    void SetImageObject(const RefPtr<ImageObject>& imageObject)
    {
        imageObject_ = imageObject;
    }

    std::optional<SizeF> MeasureContent(
        const LayoutConstraintF& contentConstraint, LayoutWrapper* layoutWrapper) override;

    const RefPtr<ImageObject>& GetImageObject() const
    {
        return imageObject_;
    }

private:
    static SizeF ProcessContentSize(const LayoutConstraintF& contentConstraint, const RefPtr<ImageObject>& imageObject);

    ImageObjSuccessCallback successCallback_;
    UploadSuccessCallback uploadSuccessCallback_;
    FailedCallback failedCallback_;
    OnPostBackgroundTask onBackgroundTaskPostCallback_;

    RefPtr<ImageObject> imageObject_;

    ACE_DISALLOW_COPY_AND_MOVE(ImageLayoutAlgorithm);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_IMAGE_IMAGE_LAYOUT_ALGORITHM_H
