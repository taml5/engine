CFLAGS = -std=gnu99 -lglfw3 -framework CoreVideo -framework OpenGL -framework IOKit -framework Cocoa -framework Carbon

engine: build/main.o build/graphics.o build/load.o build/game.o
	gcc ${CFLAGS} -O3 build/main.o build/graphics.o build/load.o build/game.o -o engine

build/main.o: src/main.c include/game.h include/graphics.h
	mkdir -p build
	gcc -I./include/ -c -o $@ $<

build/graphics.o: src/graphics.c include/game.h include/graphics.h include/bayer.h 
	gcc -I./include/ -c -o $@ $<

build/load.o: src/load.c include/game.h include/load.h 
	gcc -I./include/ -c -o $@ $<

build/game.o: src/game.c include/game.h include/graphics.h
	gcc -I./include/ -c -o $@ $<

.PHONY: debug clean
	

debug: build/main.o build/graphics.o build/load.o build/game.o
	gcc -D DEBUG ${CFLAGS} -g build/main.o build/graphics.o build/load.o build/game.o -o engine

clean:
	rm -r ./build
	rm ./engine