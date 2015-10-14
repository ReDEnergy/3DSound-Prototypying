#pragma once
#include <Editor/Windows/Interface/CustomWidget.h>

class CSoundComponentProperty;
class QLineEdit;
class QComboBox;

class CSoundPropertyEditor
	: public CustomWidget
{
	protected:
		CSoundPropertyEditor();
		virtual ~CSoundPropertyEditor() {};

	public:
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