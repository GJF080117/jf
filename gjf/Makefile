# ============================================================
# Makefile  《剑指苍穹》单机版文字 MUD 游戏
# 用法（Linux 虚拟机）：
#   make         编译生成可执行文件 mud
#   make run     编译并运行
#   make clean   清理编译产物
# ============================================================
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET   = mud

SRCS = main.cpp item.cpp creature.cpp room.cpp world.cpp game.cpp
OBJS = $(SRCS:.cpp=.o)
HDRS = types.h item.h creature.h room.h world.h game.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) save.dat

.PHONY: all run clean
