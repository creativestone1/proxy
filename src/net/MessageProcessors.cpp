#include "MessageProcessors.h"

ProxyMessageProcessor::ProxyMessageProcessor(MessageProcessorType type):_type(type){}
void ProxyMessageProcessor::setInBuff(CBuffer<uint8_t>* buffPtr){
	_in = buffPtr;
}
void ProxyMessageProcessor::setOutBuff(CBuffer<uint8_t>* buffPtr){
	_out = buffPtr;
}
void ProxyMessageProcessor::processInBuffer(MessageProcessorType type){
	size_t cpySize=std::min(_out->capacity()-_out->length(),_in->length());
	memcpy(_out->get()+_out->length(), _in->get(),cpySize);
	_out->addLength(cpySize);	
	_in->removeLength(cpySize);
}
