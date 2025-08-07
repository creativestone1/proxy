#pragma once
#include <vector>
#include <sys/select.h>
class IOMultiplexer {
protected:	
	std::vector<int> activeFdsRead;
	std::vector<int> activeFdsWrite;
	std::vector<int> activeFdsIncomeConnection;
	timeval timeoutPeriod;
public:
	int fillSocketArr(std::vector<int> &fdsReadyToRead, std::vector<int> &fdsReadyToWrite, std::vector<int> &fdsReadyToIncomeConnection);
	std::vector<int>& getActiveFdsRead();
	std::vector<int>& getActiveFdsWrite();
	std::vector<int>& getActiveFdsIncomeConnection();
	IOMultiplexer(int secondsToTimeout, int microsecondsToTimeout);
};
