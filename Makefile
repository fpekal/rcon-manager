build:
	g++ -o rcon-manager rcon-manager.cpp -std=c++26

install:
	mkdir -p ${out}/bin
	cp rcon-manager ${out}/bin
