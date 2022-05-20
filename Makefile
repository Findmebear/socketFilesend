all:
	g++ src/client.cpp src/fileserver.cpp src/libs/nodelib.cpp -c -lpack109 -Lbuild -Iinclude -std=c++11
	g++ src/libs/hashlib.cpp src/libs/lib.cpp src/libs/linkedlistlib.cpp -c -lpack109 -Lbuild -Iinclude -std=c++11
	g++ fileserver.o hashlib.o linkedlistlib.o nodelib.o lib.o -o fileserver
	g++ client.o lib.o -o client
	mkdir -p build
	mv *.o build
	mkdir -p build/run 
	mv fileserver build/run
	mv client build/run
clean: 
	rm -f *.a
	rm -f *.o 
	rm -rf build