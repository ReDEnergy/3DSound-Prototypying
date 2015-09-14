#pragma once
#include <Editor/Windows/Interface/EditorList.h>

class ComponentList
	: public EditorList
{
	public:
		ComponentList();
		virtual ~ComponentList() {};

		void Init();

	private:
		void QtItemClicked(QListWidgetItem * item);
		void QtItemRenamed(QListWidgetItem * item);
}; 