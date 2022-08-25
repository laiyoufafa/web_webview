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

#include "napi_web_cookie_manager.h"

#include <cstdint>
#include <vector>

#include "napi/native_common.h"
#include "nweb_cookie_manager.h"
#include "nweb_helper.h"
#include "securec.h"

namespace OHOS {
napi_value NapiWebCookieManager::Init(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("getCookie", NapiWebCookieManager::JsGetCookie),
        DECLARE_NAPI_STATIC_FUNCTION("setCookie", NapiWebCookieManager::JsSetCookie),
        DECLARE_NAPI_STATIC_FUNCTION("isCookieAllowed", NapiWebCookieManager::JsIsCookieAllowed),
        DECLARE_NAPI_STATIC_FUNCTION("putAcceptCookieEnabled", NapiWebCookieManager::JsPutAcceptCookieEnabled),
        DECLARE_NAPI_STATIC_FUNCTION("isThirdPartyCookieAllowed",
                                     NapiWebCookieManager::JsIsThirdPartyCookieAllowed),
        DECLARE_NAPI_STATIC_FUNCTION("putAcceptThirdPartyCookieEnabled",
                                     NapiWebCookieManager::JsPutAcceptThirdPartyCookieEnabled),
        DECLARE_NAPI_STATIC_FUNCTION("existCookie", NapiWebCookieManager::JsExistCookie),
        DECLARE_NAPI_STATIC_FUNCTION("deleteEntireCookie", NapiWebCookieManager::JsDeleteEntireCookie),
    };
    napi_value constructor = nullptr;

    napi_define_class(env, WEB_COOKIE_MANAGER_CLASS_NAME.c_str(), WEB_COOKIE_MANAGER_CLASS_NAME.length(),
        JsConstructor, nullptr, sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    NAPI_ASSERT(env, constructor != nullptr, "NapiWebCookieManager define js class failed");
    napi_status status = napi_set_named_property(env, exports, "WebCookieManager", constructor);
    NAPI_ASSERT(env, status == napi_ok, "NapiWebCookieManager set property failed");
    return exports;
}

napi_value NapiWebCookieManager::JsConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    return thisVar;
}

constexpr int MAX_STRING_LENGTH = 40960;
bool NapiWebCookieManager::GetStringPara(napi_env env, napi_value argv, std::string& outValue)
{
    size_t bufferSize = 0;
    napi_valuetype valueType = napi_null;

    napi_typeof(env, argv, &valueType);
    if (valueType != napi_string) {
        return false;
    }
    napi_get_value_string_utf8(env, argv, nullptr, 0, &bufferSize);
    if (bufferSize > MAX_STRING_LENGTH) {
        return false;
    }
    char stringValue[bufferSize + 1];
    size_t jsStringLength = 0;
    napi_get_value_string_utf8(env, argv, stringValue, bufferSize + 1, &jsStringLength);
    if (jsStringLength != bufferSize) {
        return false;
    }
    outValue = stringValue;
    return true;
}

bool NapiWebCookieManager::GetBooleanPara(napi_env env, napi_value argv, bool& outValue)
{
    napi_valuetype valueType = napi_null;

    napi_typeof(env, argv, &valueType);
    if (valueType != napi_boolean) {
        return false;
    }

    bool boolValue;
    napi_get_value_bool(env, argv, &boolValue);
    outValue = boolValue;
    return true;
}

napi_value NapiWebCookieManager::JsGetCookie(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = 1;
    napi_value argv[1] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    NAPI_ASSERT(env, argc == 1, "requires 1 parameter");

    bool ret;
    std::string url;
    ret = GetStringPara(env, argv[0], url);
    NAPI_ASSERT_BASE(env, ret, "get para0 failed", retValue);

    napi_value result = nullptr;
    std::string cookieContent = "";

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieContent = cookieManager->ReturnCookie(url);
    }
    napi_create_string_utf8(env, cookieContent.c_str(), cookieContent.length(), &result);
    return result;
}
constexpr int SETCOOKIE_PARA_NUM = 2;
napi_value NapiWebCookieManager::JsSetCookie(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = SETCOOKIE_PARA_NUM;
    napi_value argv[2] = { 0, 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    NAPI_ASSERT(env, argc == SETCOOKIE_PARA_NUM, "requires 2 parameter");

    bool ret1;
    bool ret2;
    std::string url;
    std::string value;
    ret1 = GetStringPara(env, argv[0], url);
    NAPI_ASSERT_BASE(env, ret1, "get para0 failed", retValue);
    ret2 = GetStringPara(env, argv[1], value);
    NAPI_ASSERT_BASE(env, ret2, "get para1 failed", retValue);

    napi_value result = nullptr;
    bool isSet = false;

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        isSet = cookieManager->SetCookie(url, value);
    }
    NAPI_CALL(env, napi_get_boolean(env, isSet, &result));
    return result;
}

napi_value NapiWebCookieManager::JsIsCookieAllowed(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    bool accept = true;

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        accept = cookieManager->IsAcceptCookieAllowed();
    }
    NAPI_CALL(env, napi_get_boolean(env, accept, &result));
    return result;
}

napi_value NapiWebCookieManager::JsPutAcceptCookieEnabled(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = 1;
    napi_value argv[1] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    NAPI_ASSERT(env, argc == 1, "requires 1 parameter");

    bool ret;
    bool accept;
    ret = GetBooleanPara(env, argv[0], accept);
    NAPI_ASSERT_BASE(env, ret, "get para0 failed", retValue);

    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, false, &result));

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->PutAcceptCookieEnabled(accept);
        NAPI_CALL(env, napi_get_boolean(env, true, &result));
    }
    return result;
}

napi_value NapiWebCookieManager::JsIsThirdPartyCookieAllowed(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    bool accept = true;

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        accept = cookieManager->IsThirdPartyCookieAllowed();
    }
    NAPI_CALL(env, napi_get_boolean(env, accept, &result));
    return result;
}

napi_value NapiWebCookieManager::JsPutAcceptThirdPartyCookieEnabled(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = 1;
    napi_value argv[1] = { 0 };

    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    NAPI_ASSERT(env, argc == 1, "requires 1 parameter");

    bool ret;
    bool accept;
    ret = GetBooleanPara(env, argv[0], accept);
    NAPI_ASSERT_BASE(env, ret, "get para0 failed", retValue);

    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, false, &result));

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->PutAcceptThirdPartyCookieEnabled(accept);
        NAPI_CALL(env, napi_get_boolean(env, true, &result));
    }
    return result;
}

napi_value NapiWebCookieManager::JsExistCookie(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    bool exist = true;

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        exist = cookieManager->ExistCookies();
    }
    NAPI_CALL(env, napi_get_boolean(env, exist, &result));
    return result;
}

napi_value NapiWebCookieManager::JsDeleteEntireCookie(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, false, &result));

    OHOS::NWeb::NWebCookieManager* cookieManager = OHOS::NWeb::NWebHelper::Instance().GetCookieManager();
    if (cookieManager != nullptr) {
        cookieManager->DeleteCookieEntirely(nullptr);
        NAPI_CALL(env, napi_get_boolean(env, true, &result));
    }

    return result;
}
} // namespace OHOS