#include <napi.h>
#include "uuidv47.h"
#include <string>
#include <random>
#include <chrono>
#include <time.h>
#include <atomic>
#include <thread>
#include <cinttypes>

// Thread-local high-performance random number generator
thread_local struct {
    std::mt19937_64 gen;
    bool initialized = false;

    void init() {
        if (!initialized) {
            std::random_device rd;
            // Use multiple entropy sources for better seeding
            uint64_t seed = rd();
            seed ^= std::chrono::high_resolution_clock::now().time_since_epoch().count();
            seed ^= std::hash<std::thread::id>{}(std::this_thread::get_id());
            gen.seed(seed);
            initialized = true;
        }
    }

    inline uint64_t next() {
        if (!initialized) init();
        return gen();
    }
} tl_rng;

// Fast timestamp cache with atomic operations
static std::atomic<uint64_t> cached_timestamp{0};
static std::atomic<uint64_t> cache_time{0};
static constexpr uint64_t CACHE_DURATION_NS = 1000; // 1 microsecond cache

static inline uint64_t get_milliseconds_fast() {
    // Use cached timestamp if recent enough
    uint64_t now_ns = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    uint64_t last_cache_time = cache_time.load(std::memory_order_relaxed);

    if (now_ns - last_cache_time < CACHE_DURATION_NS) {
        return cached_timestamp.load(std::memory_order_relaxed);
    }

    // Update cache
    uint64_t ms = now_ns / 1000000;
    cached_timestamp.store(ms, std::memory_order_relaxed);
    cache_time.store(now_ns, std::memory_order_relaxed);
    return ms;
}

// Optimized UUID to string conversion with pre-allocated buffer
static inline void uuid_to_string_fast(const uuid128_t &uuid, char* buf) {
    uuid_format(&uuid, buf);
}

// High-performance UUIDv7 generation
static inline uuid128_t uuidv7_generate_fast() {
    uuid128_t out;

    // Fast timestamp - use cached value when possible
    uint64_t ms = get_milliseconds_fast();

    // Write timestamp in big-endian format (optimized)
    out.b[0] = (uint8_t)(ms >> 40);
    out.b[1] = (uint8_t)(ms >> 32);
    out.b[2] = (uint8_t)(ms >> 24);
    out.b[3] = (uint8_t)(ms >> 16);
    out.b[4] = (uint8_t)(ms >> 8);
    out.b[5] = (uint8_t)(ms);

    // Generate random bytes efficiently using thread-local generator
    uint64_t rand1 = tl_rng.next();
    uint64_t rand2 = tl_rng.next();

    // Set version (7) and random bits for byte 6
    out.b[6] = (7 << 4) | (uint8_t)(rand1 & 0x0F);

    // Fill remaining bytes with random data
    out.b[7] = (uint8_t)(rand1 >> 8);
    out.b[8] = (uint8_t)(rand1 >> 16);
    out.b[9] = (uint8_t)(rand1 >> 24);
    out.b[10] = (uint8_t)(rand1 >> 32);
    out.b[11] = (uint8_t)(rand1 >> 40);
    out.b[12] = (uint8_t)(rand1 >> 48);
    out.b[13] = (uint8_t)(rand1 >> 56);

    out.b[14] = (uint8_t)(rand2);
    out.b[15] = (uint8_t)(rand2 >> 8);

    // Set variant bits (RFC 4122)
    out.b[8] = (out.b[8] & 0x3F) | 0x80;

    return out;
}

// Fast key generation using thread-local RNG
static inline uuidv47_key_t generate_key_fast() {
    uuidv47_key_t key;
    key.k0 = tl_rng.next();
    key.k1 = tl_rng.next();
    return key;
}


Napi::Object GenerateUUIDv47(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // Use optimized generation functions
    uuid128_t v7 = uuidv7_generate_fast();
    uuidv47_key_t key = generate_key_fast();
    uuid128_t facade = uuidv47_encode_v4facade(v7, key);

    // Pre-allocate string buffers to avoid heap allocations
    char v7_str[37], facade_str[37];
    uuid_to_string_fast(v7, v7_str);
    uuid_to_string_fast(facade, facade_str);

    // Pre-allocate key strings to avoid std::to_string overhead
    char k0_str[21], k1_str[21]; // Max digits for uint64_t is 20 + null terminator
    snprintf(k0_str, sizeof(k0_str), "%" PRIu64, key.k0);
    snprintf(k1_str, sizeof(k1_str), "%" PRIu64, key.k1);

    // Create result object with optimized string creation
    Napi::Object result = Napi::Object::New(env);
    result.Set("v7", Napi::String::New(env, v7_str, 36)); // Known length
    result.Set("v47", Napi::String::New(env, facade_str, 36)); // Known length
    result.Set("k0", Napi::String::New(env, k0_str));
    result.Set("k1", Napi::String::New(env, k1_str));

    return result;
}

Napi::String EncodeFacade(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 3) {
        Napi::TypeError::New(env, "Usage: encodeFacade(v7string, key0, key1)").ThrowAsJavaScriptException();
        return Napi::String::New(env, "");
    }

    bool lossless;

    // Avoid std::string allocation - get C string directly
    Napi::String v7_napi_str = info[0].As<Napi::String>();
    std::string v7str = v7_napi_str.Utf8Value(); // More efficient than As<String>()
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

    // Use stack-allocated buffer for result
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

    // Avoid std::string allocation - get C string directly
    Napi::String facade_napi_str = info[0].As<Napi::String>();
    std::string facadestr = facade_napi_str.Utf8Value(); // More efficient than As<String>()
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

    // Use stack-allocated buffer for result
    char v7_str[37];
    uuid_to_string_fast(v7, v7_str);
    return Napi::String::New(env, v7_str, 36);
}

Napi::Object Uuidv47Init(Napi::Env env, Napi::Object exports) {
    exports.Set(Napi::String::New(env, "uuidv47"), Napi::Function::New(env, GenerateUUIDv47));
    exports.Set(Napi::String::New(env, "encodeFacade"), Napi::Function::New(env, EncodeFacade));
    exports.Set(Napi::String::New(env, "decodeFacade"), Napi::Function::New(env, DecodeFacade));
    return exports;
}

NODE_API_MODULE(uuidv47, Uuidv47Init)
