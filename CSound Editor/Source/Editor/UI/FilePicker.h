#pragma once
#include <string>

#include <QFileDialog>
#include <QString>

class FilePicker : public QFileDialog
{
	public:
		FilePicker();
		virtual ~FilePicker() {};

		void OpenForSave();
		void OpenForLoad();
		bool IsSaving() const;

	private:

		enum class Reason {
			SAVE,
			LOAD
		};

		Reason reason;
};