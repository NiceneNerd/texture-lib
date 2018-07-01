/**
* @copyright 2018 - Max Bebök
* @author Max Bebök
* @license GNU-GPLv3 - see the "LICENSE" file in the root directory
*/

#include "../include/main_header.h"

void buffer_delete_callback(char* data, void* vector)
{
    delete reinterpret_cast<std::vector<u8> *> (vector);
}

u8 getParamU8(v8::Local<v8::Object>& params, std::string name)
{
    v8::Local<v8::String> paramName = Nan::New<v8::String>(name).ToLocalChecked();
    return (u8)Nan::Get(params, paramName).ToLocalChecked()->NumberValue();
}

u8 getParamU32(v8::Local<v8::Object>& params, std::string name)
{
    v8::Local<v8::String> paramName = Nan::New<v8::String>(name).ToLocalChecked();
    return (u32)Nan::Get(params, paramName).ToLocalChecked()->NumberValue();
}

NAN_METHOD(deswizzle)
{
    // check arguments
    if(info.Length() != 2) {
        return Nan::ThrowError(Nan::New("FTEX-Swizzler::encode - expected arguments bufferIn, params").ToLocalChecked());
    }

    auto bufferInObj = info[0]->ToObject();
    u8* bufferIn = (u8*)node::Buffer::Data(bufferInObj);
    auto bufferInSize = node::Buffer::Length(bufferInObj);

    auto params = info[1]->ToObject();

    auto sizeX    = getParamU32(params, "sizeX");
    auto sizeY    = getParamU32(params, "sizeY");
    auto sizeZ    = getParamU32(params, "sizeZ");
    auto index    = getParamU32(params, "index");
    auto format   = getParamU32(params, "format");
    auto tileMode = getParamU32(params, "tileMode");
    auto swizzle  = getParamU32(params, "swizzle");
    auto pitch    = getParamU32(params, "pitch");
    auto bpp      = getParamU8(params, "bpp");

    std::vector<u8> *bufferOut = new std::vector<u8>(bufferInSize);

    auto ftexSwizzler = FTEX_Swizzler();
    ftexSwizzler.deswizzle(sizeX, sizeY, sizeZ, index, format, tileMode, swizzle, pitch, bpp, bufferIn, (u8*)bufferOut->data(), bufferInSize);

    info.GetReturnValue().Set(Nan::NewBuffer((char*)bufferOut->data(), bufferOut->size(), buffer_delete_callback, bufferOut).ToLocalChecked());
}

NAN_MODULE_INIT(Initialize)
{
    NAN_EXPORT(target, deswizzle);
}

NODE_MODULE(png, Initialize);
