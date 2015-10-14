#include "UIEvents.h"

#include <Editor/Editor.h>

UIEvents::UIEvents()
{
	SubscribeToEvent("UI-clear-scene");
	SubscribeToEvent("UI-add-object");
}

UIEvents::~UIEvents()
{
}

void UIEvents::OnEvent(const string& eventID, void * data)
{
	if (eventID.compare("UI-clear-scene") == 0) {
		GUI::Get<SceneWindow>(QT_INSTACE::SCENE_EDITOR)->Clear();
	}

	if (eventID.compare("UI-add-object") == 0) {
		GUI::Get<SceneWindow>(QT_INSTACE::SCENE_EDITOR)->InsertItem((CSound3DSource*)data);
	}
}
