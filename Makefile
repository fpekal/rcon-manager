build:
	g++ -o rcon-manager rcon-manager.cpp protocol.cpp server-entry.cpp -std=c++26

install:
	mkdir -p ${out}/bin
	cp rcon-manager ${out}/bin
