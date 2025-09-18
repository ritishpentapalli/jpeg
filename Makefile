jpeg: decoder.o readFile.o
	g++ -Wall decoder.o readFile.o -o jpeg

decoder.o: decoder.cc readFile.h
	g++ -Wall -c decoder.cc

readFile.o: readFile.cc readFile.h 
	g++ -Wall -c -DDEBUGINFO readFile.cc

clean:
	rm decoder.o readFile.o decodeAPPn.o jpeg

test: jpeg
	./jpeg test-images/mouse.jpg
