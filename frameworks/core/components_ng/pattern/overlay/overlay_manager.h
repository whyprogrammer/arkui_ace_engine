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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_OVERLAY_OVERLAY_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_OVERLAY_OVERLAY_MANAGER_H

#include <unordered_map>
#include <utility>

#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/utils/noncopyable.h"
#include "core/components/common/properties/placement.h"
#include "core/components/dialog/dialog_properties.h"
#include "core/components/picker/picker_data.h"
#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/pattern/overlay/sheet_style.h"
#include "core/components_ng/pattern/picker/datepicker_event_hub.h"
#include "core/components_ng/pattern/picker/picker_type_define.h"
#include "core/components_ng/pattern/text_picker/textpicker_event_hub.h"
#include "core/pipeline_ng/ui_task_scheduler.h"

namespace OHOS::Ace::NG {

struct PopupInfo {
    int32_t popupId = -1;
    WeakPtr<FrameNode> target;
    RefPtr<FrameNode> popupNode;
    bool markNeedUpdate = false;
    bool isCurrentOnShow = false;
    bool isBlockEvent = true;
    SizeF targetSize;
    OffsetF targetOffset;
};

// StageManager is the base class for root render node to perform page switch.
class ACE_EXPORT OverlayManager : public virtual AceType {
    DECLARE_ACE_TYPE(OverlayManager, AceType);

public:
    explicit OverlayManager(const RefPtr<FrameNode>& rootNode) : rootNodeWeak_(rootNode) {}
    ~OverlayManager() override
    {
        LOGI("OverlayManager Destructor.");
        popupMap_.clear();
    }
    void ShowIndexerPopup(int32_t targetId, RefPtr<FrameNode>& customNode);
    void RemoveIndexerPopup();
    void UpdatePopupNode(int32_t targetId, const PopupInfo& popupInfo);
    void HidePopup(int32_t targetId, const PopupInfo& popupInfo);
    void ErasePopup(int32_t targetId);
    void HideAllPopups();

    const PopupInfo& GetPopupInfo(int32_t targetId)
    {
        return popupMap_[targetId];
    }

    bool HasPopupInfo(int32_t targetId) const
    {
        if (popupMap_.find(targetId) != popupMap_.end()) {
            return true;
        }
        return false;
    }

    void ShowMenu(int32_t targetId, const NG::OffsetF& offset, RefPtr<FrameNode> menu = nullptr);
    void HideMenu(int32_t targetId);
    void DeleteMenu(int32_t targetId);
    void ShowMenuInSubWindow(int32_t targetId, const NG::OffsetF& offset, RefPtr<FrameNode> menu = nullptr);
    void HideMenuInSubWindow(int32_t targetId);
    void HideMenuInSubWindow();
    void CleanMenuInSubWindow();
    void HideAllMenus();

    void ShowToast(const std::string& message, int32_t duration, const std::string& bottom, bool isRightToLeft);

    // customNode only used by customDialog, pass in nullptr if not customDialog
    RefPtr<FrameNode> ShowDialog(
        const DialogProperties& dialogProps, const RefPtr<UINode>& customNode, bool isRightToLeft = false);
    void ShowCustomDialog(const RefPtr<FrameNode>& customNode);
    void ShowDateDialog(const DialogProperties& dialogProps, const DatePickerSettingData& settingData,
        std::map<std::string, NG::DialogEvent> dialogEvent,
        std::map<std::string, NG::DialogGestureEvent> dialogCancelEvent);
    void ShowTimeDialog(const DialogProperties& dialogProps, const TimePickerSettingData& settingData,
        std::map<std::string, PickerTime> timePickerProperty, std::map<std::string, NG::DialogEvent> dialogEvent,
        std::map<std::string, NG::DialogGestureEvent> dialogCancelEvent);
    void ShowTextDialog(const DialogProperties& dialogProps, const TextPickerSettingData& settingData,
        std::map<std::string, NG::DialogTextEvent> dialogEvent,
        std::map<std::string, NG::DialogGestureEvent> dialogCancelEvent);

    void CloseDialog(const RefPtr<FrameNode>& dialogNode);

    /**  pop overlays (if any) on back press
     *
     *   @return    true if popup was removed, false if no overlay exists
     */
    bool RemoveOverlay();
    bool RemoveModalInOverlay();
    bool RemoveOverlayInSubwindow();

    void RegisterOnHideMenu(std::function<void()> callback)
    {
        onHideMenuCallback_ = callback;
    }

    void RegisterOnShowMenu(const std::function<void()>& callback)
    {
        onShowMenuCallback_ = callback;
    }

    void CallOnShowMenuCallback()
    {
        if (onShowMenuCallback_) {
            onShowMenuCallback_();
        }
    }

    void CallOnHideMenuCallback()
    {
        if (onHideMenuCallback_) {
            onHideMenuCallback_();
        }
    }

    void SetBackPressEvent(std::function<bool()> event)
    {
        backPressEvent_ = event;
    }

