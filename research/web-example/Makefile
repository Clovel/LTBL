all: web-example

web-example: main.cxx remoteRequest.cxx
	g++ -Wall --std=c++17 -ggdb -g3 main.cxx remoteRequest.cxx Relay.cxx HttpRequest.cxx -o web-example

clean:
	rm web-example
