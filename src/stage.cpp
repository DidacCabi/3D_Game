#include "game.h"
#include "utils.h"
#include "input.h"
#include "stage.h"
#include "entityMesh.h"
#include "camera.h"

std::vector<Stage*> stages;

STAGE_ID currentStage = STAGE_ID::INTRO;

extern Shader* shader;
extern std::vector<EntityMesh*> platforms;
extern std::vector<Mesh*> platformMeshes;
extern std::vector<Matrix44> platformModels;
std::vector<EntityMesh*> editorPlatforms;

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



STAGE_ID EditorStage::GetId() {
	return STAGE_ID::EDITOR;
};
void EditorStage::render() {
	for (int i = 0; i < editorPlatforms.size(); i++) {
		editorPlatforms[i]->render();
	}
};
void EditorStage::update(float seconds_elapsed) {
if(Input::wasKeyPressed(SDL_SCANCODE_G))renderInFront();
};
void EditorStage::onKeyDown(SDL_KeyboardEvent event) {
	switch (event.keysym.sym) {
		case SDLK_g: renderInFront(); break;
	}
}
void EditorStage::renderInFront() {
	Camera* cam = Game::instance->camera;
	Vector3 spawnPos = cam->center;
	Matrix44 model;
	model.translate(spawnPos.x, spawnPos.y, spawnPos.z);

	EntityMesh* entity = new EntityMesh(platformMeshes[0],NULL,shader,Vector4(1,1,1,1));
	entity->model = model;
	editorPlatforms.push_back(entity);
}



STAGE_ID EndStage::GetId() {
	return STAGE_ID::END;
};
void EndStage::render() {};
void EndStage::update(float seconds_elapsed) {};



Stage* GetStage(STAGE_ID id) {
	return stages[id];
};

Stage* GetCurrentStage() {
	return GetStage(currentStage);
};

void SetStage(STAGE_ID id) {
	currentStage = id;
};

void InitStages() {
	stages.reserve(4);
	stages.push_back(new IntroStage());
	stages.push_back(new TutorialStage());
	stages.push_back(new PlayStage());
	stages.push_back(new EditorStage());
	stages.push_back(new EndStage());
};


