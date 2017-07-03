#include "CSoundConfig.h"

#include <iostream>

#include <csound/version.h>

using namespace std;
using namespace CsoundManagerNS;

std::string Version::GetVersion()
{
	char version[20];
	sprintf(version, "%d.%d.%d", MAJOR, MINOR, REVISION);
	return version;
}

void Version::LogInfo()
{
	cout << endl;
	cout << "========== ========== ==========" << endl;
	cout << "CSoundManager " << GetVersion() << endl;
	cout << CS_PACKAGE_STRING << endl;
	cout << "========== ========== ==========" << endl;
	cout << endl;
}
