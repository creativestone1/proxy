#include "connectionCreator.h" 
#include "IOMultiplexer.h"
class SocketEventProcessor {
protected:
	ConnectionCreator connectionCreator;
	IOMultiplexer multiplexer;
public:
	void AddConnect(ConnectionInfo conInfo){
				
	}
	void addHost(ConnectionInfo conInfo){

	}
	int process(){
		
	}
	SocketEventProcessor(): multiplexer(5,0){
		connectionCreator.connectEventSubscribe([this](ConnectionInfo conInfo){
			AddConnect(conInfo);
		});
		connectionCreator.acceptEventSubscribe([this](ConnectionInfo conInfo){
			AddConnect(conInfo);
		});
	}
};
