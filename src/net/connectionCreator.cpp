#include "connectionCreator.h"

void ConnectionCreator::connectEventSubscribe(const ConnectionUpEventHandler& connectEvent) {
	connectEventHandler = connectEvent;
} 
void ConnectionCreator::acceptEventSubscribe(const ConnectionUpEventHandler& acceptEvent) {
	acceptEventHandler = acceptEvent;
}

int ConnectionCreator::addConnectEndpoint(ConnectConfig conCfg){
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
	ConnectConfigHandler* conHandler = new ConnectConfigHandler();
	conHandler->remoteAddr = remoteAddr;
	conHandler->localAddr = localAddr;
	conHandler->lastConnections = conCfg.connectionsCount;
	conHandler->lastConnectTries = conCfg.connectionTryTimes;
	conHandler->sockFd = sock;
	conHandler->tryType = conCfg.tryType;

	//pushing new connection to vector
	socketsConnect.push_back(conHandler);

	return 0;
	
}

int ConnectionCreator::addHostEndpoint(HostConfig hostCfg){
	sockaddr_in localAddr;
	//initing address		
	localAddr.sin_port = htons(hostCfg.localPort);
	localAddr.sin_addr.s_addr = INADDR_ANY;
	localAddr.sin_family = AF_INET;
	

	//socket configuring
	int sock = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
	if(sock==0)	
		return -1;
	if(sock_utils::reuse_addr(sock, 0) != 0)
		return -1;
	sock_utils::setnonblocking(sock);
	if(bind(sock, reinterpret_cast<const sockaddr*>(&localAddr), sizeof(localAddr))!=0)
		return -1;
	if(listen(sock, MAX_CONNECTIONS_COUNT)!=0)
		return -1;
	
	HostConfigHandler* hostHandler = new HostConfigHandler();
	hostHandler->localAddr = localAddr;
	hostHandler->sockFd = sock;
	hostHandler->lastConnections = hostCfg.connectionsCount;
	
	//pushing new connection to list
	socketsHost.push_back(hostHandler);

	return 0;
	
}

int ConnectionCreator::tryConnect(){
	for(auto iter = socketsConnect.begin(); iter!=socketsConnect.end();iter++){
		int conStatus(connect((*iter)->sockFd, reinterpret_cast<sockaddr*>(&(*iter)->remoteAddr), sizeof((*iter)->remoteAddr)));
		if(conStatus==0){
			(*iter)->lastConnections -= 1;
			ConnectionInfo info;
			info.remoteAddr = (*iter)->remoteAddr;
			info.sockFd = (*iter)->sockFd;
			info.localAddr = (*iter)->localAddr;
			if((*iter)->lastConnections == 0){
				sock_utils::close_socket((*iter)->sockFd);			
				delete *iter;
				socketsConnect.erase(iter);	
			}
			connectEventHandler(info);
		}
		else{
			if((*iter)->tryType == TRY_MULTIPLE_TIMES)
				(*iter)->lastConnectTries -= 1;
			if((*iter)->tryType == TRY_ONCE || 
				((*iter)->tryType == TRY_MULTIPLE_TIMES && 
				(*iter)->lastConnectTries == 0)){
				sock_utils::close_socket((*iter)->sockFd);			
				delete *iter;
				socketsConnect.erase(iter);	
			}
		}
	}
	return 0;
}
int ConnectionCreator::tryAccept(std::vector<int>& sockets){
	for(auto sock : sockets){
		for(auto iter = socketsHost.begin(); iter!=socketsHost.end();++iter){
			if(sock==(*iter)->sockFd){
				sockaddr_in remoteAddr;
#ifdef _WIN32
				int addrLen;
#else
				socklen_t addrLen;
#endif
				int remoteSock = accept(sock, reinterpret_cast<sockaddr*>(&remoteAddr), &addrLen);
				if(remoteSock==0){
					sock_utils::close_socket((*iter)->sockFd);			
					delete *iter;
					socketsHost.erase(iter);	
					return -1;
				}
				ConnectionInfo info;
				info.sockFd = remoteSock;
				info.remoteAddr = remoteAddr;
				info.localAddr = (*iter)->localAddr;

				if((*iter)->lastConnections == 0){
					sock_utils::close_socket((*iter)->sockFd);			
					delete *iter;
					socketsHost.erase(iter);	
				}
				acceptEventHandler(info);
			}
		}
	}
}
