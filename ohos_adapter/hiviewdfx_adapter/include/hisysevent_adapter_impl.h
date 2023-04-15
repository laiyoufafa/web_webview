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

#ifndef HISYSEVENT_ADAPTER_IMPL_H
#define HISYSEVENT_ADAPTER_IMPL_H

#include "hisysevent_adapter.h"

namespace OHOS::NWeb {
class HiSysEventAdapterImpl : public HiSysEventAdapter {
public:
    static HiSysEventAdapterImpl& GetInstance();

    ~HiSysEventAdapterImpl() override = default;

    int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const int, const std::string, const int, const std::string, const int,
            const std::string, const int, const std::string, const float>& data) override;

    int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const int, const std::string, const int, const std::string, const int>&
            data) override;

    int Write(const std::string& eventName, EventType type,
        const std::tuple<const std::string, const int, const std::string, const std::string, const std::string,
            const int, const std::string, const std::string>& data) override;
};
} // namespace OHOS::NWeb

#endif // HISYSEVENT_ADAPTER_IMPL_H
