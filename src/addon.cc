#include <napi.h>
#include "uuidv47.h"
#include <string>
#include <random>
#include <thread>
#include <cinttypes>

// Global key storage
static uuidv47_key_t global_key = { .k0 = 0, .k1 = 0 };
static bool key_set = false;

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


// Set global keys for simplified encoding/decoding
Napi::Boolean SetKeys(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2) {
        Napi::TypeError::New(env, "Usage: setKeys(key0, key1)").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    bool lossless;
    uint64_t k0 = info[0].As<Napi::BigInt>().Uint64Value(&lossless);
    uint64_t k1 = info[1].As<Napi::BigInt>().Uint64Value(&lossless);

    if (!lossless) {
        Napi::TypeError::New(env, "Key value does not fit into uint64_t").ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }

    global_key.k0 = k0;
    global_key.k1 = k1;
    key_set = true;

    return Napi::Boolean::New(env, true);
}

// Encode using global keys
Napi::String Encode(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (!key_set) {
        Napi::Error::New(env, "Keys not set. Call setKeys() first.").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Usage: encodeWithKeys(v7string)").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    Napi::String v7_napi_str = info[0].As<Napi::String>();
    std::string v7str = v7_napi_str.Utf8Value();

    uuid128_t v7;
    if (!uuid_parse(v7str.c_str(), &v7)) {
        Napi::Error::New(env, "Invalid UUIDv7").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    uuid128_t facade = uuidv47_encode_v4facade(v7, global_key);

    char facade_str[37];
    uuid_to_string_fast(facade, facade_str);
    return Napi::String::New(env, facade_str, 36);
}

// Decode using global keys
Napi::String Decode(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (!key_set) {
        Napi::Error::New(env, "Keys not set. Call setKeys() first.").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Usage: decodeWithKeys(facadeString)").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    Napi::String facade_napi_str = info[0].As<Napi::String>();
    std::string facadestr = facade_napi_str.Utf8Value();

    uuid128_t facade;
    if (!uuid_parse(facadestr.c_str(), &facade)) {
        Napi::Error::New(env, "Invalid UUID").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    uuid128_t v7 = uuidv47_decode_v4facade(facade, global_key);

    char v7_str[37];
    uuid_to_string_fast(v7, v7_str);
    return Napi::String::New(env, v7_str, 36);
}

// Check if keys are set
Napi::Boolean HasKeys(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, key_set);
}

Napi::Object Uuidv47Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "uuidParse"), Napi::Function::New(env, UuidParse));
    exports.Set(Napi::String::New(env, "setKeys"), Napi::Function::New(env, SetKeys));
    exports.Set(Napi::String::New(env, "encode"), Napi::Function::New(env, Encode));
    exports.Set(Napi::String::New(env, "decode"), Napi::Function::New(env, Decode));
    exports.Set(Napi::String::New(env, "hasKeys"), Napi::Function::New(env, HasKeys));
    return exports;
}

NODE_API_MODULE(uuidv47, Uuidv47Init)
