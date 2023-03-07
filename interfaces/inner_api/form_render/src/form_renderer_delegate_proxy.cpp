/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "form_renderer_delegate_proxy.h"

#include "form_renderer_delegate_interface.h"
#include "form_renderer_hilog.h"

namespace OHOS {
namespace Ace {
FormRendererDelegateProxy::FormRendererDelegateProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IFormRendererDelegate>(impl) {}

int32_t FormRendererDelegateProxy::OnSurfaceCreate(
    const std::shared_ptr<Rosen::RSSurfaceNode>& surfaceNode,
    const OHOS::AppExecFwk::FormJsInfo& formJsInfo,
    const AAFwk::Want& want)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s, failed to write interface token", __func__);
        return ERR_INVALID_VALUE;
    }

    if (surfaceNode == nullptr || !surfaceNode->Marshalling(data)) {
        HILOG_ERROR("%{public}s fail, write surfaceNode error", __func__);
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteParcelable(&formJsInfo)) {
        HILOG_ERROR("%{public}s fail, write formJsInfo error", __func__);
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteParcelable(&want)) {
        HILOG_ERROR("register callback fail, want error");
        return ERR_INVALID_VALUE;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(IFormRendererDelegate::Message::ON_SURFACE_CREATE), data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("register callback fail, error: %d", error);
        return error;
    }

    return reply.ReadInt32();
}

int32_t FormRendererDelegateProxy::OnActionEvent(const std::string& action)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s, failed to write interface token", __func__);
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteString(action)) {
        HILOG_ERROR("register callback fail, action error");
        return ERR_INVALID_VALUE;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(IFormRendererDelegate::Message::ON_ACTION_CREATE), data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("register callback fail, error: %d", error);
        return error;
    }

    return reply.ReadInt32();
}

int32_t FormRendererDelegateProxy::OnError(const std::string& code, const std::string& msg)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s, failed to write interface token", __func__);
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteString(code)) {
        HILOG_ERROR("register callback fail, code error");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteString(msg)) {
        HILOG_ERROR("register callback fail, msg error");
        return ERR_INVALID_VALUE;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = Remote()->SendRequest(
        static_cast<uint32_t>(IFormRendererDelegate::Message::ON_ERROR), data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("register callback fail, error: %d", error);
        return error;
    }

    return reply.ReadInt32();
}

int32_t FormRendererDelegateProxy::OnSurfaceChange(float width, float height)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("%{public}s, failed to write interface token", __func__);
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteFloat(width)) {
        HILOG_ERROR("write width fail, action error");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteFloat(height)) {
        HILOG_ERROR("write height fail, action error");
        return ERR_INVALID_VALUE;
    }

    MessageParcel reply;
    MessageOption option;
    int error = Remote()->SendRequest(
        static_cast<uint32_t>(IFormRendererDelegate::Message::ON_SURFACE_CHANGE), data, reply, option);
    if (error != NO_ERROR) {
        HILOG_ERROR("%{public}s, failed to SendRequest: %{public}d", __func__, error);
    }
    return reply.ReadInt32();
}

bool FormRendererDelegateProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(FormRendererDelegateProxy::GetDescriptor())) {
        HILOG_ERROR("%{public}s, failed to write interface token", __func__);
        return false;
    }
    return true;
}
} // namespace Ace
} // namespace OHOS
