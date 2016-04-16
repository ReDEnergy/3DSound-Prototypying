#pragma once
#include <string>

#include <include/dll_export.h>

class DLLExport CSoundEntity
{
	public:
		CSoundEntity(const char* name = "_entity");
		virtual ~CSoundEntity();

		virtual void SetName(const char* name);
		bool Rename(const char* name);
		virtual void Update() {};

		virtual const char* GetRender() const;
		const char* GetName() const;
		unsigned int GetUID() const;
		void SetUID(unsigned int ID);

	private:
		unsigned int UID;

	protected:
		std::string render;
		std::string name;
};
