#STATIC_LIBS = libncursesw.a libtinfo.a
LIBS = -lncursesw
CFLAGS = -Wall -Wpedantic -g
CC = gcc
OBJECTS = src/board.o src/canvas.o src/game.o src/main.o src/state.o src/ui/uiboard.o src/ui/uidialog.o src/ui/uilegend.o

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

go: $(OBJECTS)
	$(CC) $(OBJECTS) $(STATIC_LIBS) $(LIBS) -o $@ 

.PHONY: clean run

clean:
	-rm src/*.o src/ui/*.o
	-rm go

run: go
	./go
