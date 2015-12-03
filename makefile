# setup
BUILD_DIR:=$(CURDIR)
CC=gcc
CFLAGS=-Wall -lpthread -g -I /opt/X11/include -L /opt/X11/lib/ -lX11 -lm -O3 -framework GLUT -framework OpenGL
TARGET=server
DEBUG_DEFINES=-D DEBUG_PRINT

# building the servers stuff
SERVER_DIR:=$(BUILD_DIR)/se_server
SERVER_SOURCES:=server.c render.c socket.c sprite_engine.c input.c
SERVER_SOURCES:=$(addprefix $(SERVER_DIR)/, $(SERVER_SOURCES))

all: server

server: $(SERVER_SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SERVER_SOURCES)

clean:
	$(RM) $(TARGET)
