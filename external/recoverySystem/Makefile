PROJECT_DIR := $(shell pwd)
CC = gcc
PROM = recoverySystem 
OBJ = recoverySystem.o \
    strlcpy.o \
    strlcat.o \
    bootloader.o \
    update_recv/update_recv.o \
    main.o

CFLAGS ?= -I$(PROJECT_DIR) -lz -lc -lpthread
$(PROM): $(OBJ)
	$(CC) -o $(PROM) $(OBJ) $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf $(OBJ) $(PROM)

install:
	sudo install -D -m 755 recovery -t /usr/bin/
