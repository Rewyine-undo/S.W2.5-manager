cmake_minimum_required(VERSION 3.10)
project(SW25Manager)

# C++17を使う
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# wxWidgetsを探す
find_package(wxWidgets REQUIRED COMPONENTS core base)

# ソース・ヘッダファイルを収集
file(GLOB SRC_FILES src/*.cpp)
file(GLOB HEADER_FILES include/*.h)

# ==========================================
# 実行ファイル作成
# ==========================================
if(WIN32)
    add_executable(
        SW25Manager
        WIN32
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

# ==========================================
# インクルードディレクトリ
# ==========================================
target_include_directories(
    SW25Manager
    PRIVATE
    include
    ${WX_INCLUDES}
)

# ==========================================
# コンパイル定義
# ==========================================
target_compile_definitions(
    SW25Manager
    PRIVATE
    ${WX_DEFS}
)

# ==========================================
# ライブラリのリンク
# ==========================================
target_link_libraries(
    SW25Manager
    PRIVATE
    ${wxWidgets_LIBRARIES}
    /opt/homebrew/lib/libwx_osx_cocoau_richtext-3.2.dylib
)