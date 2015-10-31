#include "HrtfRecorder.h"

#include <ctime>
#include <algorithm>
#include <random>
#include <chrono>

#include <3DWorld/Csound/CSound3DSource.h>
#include <3DWorld/Csound/CSoundScene.h>

#include <Csound/CSoundComponentProperty.h>
#include <Csound/CSoundManager.h>
#include <Csound/SoundManager.h>
#include <Csound/CSoundScore.h>
#include <Csound/CSoundInstrument.h>

#include <CSoundEditor.h>

// Engine
#include <include/glm.h>
#include <include/utils.h>
#include <include/glm_utils.h>
#include <Core/GameObject.h>
#include <Core/Camera/Camera.h>
#include <Component/Transform/Transform.h>
#include <Component/Transform/FreezeTransform.h>
#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/EventSystem.h>
#include <UI/ColorPicking/ColorPicking.h>

static TimerManager<string> *dynamicEvents;
static ProjectionInfo cameraProjection;
static Transform *savedTransform;


HrtfRecorder::HrtfRecorder()
{
	dynamicEvents = Manager::GetEvent()->GetDynamicTimers();
	waitEvent = nullptr;
	playbackEvent = nullptr;
	started = false;

	playbackEvent	= dynamicEvents->Create("HRTF-test-wait", config.samplePlaybackDuration);
	waitEvent		= dynamicEvents->Create("HRTF-test-play", config.sampleInterval);
	prepareEvent	= dynamicEvents->Create("HRTF-test-start", config.prepareTime);
	ResetOptions();

	SubscribeToEvent("start-HRTF-test");
	SubscribeToEvent("stop-HRTF-test");
	SubscribeToEvent(playbackEvent->GetChannel());
	SubscribeToEvent(waitEvent->GetChannel());
	SubscribeToEvent(prepareEvent->GetChannel());
}

void HrtfRecorder::Start(HrtfTestConfig config)
{
	this->config = config;

	Stop();
	started = true;

	// Settup Scene
	CSoundEditor::GetScene()->Clear();
	Manager::GetEvent()->EmitAsync("UI-clear-scene");

	sceneCamera = Manager::GetScene()->GetActiveCamera();
	cameraProjection = sceneCamera->GetProjectionInfo();
	savedTransform = sceneCamera->transform;

	sceneCamera->transform->SetWorldPosition(glm::vec3(0, 4, -10));
	sceneCamera->transform->SetWorldRotation(glm::vec3(0, 0, 0));
	sceneCamera->SetPerspective(100, cameraProjection.aspectRatio, 0.1f, 50);
	sceneCamera->transform = new FreezeTransform(*sceneCamera->transform);

	gameObj = new CSound3DSource();
	gameObj->SetName("HRTF source");
	sceneCamera->AddChild(gameObj);

	// Event configure
	playbackEvent->SetNewInterval(config.samplePlaybackDuration);
	prepareEvent->SetNewInterval(config.prepareTime);
	waitEvent->SetNewInterval(config.sampleInterval);

	// Clear test buffers
	positions.clear();
	answers.clear();

	uint azSize = config.azimuthValues.size();
	uint elSize = config.elevationValues.size();

	uint nrSamples = config.randomTest ? azSize * elSize * config.randomIterations : azSize;

	// Setup answers list
	for (unsigned int i = 0; i < nrSamples; i++) {
		AnswerEntry A;
		A.source = glm::vec2(0);
		A.responseTime = 0;
		A.correct = false;
		A.answer = glm::vec2(0);
		answers.push_back(A);
	}


	// Generate random test
	if (config.randomTest) {

		vector<glm::vec2> values;
		for (uint i = 0; i < azSize; i++)
		{
			for (uint j = 0; j < elSize; j++)
			{
				values.push_back(glm::vec2(config.azimuthValues[i], config.elevationValues[j]));
			}
		}

		unsigned int seed = (unsigned int) chrono::system_clock::now().time_since_epoch().count();
		uint sampleID = 0;

		for (uint k = 0; k < config.randomIterations; k++)
		{
			shuffle(values.begin(), values.end(), default_random_engine(seed));
			for (auto v : values) {
				positions.push_back(ComputePosition(v.x, v.y));
				answers[sampleID].source = glm::vec2(v);
				sampleID++;
			}
		}
	}
	else {
		for (unsigned int i = 0; i < nrSamples; i++)
		{
			positions.push_back(ComputePosition(config.azimuthValues[i], config.elevationValues[i]));
			answers[i].source = glm::vec2(config.azimuthValues[i], config.elevationValues[i]);
		}
	}

	// Setup scores
	auto sampleDuration = config.samplePlaybackDuration + config.sampleInterval;
	auto score = SoundManager::GetCSManager()->GetScore("test-hrtf");
	//auto instr = score->GetEntries().front();
	//instr->SetDuration((int)ceil(positions.size() * sampleDuration));
	//score->Save();
	gameObj->SetSoundModel(score);


	// Settup Scene
	sceneCamera->RemoveChild(gameObj);
	CSoundEditor::GetScene()->AddSource(gameObj);
	Manager::GetPicker()->SetSelectedObject(gameObj);
	Manager::GetEvent()->EmitAsync("UI-add-object", gameObj);


	// Sample index
	dynamicEvents->Add(*prepareEvent);
	cout << "Test starting in " << config.prepareTime << " seconds. Please be ready" << endl;
	timelinePos = 0;
}

