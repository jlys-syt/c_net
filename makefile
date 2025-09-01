all:server client serverPs serverTd

server:server.cpp
	g++ -o server server.cpp -g
	
serverPs:mulServer.cpp
	g++ -o serverPs mulServer.cpp -g

serverTd:mulThreadServer.cpp
	g++ -o serverTd mulThreadServer.cpp -lpthread -g

client:client.cpp
	g++ -o client client.cpp -g

clean:
	rm -f server client serverPs serverTd