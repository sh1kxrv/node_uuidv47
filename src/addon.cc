#include <napi.h>
#include "uuidv47.h"
#include <string>
#include <random>
#include <thread>
#include <cinttypes>

static inline void uuid_to_string_fast(const uuid128_t &uuid, char* buf) {
    uuid_format(&uuid, buf);
}

Napi::Boolean UuidParse(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Usage: uuidParse(uuidString)").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    Napi::String uuid_napi_str = info[0].As<Napi::String>();
    std::string uuidstr = uuid_napi_str.Utf8Value();

    uuid128_t uuid;
    bool result = uuid_parse(uuidstr.c_str(), &uuid);

    return Napi::Boolean::New(env, result);
}

Napi::String EncodeFacade(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 3) {
        Napi::TypeError::New(env, "Usage: encodeFacade(v7string, key0, key1)").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    bool lossless;

    Napi::String v7_napi_str = info[0].As<Napi::String>();
    std::string v7str = v7_napi_str.Utf8Value();
    uint64_t k0 = info[1].As<Napi::BigInt>().Uint64Value(&lossless);
    uint64_t k1 = info[2].As<Napi::BigInt>().Uint64Value(&lossless);

    if (!lossless) {
        Napi::TypeError::New(env, "Key value does not fit into uint64_t").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    uuid128_t v7;
    if (!uuid_parse(v7str.c_str(), &v7)) {
        Napi::Error::New(env, "Invalid UUIDv7").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    uuidv47_key_t key = { .k0 = k0, .k1 = k1 };
    uuid128_t facade = uuidv47_encode_v4facade(v7, key);

    char facade_str[37];
    uuid_to_string_fast(facade, facade_str);
    return Napi::String::New(env, facade_str, 36);
}

Napi::String DecodeFacade(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 3) {
        Napi::TypeError::New(env, "Usage: decodeFacade(facadeString, key0, key1)").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    bool lossless;

    Napi::String facade_napi_str = info[0].As<Napi::String>();
    std::string facadestr = facade_napi_str.Utf8Value();
    uint64_t k0 = info[1].As<Napi::BigInt>().Uint64Value(&lossless);
    uint64_t k1 = info[2].As<Napi::BigInt>().Uint64Value(&lossless);

    if (!lossless) {
        Napi::TypeError::New(env, "Key value does not fit into uint64_t").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    uuid128_t facade;
    if (!uuid_parse(facadestr.c_str(), &facade)) {
        Napi::Error::New(env, "Invalid UUID").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    uuidv47_key_t key = { .k0 = k0, .k1 = k1 };
    uuid128_t v7 = uuidv47_decode_v4facade(facade, key);

    char v7_str[37];
    uuid_to_string_fast(v7, v7_str);
    return Napi::String::New(env, v7_str, 36);
}

Napi::Object Uuidv47Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "uuidParse"), Napi::Function::New(env, UuidParse));
    exports.Set(Napi::String::New(env, "encodeFacade"), Napi::Function::New(env, EncodeFacade));
    exports.Set(Napi::String::New(env, "decodeFacade"), Napi::Function::New(env, DecodeFacade));
    return exports;
}

NODE_API_MODULE(uuidv47, Uuidv47Init)
