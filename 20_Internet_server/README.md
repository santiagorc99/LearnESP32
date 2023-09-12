## Server to client 
The normal flow is the client starts the comunciation and the server replies. What happen if the server wants to start the communication. Below there are some strategies:

* Client send a request periodically: It works if the client does not need to know something has happend on the server immediately.

### Web sockets
The communication flow is: 
* Client initializes the communciation with the server, when the ocmmunication is initialized, the server stores a client ID per client.
* As the server knows the client ID, the server can communicate to the client once the communication has been stablished.
