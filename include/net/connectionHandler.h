#include "sockUtils.h"
#include "CBuffer.h"
#include "connectionCreator.h"

#define BUFF_SIZE_BYTES 1024

class ConnectionHandler{
protected:
	ConnectionInfo _info;
	CBuffer<uint8_t> _in;
	CBuffer<uint8_t> _out;
public:
	ConnectionHandler(ConnectionInfo info):_info(info),_in(),_out(){
		_in.init(BUFF_SIZE_BYTES);
		_out.init(BUFF_SIZE_BYTES);
	}
	ConnectionInfo getInfo(){return _info;}
	CBuffer<uint8_t>* getInBuffPtr(){ return &_in;}
	CBuffer<uint8_t>* getOutBuffPtr(){ return &_out; }

	int recv(){
		int recvRes(::recv(_info.sockFd,_in.get()+_in.length(), _in.capacity()-_in.length(),0));
		if(recvRes==0){
			close();
			return -1;
		}
		_in.addLength(recvRes);
		return 0;
	}
	int send(){
		int sendRes(::send(_info.sockFd,_out.get(), _out.length(),0));
		if(sendRes == -1){
			close();
			return -1;
		}
		_out.removeLength(sendRes);
		return 0;
	}
	void close(){
		sock_utils::close_socket(_info.sockFd);	
	}
	bool isReadyToWrite(){
		return _out.length()>0 ? true : false; 
	}
};
