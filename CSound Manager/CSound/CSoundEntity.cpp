#include "CSoundEntity.h"

static int counter = 0;

CSoundEntity::CSoundEntity(const char* name)
{
	if (name)
		this->name = name;

	render.reserve(200);
	UID = counter++;
}

CSoundEntity::~CSoundEntity()
{
}

void CSoundEntity::SetName(const char* name) {
	this->name = name;
}

bool CSoundEntity::Rename(const char * name)
{
	return false;
}

const char * CSoundEntity::GetRender() const
{
	return render.c_str();
}

const char* CSoundEntity::GetName() const {
	return name.c_str();
}

unsigned int CSoundEntity::GetUID() const
{
	return UID;
}

void CSoundEntity::SetUID(unsigned int ID)
{
	UID = ID;
}

