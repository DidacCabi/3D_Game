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
extern std::vector<Texture*> platformTexs;
extern std::vector<Matrix44> platformModels;
std::vector<EntityMesh*> editorPlatforms;
extern EntityMesh* player;
Matrix44 playerModel;

bool mouse_locked;
extern bool cameraLocked;

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
void PlayStage::render() {
	if (cameraLocked) {
		Vector3 eye = player->model * Vector3(0.0f, 3.0f, -6.0f);
		Vector3 center = player->model * Vector3(0.0f, 0.0f, 10.0f);
		Vector3 up = player->model.rotateVector(Vector3(0.0f, 1.0f, 0.0f));
		Game::instance->camera->lookAt(eye, center, up);
	}
	player->render();
	player->model = playerModel;

};
void PlayStage::update(float seconds_elapsed) {

	float speed = seconds_elapsed * 50.0f;//mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	Camera* camera = Game::instance->camera;
	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
	{
		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_TAB)) {
		cameraLocked = !cameraLocked;
	}

	if (cameraLocked) {
		float planeSpeed = 50.0f * seconds_elapsed;
		float rotSpeed = 90.0f * DEG2RAD * seconds_elapsed;
		if (Input::isKeyPressed(SDL_SCANCODE_W)) playerModel.translate(0.0f, 0.0f, planeSpeed);
		if (Input::isKeyPressed(SDL_SCANCODE_S)) playerModel.translate(0.0f, 0.0f, -planeSpeed);

		if (Input::isKeyPressed(SDL_SCANCODE_A)) playerModel.rotate(-rotSpeed, Vector3(0.0f, 1.0f, 0.0f));
		if (Input::isKeyPressed(SDL_SCANCODE_D)) playerModel.rotate(rotSpeed, Vector3(0.0f, 1.0f, 0.0f));
		if (Input::isKeyPressed(SDL_SCANCODE_E)) playerModel.rotate(rotSpeed, Vector3(0.0f, 0.0f, 1.0f));
		if (Input::isKeyPressed(SDL_SCANCODE_Q)) playerModel.rotate(-rotSpeed, Vector3(0.0f, 0.0f, 1.0f));
	}
	else {
		//async input to move the camera around
		cameraMove(camera, speed);
	}

	/*if (Input::wasKeyPressed(SDL_SCANCODE_F)) {
		bombAttached = false;
	}

	if (bombAttached) {
		bombModel = bombOffset * playerModel;
	}
	else {
		bombModel.translateGlobal(0.0f, -9.8f * elapsed_time * 4, 0.0f);
	}*/
};



STAGE_ID EditorStage::GetId() {
	return STAGE_ID::EDITOR;
};
void EditorStage::render() {
	for (int i = 0; i < editorPlatforms.size(); i++) {
		editorPlatforms[i]->mesh->renderBounding(editorPlatforms[i]->model);
		editorPlatforms[i]->render();
	}
};
void EditorStage::update(float seconds_elapsed) {
	float speed = 0.5f;
	Camera* camera = Game::instance->camera;

	if (Input::mouse_state & SDL_BUTTON_LEFT) //is left button pressed?
	{
		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}

	cameraMove(camera, speed);

	if(Input::wasKeyPressed(SDL_SCANCODE_G)) renderInFront(platformMeshes[0], platformTexs[0]);
};
void EditorStage::onKeyDown(SDL_KeyboardEvent event) {
	switch (event.keysym.sym) {
		case SDLK_g: renderInFront(platformMeshes[0], NULL); break;
	}
}
void EditorStage::renderInFront(Mesh* mesh, Texture* tex) {
	Camera* cam = Game::instance->camera;
	Vector3 spawnPos = cam->center;
	Matrix44 model;
	model.translate(spawnPos.x, spawnPos.y, spawnPos.z);

	EntityMesh* entity = new EntityMesh(mesh,tex,shader,Vector4(1,1,1,1));
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


void cameraMove(Camera* camera, float speed) {
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_E)) camera->move(Vector3(0.0f, -1.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_Q)) camera->move(Vector3(0.0f, 1.0f, 0.0f) * speed);
}