#pragma once
#include <unordered_map>
#include <string>

#include <CSound/CSoundForward.h>
#include <templates/ReferenceCounter.h>

class CSoundScene
{
	public:
		CSoundScene();
		~CSoundScene();

	public:
		void Init();
		void Update();
		void Clear();
		void LoadScene(const char* fileName);
		bool SaveScene();
		void SaveSceneAs(const char* fileName);
		void Play();
		void Stop();

		bool Rename(CSound3DSource* S3D, const char* newName);
		void Remove(CSound3DSource* S3D);

		void AddSource(CSound3DSource * S3D);
		CSound3DSource *CreateSource();
		CSoundScore *GetDefaultScore();
		void SetDefaultScore(CSoundScore *score);
		void SetOutputModel(const char* name);

		void AutoSave();

		void TrackScore(CSoundScore* score);
		void TrackInstrument(CSoundInstrument * instr);
		void UnTrackScore(CSoundScore* score);

		void UnTrackInstrument(CSoundInstrument * instr);

	public:
		EntityStorage<CSound3DSource> *_3DSources;
		ReferenceCounter<CSoundScore> sceneScores;
		ReferenceCounter<CSoundInstrument> sceneInstruments;

	private:
		CSoundScore *defaultScore;
		string sceneFile;
};