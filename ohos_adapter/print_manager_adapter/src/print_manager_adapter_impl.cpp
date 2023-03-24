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

#include "print_manager_adapter_impl.h"

#include "nweb_log.h"

#include "base/print/print_fwk/frameworks/innerkitsimpl/include/print_manager_client.h"

namespace OHOS::NWeb {

// static
PrintManagerAdapterImpl& PrintManagerAdapterImpl::GetInstance()
{
    static PrintManagerAdapterImpl instance;
    return instance;
}

int32_t PrintManagerAdapterImpl::StartPrint(
    const std::vector<std::string>& fileList, const std::vector<uint32_t>& fdList, std::string& taskId)
{
    int32_t ret = OHOS::Print::PrintManagerClient::GetInstance()->StartPrint(fileList, fdList, taskId);
    if (ret != 0) {
        WVLOG_E("StartPrint failed, failed id = %{public}d", ret);
        return -1;
    }
    return ret;
}
} // namespace OHOS::NWeb