bool HrtfRecorder::VerifyAnswer(float azimuth, float elevation)
{
	if (!started) return false;

	AnswerEntry &A = answers[timelinePos];
	A.answer = glm::vec2(azimuth, elevation);

	cout << "[SOURCE] " << A.source << endl;
	cout << "[ANSWER] " << A.answer << endl;

	if (A.source == A.answer) {
		cout << "[ANSWER] - CORRECT" << endl << endl;
		A.correct = true;
	}
	else {
		cout << "[ANSWER] - WRONG" << endl << endl;
	}

	A.responseTime = float(glfwGetTime() - startTimeCurrentTest);

	WaitForNextSample();
	
	return A.correct;
}

glm::vec3 HrtfRecorder::ComputePosition(float azimuth, float elevation)
{
	auto cameraPos = sceneCamera->transform->GetWorldPosition();
	float dist = -cameraPos.z;
	glm::vec3 offset;

	float tanElev = tan(elevation * TO_RADIANS);

	if (azimuth == 90.0f)
	{
		offset = glm::vec3(1, -tanElev, 0) * dist;
	} 
	else if (azimuth == -90.0f)
	{
		offset = glm::vec3(-1, -tanElev, 0) * dist;
	} 
	else
	{
		float ox = tan(azimuth * TO_RADIANS);
		float oy = tanElev / cos(azimuth * TO_RADIANS);
		offset = glm::vec3(ox, oy, -1) * dist;
	}

	gameObj->transform->SetLocalPosition(offset);

	return gameObj->transform->GetWorldPosition();
}

void HrtfRecorder::ClearEvents()
{
	dynamicEvents->Remove(*playbackEvent);
	dynamicEvents->Remove(*waitEvent);
	dynamicEvents->Remove(*prepareEvent);
}

void HrtfRecorder::PlayCurrentSample()
{
	if (timelinePos >= positions.size()) {
		Stop();
		return;
	}

	startTimeCurrentTest = glfwGetTime();
	gameObj->transform->SetWorldPosition(positions[timelinePos]);
	gameObj->SetVolume(100);
	dynamicEvents->Add(*playbackEvent);
	dynamicEvents->Remove(*waitEvent);
	printf("[TEST %d] started\n", timelinePos + 1);
}

void HrtfRecorder::WaitForNextSample()
{
	if (timelinePos + 1 >= positions.size()) {
		Stop();
		return;
	}

	gameObj->transform->SetWorldPosition(sceneCamera->transform->GetWorldPosition() + glm::vec3(0, 0, 100));
	gameObj->SetVolume(0);
	dynamicEvents->Remove(*playbackEvent);

	dynamicEvents->Add(*waitEvent);
	printf("Prepare: [TEST %d]\t", timelinePos + 2);
	cout << "playback in " << waitEvent->GetTriggerInterval() << " sec" << endl;
}

void HrtfRecorder::Update()
{
}

void HrtfRecorder::Stop()
{
	if (!started) return;

	started = false;
	gameObj->StopScore();
	gameObj->transform->SetWorldPosition(glm::vec3(0, 1, 0));
	ClearEvents();
	sceneCamera->SetProjection(cameraProjection);
	SAFE_FREE(sceneCamera->transform);
	sceneCamera->transform = savedTransform;
	Manager::GetEvent()->EmitSync("HRTF-test-end", nullptr);

	SaveTestAnswers();
	ExportTestResultsAsCSV();
}

