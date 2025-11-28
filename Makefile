jpeg: decoder.o readFile.o decodeHuffman.o
	g++ -Wall decoder.o readFile.o decodeHuffman.o -o jpeg

decoder.o: decoder.cc readFile.h
	g++ -Wall -c decoder.cc

readFile.o: readFile.cc readFile.h 
	g++ -Wall -c -DDEBUGINFO readFile.cc

decodeHuffman.o: decodeHuffman.cc decodeHuffman.h 
	g++ -Wall -c -DDEBUGINFO decodeHuffman.cc

clean:
	rm decoder.o readFile.o decodeAPPn.o decodeHuffman.o jpeg

test: jpeg
	./jpeg test-images/cat.jpg
