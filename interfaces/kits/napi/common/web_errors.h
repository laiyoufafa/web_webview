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

#ifndef WEB_ERRORS_H
#define WEB_ERRORS_H
 
 
#include "errors.h"

namespace OHOS {
namespace NWebError {
constexpr ErrCode PARAM_CHECK_ERROR = 401;
constexpr ErrCode INVALID_URL = 17100001;
constexpr ErrCode INVALID_COOKIE_VALUE = 17100009;
constexpr ErrCode INVALID_ORIGIN = 17100017;
}
}
#endif