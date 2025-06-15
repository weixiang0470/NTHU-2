#!/bin/bash

cd ../..

# 創建 tar 檔案
tar -zcvf HW3_grading2/student/X1136010/CS6135_HW3_X1136010.tar.gz HW3/

# 進入 HW3_grading 目錄並執行 grading 腳本
cd HW3_grading2
bash HW3_grading.sh