    bool FireBackPressEvent() const
    {
        if (backPressEvent_) {
            return backPressEvent_();
        }
        return false;
    }

#ifdef ENABLE_DRAG_FRAMEWORK
    bool GetHasPixelMap()
    {
        return hasPixelMap_;
    }

    void SetHasPixelMap(bool hasPixelMap)
    {
        hasPixelMap_ = hasPixelMap;
    }

    bool GetHasFilter()
    {
        return hasFilter_;
    }

    void SetHasFilter(bool hasFilter)
    {
        hasFilter_ = hasFilter;
    }

    bool GetHasEvent()
    {
        return hasEvent_;
    }

    void SetHasEvent(bool hasEvent)
    {
        hasEvent_ = hasEvent;
    }

    bool GetIsOnAnimation()
    {
        return isOnAnimation_;
    }

    void SetIsOnAnimation(bool isOnAnimation)
    {
        isOnAnimation_ = isOnAnimation;
    }

    void SetFilterColumnNode(const RefPtr<FrameNode>& columnNode)
    {
        filterColumnNodeWeak_ = columnNode;
    }

    void MountPixelmapToRootNode(const RefPtr<FrameNode>& columnNode);
    void MountEventToRootNode(const RefPtr<FrameNode>& columnNode);
    void RemovePixelMap();
    void RemovePixelMapAnimation(bool startDrag, double x, double y);
    void UpdatePixelMapScale(float& scale);
    void RemoveFilter();
    void RemoveEventColumn();
#endif // ENABLE_DRAG_FRAMEWORK
    void BindContentCover(bool isShow, std::function<void(const std::string&)>&& callback,
        std::function<RefPtr<UINode>()>&& buildNodeFunc, int32_t type, int32_t targetId);

    void BindSheet(bool isShow, std::function<void(const std::string&)>&& callback,
        std::function<RefPtr<UINode>()>&& buildNodeFunc, NG::SheetStyle& sheetStyle, int32_t targetId);

    void DestroySheet(const RefPtr<FrameNode>& sheetNode, int32_t targetId);

private:
    void PopToast(int32_t targetId);

    // toast should contain id to avoid multiple delete.
    std::unordered_map<int32_t, WeakPtr<FrameNode>> toastMap_;

    /**  find/register menu node and update menu's display position
     *
     *   @return     true if process is successful
     */
    bool ShowMenuHelper(RefPtr<FrameNode>& menu, int32_t targetId, const NG::OffsetF& offset);

    void FocusOverlayNode(const RefPtr<FrameNode>& overlayNode, bool isInSubWindow = false);
    void BlurOverlayNode(bool isInSubWindow = false);

    void ShowMenuAnimation(const RefPtr<FrameNode>& menu, bool isInSubWindow = false);
    void PopMenuAnimation(const RefPtr<FrameNode>& menu);

    void OpenDialogAnimation(const RefPtr<FrameNode>& node);
    void CloseDialogAnimation(const RefPtr<FrameNode>& node);

    void AdaptToSafeArea(const RefPtr<FrameNode>& node);

    void SaveLastModalNode();
    void PlayDefaultModalTransition(const RefPtr<FrameNode>& modalNode, bool isTransitionIn);
    void DefaultModalTransition(bool isTransitionIn);
    void PlayAlphaModalTransition(const RefPtr<FrameNode>& modalNode, bool isTransitionIn);

    void PlaySheetTransition(RefPtr<FrameNode> sheetNode, bool isTransitionIn, bool isFirstTransition = true);

    void ComputeSheetOffset(NG::SheetStyle& sheetStyle);

    // Key: target Id, Value: PopupInfo
    std::unordered_map<int32_t, NG::PopupInfo> popupMap_;
    // K: target frameNode ID, V: menuNode
    std::unordered_map<int32_t, RefPtr<FrameNode>> menuMap_;
    std::unordered_map<int32_t, RefPtr<FrameNode>> customPopupMap_;
    std::stack<WeakPtr<FrameNode>> modalStack_;
    WeakPtr<FrameNode> lastModalNode_;
    float sheetHeight_ {0.0};
    WeakPtr<UINode> rootNodeWeak_;
#ifdef ENABLE_DRAG_FRAMEWORK
    bool hasPixelMap_ {false};
    bool hasFilter_ {false};
    bool hasEvent_ {false};
    bool isOnAnimation_ {false};
    WeakPtr<FrameNode> pixelmapColumnNodeWeak_;
    WeakPtr<FrameNode> filterColumnNodeWeak_;
    WeakPtr<FrameNode> eventColumnNodeWeak_;
#endif // ENABLE_DRAG_FRAMEWORK

    std::function<void()> onHideMenuCallback_ = nullptr;
    std::function<void()> onShowMenuCallback_;
    CancelableCallback<void()> continuousTask_;
    std::function<bool()> backPressEvent_ = nullptr;

    ACE_DISALLOW_COPY_AND_MOVE(OverlayManager);
};
} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_OVERLAY_OVERLAY_MANAGER_H
