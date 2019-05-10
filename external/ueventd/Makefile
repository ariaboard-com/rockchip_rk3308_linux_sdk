PROJECT_DIR := $(shell pwd)
CC = gcc
PROM = ueventd
OBJ = devices.o \
util.o \
ueventd.o \
ueventd_parser.o \
parser.o

CFLAGS ?= -I$(PROJECT_DIR) -I/usr/include/ -I$(PROJECT_DIR)/include -D_GNU_SOURCE
$(PROM): $(OBJ)
	$(CC) -o $(PROM) $(OBJ) $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf $(OBJ) $(PROM)

install:
	sudo install -D -m 755 $(PROM) -t /usr/bin/
