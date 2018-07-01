/**
* @copyright 2018 - Max Bebök
* @author Max Bebök
* @license GNU-GPLv3 - see the "LICENSE" file in the root directory
*/

#include "../include/main_header.h"

u32 getParamU32(napi_env &env, napi_value &params, const char* name)
{
    napi_value objNapiVal;
    u32 objVal = 0;
    napi_get_named_property(env, params, name, &objNapiVal);
    napi_get_value_uint32(env, objNapiVal, &objVal);
    return objVal;
}

u8 getParamU8(napi_env &env, napi_value &params, const char* name)
{
    napi_value objNapiVal;
    u32 objVal = 0;
    napi_get_named_property(env, params, name, &objNapiVal);
    napi_get_value_uint32(env, objNapiVal, &objVal);
    return (u8)objVal;
}

napi_value FTEX_Deswizzle(napi_env env, napi_callback_info info) 
{
    size_t argc = 2;
    napi_value argv[2];

    if(napi_get_cb_info(env, info, &argc, argv, NULL, NULL) != napi_ok)
    {
        napi_throw_error(env, NULL, "Failed to parse arguments");
        return nullptr;
    }

    int32_t bufferSizeLimit = -1;
    u8* bufferIn = nullptr;
    size_t bufferInSize = 0;

    if(napi_get_buffer_info(env, argv[0], (void**)&bufferIn, &bufferInSize) != napi_ok)
    {
        napi_throw_error(env, NULL, "Invalid Buffer was passed as argument");
        return nullptr;
    }

    auto params = argv[0];
    auto sizeX    = getParamU32(env, params, "sizeX");
    auto sizeY    = getParamU32(env, params, "sizeY");
    auto sizeZ    = getParamU32(env, params, "sizeZ");
    auto index    = getParamU32(env, params, "index");
    auto format   = getParamU32(env, params, "format");
    auto tileMode = getParamU32(env, params, "tileMode");
    auto swizzle  = getParamU32(env, params, "swizzle");
    auto pitch    = getParamU32(env, params, "pitch");
    auto bpp      = getParamU8(env, params, "bpp");

    auto bufferOut = std::vector<u8>(bufferInSize);

    auto ftexSwizzler = FTEX_Swizzler();
    ftexSwizzler.deswizzle(sizeX, sizeY, sizeZ, index, format, tileMode, swizzle, pitch, bpp, bufferIn, (u8*)bufferOut.data(), bufferInSize);

    napi_value resultBuffer;
    void* createdBuffer;

    if(napi_create_buffer_copy(env, bufferInSize, (u8*)bufferOut.data(), &createdBuffer, &resultBuffer) != napi_ok)
    {
        napi_throw_error(env, NULL, "Unable to create Buffer");
        return nullptr;
    }

    return resultBuffer;
}

napi_value Init(napi_env env, napi_value exports) 
{
    napi_value fnEncode;
    if(napi_create_function(env, NULL, 0, FTEX_Deswizzle, NULL, &fnEncode) != napi_ok) 
        napi_throw_error(env, NULL, "Unable to wrap native function");

    if(napi_set_named_property(env, exports, "deswizzle", fnEncode) != napi_ok) 
        napi_throw_error(env, NULL, "Unable to populate exports");

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)