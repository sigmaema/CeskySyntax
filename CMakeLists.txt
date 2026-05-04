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

# Optional Dear ImGui GUI editor (off by default)
option(BUILD_GUI "Build Dear ImGui GUI editor" OFF)
if(BUILD_GUI)
    set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui")
    set(GLFW_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw")
    set(IMGUI_TEXTEDITOR_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/ImGuiColorTextEdit")

    # Prefer vendored GLFW for deterministic builds.
    if(EXISTS "${GLFW_DIR}/CMakeLists.txt")
        set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
        add_subdirectory("${GLFW_DIR}" "${CMAKE_BINARY_DIR}/glfw")
    endif()

    # Build vendored Dear ImGui as a static library when available.
    if(EXISTS "${IMGUI_DIR}/imgui.h")
        add_library(imgui_vendor STATIC
            "${IMGUI_DIR}/imgui.cpp"
            "${IMGUI_DIR}/imgui_draw.cpp"
            "${IMGUI_DIR}/imgui_tables.cpp"
            "${IMGUI_DIR}/imgui_widgets.cpp"
            "${IMGUI_DIR}/imgui_demo.cpp"
            "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
            "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
            "${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp"
        )
        target_include_directories(imgui_vendor PUBLIC
            "${IMGUI_DIR}"
            "${IMGUI_DIR}/backends"
            "${IMGUI_DIR}/misc/cpp"
            "${GLFW_DIR}/include"
        )
    endif()

    add_executable(cesky_gui
        src/gui_editor.cpp
        src/ImGuiSimpleEditor.cpp
    )
    target_include_directories(cesky_gui PRIVATE include)

    # Link to vendored or user-provided GLFW/ImGui libraries.
    if(TARGET glfw)
        target_link_libraries(cesky_gui PRIVATE glfw)
    endif()

    find_package(OpenGL REQUIRED)
    target_link_libraries(cesky_gui PRIVATE OpenGL::GL)

    if(TARGET imgui_vendor)
        target_link_libraries(cesky_gui PRIVATE imgui_vendor)
        target_include_directories(cesky_gui PRIVATE
            "${IMGUI_DIR}"
            "${IMGUI_DIR}/backends"
            "${IMGUI_DIR}/misc/cpp"
        )
    endif()

    # Optionally link to an external `imgui` target if provided by the user
    if(TARGET imgui)
        target_link_libraries(cesky_gui PRIVATE imgui)
    endif()

    # Optional richer editor component (vendored): ImGuiColorTextEdit
    if(EXISTS "${IMGUI_TEXTEDITOR_DIR}/TextEditor.cpp" AND EXISTS "${IMGUI_TEXTEDITOR_DIR}/TextEditor.h")
        target_sources(cesky_gui PRIVATE "${IMGUI_TEXTEDITOR_DIR}/TextEditor.cpp")
        target_include_directories(cesky_gui PRIVATE "${IMGUI_TEXTEDITOR_DIR}")
        target_compile_definitions(cesky_gui PRIVATE CESKYSYNTAX_HAS_TEXTEDITOR=1)
    endif()
endif()
