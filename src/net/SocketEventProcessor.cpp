#include "SocketEventProcessor.h"
void SocketEventProcessor::AddConnectConnectionToSession(ConnectionInfo* conInfo){
	Logger::debug("connected successfully!");
	bool areSessionsFull = true;
	for(auto sessionPtr : _sessions){
		if(sessionPtr->addConnection(CONNECT, conInfo)==0){
			areSessionsFull = false;	
			_socketHashMap.emplace(conInfo->sockFd, sessionPtr);
			break;
		} 
	}
	if(areSessionsFull){
		if(_sessions.size()>=_maxSessions)
			return; //max _sessions reached
		
		_sessions.push_back(new ProxySessionHandler({_type}));
		_sessions.back()->addConnection(CONNECT, conInfo);
		_socketHashMap.emplace(conInfo->sockFd, _sessions.back());	
	}
	print();
	return;
}
void SocketEventProcessor::addAcceptConnectionToSession(ConnectionInfo* conInfo){
	Logger::debug("accepted new connection!");
	bool areSessionsFull = true;
	for(auto sessionPtr : _sessions){
		if(sessionPtr->addConnection(ACCEPT, conInfo)==0){
			Logger::debug("found free space in Session\n");
			areSessionsFull = false;	
			_socketHashMap.emplace(conInfo->sockFd, sessionPtr);
			break;
		} 
	}
	if(areSessionsFull){
		if(_sessions.size()>=_maxSessions)
			return; //max _sessions reached
		
		Logger::debug("pushing new session\n");
		_sessions.push_back(new ProxySessionHandler({_type}));
		_sessions.back()->addConnection(ACCEPT, conInfo);
		_socketHashMap.emplace(conInfo->sockFd, _sessions.back());	
	}
	print();
	return;
}
int SocketEventProcessor::processMultiplexer(){
	std::vector<int> fdsReadyToRead;
	fdsReadyToRead.reserve(_socketHashMap.size());
	std::vector<int> fdsReadyToWrite;
	fdsReadyToWrite.reserve(_socketHashMap.size());
	std::vector<int> fdsReadyToIncomeConnection;
	for(auto iter = _sessions.begin(); iter!=_sessions.end();++iter){
		std::vector readVec = (*iter)->getSocketsReadyToRead();
		for(auto socksIter : readVec)
			fdsReadyToRead.push_back(socksIter);

		std::vector writeVec = (*iter)->getSocketsReadyToWrite();
		for(auto socksIter : writeVec)
			fdsReadyToWrite.push_back(socksIter);
	}

	fdsReadyToIncomeConnection = _connectionCreator.getAcceptSockets();
	return _multiplexer.fillSocketArr(fdsReadyToRead, fdsReadyToWrite, fdsReadyToIncomeConnection);
}
void SocketEventProcessor::closeConnection(std::unordered_map<int, ProxySessionHandler*>::iterator socketHashMapIterator){	
	if(socketHashMapIterator->second->hasFreeSpace(ALL)){	
		for(auto iter = _sessions.begin(); iter!= _sessions.end();++iter){
			if(*iter == socketHashMapIterator->second){
				_sessions.erase(iter);
				break;
			}
		}
	}
	_socketHashMap.erase(socketHashMapIterator);	
	print();
}
void SocketEventProcessor::print(){
	for(int i = 0; i< _sessions.size(); ++i){
		Logger::info("-+-+-+-Session "+std::to_string(i)+"+-+-+-+");
		_sessions[i]->print();
		Logger::info("+-+-+-+-+-+-+-+-+-+-+-+\n\n");
	}
}
int SocketEventProcessor::process(){
	if(processMultiplexer()==-1){	
		std::cout<< "processMultiplexer error!\n";
		return -1;
	}
	_connectionCreator.tryAccept(_multiplexer.getActiveFdsIncomeConnection());
	_connectionCreator.tryConnect();
	//receiving
	for(auto sockfd:_multiplexer.getActiveFdsRead()){
		auto hashMapIterator = _socketHashMap.find(sockfd);
		int recvRes(hashMapIterator->second->recv(sockfd));
		//if connection is down
		if(recvRes==-1)
			closeConnection(hashMapIterator);
	}
	
	//sending
	for(auto sockfd:_multiplexer.getActiveFdsWrite()){
		auto hashMapIterator = _socketHashMap.find(sockfd);
		int sendRes(hashMapIterator->second->send(sockfd));
		//if connection is down
		if(sendRes==-1)
			closeConnection(hashMapIterator);
	}
	return 0;
}
int SocketEventProcessor::addConnectEndpoint(ConnectConfig& cfg){
	return _connectionCreator.addConnectEndpoint(cfg);
}
int SocketEventProcessor::addAcceptEndpoint(AcceptConfig& cfg){
	return _connectionCreator.addAcceptEndpoint(cfg);
}
SocketEventProcessor::SocketEventProcessor(int MaxSessions, HostType type): _maxSessions(MaxSessions), _multiplexer(1,0), _type(type){
	_connectionCreator.connectEventSubscribe([this](ConnectionInfo* conInfo){
		AddConnectConnectionToSession(conInfo);
	});
	_connectionCreator.acceptEventSubscribe([this](ConnectionInfo* conInfo){
		addAcceptConnectionToSession(conInfo);
	});
}
