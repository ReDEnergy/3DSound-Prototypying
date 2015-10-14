#pragma once
#include <unordered_map>

using namespace std;

#include <CSound/CSoundForward.h>

class CSoundScene;
class Game;

class CSoundEditor
{
	private:
		CSoundEditor() {};
		~CSoundEditor() {};

	public:
		static void Init();
		static CSoundScene* GetScene();
		static Game* GetGame();
		static string GetStyleSheetFilePath(const char* fileName);

	private:
		static CSoundScene *Scene;
		static Game *game;
		static CSoundInstrumentBlock *outputModel;
};