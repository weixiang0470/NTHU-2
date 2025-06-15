#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: ./tar.sh <studentID>"
    exit 1
fi

STUDENT_ID=$1
TAR_FILE="${STUDENT_ID}.tar.gz"

# 定义要压缩的文件和目录
FILES_TO_ARCHIVE=(
    "main.cpp"
    "Makefile"
    "run.sh"
    "testcases/"
    "ReadMe"
    "tar.sh"
)

# 使用 tar 的 --transform 选项添加 studentID/ 目录前缀
tar -czvf "$TAR_FILE" --transform "s|^|${STUDENT_ID}/|" "${FILES_TO_ARCHIVE[@]}"

echo "Submission package created: $TAR_FILE"
