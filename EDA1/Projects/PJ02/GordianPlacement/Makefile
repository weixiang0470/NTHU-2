#---------------------------------------------------
# Makefile for Gordian_Placement (含 EasyBMP)
#---------------------------------------------------

# --- 編譯器設定 ---
CXX       := g++
CXXFLAGS  := -std=c++14 -O2 -Wall -g \
			 -Ithird_party/armadillo/include \
			 -I/usr/include/armadillo
# LDFLAGS 需要同時包含 -L 搜尋路徑 和 -rpath 執行時路徑
LDFLAGS := -Lthird_party/armadillo/lib64 -Lthird_party/openblas/lib \
           -Wl,-rpath,$(PWD)/third_party/armadillo/lib64 \
           -Wl,-rpath,$(PWD)/third_party/openblas/lib
           # 注意：$(PWD) 會在 make 執行時擴展為當前目錄的絕對路徑

# LDFLAGS :=  -Lthird_party/armadillo/lib64 -Lthird_party/openblas/lib

LDLIBS := $(LDFLAGS) -larmadillo -lopenblas -lgfortran -lm
# LDLIBS := $(LDFLAGS) -llapack -lblas -lgfortran -lm
# --- 專案原始碼清單 ---
SRCS      := main.cpp \
             EasyBMP.cpp \
             EasyBMP_Geometry.cpp \
             Gordian.cpp

# --- 自動產生物件檔列表 (.o) ---
OBJS      := $(SRCS:.cpp=.o)

# --- 最終可執行檔名稱 ---
TARGET    := GordianPlacement

#---------------------------------------------------
# 1) 預設目標：編譯所有
#---------------------------------------------------
all: $(TARGET)

#---------------------------------------------------
# 2) 連結：把所有 .o 串起來
#---------------------------------------------------
$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDLIBS)

#---------------------------------------------------
# 3) 編譯規則：.cpp → .o
#---------------------------------------------------
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

#---------------------------------------------------
# 4) 清理中間檔與執行檔
#---------------------------------------------------
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)

#---------------------------------------------------
# 5) 檢查變數（debug 用）
#---------------------------------------------------
.PHONY: info
info:
	@echo "CXX      = $(CXX)"
	@echo "CXXFLAGS = $(CXXFLAGS)"
	# @echo "LDFLAGS  = $(LDFLAGS)"
	@echo "LDLIBS   = $(LDLIBS)"
	@echo "SRCS     = $(SRCS)"
	@echo "OBJS     = $(OBJS)"
	@echo "TARGET   = $(TARGET)"



# # 獲取當前目錄的絕對路徑
# PROJECT_DIR=$(pwd)

# # 設定 LD_LIBRARY_PATH，將您的函式庫目錄加入到搜尋路徑中
# # 注意：需要將 armadillo 和 openblas 的 lib 目錄都加進去
# export LD_LIBRARY_PATH=$PROJECT_DIR/third_party/armadillo/lib64:$PROJECT_DIR/third_party/openblas/lib:$LD_LIBRARY_PATH

# # 現在再執行您的程式
# ./GordianPlacement