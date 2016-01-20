all: download server
download: download.cpp
	g++ -o download download.cpp
server:	server.cpp
	g++ -o server server.cpp
clean:
	rm server download
