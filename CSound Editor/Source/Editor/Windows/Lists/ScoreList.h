#pragma once
#include <Editor/EditorForward.h>
#include <Editor/Windows/Interface/EditorList.h>

class ScoreList
	: public EditorList
{
	public:
		ScoreList();
		virtual ~ScoreList() {};

		void Init();

	private:
		void AddItem();
		void SetDefaultScore();
		void QtItemClicked(QListWidgetItem * item);
		void QtItemRenamed(QListWidgetItem * item);
};