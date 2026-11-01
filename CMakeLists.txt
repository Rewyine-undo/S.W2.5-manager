
cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0091 NEW)

project(SW25Manager LANGUAGES CXX)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==========================================
# Windows dependencies
# ==========================================

if(WIN32)

    # Static MSVC Runtime (/MT)
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )

    include(FetchContent)

    # wxWidgets
    set(wxBUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(wxBUILD_USE_STATIC_RUNTIME ON CACHE BOOL "" FORCE)
    set(wxBUILD_SAMPLES OFF CACHE STRING "" FORCE)
    set(wxBUILD_TESTS OFF CACHE STRING "" FORCE)

    FetchContent_Declare(
        wxWidgets
        GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
        GIT_TAG v3.2.6
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(wxWidgets)

    # nlohmann/json
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(nlohmann_json)

    set(WX_LIBS
        wx::core
        wx::base
        wx::richtext
    )

# ==========================================
# macOS dependencies
# ==========================================

else()

    find_package(
        wxWidgets REQUIRED
        COMPONENTS core base richtext
    )

    set(WX_LIBS ${wxWidgets_LIBRARIES})
    set(WX_INCLUDES ${wxWidgets_INCLUDE_DIRS})
    set(WX_DEFS ${wxWidgets_DEFINITIONS})

endif()

# ==========================================
# Source files
# ==========================================

file(GLOB SRC_FILES src/*.cpp)
file(GLOB HEADER_FILES include/*.h)

# Windows GUI application
if(WIN32)
    add_executable(
        SW25Manager WIN32
        ${SRC_FILES}
        ${HEADER_FILES}
    )
else()
    add_executable(
        SW25Manager
        ${SRC_FILES}
        ${HEADER_FILES}
    )
endif()

# Include directories
target_include_directories(
    SW25Manager
    PRIVATE
    include
    ${WX_INCLUDES}
)

# Compile definitions
target_compile_definitions(
    SW25Manager
    PRIVATE
    ${WX_DEFS}
)

# Link libraries
target_link_libraries(
    SW25Manager
    PRIVATE
    ${WX_LIBS}
)

# JSON library (Windows)
if(WIN32)
    target_link_libraries(
        SW25Manager
        PRIVATE
        nlohmann_json::nlohmann_json
    )
endif()
