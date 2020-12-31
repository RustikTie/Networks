Networks and Online Games 

Multiplayer Game in C++

Done by 
- Andres Saladrigas
- Roger Leon
- Clara Ratera

Instructions: 
- Connect by entering a name and selecting a spaceship type, then press Connect to server
- Rotate left/right with A/D keys
- Accelerate with Down Arrow
- Fire with Left Arrow
- Disconnect at any time pressing Disconnect button top left

Features: 

	Complete: 
	- Accept 8 simultaneous players (Andres, Roger, Clara)*
	- Handle players join/leave events (Andres, Roger, Clara)*
	- World state replication (Andres, Roger, Clara)*
	- Show life bars for players (Andres, Roger, Clara)*
	
	In Progress:
	- Client side prediction (Andres, Roger, Clara)*
		We began working on implementing it but couldn't finish it properly for the delivery
	- Delivery manager notifications (Andres, Roger, Clara)*
		We have yet to send "recieved" packets


	Incomplete: stuff we didn't get to work on yet
	- Redundant sending of input packets
	- Entity interpolation
	
	* we worked together in discord sessions while sharing our screens
	
Known bugs:
	- Sometimes the client will not allow a connection and will disconnect automatically, we believe the issue has to do with the packet being sent and the timeout we have set. 
