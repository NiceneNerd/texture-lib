{
    "targets": [
    {
        "target_name": "texture",
        "sources": [
            "src/main.cpp",
            "src/swizzle/ftex_swizzler.cpp"
        ],
        "cflags": [
          "-std=c++11",
          '-O3'
        ],
        'cflags_cc': [ '-fexceptions' ]
    }
    ]
}
