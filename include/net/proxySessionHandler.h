#include "connectionHandler.h"
enum ConnectionType{
	CONNECT,
	ACCEPT
};
class ProxySessionHandler{
protected:
	ConnectionHandler* connectH;
	ConnectionHandler* acceptH;
public:
	int	recv(int sockfd){
		if(connectH->getInfo().sockFd == sockfd){
			if(connectH->recv()==-1){
				free(connectH);
				return -1;		
			}
			return 0;
		}
		if(acceptH->getInfo().sockFd == sockfd){
			if(acceptH->recv()==-1){
				free(acceptH);
				return -1;		
			}
			return 0;
		}
		return 0;
	}
	int send(int sockfd){

		if(connectH->getInfo().sockFd == sockfd){
			if(connectH->send()==-1){
				free(connectH);
				return -1;		
			}
			return 0;
		}
		if(acceptH->getInfo().sockFd == sockfd){
			if(acceptH->send()==-1){
				free(acceptH);
				return -1;		
			}
			return 0;
		}
		return 0;
	}
	std::vector<int> getSocketsReadyToWrite(){
		std::vector<int> vec;	
		if(connectH->isReadyToWrite())
			vec.push_back(connectH->getInfo().sockFd);
		if(acceptH->isReadyToWrite())
			vec.push_back(acceptH->getInfo().sockFd);
		return vec;
	}
	bool hasFreeSpace(ConnectionType type){
		switch(type){
			case CONNECT:
				return connectH == nullptr ? false : true;
			case ACCEPT:
				return acceptH == nullptr ? false : true;
		}
		return false;
	}
	ProxySessionHandler():connectH(nullptr),acceptH(nullptr){}
	~ProxySessionHandler(){
		connectH->close();
		acceptH->close();
		free(connectH);
		free(acceptH);
	}
};
