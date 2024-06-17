engine: src/main.c
	gcc -std=gnu99 -lglfw3 -framework CoreVideo -framework OpenGL -framework IOKit -framework Cocoa -framework Carbon -o engine src/main.c

.PHONY clean:
	rm ./engine