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

#include "napi_web_storage.h"
#include <unistd.h>
#include "hilog/log.h"
#include "securec.h"

namespace OHOS {
napi_value NapiWebStorage::Init(napi_env env, napi_value exports)
{
    const std::string WEB_STORAGE_CLASS_NAME = "WebStorage";
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_STATIC_FUNCTION("deleteAllData", NapiWebStorage::JsDeleteAllData),
        DECLARE_NAPI_STATIC_FUNCTION("deleteOrigin", NapiWebStorage::JsDeleteOrigin),
        DECLARE_NAPI_STATIC_FUNCTION("getOrigins", NapiWebStorage::JsGetOrigins),
        DECLARE_NAPI_STATIC_FUNCTION("getOriginQuota", NapiWebStorage::JsGetOriginQuota),
        DECLARE_NAPI_STATIC_FUNCTION("getOriginUsage", NapiWebStorage::JsGetOriginUsage),
    };
    napi_value constructor = nullptr;
    napi_define_class(env, WEB_STORAGE_CLASS_NAME.c_str(), WEB_STORAGE_CLASS_NAME.length(), JsConstructor, nullptr,
        sizeof(properties) / sizeof(properties[0]), properties, &constructor);
    NAPI_ASSERT(env, constructor != nullptr, "define js class TestAsync failed");
    napi_status status = napi_set_named_property(env, exports, "WebStorage", constructor);
    NAPI_ASSERT(env, status == napi_ok, "set property WebStorage failed");
    return exports;
}
napi_value NapiWebStorage::JsDeleteAllData(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    OHOS::NWeb::NWebWebStorage* web_storage = OHOS::NWeb::NWebHelper::Instance().GetWebStorage();
    if (web_storage) {
        web_storage->DeleteAllData();
    }
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiWebStorage::JsDeleteOrigin(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = 1;
    napi_value argv = nullptr;
    napi_value result = nullptr;
    napi_get_cb_info(env, info, &argc, &argv, &retValue, nullptr);
    NAPI_ASSERT(env, argc == 1, "requires 1 parameter");
    size_t bufferSize = 0;
    napi_valuetype valueType = napi_null;
    napi_typeof(env, argv, &valueType);
    NAPI_ASSERT(env, valueType == napi_string, "type mismatch for parameter 1");
    napi_get_value_string_utf8(env, argv, nullptr, 0, &bufferSize);
    NAPI_ASSERT_BASE(env, bufferSize < MAX_WEB_STRING_LENGTH, "string length too large", retValue);
    char stringValue[bufferSize + 1];
    size_t jsStringLength = 0;
    napi_get_value_string_utf8(env, argv, stringValue, bufferSize + 1, &jsStringLength);
    NAPI_ASSERT_BASE(env, jsStringLength == bufferSize, "string length wrong", retValue);
    std::string origin(stringValue);
    OHOS::NWeb::NWebWebStorage* web_storage = OHOS::NWeb::NWebHelper::Instance().GetWebStorage();
    if (web_storage) {
        web_storage->DeleteOrigin(origin);
    }
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiWebStorage::GetErrorCodeValue(napi_env env, int errCode)
{
    napi_value jsObject = nullptr;
    napi_value jsValue = nullptr;
    NAPI_CALL(env, napi_create_int32(env, errCode, &jsValue));
    NAPI_CALL(env, napi_create_object(env, &jsObject));
    NAPI_CALL(env, napi_set_named_property(env, jsObject, "code", jsValue));
    return jsObject;
}

void NapiWebStorage::ExecuteGetOrigins(napi_env env, void *data)
{
    GetOriginsParam *param = reinterpret_cast<GetOriginsParam *>(data);
    OHOS::NWeb::NWebWebStorage* web_storage = OHOS::NWeb::NWebHelper::Instance().GetWebStorage();
    if (!web_storage) {
        param->errCode = INTERFACE_ERROR;
        param->status = napi_generic_failure;
        return;
    }
    std::vector<OHOS::NWeb::NWebWebStorageOrigin> origins = web_storage->GetOrigins();
    for (auto origin : origins) {
        NapiWebStorageOrigin napiOrigin;
        napiOrigin.origin = origin.GetOrigin();
        napiOrigin.quota = origin.GetQuota();
        napiOrigin.usage = origin.GetUsage();
        param->origins.push_back(napiOrigin);
    }
    param->errCode = param->origins.empty() ? INTERFACE_ERROR : INTERFACE_OK;
    param->status = param->errCode == INTERFACE_OK ? napi_ok : napi_generic_failure;
}

void NapiWebStorage::GetNapiWebStorageOriginForResult(napi_env env,
    const std::vector<NapiWebStorageOrigin> &info, napi_value result)
{
    int32_t index = 0;
    for (auto item : info) {
        napi_value napiWebStorageOrigin = nullptr;
        napi_create_object(env, &napiWebStorageOrigin);

        napi_value origin = nullptr;
        napi_create_string_utf8(env, item.origin.c_str(), NAPI_AUTO_LENGTH, &origin);
        napi_set_named_property(env, napiWebStorageOrigin, "origin", origin);

        napi_value quota = nullptr;
        napi_create_uint32(env, static_cast<uint32_t>(item.quota), &quota);
        napi_set_named_property(env, napiWebStorageOrigin, "quota", quota);

        napi_value usage = nullptr;
        napi_create_uint32(env, static_cast<uint32_t>(item.usage), &usage);
        napi_set_named_property(env, napiWebStorageOrigin, "usage", usage);

        napi_set_element(env, result, index, napiWebStorageOrigin);
        index++;
    }
}

void NapiWebStorage::GetOriginComplete(napi_env env, napi_status status, void *data)
{
    GetOriginsParam* param = (GetOriginsParam*)data;
    napi_value setResult[RESULT_COUNT] = {0};
    if (param->status) {
        setResult[PARAMZERO] = GetErrorCodeValue(env, param->errCode);
        napi_get_undefined(env, &setResult[PARAMONE]);
    } else {
        napi_get_undefined(env, &setResult[PARAMZERO]);
        napi_create_array(env, &setResult[PARAMONE]);
        GetNapiWebStorageOriginForResult(env, param->origins, setResult[PARAMONE]);
    }
    napi_value args[RESULT_COUNT] = {setResult[PARAMZERO], setResult[PARAMONE]};
    napi_value callback = nullptr;
    napi_get_reference_value(env, param->callbackRef, &callback);
    napi_value returnVal = nullptr;
    napi_call_function(env, nullptr, callback, RESULT_COUNT, args, &returnVal);
    napi_delete_reference(env, param->callbackRef);
    napi_delete_async_work(env, param->asyncWork);
    delete param;
}

void NapiWebStorage::GetOriginsPromiseComplete(napi_env env, napi_status status, void *data)
{
    GetOriginsParam* param = (GetOriginsParam*)data;
    napi_value setResult[RESULT_COUNT] = {0};
    setResult[PARAMZERO] = GetErrorCodeValue(env, param->errCode);
    napi_create_array(env, &setResult[PARAMONE]);
    GetNapiWebStorageOriginForResult(env, param->origins, setResult[PARAMONE]);
    napi_value args[RESULT_COUNT] = {setResult[PARAMZERO], setResult[PARAMONE]};
    if (param->status == napi_ok) {
        napi_resolve_deferred(env, param->deferred, args[1]);
    } else {
        napi_reject_deferred(env, param->deferred, args[0]);
    }
    napi_delete_async_work(env, param->asyncWork);
    delete param;
}

napi_value NapiWebStorage::GetOriginsAsync(napi_env env, napi_value *argv)
{
    napi_value result = nullptr;
    napi_value resourceName = nullptr;

    GetOriginsParam *param = new GetOriginsParam {
        .asyncWork = nullptr,
        .deferred = nullptr,
        .callbackRef = nullptr,
        .env = env,
    };
    napi_create_reference(env, *argv, 1, &param->callbackRef);
    NAPI_CALL(env, napi_create_string_utf8(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, ExecuteGetOrigins,
        GetOriginComplete, (void *)param, &param->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, param->asyncWork));
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiWebStorage::GetOriginsPromise(napi_env env)
{
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_create_promise(env, &deferred, &promise);

    GetOriginsParam *param = new GetOriginsParam {
        .asyncWork = nullptr,
        .deferred = deferred,
        .callbackRef = nullptr,
        .env = env,
    };
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, ExecuteGetOrigins,
        GetOriginsPromiseComplete, (void *)param, &param->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, param->asyncWork));
    return promise;
}

napi_value NapiWebStorage::JsGetOrigins(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = 0;
    size_t argcPromise = 0;
    size_t argcCallback = 1;
    napi_value argv = nullptr;
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    napi_get_cb_info(env, info, &argc, &argv, &retValue, nullptr);
    NAPI_ASSERT(env, argc == argcPromise || argc == argcCallback, "requires 0 or 1 parameter");
    if (argc == argcCallback) {
        napi_valuetype valueType = napi_undefined;
        napi_get_cb_info(env, info, &argc, &argv, &retValue, nullptr);
        napi_typeof(env, argv, &valueType);
        if (valueType == napi_function) {
            return GetOriginsAsync(env, &argv);
        }
        return result;
    }
    return GetOriginsPromise(env);
}

void NapiWebStorage::ExecuteGetOriginUsageOrQuota(napi_env env, void *data)
{
    GetOriginUsageOrQuotaParam *param = reinterpret_cast<GetOriginUsageOrQuotaParam *>(data);
    OHOS::NWeb::NWebWebStorage* web_storage = OHOS::NWeb::NWebHelper::Instance().GetWebStorage();
    if (!web_storage) {
        param->errCode = INTERFACE_ERROR;
        param->status = napi_generic_failure;
        return;
    }
    if (param->isQuato) {
        param->retValue = web_storage->GetOriginQuota(param->origin);
        param->errCode = param->retValue == -1 ? INTERFACE_ERROR : INTERFACE_OK;
        param->status = param->errCode == INTERFACE_OK ? napi_ok : napi_generic_failure;
        return;
    }
    param->retValue = web_storage->GetOriginUsage(param->origin);
    param->errCode = param->retValue == -1 ? INTERFACE_ERROR : INTERFACE_OK;
    param->status = param->errCode == INTERFACE_OK ? napi_ok : napi_generic_failure;
}

void NapiWebStorage::GetOriginUsageOrQuotaComplete(napi_env env, napi_status status, void *data)
{
    GetOriginUsageOrQuotaParam* param = (GetOriginUsageOrQuotaParam*)data;
    napi_value setResult[RESULT_COUNT] = {0};
    if (param->status) {
        setResult[PARAMZERO] = GetErrorCodeValue(env, param->errCode);
        napi_get_undefined(env, &setResult[PARAMONE]);
    } else {
        napi_get_undefined(env, &setResult[PARAMZERO]);
        napi_create_array(env, &setResult[PARAMONE]);
        napi_create_uint32(env, static_cast<uint32_t>(param->retValue), &setResult[PARAMONE]);
    }
    napi_value args[RESULT_COUNT] = {setResult[PARAMZERO], setResult[PARAMONE]};
    napi_value callback = nullptr;
    napi_get_reference_value(env, param->callbackRef, &callback);
    napi_value returnVal = nullptr;
    napi_call_function(env, nullptr, callback, RESULT_COUNT, &args[0], &returnVal);
    napi_delete_reference(env, param->jsStringRef);
    napi_delete_reference(env, param->callbackRef);
    napi_delete_async_work(env, param->asyncWork);
    delete param;
}

void NapiWebStorage::GetOriginUsageOrQuotaPromiseComplete(napi_env env, napi_status status, void *data)
{
    GetOriginUsageOrQuotaParam* param = (GetOriginUsageOrQuotaParam*)data;
    napi_value setResult[RESULT_COUNT] = {0};
    setResult[PARAMZERO] = GetErrorCodeValue(env, param->errCode);
    napi_create_uint32(env, static_cast<uint32_t>(param->retValue), &setResult[PARAMONE]);
    napi_value args[RESULT_COUNT] = {setResult[PARAMZERO], setResult[PARAMONE]};
    if (param->status == napi_ok) {
        napi_resolve_deferred(env, param->deferred, args[1]);
    } else {
        napi_reject_deferred(env, param->deferred, args[0]);
    }
    napi_delete_reference(env, param->jsStringRef);
    napi_delete_async_work(env, param->asyncWork);
    delete param;
}

napi_value NapiWebStorage::GetOriginUsageOrQuotaAsync(napi_env env,
    napi_value *argv, const std::string origin, bool isQuato)
{
    napi_value result = nullptr;
    napi_value resourceName = nullptr;

    GetOriginUsageOrQuotaParam *param = new GetOriginUsageOrQuotaParam {
        .asyncWork = nullptr,
        .deferred = nullptr,
        .isQuato = isQuato,
        .origin = origin,
        .retValue = -1,
        .jsStringRef = nullptr,
        .callbackRef = nullptr,
        .env = env,
    };
    napi_create_reference(env, argv[0], 1, &param->jsStringRef);
    napi_create_reference(env, argv[1], 1, &param->callbackRef);
    NAPI_CALL(env, napi_create_string_utf8(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, ExecuteGetOriginUsageOrQuota,
        GetOriginUsageOrQuotaComplete, (void *)param, &param->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, param->asyncWork));
    napi_get_undefined(env, &result);
    return result;
}

napi_value NapiWebStorage::GetOriginUsageOrQuotaPromise(napi_env env,
    napi_value *argv, const std::string origin, bool isQuato)
{
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_create_promise(env, &deferred, &promise);

    GetOriginUsageOrQuotaParam *param = new GetOriginUsageOrQuotaParam {
        .asyncWork = nullptr,
        .deferred = deferred,
        .isQuato = isQuato,
        .origin = origin,
        .retValue = -1,
        .jsStringRef = nullptr,
        .callbackRef = nullptr,
        .env = env,
    };
    napi_create_reference(env, argv[0], 1, &param->jsStringRef);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, nullptr, resourceName, ExecuteGetOriginUsageOrQuota,
        GetOriginUsageOrQuotaPromiseComplete, (void *)param, &param->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, param->asyncWork));
    return promise;
}

