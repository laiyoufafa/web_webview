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

#include "access_token_adapter_impl.h"

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "nweb_log.h"

namespace OHOS::NWeb {
AccessTokenAdapterImpl& AccessTokenAdapterImpl::GetInstance()
{
    static AccessTokenAdapterImpl instance;
    return instance;
}

bool AccessTokenAdapterImpl::VerifyAccessToken(const std::string& permissionName)
{
    uint32_t tokenID = IPCSkeleton::GetCallingTokenID();
    return Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenID, permissionName) ==
        Security::AccessToken::PERMISSION_GRANTED;
}
} // namespace OHOS::NWeb
