#pragma once
#include "SockUtils.h"
#include "MessageProcessors.h"
#include "CBuffer.h"
#include "ConnectionCreator.h"

#define BUFF_SIZE_BYTES 1024

class ConnectionHandler{
protected:
	ConnectionInfo* _info;
	CBuffer<uint8_t> _in;
	CBuffer<uint8_t> _out;
public:
	void print(){
		char remoteIp[INET_ADDRSTRLEN] = {0};
		char localIp[INET_ADDRSTRLEN] = {0};

		// Конвертация IP адресов в текстовый вид
		inet_ntop(AF_INET, &(_info->remoteAddr.sin_addr), remoteIp, INET_ADDRSTRLEN);
		inet_ntop(AF_INET, &(_info->localAddr.sin_addr), localIp, INET_ADDRSTRLEN);

		Logger::info("|sockfd         : " + std::to_string(_info->sockFd));
		Logger::info(std::string("|remote address : ") + remoteIp + ":" + std::to_string(ntohs(_info->remoteAddr.sin_port)));
		Logger::info(std::string("|local  address : ") + localIp + ":" + std::to_string(ntohs(_info->localAddr.sin_port)));
		//Logger::info("|in  buff size  : " + std::to_string(_in.length()));
		//Logger::info("|out buff size  : " + std::to_string(_out.length()));	
	}
	ConnectionHandler(ConnectionInfo* info);
	~ConnectionHandler();

	ConnectionInfo getInfo();
	CBuffer<uint8_t>* getInBuffPtr();
	CBuffer<uint8_t>* getOutBuffPtr();

	int recv();
	int send();
	bool isReadyToWrite();
};
