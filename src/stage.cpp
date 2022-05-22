#include "game.h"
#include "utils.h"
#include "input.h"
#include "stage.h"
#include "entityMesh.h"
#include "camera.h"
#include "collision.h"
#include <iostream>
#include <fstream>

std::vector<Stage*> stages;

STAGE_ID currentStage = STAGE_ID::INTRO;

extern Shader* shader;
extern std::vector<EntityMesh*> platforms;
extern std::vector<EntityMesh*> staticObjects;
extern std::vector<Mesh*> platformMeshes;
extern std::vector<Texture*> platformTexs;
extern std::vector<Matrix44> platformModels;

std::vector<EntityMesh*> editorPlatforms;
extern EntityMesh* player;
Matrix44 playerModel;

bool mouse_locked;
extern bool cameraLocked;

int direction = 0;


//Introduction Stage methods
STAGE_ID IntroStage::GetId() {
	return STAGE_ID::INTRO;
}
void IntroStage::render() {
	drawText((Game::instance->window_width / 2) - 280, Game::instance->window_height / 2, "INTRO STAGE", Vector3(1, 1, 1), 10.0f);
};
void IntroStage::update(float seconds_elapsed) {
	if (Input::isKeyPressed(SDL_SCANCODE_S)) {
		SetStage((STAGE_ID)TUTORIAL);
	}
};



