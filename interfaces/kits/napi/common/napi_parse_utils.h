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

#ifndef NAPI_PARSE_UTILS_H
#define NAPI_PARSE_UTILS_H

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace NWeb {
constexpr int INTEGER_ZERO = 0;
constexpr int INTEGER_ONE = 1;
constexpr int INTEGER_TWO = 2;
constexpr int INTEGER_THREE = 3;
constexpr int INTEGER_FOUR = 4;
constexpr int INTEGER_FIVE = 5;

class NapiParseUtils {
public:
    static bool ParseInt32(napi_env env, napi_value argv, int32_t& outValue);
    static bool ParseString(napi_env env, napi_value argv, std::string& outValue);
    static bool ParseBoolean(napi_env env, napi_value argv, bool& outValue);
};
} // namespace NWeb
} // namespace OHOS
#endif