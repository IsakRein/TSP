run: build/main input.txt
	time ./build/main < input.txt
build/main: main.cpp *.h
	g++ -std=c++17 -o build/main main.cpp
debug: main.cpp
	g++ -std=c++17 -fdiagnostics-color=always -g -o build/debug main.cpp