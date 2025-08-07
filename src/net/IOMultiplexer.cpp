#include "IOMultiplexer.h"
#include "Logger.h"
#include <iostream>
int IOMultiplexer::fillSocketArr(std::vector<int> &fdsReadyToRead,
								 std::vector<int> &fdsReadyToWrite,
								 std::vector<int> &fdsReadyToIncomeConnection){
	//Logger::debug("_select_");
	activeFdsRead.clear();
	activeFdsWrite.clear();
	activeFdsIncomeConnection.clear();
	
	fd_set readFdSet;
	fd_set writeFdSet;
	FD_ZERO(&readFdSet);
	FD_ZERO(&writeFdSet);

	timeval time;
	time = timeoutPeriod;

	//Logger::debug("\tfdsReadyToIncomeConnection : "+ std::to_string(fdsReadyToIncomeConnection.size()));
	//Logger::debug("\tfdsReadyToRead : "+ std::to_string(fdsReadyToRead.size()));
	//Logger::debug("\tfdsReadyToWrite : "+ std::to_string(fdsReadyToWrite.size()));
	int maxSocket(0);
	//fill fd_sets
	for(auto i: fdsReadyToRead){
		FD_SET(i,&readFdSet);
		maxSocket = std::max(maxSocket, i);
	}
	for(auto i: fdsReadyToWrite){
		FD_SET(i,&writeFdSet);
		maxSocket = std::max(maxSocket, i);
	}
	for(auto i: fdsReadyToIncomeConnection){
		FD_SET(i,&readFdSet);
		maxSocket = std::max(maxSocket, i);
	}


	int selectCount = select(maxSocket+1, &readFdSet, &writeFdSet, nullptr,&time);
	//Logger::debug( "____"+ std::to_string(selectCount)+"___\n");
	if(selectCount == -1)
		return -1;
	int socketsAdded = 0;
	for(auto i: fdsReadyToRead){
		if(socketsAdded >= selectCount)
			return 1;
		if(FD_ISSET(i,&readFdSet)){
			activeFdsRead.push_back(i);
			++socketsAdded;
		}
	}
	for(auto i: fdsReadyToWrite){
		if(socketsAdded >= selectCount)
			return 1;
		if(FD_ISSET(i,&writeFdSet)){
			activeFdsWrite.push_back(i);
			++socketsAdded;
		}
	}
	for(auto i: fdsReadyToIncomeConnection){
		if(socketsAdded >= selectCount)
			return 1;
		if(FD_ISSET(i,&readFdSet)){
			activeFdsIncomeConnection.push_back(i);
			++socketsAdded;
		}
	}
	return 1;
}
std::vector<int>& IOMultiplexer::getActiveFdsRead(){
	return activeFdsRead;
}
std::vector<int>& IOMultiplexer::getActiveFdsWrite(){
	return activeFdsWrite;
}
std::vector<int>& IOMultiplexer::getActiveFdsIncomeConnection(){
	return activeFdsIncomeConnection;
}
IOMultiplexer::IOMultiplexer(int secondsToTimeout, int microsecondsToTimeout){
	timeoutPeriod.tv_usec = microsecondsToTimeout;
	timeoutPeriod.tv_sec = secondsToTimeout;
}

