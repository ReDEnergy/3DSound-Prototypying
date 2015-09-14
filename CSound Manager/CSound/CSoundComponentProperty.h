#pragma once
#include <string>

#include <CSound/CSoundForward.h>
#include <CSound/CSoundEntity.h>

class DLLExport CSoundComponentProperty
	: public CSoundEntity
{
	public:
		CSoundComponentProperty(CSoundComponent *parent);
		CSoundComponentProperty(const CSoundComponentProperty &Prop);
		~CSoundComponentProperty() {};

		bool IsValueSet() const;
		const char *GetValue() const;
		const char *GetDefaultValue() const;

		void SetValue(const char *value);
		void SetName(const char *name);
		void SetDefault(const char *value);
		void SetParent(CSoundComponent *component);

	private:
		const char *GetDefaut() const;

	private:
		CSoundComponent *parent;
		string value;
		string default;
};

