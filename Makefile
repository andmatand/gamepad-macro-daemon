CC=gcc
CFLAGS=-Wall `sdl2-config --cflags`
LIBS=`sdl2-config --libs`
TARGET=gamepad-macro-daemon

OBJ=main.o

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $(TARGET)
clean:
	rm -f $(TARGET) $(OBJ)
