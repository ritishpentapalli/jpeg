jpeg: decoder.o decodeAPPn.o
	g++ -Wall decoder.o decodeAPPn.o -o jpeg


decoder.o: decoder.cc decoder.h decodeAPPn.h
	g++ -Wall -DDEBUGINFO -c decoder.cc

decodeAPPn.o: decodeAPPn.cc decodeAPPn.h
	g++ -Wall -c decodeAPPn.cc

clean:
	rm decoder.o decodeAPPn.o jpeg

test: jpeg
	./jpeg test-images/mouse.jpg