//Tutorial Stage methods
STAGE_ID TutorialStage::GetId() {
	return STAGE_ID::TUTORIAL;
}
void TutorialStage::render() {
	drawText((Game::instance->window_width / 2) - 290, Game::instance->window_height / 2, "TUTORIAL STAGE", Vector3(1, 0.5, 1), 8.0f);
};
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
	for (size_t i = 0; i < staticObjects.size(); i++)
	{
		EntityMesh* obj = staticObjects[i];
		obj->render();
	}
	player->render();
	player->model = playerModel;

};
void PlayStage::update(float seconds_elapsed) {

	float speed = seconds_elapsed * 50.0f; //mouse_speed, the speed is defined by the seconds_elapsed so it goes constant
	Vector3 playerVel;

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
		float playerSpeed = 10.0f * seconds_elapsed;
		if (Input::isKeyPressed(SDL_SCANCODE_W)) {
			//playerVel = playerVel + (Vector3(0,0,1) * playerSpeed);
			playerModel.translate(0.0f, 0.0f, playerSpeed);
			direction = 0;
		}
		if (Input::isKeyPressed(SDL_SCANCODE_S)) { 
			//playerVel = playerVel + (Vector3(0, 0, -1) * playerSpeed);
			playerModel.translate(0.0f, 0.0f, -playerSpeed);
			direction = 1;
		}
	
		if (Input::isKeyPressed(SDL_SCANCODE_A)) {
			//playerVel = playerVel + (Vector3(1, 0, 0) * playerSpeed);
			playerModel.translate(playerSpeed, 0.0f, 0.0f);
			direction = 2;
		}
		if (Input::isKeyPressed(SDL_SCANCODE_D)) { 
			//playerVel = playerVel + (Vector3(-1, 0, 0) * playerSpeed);
			playerModel.translate(-playerSpeed, 0.0f, 0.0f);
			direction = 3;
		}
		// TODO 
		if (Input::isKeyPressed(SDL_SCANCODE_SPACE) && canJump){
			jumpTime += seconds_elapsed;
			if (jumpTime < 1.0f) {
				//playerVel = playerVel + (Vector3(0, 1, 0) * playerSpeed);
				playerModel.translate(0.0f, 10.0f * seconds_elapsed, 0.0f); //JUMP
			}
			else {
				canJump = false; 
				jumpTime = 0;
			}
		}

		if (Input::wasKeyPressed(SDL_SCANCODE_E)) {   //Dash
			float boost = 80.0f;
			switch(direction){
			case 0:
				playerModel.translate(0.0f, 0.0f, playerSpeed * boost);
				break;
			case 1:
				playerModel.translate(0.0f, 0.0f, -playerSpeed * boost);
				break;
			case 2:
				playerModel.translate(playerSpeed * boost, 0.0f, 0.0f);
				break;
			case 3:
				playerModel.translate(-playerSpeed * boost, 0.0f, 0.0f);
				break;
			}
		}
	}
	else {
		//async input to move the camera around
		cameraMove(camera, speed);
	}

	float gravity = -6.0f * seconds_elapsed;    //implementació cutre de gravity
	if (player->getPosition().y > 0 && !Collision::testBelowPlayerColl(player)) {    //TODO
		//playerVel = playerVel + (Vector3(0, 0, -1) * gravity);
		playerModel.translate(0, gravity, 0);
	}
	else {
		canJump = true;
	}
	
	//Vector3 nextPos = player->getPosition() + playerVel;
	//Collision::testSidePlayerColl(player, nextPos, seconds_elapsed);

	//player->model.translate(nextPos.x, nextPos.y, nextPos.z);


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

	if (Input::wasKeyPressed(SDL_SCANCODE_G)) {
		if (!mode) {
			switch (objectNum) {
			case 0:
				renderInFront("data/platforms/blockSnow.obj", "data/platforms/blockSnow.png");
				break;
			case 1:
				renderInFront("data/platforms/blockSnowCliff.obj", "data/platforms/blockSnowCliff.png");
				break;
			case 2:
				renderInFront("data / platforms / blockRounded.obj", "data / platforms / blockRounded.png");
				break;
			}
		}
		else {
			switch (decorationNum) {
			case 0:
				renderInFront("data/decoration/SM_Env_Cactus_14_74.obj", "data/decoration/westernTex.png");
				break;
			case 1:
				renderInFront("data/decoration/SM_Env_Cactus_13_53.obj", "data/decoration/westernTex.png");
				break;
			case 2:
				renderInFront("data/decoration/SM_Env_Cactus_12_64.obj", "data/decoration/westernTex.png");
				break;
			case 3:
				renderInFront("data/decoration/SM_Env_Cactus_11_63.obj", "data/decoration/westernTex.png");
				break;
			case 4:
				renderInFront("data/decoration/SM_Env_Cactus_10_5.obj", "data/decoration/westernTex.png");
				break;
			case 5:
				renderInFront("data/decoration/SM_Env_Cactus_09_61.obj", "data/decoration/westernTex.png");
				break;
			case 6:
				renderInFront("data/decoration/SM_Env_Cactus_07_0.obj", "data/decoration/westernTex.png");
				break;
			}
		}
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_F)) {
		if (!mode) objectNum = (objectNum + 1) % 3;
		else decorationNum = (decorationNum + 1) % 7;
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_1)) mode = !mode;

	if (Input::wasKeyPressed(SDL_SCANCODE_T)) saveScene();
	if (Input::wasKeyPressed(SDL_SCANCODE_R)) readScene();

	if (Input::wasKeyPressed(SDL_SCANCODE_K)) selectObject();

	if (Input::wasKeyPressed(SDL_SCANCODE_X) && selected != NULL) selected->model.rotate(10 * DEG2RAD, Vector3(0, 1, 0));
	if (Input::wasKeyPressed(SDL_SCANCODE_Z) && selected != NULL) selected->model.rotate(-10 * DEG2RAD, Vector3(0, 1, 0));

	if (Input::isKeyPressed(SDL_SCANCODE_UP) && selected != NULL) selected->model.translate(0, 0, 0.5);
	if (Input::isKeyPressed(SDL_SCANCODE_DOWN) && selected != NULL) selected->model.translate(0, 0, -0.5);
	if (Input::isKeyPressed(SDL_SCANCODE_LEFT) && selected != NULL) selected->model.translate(0.5,0,0);
	if (Input::isKeyPressed(SDL_SCANCODE_RIGHT) && selected != NULL) selected->model.translate(-0.5, 0, 0);
	if (Input::isKeyPressed(SDL_SCANCODE_M) && selected != NULL) selected->model.translate(0, 0.5, 0);
	if (Input::isKeyPressed(SDL_SCANCODE_N) && selected != NULL) selected->model.translate(0, -0.5, 0);



};
void EditorStage::onKeyDown(SDL_KeyboardEvent event) { 
	switch (event.keysym.sym) {
		//case SDLK_g: renderInFront(platformMeshes[0], NULL); break;
		
	}

}
void EditorStage::renderInFront(std::string meshPath, std::string texPath) {
	Game* g = Game::instance;
	Camera* cam = g->camera;

	Vector2 mouse = Input::mouse_position;
	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, g->window_width, g->window_height);
	Vector3 rayOrigin = cam->eye;

	Vector3 spawnPos = RayPlaneCollision(Vector3(0,0,0), Vector3(0, 1, 0), rayOrigin, dir);
	Matrix44 model;
	model.translate(spawnPos.x, spawnPos.y, spawnPos.z);


	EntityMesh* entity = new EntityMesh(NULL, NULL, shader, Vector4(1, 1, 1, 1), meshPath, texPath);
	entity->model = model;
	editorPlatforms.reserve(1);
	editorPlatforms.push_back(entity);
}
void EditorStage::selectObject() {
	selected = Collision::RayPick(Game::instance->camera);
}
void EditorStage::saveScene() {
	std::ofstream myfile;
	printf("\nCreating file to save the actual editor scene...\n");
	myfile.open("editorScene1.txt");
	myfile << editorPlatforms.size() << "\n";
	for (size_t i = 0; i < editorPlatforms.size(); i++)
	{
		EntityMesh* platform = editorPlatforms[i];
		Matrix44 m = platform->model;
		
		for (int j = 0; j < 16; j++)  //print the model to the file
		{
			myfile << m.m[j] << ", ";
		}

		myfile << "\n" << platform->meshPath << "\n";
		myfile << platform->texPath  << "\n";
	}

	myfile.close();
}
void EditorStage::readScene() {
	std::string line;
	std::ifstream myfile;
	Matrix44 model;
	std::string meshPath;
	std::string texPath;
	printf("\nOpening file to charge the saved editor scene...\n");
	myfile.open("editorScene1.txt");
	if (myfile.is_open())
	{
		std::getline(myfile, line);
		int numObjects = std::stoi(line);
		for (size_t i = 0; i < numObjects; i++)
		{
			for (int j = 0; j < 16; j++) {
				std::getline(myfile, line, ',');
				std::cout << "Element of the model : "<< line << "\n"; 
				model.m[j] = std::atof(line.c_str());
			}
			std::getline(myfile, line);
			std::getline(myfile, line);
			std::cout << "MeshPath: " << line << "\n";
			meshPath = line;
			std::getline(myfile, line);
			std::cout << "TexPath: " << line << "\n";
			texPath = line;
			
			EntityMesh* object = new EntityMesh(NULL, NULL, shader, Vector4(1,1,1,1), meshPath, texPath);
			object->model = model;
			staticObjects.push_back(object);
		}
		myfile.close();
	}

	else std::cout << "\n[!]Unable to open file";
}

		
STAGE_ID EndStage::GetId() {
	return STAGE_ID::END;
};
void EndStage::render() {
	drawText((Game::instance->window_width / 2) - 290, Game::instance->window_height / 2, "END STAGE", Vector3(0.6, 0.5, 1), 8.0f);
};
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
	if (Input::isKeyPressed(SDL_SCANCODE_W)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_S)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_A)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_D)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_E)) camera->move(Vector3(0.0f, -1.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_Q)) camera->move(Vector3(0.0f, 1.0f, 0.0f) * speed);
}