napi_value NapiWebStorage::JsGetOriginQuota(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = 1;
    size_t argcPromise = 1;
    size_t argcCallback = 2;
    napi_value argv[2] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    NAPI_ASSERT(env, argc == argcPromise || argc == argcCallback, "requires 1 or 2 parameter");
    napi_valuetype valueType = napi_null;
    napi_typeof(env, argv[0], &valueType);
    NAPI_ASSERT(env, valueType == napi_string, "type mismatch for parameter 1");
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &bufferSize);
    NAPI_ASSERT_BASE(env, bufferSize < MAX_WEB_STRING_LENGTH, "string length too large", retValue);
    char stringValue[bufferSize + 1];
    size_t jsStringLength = 0;
    napi_get_value_string_utf8(env, argv[0], stringValue, bufferSize + 1, &jsStringLength);
    NAPI_ASSERT_BASE(env, jsStringLength == bufferSize, "string length wrong", retValue);
    std::string origin(stringValue);
    if (argc == argcCallback) {
        valueType = napi_undefined;
        napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
        napi_typeof(env, argv[argcCallback - 1], &valueType);
        if (valueType == napi_function) {
            return GetOriginUsageOrQuotaAsync(env, argv, origin, true);
        }
    }
    return GetOriginUsageOrQuotaPromise(env, argv, origin, true);
}

