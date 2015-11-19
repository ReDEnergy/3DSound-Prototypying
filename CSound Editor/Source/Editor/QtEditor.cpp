#include "QtEditor.h"

#include <QIcon>

namespace RESOURCE {
	const string ICONS = "Resources/Icons/";
}

QIcon* QtEditor::GetIcon(const char* fileName)
{
	return new QIcon((RESOURCE::ICONS + fileName).c_str());
}
