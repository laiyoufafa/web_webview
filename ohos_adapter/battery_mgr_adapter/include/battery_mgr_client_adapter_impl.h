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

#ifndef BATTERY_MGR_CLIENT_ADAPTER_IMPL_H
#define BATTERY_MGR_CLIENT_ADAPTER_IMPL_H

#include "battery_mgr_client_adapter.h"
#include "battery_info.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "matching_skills.h"
#include "want.h"
#include "common_event_subscriber.h"

namespace OHOS::NWeb {

class NWebBatteryEventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    NWebBatteryEventSubscriber(const EventFwk::CommonEventSubscribeInfo &subscribeInfo,
                                BatteryEventCallback& eventCallback);
    ~NWebBatteryEventSubscriber() override = default;
    void OnReceiveEvent(const EventFwk::CommonEventData &data) override;
private:
    BatteryEventCallback eventCallback_;
};

class BatteryMgrClientAdapterImpl : public BatteryMgrClientAdapter {
public:
    BatteryMgrClientAdapterImpl() = default;
    virtual ~BatteryMgrClientAdapterImpl() = default;

    void RegBatteryEvent(const BatteryEventCallback&& eventCallback) override;
  
    bool StartListen() override;

    void StopListen() override;

    std::unique_ptr<WebBatteryInfo> RequestBatteryInfo() override;
private:
    BatteryEventCallback cb = nullptr;
    std::shared_ptr<EventFwk::CommonEventSubscriber> commonEventSubscriber_ = nullptr;
};
}

#endif