EXECUTABLE = SnakeAndRat
CPP_FILE = main.cpp
SFML_LIBRARIES = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

all: $(EXECUTABLE)

$(EXECUTABLE): $(CPP_FILE)
	g++ -std=c++17 -o $(EXECUTABLE) $(CPP_FILE) $(SFML_LIBRARIES)


install:
	sudo apt update
	sudo apt install g++
	sudo apt install libsfml-dev

clean:
	rm -rf $(EXECUTABLE)
