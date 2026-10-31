
cmake_minimum_required(VERSION 3.10)
project(SW25Manager)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# wxWidgetsを探す
find_package(wxWidgets REQUIRED COMPONENTS core base richtext)

# ソース・ヘッダファイルを収集
file(GLOB SRC_FILES src/*.cpp)
file(GLOB HEADER_FILES include/*.h)

# 実行ファイルの作成
add_executable(SW25Manager ${SRC_FILES} ${HEADER_FILES})

# インクルードディレクトリ
target_include_directories(
    SW25Manager
    PRIVATE
    include
    ${wxWidgets_INCLUDE_DIRS}
)

# wxWidgetsのコンパイル時定義
target_compile_definitions(
    SW25Manager
    PRIVATE
    ${wxWidgets_DEFINITIONS}
)

# wxWidgetsライブラリをリンク
target_link_libraries(
    SW25Manager
    PRIVATE
    ${wxWidgets_LIBRARIES}
)
