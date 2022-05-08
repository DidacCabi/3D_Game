#include "game.h"
#include "utils.h"
#include "input.h"
#include "stage.h"


std::vector<Stage*> stages;

STAGE_ID current_stage = STAGE_ID::INTRO;




//Introduction Stage methods
STAGE_ID IntroStage::GetId() {
	return STAGE_ID::INTRO;
}

void IntroStage::render() {};

void IntroStage::update(float seconds_elapsed) {

	if (Input::isKeyPressed(SDL_SCANCODE_S)) {
		SetStage((STAGE_ID)TUTORIAL);

	}

};

//Tutorial Stage methods
STAGE_ID TutorialStage::GetId() {
	return STAGE_ID::TUTORIAL;
}

void TutorialStage::render() {};

void TutorialStage::update(float seconds_elapsed) {};

STAGE_ID PlayStage::GetId() {
	return STAGE_ID::PLAY;
};

void PlayStage::render() {};

void PlayStage::update(float seconds_elapsed) {};

STAGE_ID EndStage::GetId() {
	return STAGE_ID::END;
};

void EndStage::render() {};


Stage* GetStage(STAGE_ID id) {
	return stages[id];
};

Stage* GetCurrentStage() {
	return GetStage(current_stage);
};

void SetStage(STAGE_ID id) {
	current_stage = id;
};

void InitStages() {
	stages.reserve(4);
	stages.push_back(new IntroStage());
	stages.push_back(new TutorialStage());
	stages.push_back(new PlayStage());
	stages.push_back(new EndStage());
};


