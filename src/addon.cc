#include <napi.h>
#include "uuidv47.h"
#include <string>
#include <random>
#include <chrono>
#include <time.h>

static uint64_t get_milliseconds(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return ((tp.tv_sec * 1000) + (tp.tv_nsec / 1000000));
}

static std::string uuid_to_string(const uuid128_t &uuid) {
    char buf[37];
    uuid_format(&uuid, buf);
    return std::string(buf);
}

static uuid128_t uuidv7_generate() {
    uuid128_t out;

    uint64_t ms = get_milliseconds();

    out.b[0] = (uint8_t)((ms >> 40) & 0xFF);
    out.b[1] = (uint8_t)((ms >> 32) & 0xFF);
    out.b[2] = (uint8_t)((ms >> 24) & 0xFF);
    out.b[3] = (uint8_t)((ms >> 16) & 0xFF);
    out.b[4] = (uint8_t)((ms >> 8) & 0xFF);
    out.b[5] = (uint8_t)(ms & 0xFF);

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint8_t> dist(0, 255);

    out.b[6] = (7 << 4) | (dist(gen) & 0x0F);

    for (int i = 7; i < 16; i++) {
        out.b[i] = dist(gen);
    }

    out.b[8] &= 0x3F;
    out.b[8] |= 0x80;

    return out;
}


Napi::Object GenerateUUIDv47(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    uuid128_t v7 = uuidv7_generate();

    std::random_device rd;
    uuidv47_key_t key;
    key.k0 = ((uint64_t)rd() << 32) ^ rd();
    key.k1 = ((uint64_t)rd() << 32) ^ rd();

    uuid128_t facade = uuidv47_encode_v4facade(v7, key);

    char v7_str[37], facade_str[37];
    uuid_format(&v7, v7_str);
    uuid_format(&facade, facade_str);

    Napi::Object result = Napi::Object::New(env);
    result.Set("v7", Napi::String::New(env, v7_str));
    result.Set("v47", Napi::String::New(env, facade_str));
    result.Set("k0", Napi::String::New(env, std::to_string(key.k0)));
    result.Set("k1", Napi::String::New(env, std::to_string(key.k1)));

    return result;
}

Napi::String EncodeFacade(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 3) {
        Napi::TypeError::New(env, "Usage: encodeFacade(v7string, key0, key1)").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    bool lossless;

    std::string v7str = info[0].As<Napi::String>();
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

    return Napi::String::New(env, uuid_to_string(facade));
}

Napi::String DecodeFacade(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 3) {
        Napi::TypeError::New(env, "Usage: decodeFacade(facadeString, key0, key1)").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    bool lossless;

    std::string facadestr = info[0].As<Napi::String>();
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

    return Napi::String::New(env, uuid_to_string(v7));
}

Napi::Object Uuidv47Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "uuidv47"), Napi::Function::New(env, GenerateUUIDv47));
    exports.Set(Napi::String::New(env, "encodeFacade"), Napi::Function::New(env, EncodeFacade));
    exports.Set(Napi::String::New(env, "decodeFacade"), Napi::Function::New(env, DecodeFacade));
    return exports;
}

NODE_API_MODULE(uuidv47, Uuidv47Init)
