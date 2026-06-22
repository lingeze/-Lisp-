cmake_minimum_required(VERSION 3.24.2)

project(mini_lisp)
include(FetchContent)
FetchContent_Declare(replxx
    GIT_REPOSITORY https://github.com/AmokHuginnsson/replxx.git
    GIT_TAG release-0.0.4)
FetchContent_MakeAvailable(replxx)
aux_source_directory(src SOURCES)
add_executable(mini_lisp ${SOURCES})
target_link_libraries(mini_lisp replxx)
set_target_properties(
  mini_lisp
  PROPERTIES CXX_STANDARD 20
             CXX_STANDARD_REQUIRED ON
             RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
             RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin
             RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
if(MSVC)
  target_compile_options(mini_lisp PRIVATE /utf-8 /Zc:preprocessor)
endif()
if(MINGW)
  target_link_options(mini_lisp PRIVATE -static)
endif()
