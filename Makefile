CFLAGS = -std=gnu99 -O3 -lglfw3 -framework CoreVideo -framework OpenGL -framework IOKit -framework Cocoa -framework Carbon

engine: build/main.o build/graphics.o build/load.o build/game.o
	gcc ${CFLAGS} build/main.o build/graphics.o build/load.o build/game.o -o engine

build/main.o: src/main.c include/graphics.h include/game.h
	@mkdir -p ./build/
	gcc -I./include/ -c -o $@ $<

build/graphics.o: src/graphics.c include/graphics.h
	gcc -I./include/ -c -o $@ $<

build/load.o: src/load.c include/load.h
	gcc -I./include/ -c -o $@ $<

build/game.o: src/game.c include/game.h
	gcc -I./include/ -c -o $@ $<

.PHONY: debug clean
	

debug: main.o graphics.o
	gcc -D DEBUG ${CFLAGS} build/main.o build/graphics.o -o engine

clean:
	rm -r ./build/
	rm ./engine