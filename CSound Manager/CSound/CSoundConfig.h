#pragma once
#include <string>

namespace CsoundManagerNS
{
	// Root directory for all Csound resources
	const std::string ROOT = "./Resources/CSound/";

	namespace ResourcePath
	{
		// Directory where Score files will be generated
		const std::string GENERATED_CSD = ROOT + "Generated Synthesizers/";

		// Directory where Score files will be generated
		const std::string CSD_DATABASE = ROOT + "Synthesizers/";

		// Directory for HRTF database - not used
		const std::string HRTF_DATABASE = ROOT + "hrtf/";

		// Directory for Csound Plugins
		const std::string PLUGINS_WIN_X86 = ROOT + "plugins/win32";
		const std::string PLUGINS_WIN_X64 = ROOT + "plugins/win64";
		const std::string PLUGINS_ARM_X64 = ROOT + "plugins/arm";
		const std::string PLUGINS_LINUX_X64 = ROOT + "plugins/linux";
	};

	namespace ResourceFile
	{
		// XML file for loading defined instrument components
		const std::string TEMPLATES_CONFIG_XML = ROOT + "components.xml";

		// XML file for loading defined instruments
		const std::string INSTRUMENTS_CONFIG_XML = ROOT + "instruments.xml";

		// XML file for loading defined CsoundSynthesizer
		const std::string SCORES_CONFIG_XML = ROOT + "scores.xml";

		// Default configuration for a score file
		const std::string SYNTHESIZER_OPTIONS = ROOT + "score-config.xml";
	}

	class Version
	{
		public:
			Version() = delete;

			static std::string GetVersion();
			static void LogInfo();

		public:
			static const unsigned int MAJOR = 0;
			static const unsigned int MINOR = 1;
			static const unsigned int REVISION = 1;
	};
}