
project(glang)

add_executable(${PROJECT_NAME}
    main.c
    )

target_link_libraries(${PROJECT_NAME}
    parser
    scanner
    symbols
    utils
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/../include
    )

target_compile_options(${PROJECT_NAME}
    PRIVATE "-Wall" "-Wextra" "-g" "-D_DEBUGGING"
        "-I/usr/lib/llvm-7/include"
        "-D_GNU_SOURCE"
        "-D__STDC_CONSTANT_MACROS"
        "-D__STDC_FORMAT_MACROS"
        "-D__STDC_LIMIT_MACROS"
        "-L/usr/lib/llvm-7/lib"
        "-lLLVM-7"
        )