napi_value NapiWebStorage::JsGetOriginUsage(napi_env env, napi_callback_info info)
{
    napi_value retValue = nullptr;
    size_t argc = 1;
    size_t argcPromise = 1;
    size_t argcCallback = 2;
    napi_value argv[2] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
    NAPI_ASSERT(env, argc == argcPromise || argc == argcCallback, "requires 1 or 2 parameter");
    napi_valuetype valueType = napi_null;
    napi_typeof(env, argv[0], &valueType);
    NAPI_ASSERT(env, valueType == napi_string, "type mismatch for parameter 1");
    size_t bufferSize = 0;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &bufferSize);
    NAPI_ASSERT_BASE(env, bufferSize < MAX_WEB_STRING_LENGTH, "string length too large", retValue);
    char stringValue[bufferSize + 1];
    size_t jsStringLength = 0;
    napi_get_value_string_utf8(env, argv[0], stringValue, bufferSize + 1, &jsStringLength);
    NAPI_ASSERT_BASE(env, jsStringLength == bufferSize, "string length wrong", retValue);
    std::string origin(stringValue);
    if (argc == argcCallback) {
        valueType = napi_undefined;
        napi_get_cb_info(env, info, &argc, argv, &retValue, nullptr);
        napi_typeof(env, argv[argcCallback - 1], &valueType);
        if (valueType == napi_function) {
            return GetOriginUsageOrQuotaAsync(env, argv, origin, false);
        }
    }
    return GetOriginUsageOrQuotaPromise(env, argv, origin, false);
}

napi_value NapiWebStorage::JsConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    return thisVar;
}
} // namespace OHOS