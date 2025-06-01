# PROJECT_ROOT_PATH = /home/zcm/AFVM
# VERSION = 1.0.0

# SOURCE_DIR = ./src
# ARCH_DIR = ./src/arch/AMD-V
# INCLUDE_DIR = ./include/arch/AMD

# CC = gcc
# CFLAGS = -Wall -g
# SOURCES = $(wildcard $(SOURCE_DIR)/*.c)
# SOURCES += $(wildcard $(ARCH_DIR)/*.c)
# INCLUDE = -I$(INCLUDE_DIR)
# OBJ = $(patsubst %.c, %.o, $(SOURCES))
# TARGET = main

# $(TARGET):$(OBJ)
# 	@mkdir -p output
# 	$(CC) -m32 $(CFLAGS) $(OBJ) -o output/$(TARGET).$(VERSION)
# 	@rm -rf $(OBJ)

# %.o:%.c
# 	$(CC) -m32 $(INCLUDE) -c $(CFLAGS) $< -o $@

# .PHONY: clean
# clean:
# 	@echo "clean..."
# 	rm -rf $(OBJ) $(TARGET) output

PROJECT_ROOT_PATH = /home/zcm/AFVM/
VERSION = 1.0.0

all: lib main

lib:
	$(MAKE) -C src/arch/AMD-V/
	$(MAKE) -C lib/

main:
	$(MAKE) -C src/

.PHONY: lib main clean
clean:
	@echo "clean..."
	rm -rf $(OBJ) $(TARGET) output
