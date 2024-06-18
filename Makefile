FFLAGS = -lglfw3 -framework CoreVideo -framework OpenGL -framework IOKit -framework Cocoa -framework Carbon

all: engine

engine: main.o graphics.o
	gcc -std=gnu99 ${FFLAGS} build/main.o build/graphics.o -o engine

debug: main.o graphics.o
	gcc -std=gnu99 -D DEBUG ${FFLAGS} build/main.o build/graphics.o -o engine

main.o: src/main.c include/graphics.h include/game.h
	gcc -I./include/ -c -o build/$@ $<

graphics.o: src/graphics.c include/graphics.h
	gcc -I./include/ -c -o build/$@ $<

.PHONY clean:
	rm -r ./build
	rm ./engine