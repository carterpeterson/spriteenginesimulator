# setup
BUILD_DIR:=$(CURDIR)
CC=gcc
CFLAGS=-Wall -lpthread -g -I /opt/X11/include -L /opt/X11/lib/ -lX11 -lm
TARGET=server
DEBUG_DEFINES=-D DEBUG_PRINT

# building the servers stuff
SERVER_DIR:=$(BUILD_DIR)/se_server
SERVER_SOURCES:=server.c render.c socket.c
SERVER_SOURCES:=$(addprefix $(SERVER_DIR)/, $(SERVER_SOURCES))

all: server

server:
	$(CC) $(CFLAGS) -o $(TARGET) $(SERVER_SOURCES)

clean:
	$(RM) $(TARGET)
