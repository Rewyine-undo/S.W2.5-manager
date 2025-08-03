cmake_minimum_required(VERSION 3.10)
project(SW25Manager)

# C++17 を使う（必要に応じて変更）
set(CMAKE_CXX_STANDARD 17)

# ヘッダファイルの場所
include_directories(include)

# wxWidgets を探す（coreとbaseを使う）
find_package(wxWidgets REQUIRED COMPONENTS core base)

# wxWidgets の include / link を有効にする
include(${wxWidgets_USE_FILE})

# ソースファイルとヘッダファイルを収集
file(GLOB SRC_FILES src/*.cpp)
file(GLOB HEADER_FILES include/*.h)

# 実行ファイルを作成
add_executable(SW25Manager ${SRC_FILES} ${HEADER_FILES})

# wxWidgets ライブラリをリンク
target_link_libraries(SW25Manager ${wxWidgets_LIBRARIES})
