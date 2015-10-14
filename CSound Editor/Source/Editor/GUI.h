#pragma once
#include <unordered_map>

using namespace std;

enum class QT_INSTACE {
	TEXT_PREVIEW,
	INSTRUMENT_COMPOSER,
	INSTRUMENT_LIST,
	COMPONENT_COMPOSER,
	COMPONENT_LIST,
	SCORE_COMPOSER,
	SCORE_LIST,
	SCENE_EDITOR,
	PROPERTY_EDITOR,
	_3D_SCENE_WINDOW,
	FILE_PICKER,
};

class GUI
{
	private:
		GUI() {};
		~GUI() {};

	public:
		static void Init();

		static void Set(QT_INSTACE key, void* instace);

		template <class T>
		static T* Get(QT_INSTACE key);

	private:
		static unordered_map<QT_INSTACE, void*> instances;
};

template<class T>
inline T* GUI::Get(QT_INSTACE key)
{
	if (instances.find(key) != instances.end()) {
		return (T*)(instances[key]);
	}

	return nullptr;
}


