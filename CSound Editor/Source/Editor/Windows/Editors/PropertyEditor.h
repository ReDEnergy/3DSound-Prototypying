#pragma once
// #include <Editor/Windows/Interface/DockWindow.h>
#include <Editor/Windows/Interface/CustomWidget.h>

class CSoundComponentProperty;

class PropertyEditor
	: public CustomWidget
{
	public:
		PropertyEditor();
		virtual ~PropertyEditor() {};

		void Init();
		void SetContext(CSoundComponentProperty *prop);

	private:
		void InitUI();
		void Update();

	private:
		CSoundComponentProperty *context;
		QLineEdit *value;
		QLineEdit *defaultValue;
		QComboBox *dropdown;
};