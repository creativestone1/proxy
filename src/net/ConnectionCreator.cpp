#include "ConnectionCreator.h"

void ConnectionCreator::connectEventSubscribe(const ConnectionUpEventHandler& connectEvent) {
	_connectEventHandler = connectEvent;
} 
void ConnectionCreator::acceptEventSubscribe(const ConnectionUpEventHandler& acceptEvent) {
	_acceptEventHandler = acceptEvent;
}

int ConnectionCreator::addConnectEndpoint(ConnectConfig& conCfg){
	sockaddr_in remoteAddr;
	sockaddr_in localAddr;
	//initing remote in_addr
	remoteAddr.sin_port = htons(conCfg.remotePort);
	inet_pton(AF_INET, conCfg.remoteIp.c_str(), &(remoteAddr.sin_addr));
	remoteAddr.sin_family = AF_INET;

	//initing local in_addr
	if(conCfg.localPort == 0)
		localAddr.sin_port = 0;
	else {
		localAddr.sin_port = htons(conCfg.localPort);
		localAddr.sin_addr.s_addr = INADDR_ANY;
		localAddr.sin_family = AF_INET;
	}

	//socket configuring
	int sock = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
	if(sock == 0)	
		return -1;
	if(sock_utils::reuse_addr(sock, 0) != 0)
		return -1;
	sock_utils::setnonblocking(sock);
	if(conCfg.localPort!=0){
		if(bind(sock, reinterpret_cast<const sockaddr*>(&localAddr), sizeof(localAddr))!=0)
			return -1;
	}
	Logger::debug("opened connect socket "+std::to_string(sock));
	InternalConnectConfig* conHandler = new InternalConnectConfig();
	conHandler->remoteAddr = remoteAddr;
	conHandler->localAddr = localAddr;
	conHandler->lastConnectTries = conCfg.connectionTryTimes;
	conHandler->sockFd = sock;
	conHandler->tryType = conCfg.tryType;

	//pushing new connection to vector
	_socketsConnect.push_back(conHandler);

	return 0;
	
}

int ConnectionCreator::addAcceptEndpoint(AcceptConfig& hostCfg){
	sockaddr_in localAddr;
	//initing address		
	localAddr.sin_port = htons(hostCfg.localPort);
	localAddr.sin_addr.s_addr = INADDR_ANY;
	localAddr.sin_family = AF_INET;
	

	//socket configuring
	int sock = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);

	Logger::debug("opened accept socket "+std::to_string(sock));

	if(sock==0)	
		return -1;
	if(sock_utils::reuse_addr(sock, 0) != 0)
		return -1;
	sock_utils::setnonblocking(sock);
	if(bind(sock, reinterpret_cast<const sockaddr*>(&localAddr), sizeof(localAddr))!=0)
		return -1;
	if(listen(sock, MAX_CONNECTIONS_COUNT)!=0)
		return -1;
	
	InternalAcceptConfig* hostHandler = new InternalAcceptConfig();
	hostHandler->localAddr = localAddr;
	hostHandler->sockFd = sock;
	hostHandler->lastConnections = hostCfg.connectionsCount;
	
	//pushing new connection to list
	_socketsAccept.push_back(hostHandler);

	return 0;
	
}

int ConnectionCreator::tryConnect(){
	for(auto iter = _socketsConnect.begin(); iter!=_socketsConnect.end();){
		int conStatus(connect((*iter)->sockFd, reinterpret_cast<sockaddr*>(&(*iter)->remoteAddr), sizeof((*iter)->remoteAddr)));
		if(conStatus==0){
			(*iter)->lastConnections -= 1;
			ConnectionInfo* info = new ConnectionInfo();
			info->remoteAddr = (*iter)->remoteAddr;
			info->sockFd = (*iter)->sockFd;
			info->localAddr = (*iter)->localAddr;
			if((*iter)->lastConnections == 0){
				delete *iter;
				_socketsConnect.erase(iter);	
			}
			else {
				iter++;
			}
			
			Logger::debug("connected: sockFd = "+std::to_string(info->sockFd)+'\n');
			_connectEventHandler(info);
		}
		else{
			if((*iter)->tryType == TRY_MULTIPLE_TIMES)
				(*iter)->lastConnectTries -= 1;
			if((*iter)->tryType == TRY_ONCE || 
				((*iter)->tryType == TRY_MULTIPLE_TIMES && 
				(*iter)->lastConnectTries == 0)){
				delete *iter;
				_socketsConnect.erase(iter);	
			}
			else {
			iter++;
			}
		}
	}
	return 0;
}
int ConnectionCreator::tryAccept(std::vector<int>& sockets){
	for(auto sock : sockets){
		for(auto iter = _socketsAccept.begin(); iter!=_socketsAccept.end();){
			if(sock==(*iter)->sockFd){
				sockaddr_in remoteAddr;
#ifdef _WIN32
				int addrLen = sizeof(remoteAddr);
#else
				socklen_t addrLen = sizeof(remoteAddr);
#endif
				int remoteSock = accept(sock, reinterpret_cast<sockaddr*>(&remoteAddr), &addrLen);
				Logger::debug("accepted: sockFd = "+std::to_string(remoteSock)+'\n');
				if(remoteSock<=0){
					perror("accept");
					sock_utils::close_socket((*iter)->sockFd);			
					delete *iter;
					_socketsAccept.erase(iter);	
					return -1;
				}

				sock_utils::setnonblocking(remoteSock);

				ConnectionInfo* info = new ConnectionInfo();
				info->sockFd = remoteSock;
				info->remoteAddr = remoteAddr;
				info->localAddr = (*iter)->localAddr;

				(*iter)->lastConnections--;

				if((*iter)->lastConnections == 0){
					sock_utils::close_socket((*iter)->sockFd);			
					delete *iter;
					_socketsAccept.erase(iter);	
				}
				else {
					iter++;
				}
				_acceptEventHandler(info);
			}
		}
	}
	return 0;
}
std::vector<int> ConnectionCreator::getAcceptSockets(){
	std::vector<int> vec;
	vec.reserve(_socketsAccept.size());
	for(auto iter:_socketsAccept)
		vec.push_back(iter->sockFd);
	return vec;
}
