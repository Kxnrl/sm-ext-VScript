add_library(VScript SHARED
    ${CMAKE_CURRENT_LIST_DIR}/extension.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils.cpp
)

target_include_directories(VScript PUBLIC
    ./
)

if (MSVC)
    target_compile_options(VScript PRIVATE /wd4091)
endif()

if (NOT WIN32)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(VScript PRIVATE -Wno-return-type)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(VScript PRIVATE -Wno-null-dereference)
    endif()
endif()

target_link_libraries(VScript PUBLIC amsdk)
set_target_properties(VScript PROPERTIES PREFIX "" SUFFIX ".ext.2.${HL2SDK_ENGINE_NAME}${HL2SDK_LIB_SHARED_EXT}")