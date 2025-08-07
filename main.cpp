#include "SocketEventProcessor.h"
#include <iostream>
#include "config.h"
int main(){
	Config cfg;
	if(!cfg.loadFromFile("config.txt"))	{
		std::cout << "please, make config. Here is the example:\n"
				  << "[Host]\n"
				  << "CLIENT\n\n"
				  << "[Connect]\n"
				  << "remoteIp=127.0.0.1\n"
				  << "remotePort=5001\n\n"
				  << "[Accept]\n"
				  << "localPort=7001\n"
				  << "connectionsCount=2\n\n"
				  << "[Connect]\n"
				  << "remoteIp=127.0.0.1\n"
				  << "remotePort=5002\n";
		return -1;
	}
	SocketEventProcessor eventProcessor(10,cfg.hostType);
	for(auto iter : cfg.acceptConfigs)
		if(eventProcessor.addAcceptEndpoint(iter)!=0)
			return -1;
	for(auto iter : cfg.connectConfigs)
		if(eventProcessor.addConnectEndpoint(iter)!=0)
			return -1;
	while(1)
		if (eventProcessor.process())
			return -1;	
}

