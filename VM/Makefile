PROJECT_ROOT_PATH = /home/zcm/AFVM/VM
VERSION = 1.0.0

SOURCE_DIR = ./src
ARCH_DIR = ./src/arch/AMD-V
INCLUDE_DIR = ./include/

CC = gcc
CFLAGS = -Wall
SOURCES = $(wildcard $(SOURCE_DIR)/*.c)
# SOURCES += $(wildcard $(ARCH_DIR)/*.c)
INCLUDE = -I$(INCLUDE_DIR)
OBJ = $(patsubst %.c, %.o, $(SOURCES))
TARGET = light-qemu

$(TARGET):$(OBJ)
	@mkdir -p output
	$(CC) -m32 $(OBJ) -o output/$(TARGET).$(VERSION)
	@rm -rf $(OBJ)

%.o:%.c
	$(CC) -m32 $(INCLUDE) -c $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@echo "clean..."
	rm -rf $(OBJ) $(TARGET) output
