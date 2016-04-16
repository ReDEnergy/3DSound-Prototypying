#include "CSoundEvent.h"

CSoundEvent::CSoundEvent(char type, const char * instrumentID, string parameters)
{
	this->type = (type == 0) ? 'i' : type;
	this->instrumentID = instrumentID;
	this->parameters = parameters;
}

CSoundEvent::~CSoundEvent()
{
}

string CSoundEvent::GetRender() const
{
	string value;
	value += type;
	return value + " " + instrumentID + " " + parameters + "\n";
}

string CSoundEvent::GetRender(unsigned int instrumentIndex) const
{
	string value;
	value += type;
	return value + " " + to_string(instrumentIndex) + " " + parameters + "\n";
}

const char * CSoundEvent::GetInstrumentID() const
{
	return instrumentID.c_str();
}

