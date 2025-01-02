CXX = g++
SRC = main.cpp
TARGET = main
FLAGS = -Wall -Wextra -Wconversion -Werror

all:
	@$(CXX) $(FLAGS) $(SRC) -g -o $(TARGET)
	@./$(TARGET)
