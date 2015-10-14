#pragma once
#include <Editor/Windows/Interface/DockWindow.h>
#include <Editor/Windows/Interface/CustomWidget.h>

class CSoundComponentProperty;

class ComponentComposer;
class CSoundPropertyEditor;

class ComponentEditor
	: public DockWindow
{
	public:
		ComponentEditor();
		virtual ~ComponentEditor() {};

		void Init();

	private:
		void InitUI();

	private:
		ComponentComposer *composer;
		CSoundPropertyEditor *propertyEditor;
};