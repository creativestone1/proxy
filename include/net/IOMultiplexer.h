#include <vector>
#include <sys/select.h>
class IOMultiplexer {
protected:	
	std::vector<int> activeFdsRead;
	std::vector<int> activeFdsWrite;
	std::vector<int> activeFdsIncomeConnection;
	timeval timeoutPeriod;
public:
	int fillSocketArr(std::vector<int> &fdsReadyToRead, std::vector<int> &fdsReadyToWrite, std::vector<int> &fdsReadyToIncomeConnection, int maxSocket) {

		activeFdsRead.clear();
		activeFdsWrite.clear();
		activeFdsIncomeConnection.clear();
		
		fd_set readFdSet;
		fd_set writeFdSet;

		timeval time;
		time = timeoutPeriod;


		//fill fd_sets
		for(auto i: fdsReadyToRead)
			FD_SET(i,&readFdSet);
		for(auto i: fdsReadyToWrite)
			FD_SET(i,&writeFdSet);
		for(auto i: fdsReadyToIncomeConnection)
			FD_SET(i,&readFdSet);


		int selectCount = select(maxSocket, &readFdSet, &writeFdSet, nullptr,&time);	
		int socketsAdded = 0;
		for(auto i: fdsReadyToRead){
			if(FD_ISSET(i,&readFdSet)){
				activeFdsRead.push_back(i);
				++socketsAdded;
			}
			if(socketsAdded >= selectCount)
				return 1;
		}
		for(auto i: fdsReadyToWrite){
			if(FD_ISSET(i,&writeFdSet)){
				activeFdsWrite.push_back(i);
				++socketsAdded;
			}
			if(socketsAdded >= selectCount)
				return 1;
		}
		for(auto i: fdsReadyToIncomeConnection){
			if(FD_ISSET(i,&readFdSet)){
				activeFdsIncomeConnection.push_back(i);
				++socketsAdded;
			}
			if(socketsAdded >= selectCount)
				return 1;
		}
		return 1;
	}
	std::vector<int>& getActiveFdsRead(){
		return activeFdsRead;
	}
	std::vector<int>& getActiveFdsWrite(){
		return activeFdsWrite;
	}
	std::vector<int>& getActiveFdsIncomeConnection(){
		return activeFdsIncomeConnection;
	}
	IOMultiplexer(int secondsToTimeout, int microsecondsToTimeout){
		timeoutPeriod.tv_usec = microsecondsToTimeout;
		timeoutPeriod.tv_sec = secondsToTimeout;
	}
};
