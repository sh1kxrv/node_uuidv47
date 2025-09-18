{
  "targets": [
    {
      "target_name": "uuidv47",
      "sources": [
        "src/addon.cc",
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      "defines": [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
      "cflags_c": [ "-std=c11 -O3 -march=native" ],
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions -O3 -march=native" ],
    }
  ]
}
