cmake_minimum_required(VERSION 3.16)
project(CeskySyntax LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(cesky_transpiler
    src/main.cpp
    src/Lexer.cpp
    src/Translator.cpp
    src/CodeGenerator.cpp
    src/Executor.cpp
)

target_include_directories(cesky_transpiler PRIVATE include)

add_executable(cesky_editor
    src/tui_editor.cpp
)

target_include_directories(cesky_editor PRIVATE include)
