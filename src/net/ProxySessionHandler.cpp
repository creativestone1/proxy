#include "ProxySessionHandler.h"

std::string ProxySessionHandler::moduleStateToString(ConnectionModuleState state){
	std::string str;	
	switch (state) {
		case CONMODULE_IDLE:
			str = "  IDLE  ";
			break;
		case CONMODULE_LINKED:
			str = " LINKED ";
			break;
		case CONMODULE_TRANSFER:
			str = "TRANSFER";
	}
	return str;
}
void ProxySessionHandler::print(){
	Logger::info("____connectModule_____");
	if(_connectCM.conHandler == nullptr)
		 Logger::info("|       nullptr      |");
	else
		_connectCM.conHandler->print();	
	Logger::info("|____________________|");
	Logger::info("|      "+ moduleStateToString(_connectCM.state)+"      |");
	Logger::info("______________________\n");

	Logger::info("____acceptModule____");
	if(_acceptCM.conHandler == nullptr)
		Logger::info("|       nullptr      |");
	else
		_acceptCM.conHandler->print();	
	Logger::info("|____________________|");
	Logger::info("|      "+ moduleStateToString(_acceptCM.state)+"      |");
	Logger::info("______________________\n");
}
void ProxySessionHandler::updateCMStates(){
	//IDLE -> LINKED
	if(_connectCM.state == CONMODULE_IDLE && _connectCM.conHandler != nullptr){
		_connectCM.state = CONMODULE_LINKED;
		_connectCM.messageProcessor->setInBuff(_connectCM.conHandler->getInBuffPtr());
	}
	if(_acceptCM.state == CONMODULE_IDLE && _acceptCM.conHandler != nullptr){
		_acceptCM.state = CONMODULE_LINKED;
		_acceptCM.messageProcessor->setInBuff(_acceptCM.conHandler->getInBuffPtr());
	}

	//LINKED -> TRANSFER
	if(_connectCM.state == CONMODULE_LINKED && _acceptCM.state == CONMODULE_LINKED){
		_connectCM.state = CONMODULE_TRANSFER;
		_acceptCM.state = CONMODULE_TRANSFER;
		_connectCM.messageProcessor->setOutBuff(_acceptCM.conHandler->getOutBuffPtr());
		_acceptCM.messageProcessor->setOutBuff(_connectCM.conHandler->getOutBuffPtr());
	}

	//TRANSFER -> LINKED
	if(_connectCM.state == CONMODULE_TRANSFER && (_connectCM.conHandler == nullptr || _acceptCM.conHandler == nullptr))
		_connectCM.state = CONMODULE_LINKED;
	if(_acceptCM.state == CONMODULE_TRANSFER && (_connectCM.conHandler == nullptr || _acceptCM.conHandler == nullptr)) 
		_acceptCM.state = CONMODULE_LINKED;

	//LINKED-> IDLE
	if(_connectCM.state == CONMODULE_LINKED && _connectCM.conHandler == nullptr)
		_connectCM.state = CONMODULE_IDLE;
	if(_acceptCM.state == CONMODULE_LINKED && _acceptCM.conHandler == nullptr)
		_acceptCM.state = CONMODULE_IDLE;

	return;
}
int	ProxySessionHandler::recv(int sockfd){
	if(_connectCM.state != CONMODULE_IDLE && _connectCM.conHandler->getInfo().sockFd == sockfd){
		if(_connectCM.conHandler->recv()==-1){
			removeConnection(_connectCM.conHandler);
			return -1;		
		}
		if(_connectCM.state == CONMODULE_TRANSFER)
			_connectCM.messageProcessor->processInBuffer(_hostType);
		return 0;
	}
	if(_acceptCM.state != CONMODULE_IDLE &&_acceptCM.conHandler->getInfo().sockFd == sockfd){
		if(_acceptCM.conHandler->recv()==-1){
			removeConnection(_acceptCM.conHandler);
			return -1;		
		}
		if(_acceptCM.state == CONMODULE_TRANSFER)
			_acceptCM.messageProcessor->processInBuffer(_hostType);
		return 0;
	}
	return 0;
}
int ProxySessionHandler::send(int sockfd){
	if(_connectCM.state != CONMODULE_IDLE && _connectCM.conHandler->getInfo().sockFd == sockfd){
		if(_connectCM.conHandler->send()==-1){
			removeConnection(_connectCM.conHandler);
			return -1;		
		}
		return 0;
	}
	if(_acceptCM.state != CONMODULE_IDLE && _acceptCM.conHandler->getInfo().sockFd == sockfd){
		if(_acceptCM.conHandler->send()==-1){
			removeConnection(_acceptCM.conHandler);
			return -1;		
		}
		return 0;
	}
	return 0;
}
std::vector<int> ProxySessionHandler::getSocketsReadyToRead(){
	std::vector<int> vec;
	vec.reserve(2);
	if(_connectCM.state == CONMODULE_LINKED || _connectCM.state == CONMODULE_TRANSFER)
		vec.push_back(_connectCM.conHandler->getInfo().sockFd);
	if(_acceptCM.state == CONMODULE_LINKED || _acceptCM.state == CONMODULE_TRANSFER)
		vec.push_back(_acceptCM.conHandler->getInfo().sockFd);
	return vec;	
}
std::vector<int> ProxySessionHandler::getSocketsReadyToWrite(){
	std::vector<int> vec;	
	vec.reserve(2);
	if((_connectCM.state == CONMODULE_LINKED || _connectCM.state == CONMODULE_TRANSFER) && 
		_connectCM.conHandler->isReadyToWrite())
		vec.push_back(_connectCM.conHandler->getInfo().sockFd);
	if((_acceptCM.state == CONMODULE_LINKED || _acceptCM.state == CONMODULE_TRANSFER) &&
		_acceptCM.conHandler->isReadyToWrite())
		vec.push_back(_acceptCM.conHandler->getInfo().sockFd);
	return vec;
}
int ProxySessionHandler::addConnection(ConnectionType conType, ConnectionInfo* conHandler){
	if(!hasFreeSpace(conType))
		return -1;
	switch(conType){
		case CONNECT:{
			_connectCM.conHandler = new ConnectionHandler(conHandler);
			break;
		}
		case ACCEPT:{
			_acceptCM.conHandler = new ConnectionHandler(conHandler);
			break;
		}
		case ALL:
			return -2;
	}
	updateCMStates();
	return 0;
}
void ProxySessionHandler::removeConnection(ConnectionHandler*& connectionHandler){
	delete connectionHandler;
	connectionHandler = nullptr;
	updateCMStates();
}
bool ProxySessionHandler::hasFreeSpace(ConnectionType type){
	switch(type){
		case CONNECT:
			return _connectCM.conHandler == nullptr ? true : false;
		case ACCEPT:
			return _acceptCM.conHandler == nullptr ? true : false;
		case ALL:
			return (_acceptCM.conHandler == nullptr && _connectCM.conHandler == nullptr) ? true : false;
	}
	return false;
}
ProxySessionHandler::ProxySessionHandler(ProxySessionConfig cfg):
	_connectCM({nullptr,new ProxyMessageProcessor(_hostType),CONMODULE_IDLE}),
	_acceptCM({nullptr,new ProxyMessageProcessor(_hostType),CONMODULE_IDLE}),
	_hostType(cfg._hostType){}
ProxySessionHandler::~ProxySessionHandler(){
	delete _connectCM.conHandler;
	delete _acceptCM.conHandler;
	delete _connectCM.messageProcessor;
	delete _acceptCM.messageProcessor;
}

