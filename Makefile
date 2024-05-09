# TARGET = phong_default
TARGET = lighting

CC = g++

# CFLAGS = -ggdb -Wall -std=c++17 -DPHONG_DEFAULT
CFLAGS = -ggdb -Wall -std=c++17

INCLUDE = -Iinclude
LIBS = -Llib -lraylib -lopengl32 -lgdi32 -lwinmm

SRC = lighting.cpp

OBJ = $(SRC:.cpp=.o)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

run:
	@$(TARGET)

.PHONY: run

