#pragma once

#include <templates/singleton.h>

#include <3DWorld/Game.h>
#include <3DWorld/CSound/CsoundScene.h>

#include <Editor/QT/Utils.h>

#include <Editor/GUI.h>

#include <Editor/Windows/SceneWindow.h>
#include <Editor/Windows/Editors/ComponentEditor.h>
#include <Editor/Windows/Editors/InstrumentEditor.h>
#include <Editor/Windows/Editors/ScoreEditor.h>
#include <Editor/Windows/Editors/PropertyEditor.h>
#include <Editor/Windows/Editors/SceneObjectProperties.h>
#include <Editor/Windows/Features/HrtfTest.h>
#include <Editor/Windows/Features/MovingPlane/MovingPlaneWindow.h>
#include <Editor/Windows/Interface/QtListTemplates.h>
#include <Editor/Windows/Interface/DockWindow.h>
#include <Editor/Windows/Interface/CustomWidget.h>
#include <Editor/Windows/Lists/ComponentList.h>
#include <Editor/Windows/Lists/InstrumentList.h>
#include <Editor/Windows/Lists/ScoreList.h>
#include <Editor/Windows/TextPreview.h>
#include <Editor/Windows/OpenGL/GameWindow.h>

#include <Editor/Events/UIEvents.h>

#include <Editor/UI/GUIEntity.h>
#include <Editor/UI/FilePicker.h>

#include <CSoundEditor.h>