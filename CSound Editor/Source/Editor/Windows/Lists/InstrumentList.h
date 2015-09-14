#pragma once
#include <Editor/Windows/Interface/EditorList.h>

class InstrumentList
	: public EditorList
{
	public:
		InstrumentList();
		virtual ~InstrumentList() {};

		void Init();

	private:
		void QtItemClicked(QListWidgetItem * item);
		void QtItemRenamed(QListWidgetItem * item);
}; 