
OUT = gravity
COMP = clang++
FLAGS = -O2 -I/usr/include/SDL2
LIBS = -lSDL2 -lSDL2_gfx
OBJECTS = main.o

build: $(OBJECTS) Makefile
	$(COMP) -o $(OUT) $(OBJECTS) $(FLAGS) $(LIBS)

run: build
	./$(OUT)

clean:
	rm $(OUT)
	rm $(OBJECTS)

%.o: %.cpp mat2.h
	$(COMP) -c $< -o $@ $(FLAGS) 