void HrtfRecorder::SetSoundFile(const char * soundFile)
{
}

void HrtfRecorder::ResetOptions()
{
	config = HrtfTestConfig();
}

void HrtfRecorder::SaveTestAnswers() const
{
	time_t now = time(0);
	char timeString[32];
	strftime(timeString, 32, "%d-%b [%Hh%Mm].txt", std::localtime(&now));

	string fileName = "HRTF-Tests/" + (config.testName.size() ? config.testName + " " : "") + timeString;

	FILE *F = fopen(fileName.c_str(), "w");

	if (!F)
	{
		cout << "[ERROR] File could not be oppened" << endl;
		return;
	}

	fprintf(F, "HRTF test\n");
	fprintf(F, "----------------------\n");

	unsigned int correct = 0;
	unsigned int correctAzimuth = 0;
	unsigned int correctElevation = 0;
	unsigned int size = answers.size();
	unsigned int index = 0;

	for (auto &A : answers) {
		if (A.correct) {
			correct++;
			correctAzimuth++;
			correctElevation++;
		}
		else {
			if (A.source.x == A.answer.x) correctAzimuth++;
			if (A.source.y == A.answer.y) correctElevation++;
		}
	}

	fprintf(F, "Test size:\t %d\n", size);
	fprintf(F, "Correct:\t %d\n", correct);
	fprintf(F, "Wrong:  \t %d\n", size - correct);
	fprintf(F, "----------------------\n");
	fprintf(F, "Correct Azimuth:\t %d\n", correctAzimuth);
	fprintf(F, "Correct Elevation:\t %d\n", correctElevation);

	fprintf(F, "\n");

	for (auto &A : answers)
	{
		index++;
		fprintf(F, "Test %d\t\t", index);

		if (A.correct) {
			fprintf(F, "[CORRECT]\n");
		}
		else {
			fprintf(F, "[WRONG]\n");
		}

		fprintf(F, "----------------------\n");
		fprintf(F, "Location:\t %.2f\t%.2f\n", A.source.x, A.source.y);
		fprintf(F, "Response:\t %.2f\t%.2f\n", A.answer.x, A.answer.y);
		fprintf(F, "Time:\t %.3f seconds\n\n", A.responseTime);
	}

	fclose(F);
}

void HrtfRecorder::ExportTestResultsAsCSV() const
{
	time_t now = time(0);
	char timeString[32];
	strftime(timeString, 32, "%d-%b [%Hh%Mm].csv", std::localtime(&now));

	string fileName = "HRTF-Tests/" + (config.testName.size() ? config.testName + " " : "") + timeString;

	FILE *F = fopen(fileName.c_str(), "w");

	if (!F)
	{
		cout << "[ERROR] File could not be oppened" << endl;
		return;
	}

	fprintf(F, "Test number,Source Azimuth,Source Elevation,Response Azimuth,Response Elevation,Response time\n");

	unsigned int index = 0;
	for (auto &A : answers)
	{
		index++;
		fprintf(F, "%d,", index);
		fprintf(F, "%.2f,%.2f,", A.source.x, A.source.y);
		fprintf(F, "%.2f,%.2f,", A.answer.x, A.answer.y);
		fprintf(F, "%.2f\n", A.responseTime);
	}

	fclose(F);
}

void HrtfRecorder::OnEvent(const string& eventID, void * data)
{
	if (eventID.compare("start-HRTF-test") == 0) {
		Start(*(HrtfTestConfig*)data);
		return;
	}

	if (eventID.compare("stop-HRTF-test") == 0) {
		Stop();
		return;
	}

	if (eventID.compare(playbackEvent->GetChannel()) == 0) {
		gameObj->transform->SetWorldPosition(sceneCamera->transform->GetWorldPosition() + glm::vec3(0, 0, 100));
		dynamicEvents->Remove(*playbackEvent);

		if (config.waitForInput) {
			return;
		}

		WaitForNextSample();
		return;
	};

	if (eventID.compare(waitEvent->GetChannel()) == 0) {
		timelinePos++;
		PlayCurrentSample();
		return;
	};

	// Start playback
	if (eventID.compare(prepareEvent->GetChannel()) == 0) {
		dynamicEvents->Remove(*prepareEvent);
		CSoundEditor::GetScene()->Play();
		PlayCurrentSample();
	}
}