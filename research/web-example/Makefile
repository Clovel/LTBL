all: web-example

web-example: main.cxx remoteRequest.cxx
	g++ -Wall main.cxx remoteRequest.cxx Relay.cxx -o web-example

clean:
	rm web-example
