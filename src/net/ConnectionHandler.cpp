#include "ConnectionHandler.h"

ConnectionHandler::ConnectionHandler(ConnectionInfo* info):_info(info), _in(), _out(){
	Logger::debug("ConnectionHandler created!\n");
	_in.init(BUFF_SIZE_BYTES);
	_out.init(BUFF_SIZE_BYTES);
}
ConnectionHandler::~ConnectionHandler(){
	Logger::error("connection closed by destructor!\n");
	sock_utils::close_socket(_info->sockFd);
	delete _info; 
}
ConnectionInfo ConnectionHandler::getInfo(){return *_info;}
CBuffer<uint8_t>* ConnectionHandler::getInBuffPtr(){ return &_in;}
CBuffer<uint8_t>* ConnectionHandler::getOutBuffPtr(){ return &_out; }

int ConnectionHandler::recv(){
	int recvRes(::recv(_info->sockFd,_in.get()+_in.length(), _in.capacity()-_in.length(),0));
	if(recvRes==0)
		return -1;
	_in.addLength(recvRes);
	return 0;
}
int ConnectionHandler::send(){
	int sendRes(::send(_info->sockFd,_out.get(), _out.length(),0));
	if(sendRes == -1)
		return -1;
	_out.removeLength(sendRes);
	return 0;
}
bool ConnectionHandler::isReadyToWrite(){
	return _out.length()>0 ? true : false; 
}
