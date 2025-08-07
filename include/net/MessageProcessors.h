#pragma once
#include "CBuffer.h"
#include <memory>
#include <cstring>
#include <algorithm>
enum MessageProcessorType{
	CLIENT,
	SERVER
};
class ProxyMessageProcessor{
protected:
	MessageProcessorType _type;
	CBuffer<uint8_t>* _in;
	CBuffer<uint8_t>* _out;
public:
	ProxyMessageProcessor(MessageProcessorType type);

	void setInBuff(CBuffer<uint8_t>* buffPtr);
	void setOutBuff(CBuffer<uint8_t>* buffPtr);

	void processInBuffer(MessageProcessorType type);
};
