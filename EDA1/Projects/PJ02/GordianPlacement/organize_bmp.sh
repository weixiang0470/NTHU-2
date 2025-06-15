#!/bin/bash

# 定義 Result 資料夾相對於當前位置的路徑
RESULT_DIR="Result"

# --- 修改部分：從命令列參數獲取資料夾名稱 ---
# 檢查是否有提供參數 ($# 表示參數的數量)
if [ $# -eq 0 ]; then
    echo "使用方法: $0 <新資料夾名稱>"
    echo "範例: $0 case1"
    exit 1 # 如果沒有提供參數，顯示使用方法並退出
fi

# 將第一個命令列參數 ($1) 設定為新的資料夾名稱
NEW_FOLDER_NAME="$1"
# --- 修改部分結束 ---

# 檢查 Result 資料夾是否存在
if [ ! -d "$RESULT_DIR" ]; then
    echo "錯誤：在當前位置找不到 '$RESULT_DIR' 資料夾。"
    echo "請確保在執行腳本的目錄下有名為 'Result' 的資料夾。"
    exit 1
fi

# 構建新資料夾的完整路徑 (在 Result 資料夾下)
TARGET_DIR="$RESULT_DIR/$NEW_FOLDER_NAME"

# 檢查目標資料夾是否已經存在
if [ -d "$TARGET_DIR" ]; then
    echo "警告：資料夾 '$TARGET_DIR' 已經存在。"
    echo "將把 .bmp 檔案移動到這個現有的資料夾中。"
else
    # 建立新的資料夾
    mkdir "$TARGET_DIR"
    # 檢查資料夾是否建立成功
    if [ $? -ne 0 ]; then
        echo "錯誤：無法建立資料夾 '$TARGET_DIR'。"
        exit 1
    fi
    echo "已建立資料夾：'$TARGET_DIR'"
fi

# 使用 shopt -s nullglob 確保在沒有匹配檔案時，*.bmp 不會被當作字串處理
shopt -s nullglob
# 將當前資料夾中所有 .bmp 檔案的路徑存入陣列
bmp_files=(*.bmp)
# 關閉 nullglob 選項
shopt -u nullglob

# 檢查是否有找到 .bmp 檔案
if [ ${#bmp_files[@]} -eq 0 ]; then
    echo "在當前資料夾找不到任何 .bmp 檔案。"
else
    echo "正在移動 .bmp 檔案到 '$TARGET_DIR'..."
    # 將找到的所有 .bmp 檔案移動到目標資料夾
    mv -- "${bmp_files[@]}" "$TARGET_DIR"
    # 檢查移動命令是否成功
    if [ $? -ne 0 ]; then
        echo "警告：移動檔案時可能發生錯誤。"
    else
        echo "已成功移動 ${#bmp_files[@]} 個 .bmp 檔案到 '$TARGET_DIR'。"
    fi
fi

echo "腳本執行完成。"