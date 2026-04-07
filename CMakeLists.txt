cmake_minimum_required(VERSION 3.14)

project(humancw
    VERSION 0.1.0
    DESCRIPTION "Continuous Wave Encoder/Decoder"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# ---------------------------------------------------------------------------
# FTXUI – Terminal UI library
# ---------------------------------------------------------------------------
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG        v5.0.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(ftxui)

# ---------------------------------------------------------------------------
# PortAudio – Cross-platform audio I/O library
# ---------------------------------------------------------------------------
FetchContent_Declare(
    portaudio
    GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
    GIT_TAG        v19.7.0
    GIT_SHALLOW    TRUE
)
set(PA_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(PA_BUILD_STATIC ON  CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(portaudio)

# ---------------------------------------------------------------------------
# humancw executable
# ---------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
    ${portaudio_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
    portaudio_static
)
