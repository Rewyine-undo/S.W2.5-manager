#!/bin/bash

# 変換対象ディレクトリ（例: src）
TARGET_DIR=$1

if [ -z "$TARGET_DIR" ]; then
  echo "Usage: $0 <directory>"
  exit 1
fi

# 拡張子の指定（必要に応じて追加）
EXTENSIONS=("*.cpp" "*.h")

for ext in "${EXTENSIONS[@]}"
do
  find "$TARGET_DIR" -type f -name "$ext" | while read file; do
    echo "Converting $file from Shift-JIS to UTF-8..."
    # 一時ファイルに変換して上書き
    iconv -f SHIFT-JIS -t UTF-8 "$file" > "${file}.utf8tmp" && mv "${file}.utf8tmp" "$file"
  done
done

echo "Conversion complete."
