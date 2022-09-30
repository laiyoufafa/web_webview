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

#include "web_errors.h"

#include <string>
#include <unordered_map>

namespace {
// error message
const std::string PARAM_CHECK_ERROR_MSG = "Invaild input parameter";
const std::string INVALID_URL_MSG = "Invaild url";
const std::string INVALID_COOKIE_VALUE_MSG = "Invaild cookie value";
const std::string INVALID_ORIGIN_MSG = "Invaild permission origin";
const std::string NO_WEBSTORAGE_ORIGIN_MSG = "Invaild web storage origin";
const std::string NO_WEB_INSTANCE_BIND_MSG = "No web instance bind with controller";
const std::string CREATE_MSG_PORT_FAIL_MSG = "Create message port failed";
}

namespace OHOS {
namespace NWebError {
std::unordered_map<ErrCode, std::string> g_errCodeMsgMap = {
    {PARAM_CHECK_ERROR, PARAM_CHECK_ERROR_MSG},
    {INVALID_URL, INVALID_URL_MSG},
    {INVALID_COOKIE_VALUE, INVALID_COOKIE_VALUE_MSG},
    {INVALID_ORIGIN, INVALID_ORIGIN_MSG},
    {NO_WEBSTORAGE_ORIGIN, NO_WEBSTORAGE_ORIGIN_MSG},
    {NO_WEB_INSTANCE_BIND, NO_WEB_INSTANCE_BIND_MSG},
    {CREATE_MSG_PORT_FAIL, CREATE_MSG_PORT_FAIL_MSG}
};

std::string GetErrMsgByErrCode(ErrCode code)
{
    return g_errCodeMsgMap[code];
}
} // namespace NWebError
} // namespace OHOS