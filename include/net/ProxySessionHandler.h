#pragma once
#include "ConnectionHandler.h"
#include "MessageProcessors.h"
//Enumerators
enum ConnectionType{
	CONNECT,
	ACCEPT,
	ALL
};
using HostType = MessageProcessorType;

struct ProxySessionConfig{
	HostType _hostType;
};
//class
class ProxySessionHandler{
protected:
	enum ConnectionModuleState{
		CONMODULE_IDLE,
		CONMODULE_LINKED,
		CONMODULE_TRANSFER
	};
	struct ConnectionModule{
		ConnectionHandler* conHandler;
		ProxyMessageProcessor* messageProcessor;
		ConnectionModuleState state;	
	};
	HostType _hostType;
	ConnectionModule _connectCM;
	ConnectionModule _acceptCM;
private:	
	std::string moduleStateToString(ConnectionModuleState state);
	void updateCMStates();
public:
	void print();

	int addConnection(ConnectionType type, ConnectionInfo* conInfo);
	void removeConnection(ConnectionHandler*& connectionHandler);

	bool hasFreeSpace(ConnectionType type);
	int	recv(int sockfd);
	int send(int sockfd);

	std::vector<int> getSocketsReadyToRead();	
	std::vector<int> getSocketsReadyToWrite();

	ProxySessionHandler(ProxySessionConfig cfg);
	~ProxySessionHandler();
};
