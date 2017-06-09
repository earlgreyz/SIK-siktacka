CXX=g++
CXXFLAGS=-Wall -Wextra -O2 -std=c++14

EXECS=siktacka-server siktacka-client
NETWORK=network__receiver.o network__sender.o network__socket.o
EVENTS=siktacka__event.o siktacka__event_game_over.o siktacka__event_new_game.o siktacka__event_pixel.o siktacka__event_player_eliminated.o
GAME=siktacka__game.o siktacka__board.o siktacka__random.o siktacka__snake.o $(EVENTS)
PROTOCOL=siktacka__client_message.o siktacka__server_message.o $(EVENTS) siktacka__event_factory.o
SERVER=sikserver__server.o sikserver__events.o sikserver__connections.o sikserver__buffer.o $(GAME) $(NETWORK) $(PROTOCOL)
CLIENT=sikclient__client.o sikclient__gui_client.o sikclient__parser.o $(EVENTS) $(NETWORK) $(PROTOCOL)

TARGET: $(EXECS)

network__%.o: src/network/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

siktacka__%.o: src/siktacka/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

siktacka__%.o: src/siktacka/events/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

siktacka__%.o: src/siktacka/protocol/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

sikserver__%.o: src/server/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

sikclient__%.o: src/client/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

siktacka-server: $(SERVER) siktacka-server.o
	$(CXX) -o $@ $^ $(CXXFLAGS) -pthread -lboost_program_options

siktacka-client: $(CLIENT) siktacka-client.o
	$(CXX) -o $@ $^ $(CXXFLAGS) -pthread -lboost_program_options


.PHONY: clean
clean:
	$(RM) *.o $(EXECS)
