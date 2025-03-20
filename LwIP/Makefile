CC = gcc
CFLAGS = -I./lwip/src/include -I./lwip/contrib/ports/unix/port/include
SRC = ./lwip/src/core/lwip.c ./lwip/src/api/api_lib.c ./main.c# 添加你需要的源文件
OBJ = $(SRC:.c=.o)
TARGET = lwip_example

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

