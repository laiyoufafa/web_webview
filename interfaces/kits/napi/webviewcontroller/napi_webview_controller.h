/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NWEB_NAPI_WEBVIEW_CONTROLLER_H
#define NWEB_NAPI_WEBVIEW_CONTROLLER_H

#include "hilog/log.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS {
const std::string WEBVIEW_CONTROLLER_CLASS_NAME = "WebviewController";

class NapiWebviewController {
public:
    NapiWebviewController(napi_env env, napi_value thisVar, int32_t webId);
    ~NapiWebviewController() = default;

    static napi_value Init(napi_env env, napi_value exports);

private:
    static napi_value JsConstructor(napi_env env, napi_callback_info info);

    static napi_value JsSetWebId(napi_env env, napi_callback_info info);

    static bool GetStringPara(napi_env env, napi_value argv, std::string& outValue);

    static bool GetIntPara(napi_env env, napi_value argv, int32_t& outValue);

    static napi_value JsAccessForward(napi_env env, napi_callback_info info);
    bool JsAccessForwardInternal(napi_env env, napi_callback_info info);

    static napi_value JsAccessBackward(napi_env env, napi_callback_info info);
    bool JsAccessBackwardInternal(napi_env env, napi_callback_info info);

    static napi_value JsForward(napi_env env, napi_callback_info info);
    void JsForwardInternal(napi_env env, napi_callback_info info);

    static napi_value JsBackward(napi_env env, napi_callback_info info);
    void JsBackwardInternal(napi_env env, napi_callback_info info);
    
    int32_t nwebId { -1 };
};
} // namespace OHOS

#endif // NWEB_NAPI_WEBVIEW_CONTROLLER_H
