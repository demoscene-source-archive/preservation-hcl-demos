cmake_minimum_required(VERSION 3.15)
project(hcl_unpack LANGUAGES C)

add_executable(hcl_unpack hcl_unpack.c)
set_target_properties(hcl_unpack PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin$<0:>"
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

if(MSVC)
    target_compile_options(hcl_unpack PRIVATE /W4 /WX)
else()
    target_compile_options(hcl_unpack PRIVATE -Wall -Wextra -Werror)
endif()
