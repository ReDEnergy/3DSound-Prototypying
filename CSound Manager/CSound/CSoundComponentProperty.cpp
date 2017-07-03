#include "CSoundComponentProperty.h"

#include <CSound/CSoundComponent.h>

CSoundComponentProperty::CSoundComponentProperty(CSoundComponent * parent)
{
	this->parent = parent;
}

CSoundComponentProperty::CSoundComponentProperty(const CSoundComponentProperty & Prop)
{
	parent = nullptr;
	SetValue(Prop.GetValue());
	SetName(Prop.GetName());
	SetDefaultValue(Prop.GetDefaultValue());
}

bool CSoundComponentProperty::IsValueSet() const
{
	if (value.length() == 0)
		return false;

	return value != defaultValue;
}

const char * CSoundComponentProperty::GetValue() const
{
	if (value.length() == 0)
		return defaultValue.c_str();
	return value.c_str();
}

const char * CSoundComponentProperty::GetDefaultValue() const
{
	return defaultValue.c_str();
}

void CSoundComponentProperty::SetValue(const char * value)
{
	this->value = value;
	if (parent)
		parent->Update();
}

void CSoundComponentProperty::SetName(const char * name)
{
	//bool ctrl = (!this->name.compare("control")) || (!strcmp(name, "control"));
	CSoundEntity::SetName(name);
	if (parent) {
		parent->Update();
	}
}

void CSoundComponentProperty::SetDefaultValue(const char * value)
{
	defaultValue = value;
}

void CSoundComponentProperty::SetParent(CSoundComponent *component)
{
	parent = component;
}
