#pragma once
#include "ConnectionCreator.h" 
#include "IOMultiplexer.h"
#include "ProxySessionHandler.h"
class SocketEventProcessor {
protected:
	HostType _type;
	int _maxSessions;
	ConnectionCreator _connectionCreator;
	IOMultiplexer _multiplexer;
	std::unordered_map<int, ProxySessionHandler*> _socketHashMap;
	std::vector<ProxySessionHandler*> _sessions; 
private:
	void AddConnectConnectionToSession(ConnectionInfo* conInfo);
	void addAcceptConnectionToSession(ConnectionInfo* conInfo);
	int processMultiplexer();
	void closeConnection(std::unordered_map<int, ProxySessionHandler*>::iterator socketHashMapIterator);
public:
	void print();
	int process();
	int addConnectEndpoint(ConnectConfig& cfg);
	int addAcceptEndpoint(AcceptConfig& cfg);
	SocketEventProcessor(int MaxSessions, HostType type);
};
