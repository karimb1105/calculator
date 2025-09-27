CC = gcc
CFLAGS = $(shell pkg-config --cflags gtk+-3.0) -Wno-deprecated-declarations
LIBS = $(shell pkg-config --libs gtk+-3.0)

calculator: calculator.c
	@echo "gcc -o calculator calculator.c"
	@$(CC) -o calculator calculator.c $(CFLAGS) $(LIBS)

clean:
	rm -f calculator

