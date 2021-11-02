
OUT = gravity
COMP = clang++
FLAGS = -O2
LIBS = -lSDL2 -lSDL2_gfx
OBJECTS = main.o

build: $(OBJECTS) Makefile
	$(COMP) -o $(OUT) $(OBJECTS) $(FLAGS) $(LIBS)

run: build
	./$(OUT)

clean:
	rm $(OUT)
	rm $(OBJECTS)

%.o: %.cpp
	$(COMP) -c $< -o $@ $(FLAGS) 



