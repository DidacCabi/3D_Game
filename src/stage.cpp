#include "game.h" 
#include "utils.h"
#include "input.h"
#include "stage.h"
#include "entityMesh.h"
#include "camera.h"
#include "collision.h"
#include "ai.h"
#include <iostream>
#include <fstream>
#include "gui.h"


std::vector<Stage*> stages;

STAGE_ID currentStage = STAGE_ID::INTRO;

int level = 0;
int levels = 4;
float jumpCounter;

extern Shader* shader;
extern std::vector<EntityMesh*> platforms;
extern std::vector<EntityMesh*> staticObjects;

extern EntityMesh* ground;
extern EntityMesh* lavaGround;
extern EntityMesh* sky;
extern EntityMesh* jetpack;
extern EntityMesh* aiSun;
extern EntityMesh* npc;
extern EntityMesh* finalnpc;
extern EntityMesh* wall;
extern EntityMesh* fence;
extern EntityMesh* pool;
extern EntityMesh* volcano;

extern Animation* idle;
extern Animation* walk;
extern Animation* dance;

std::vector<EntityMesh*> editorPlatforms;
std::vector<EntityMesh*> decoration;
extern EntityMesh* player;
sPlayer playerStruct;
Matrix44 playerModel;

bool lvl3isLoaded = false;
bool isWin = false;
bool menuSong = false;

bool mouse_locked;
extern bool cameraLocked;


HCHANNEL ostChannel;
HCHANNEL lvls123SongChannel;
HCHANNEL lvl4SongChannel;

float introTime = 2.0f;
//Introduction Stage methods
STAGE_ID IntroStage::GetId() {
	return STAGE_ID::INTRO;
}
void IntroStage::render() {
	GUI::RenderIntroGUI();
};
void IntroStage::update(float seconds_elapsed) {
	if (!menuSong) {
		ostChannel = PlayGameSound("data/ost.wav", true);
		menuSong = true;
	}
	introTime -= seconds_elapsed;
	if (introTime < 0) SetStage(STAGE_ID::MENU);
};


//Introduction Stage methods
STAGE_ID MenuStage::GetId() {
	return STAGE_ID::MENU;
}
void MenuStage::render() {
	GUI::RenderMenuGUI();
};
void MenuStage::update(float seconds_elapsed) {
	
	float mouseX = Input::mouse_position.x;
	float mouseY = Input::mouse_position.y;

	if (315 < mouseX && mouseX < 485 && 260 < mouseY && mouseY < 340 && (Input::mouse_state & SDL_BUTTON_LEFT)) {
		BASS_ChannelStop(ostChannel);
		lvls123SongChannel = PlayGameSound("data/level1_2_3.wav",true);
		SetStage(STAGE_ID::LOADING);
	}

	if (315 < mouseX && mouseX < 485 && 360 < mouseY && mouseY < 440 && (Input::mouse_state & SDL_BUTTON_LEFT)) SetStage(STAGE_ID::TUTORIAL);

};

//Introduction Stage methods
STAGE_ID TutoStage::GetId() {
	return STAGE_ID::TUTORIAL;
}
void TutoStage::render() {
	GUI::RenderTutoGUI();
};
void TutoStage::update(float seconds_elapsed) {

	float mouseX = Input::mouse_position.x;
	float mouseY = Input::mouse_position.y;
	if (318 < mouseX && mouseX < 482 && 160 < mouseY && mouseY < 240 && (Input::mouse_state & SDL_BUTTON_LEFT)) {
		BASS_ChannelStop(ostChannel);
		lvls123SongChannel = PlayGameSound("data/level1_2_3.wav", true);
		SetStage(STAGE_ID::LOADING);
	}

};


float loadTimer = 0.8f;
//Tutorial Stage methods
STAGE_ID LoadingStage::GetId() {
	return STAGE_ID::LOADING;
}
void LoadingStage::render() {
	GUI::RenderLoadingGUI();
};
void LoadingStage::update(float seconds_elapsed) {
	loadTimer -= seconds_elapsed;
	if (loadTimer < 0.0f) SetStage(STAGE_ID::PLAY);
};



STAGE_ID PlayStage::GetId() {
	return STAGE_ID::PLAY;
};
void PlayStage::render() {

	if (level == (levels - 1)) aiSun->render();
	if (level == 3) {              //npc pointing the right direction
		npc->model.setScale(0.01f, 0.01f, 0.01f);
		npc->model.rotate(-90 * DEG2RAD, Vector3(0, 1, 0));
		npc->model.translate(-10, 0, 0);
		npc->render();
	}

	if (!readedDecoration) {   //decoration
		readScene("decorationScene.txt", &decoration);
		readedDecoration = true;
	}

	if (level == 2)  //ground
		lavaGround->render(100.0f);
	else
		ground->render(80.0f); 

	sky->render();

	if (cameraLocked) {
		if (level == 1) {
			Vector3 eye = player->model * Vector3(0.0f, 15.0f, 0.0f);
			Vector3 center = player->model * Vector3(1.0f, 1.0f, 1.0f);
			Vector3 up = player->model.rotateVector(Vector3(0.0f, 1.0f, 1.0f));
			Game::instance->camera->lookAt(eye, center, up);
		}
		else if(level == 3) {
			Vector3 eye = player->model * Vector3(0.0f, 7.0f, -15.0f);
			Vector3 center = player->model * Vector3(0.0f, 0.0f, 10.0f);
			Vector3 up = player->model.rotateVector(Vector3(0.0f, 1.0f, 0.0f));
			Game::instance->camera->lookAt(eye, center, up);
		}
		else {
			Vector3 eye = player->model * Vector3(0.0f, 3.0f, -6.0f);
			Vector3 center = player->model * Vector3(0.0f, 0.0f, 10.0f);
			Vector3 up = player->model.rotateVector(Vector3(0.0f, 1.0f, 0.0f));
			Game::instance->camera->lookAt(eye, center, up);
		}
	}
	for (size_t i = 0; i < staticObjects.size(); i++)
	{
		EntityMesh* obj = staticObjects[i];
		obj->render();
	}
	for (size_t i = 0; i < decoration.size(); i++)
	{
		EntityMesh* obj = decoration[i];
		obj->render();
	}

	loadLevel(playerStruct.pos);
	playerStruct.scale = 0.1f;
	player->render();


	jetpack->render();
	GUI::RenderAllGUI(canJump);
	GUI::fillWaterSquare(level);

};
void PlayStage::update(float seconds_elapsed) {

	//std::cout << "level: " << level << std::endl;

	isJumping = false;
	float speed = seconds_elapsed * 50.0f; //mouse_speed, the speed is defined by the seconds_elapsed so it goes constant
	Matrix44 playerRotation;
	playerRotation.rotate(playerStruct.yaw * DEG2RAD, Vector3(0, 1, 0));
	Vector3 playerVel;
	Vector3 forward = playerRotation.rotateVector(Vector3(0, 0, -1));
	Vector3 right = playerRotation.rotateVector(Vector3(1, 0, 0));
	Vector3 up = playerRotation.rotateVector(Vector3(0, 1, 0));


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
			playerVel = playerVel - (forward * playerSpeed);
		}
		if (Input::isKeyPressed(SDL_SCANCODE_S)) {
			playerVel = playerVel + (forward * playerSpeed);
		}

		if (Input::isKeyPressed(SDL_SCANCODE_A)) {
			playerVel = playerVel + (right * playerSpeed);
		}
		if (Input::isKeyPressed(SDL_SCANCODE_D)) {
			playerVel = playerVel - (right * playerSpeed);
		}

		if (Input::isKeyPressed(SDL_SCANCODE_SPACE) && canJump) {   //jump
			jumpCounter -= seconds_elapsed;
			if (jumpCounter > 0.0f) {
				playerVel = playerVel + (up * playerSpeed);
				isJumping = true;
				//PlayGameSound("data/jetpack2.wav");
			}
			else {
				canJump = false;
				jumpCounter = jumpTime;
				if (level == 3) jumpCounter = 1.0f;
			}
		}

	}
	else {
		//async input to move the camera around
		cameraMove(camera, speed);
	}

	//std::cout << "jump timer: " << jumpCounter << std::endl;

	//std::cout << "col with ground: " << Collision::testBelowPlayerColl(player) << std::endl;
	float gravity = 9.0f * seconds_elapsed;
	if (playerStruct.pos.y > 0 && !Collision::testBelowPlayerColl(player) && !isJumping) {    //TODO
		playerVel = playerVel - (up * gravity);
	}
	else {
		canJump = true;
	}

	if (playerStruct.pos.y < 0.2f || Collision::testBelowPlayerColl(player)) {
		canJump = true;
		jumpCounter = jumpTime;
		if (level == 3) jumpCounter = 1.0f;
	}

	Vector3 sunColl, sunCollnorm;
	if(level == 3) 
		if (aiSun->mesh->testSphereCollision(aiSun->model, player->getPosition(), 1.5f, sunColl, sunCollnorm)) {  //check sun killing skeleton
			isWin = false;
			aiSun->model.setTranslation(0, 10, -20);
			SetStage(STAGE_ID::END);
		}

	Vector3 nextPos = playerStruct.pos + playerVel;
	if (!Collision::testBelowPlayerColl(player)) nextPos = Collision::testSidePlayerColl(player, playerStruct.pos, nextPos, seconds_elapsed, aiSun, level);
	if (nextPos.y < 0.1f) nextPos.y = 0.1f;
	if (nextPos.y < 0.2f && level == 2) { 
		isWin = false;
		PlayGameSound("data/game_over.wav", false);
		BASS_ChannelStop(lvls123SongChannel);
		SetStage(STAGE_ID::END); 
	}
	if (level == 1){ 
		nextPos.x = clamp(nextPos.x, -9, 27);
		nextPos.z = clamp(nextPos.z, -9, 28);
	}
	if (level == 3) {
		nextPos.x = clamp(nextPos.x, -4.2f, 9);
		nextPos.z = clamp(nextPos.z, 0, 300);
	}
	
	playerStruct.pos = nextPos;

	player->model.setTranslationVec(playerStruct.pos);
	player->model.rotate(playerStruct.yaw * DEG2RAD, Vector3(0, 1, 0));

	//if (playerVel.x != 0.0f || playerVel.y != 0.0f || playerVel.z != 0.0f) player->anim = walk;  //player animations
	//else player->anim = idle;

	jetpack->model.setTranslation(playerStruct.pos.x, playerStruct.pos.y + 0.73f, playerStruct.pos.z - 0.3f);

	if (level == (levels - 1)) AI::following_AI(seconds_elapsed);
};


STAGE_ID EditorStage::GetId() {
	return STAGE_ID::EDITOR;
};
void EditorStage::render() {

	player->render();
	jetpack->render();

	if (level == 2)  //ground
		lavaGround->render(50.0f);
	else
		ground->render(80.0f);

	for (int i = 0; i < staticObjects.size(); i++) {
		staticObjects[i]->mesh->renderBounding(staticObjects[i]->model);
		staticObjects[i]->render();
	}
	loadLevel(Vector3(0,0,0));

	drawText(Game::instance->window_width - 180, 10, "platforms left: " + std::to_string(objectsLeft[level]), Vector3(1, 1, 1), 2.0f);
};
void EditorStage::update(float seconds_elapsed) {

	float speed = 30.0f * seconds_elapsed;
	Camera* camera = Game::instance->camera;
	if (Input::mouse_state & SDL_BUTTON_LEFT) //is left button pressed?
	{
		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}

	cameraMove(camera, speed);

	if (Input::wasKeyPressed(SDL_SCANCODE_G)) {
		if (!mode && objectsLeft[level] > 0) {
			objectsLeft[level] -= 1;
			switch (objectNum) {
			case 0:
				renderInFront("data/platforms/blockSnow.obj", "data/platforms/blockSnow.png");
				break;
			case 1:
				renderInFront("data/platforms/blockSnowCliff.obj", "data/platforms/blockSnowCliff.png");
				break;
			case 2:
				renderInFront("data/platforms/blockRounded.obj", "data/platforms/blockRounded.png");
				break;
			case 3:
				renderInFront("data/platforms/tile-plain_sand_1.obj", "data/platforms/color-atlas-new.png");
				break;
			}
		}
		if (mode) {
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
			case 7:
				renderInFront("data/clouds/cloud-big_5.obj", "data/color-atlas-new.png");
				break;
			case 8:
				renderInFront("data/clouds/cloud-fluffy_0.obj", "data/color-atlas-new.png");
				break;
			case 9:
				renderInFront("data/clouds/cloud-long_4.obj", "data/color-atlas-new.png");
				break;
			case 10:
				renderInFront("data/clouds/cloud-small_2.obj", "data/color-atlas-new.png");
				break;
			case 11:
				renderInFront("data/clouds/cloud-triangle_3.obj", "data/color-atlas-new.png");
				break;
			}
		}
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_F)) {
		if (!mode) objectNum = (objectNum + 1) % 4;
		else decorationNum = (decorationNum + 1) % 12;
	}
	if (Input::wasKeyPressed(SDL_SCANCODE_1)) mode = !mode;

	if (Input::wasKeyPressed(SDL_SCANCODE_T) && !mode) saveScene("editorScene.txt");
	else if (Input::wasKeyPressed(SDL_SCANCODE_T) && mode) saveScene("decorationScene.txt");
	if (Input::wasKeyPressed(SDL_SCANCODE_R) && !mode) readScene("editorScene.txt", &staticObjects);
	else if (Input::wasKeyPressed(SDL_SCANCODE_R) && mode) readScene("decorationScene.txt", &decoration);


	if (Input::wasKeyPressed(SDL_SCANCODE_K)) selectObject();

	float rotateSpeed = 100.0f * seconds_elapsed;
	if (Input::isKeyPressed(SDL_SCANCODE_X) && selected != NULL) selected->model.rotate(rotateSpeed * DEG2RAD, Vector3(0, 1, 0));
	if (Input::isKeyPressed(SDL_SCANCODE_Z) && selected != NULL) selected->model.rotate(-rotateSpeed * DEG2RAD, Vector3(0, 1, 0));

	float objectMoveSpeed = 20.0f * seconds_elapsed;
	if (Input::isKeyPressed(SDL_SCANCODE_UP) && selected != NULL) selected->model.translate(0, 0, objectMoveSpeed);
	if (Input::isKeyPressed(SDL_SCANCODE_DOWN) && selected != NULL) selected->model.translate(0, 0, -objectMoveSpeed);
	if (Input::isKeyPressed(SDL_SCANCODE_LEFT) && selected != NULL) selected->model.translate(objectMoveSpeed, 0, 0);
	if (Input::isKeyPressed(SDL_SCANCODE_RIGHT) && selected != NULL) selected->model.translate(-objectMoveSpeed, 0, 0);
	if (Input::isKeyPressed(SDL_SCANCODE_M) && selected != NULL) selected->model.translate(0, objectMoveSpeed, 0);
	if (Input::isKeyPressed(SDL_SCANCODE_N) && selected != NULL && selected->getPosition().y > 0.1f) selected->model.translate(0, -objectMoveSpeed, 0);
};
void EditorStage::renderInFront(std::string meshPath, std::string texPath) {
	Game* g = Game::instance;
	Camera* cam = g->camera;

	Vector2 mouse = Input::mouse_position;
	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, g->window_width, g->window_height);
	Vector3 rayOrigin = cam->eye;

	Vector3 spawnPos = RayPlaneCollision(Vector3(0, 0, 0), Vector3(0, 1, 0), rayOrigin, dir);
	Matrix44 model;
	model.translate(spawnPos.x, spawnPos.y, spawnPos.z);

	EntityMesh* entity = new EntityMesh(NULL, NULL, shader, Vector4(1, 1, 1, 1), meshPath, texPath, NULL);
	entity->model = model;
	staticObjects.reserve(1);
	staticObjects.push_back(entity);
}
void EditorStage::selectObject() {
	selected = Collision::RayPick(Game::instance->camera);
}


STAGE_ID EndStage::GetId() {
	return STAGE_ID::END;
};
void EndStage::render() {
	GUI::RenderEndWinGUI(isWin);
};
void EndStage::update(float seconds_elapsed) {
	float mouseX = Input::mouse_position.x;
	float mouseY = Input::mouse_position.y;
	if (275 < mouseX && mouseX < 525 && 515 < mouseY && mouseY < 585 && (Input::mouse_state & SDL_BUTTON_LEFT)) { 
		lvl3isLoaded = false;    //reset variables
		loadTimer = 0.8f;
		isWin = false;
		menuSong = false;
		level = 0;
		staticObjects.clear();
		SetStage(STAGE_ID::INTRO); //return to the intro stage
	}
};


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
	stages.reserve(5);
	stages.push_back(new IntroStage());
	stages.push_back(new MenuStage());
	stages.push_back(new TutoStage());
	stages.push_back(new LoadingStage());
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
void saveScene(const char* fileName) {

	std::ofstream myfile;
	printf("\nCreating file to save the actual editor scene...\n");
	myfile.open(fileName);
	myfile << staticObjects.size() << "\n";
	for (size_t i = 0; i < staticObjects.size(); i++)
	{
		EntityMesh* platform = staticObjects[i];
		Matrix44 m = platform->model;

		for (int j = 0; j < 16; j++)  //print the model to the file
		{
			myfile << m.m[j] << ", ";
		}

		myfile << "\n" << platform->meshPath << "\n";
		myfile << platform->texPath << "\n";
	}
	myfile.close();
}
void readScene(const char* fileName, std::vector<EntityMesh*>* vector) {
	vector->clear();

	std::string line;
	std::ifstream myfile;
	Matrix44 model;
	std::string meshPath;
	std::string texPath;
	printf("\nOpening file to charge the saved editor scene...\n");
	myfile.open(fileName);
	if (myfile.is_open())
	{
		std::getline(myfile, line);
		int numObjects = std::stoi(line);
		for (size_t i = 0; i < numObjects; i++)
		{
			for (int j = 0; j < 16; j++) {
				std::getline(myfile, line, ',');
				std::cout << "Element of the model : " << line << "\n";
				model.m[j] = std::atof(line.c_str());
			}
			std::getline(myfile, line);
			std::getline(myfile, line);
			std::cout << "MeshPath: " << line << "\n";
			meshPath = line;
			std::getline(myfile, line);
			std::cout << "TexPath: " << line << "\n";
			texPath = line;

			EntityMesh* object = new EntityMesh(NULL, NULL, shader, Vector4(1, 1, 1, 1), meshPath, texPath, NULL);
			object->model = model;
			vector->push_back(object);
		}
		myfile.close();
	}
	else std::cout << "\n[!]Unable to open file";
}

void loadLevel(Vector3 playerPos) {
	Vector3 coinPos[4] = {  //five levels, the first one will be like a tuto
		Vector3(0,20,40),
		Vector3(10,40,20),
		Vector3(0,10,100),
		Vector3(2,15,180)
	};
	EntityMesh* water = new EntityMesh(NULL, NULL, shader, Vector4(1, 1, 1, 1), "data/decoration/rain.obj", "data/color-atlas-new.png", NULL);

	if (level == 1) {
		wall->model.setTranslation(-10, 0, 29);
		wall->render();
		EntityMesh wall1 = *wall;
		wall1.model.setTranslation(-10, 0, -10);
		wall1.model.rotate(90 * DEG2RAD, Vector3(0, 1, 0));
		wall1.render();
		EntityMesh wall2 = wall1;
		wall2.model.translate(0, 0, -38);
		wall2.render();
		EntityMesh wall3 = *wall;
		wall3.model.setTranslation(-9, 0, -10);
		wall3.render();
	}
	if (level == 2) {
		volcano->model.setTranslation(0, 0, 300);
		volcano->render();
	}
	if (level == 3) {
		float distanceBetweenFences = 5.2f;
		fence->model.setTranslation(-5, 0, 0);
		fence->render();


		EntityMesh* fences[155];
		for (size_t i = 0; i < 150; i+=4)
		{
			EntityMesh fenceAux = *fence;
			fences[i] = &fenceAux;
			fences[i]->model.setTranslation(-5, 0, distanceBetweenFences*(i/4));
			fences[i]->render();

			EntityMesh fenceUp = *fence;
			fences[i + 2] = &fenceUp;
			fences[i + 2]->model.setTranslation(-5, 10, distanceBetweenFences * (i / 4));
			fences[i + 2]->render();

			EntityMesh fenceLeft = *fence;
			fences[i+1] = &fenceLeft;
			fences[i+1]->model.setTranslation(10, 0, distanceBetweenFences*(i/4));
			fences[i+1]->model.rotate(180 * DEG2RAD, Vector3(0, 1, 0));
			fences[i+1]->render();

			EntityMesh fenceLeftUp = *fence;
			fences[i + 3] = &fenceLeftUp;
			fences[i + 3]->model.setTranslation(10, 10, distanceBetweenFences * (i / 4));
			fences[i + 3]->model.rotate(180 * DEG2RAD, Vector3(0, 1, 0));
			fences[i + 3]->render();
		}

		if (!lvl3isLoaded) {    //load level 3 blocks, done only once

			const int blocksSize = 13;
			Vector3 blocksPos[blocksSize] = {
				Vector3(0,0,10),
				Vector3(2,3,15),
				Vector3(0,7,20),
				Vector3(-2,10,35),
				Vector3(4,12,45),
				Vector3(1,12,60),
				Vector3(2,13,67),
				Vector3(-2,10,74),
				Vector3(-2,12,87),
				Vector3(-2,13,92),
				Vector3(2,14,170),
				Vector3(2,14,160),
				Vector3(2,14,150)
			};
			for (size_t i = 0; i < blocksSize; i++)
			{
				EntityMesh* block = new EntityMesh(NULL, NULL, shader, Vector4(1, 1, 1, 1), "data/platforms/blockRounded.obj", "data/platforms/blockRounded.png", NULL);
				block->model.setTranslationVec(blocksPos[i]);
				staticObjects.push_back(block);
			}
			lvl3isLoaded = true;
		}
	}

	if (level != 3) {
		water->model.setTranslationVec(coinPos[level]);
		water->render();
	}
	else {
		finalnpc->model.setScale(0.01f, 0.01f, 0.01f);
		finalnpc->model.rotate(180 * DEG2RAD, Vector3(0, 1, 0));
		finalnpc->model.translateGlobal(coinPos[level].x, coinPos[level].y + 2, coinPos[level].z + 3.7f);
		finalnpc->render();
		pool->model.setScale(0.5f, 0.5f, 0.5f);
		pool->model.translateGlobal(coinPos[level].x, coinPos[level].y, coinPos[level].z);
		pool->render();
	}
	
	

	if (playerPos.distance(coinPos[level]) < 1.5f) {  //check if player got the coin, then change the level
		
		if (level == 2) {					//play final level song
			BASS_ChannelStop(lvls123SongChannel);
			lvl4SongChannel = PlayGameSound("data/level4.wav",true);
		}

		if (level == (levels - 1)) {
			staticObjects.clear();
			isWin = true;
			SetStage(STAGE_ID::END);
			aiSun->model.setTranslation(0, 10, -20);
			BASS_ChannelStop(lvl4SongChannel);
			if (isWin) PlayGameSound("data/win.wav", false);
			else PlayGameSound("data/game_over.wav", false);
		}
		else {
			SetStage(STAGE_ID::EDITOR);
		}

		if (level == 1)
			playerStruct.pos = Vector3(2, 30, 0); //reset player position
		else
			playerStruct.pos = Vector3(2, 0, 0); //reset player position

 		Game::instance->camera->lookAt(playerStruct.pos + Vector3(0,5,-10), playerStruct.pos, Vector3(0, 1, 0)); //resituate the camera for the editor

		level = (level + 1) % levels;
		GUI::RenderGUI(65, 55, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/blue_button07.png"));
	}
}



HSAMPLE LoadSample(const char* fileName, bool loop) {

	HSAMPLE hSample;
	if(loop)
		hSample = BASS_SampleLoad(false, fileName, 0, 0, 3, BASS_SAMPLE_LOOP);
	else
		hSample = BASS_SampleLoad(false, fileName, 0, 0, 3, 0);

	if (hSample == 0) {
		std::cout << "ERROR loading " << fileName << std::endl;
	}
	std::cout << "AUDIO loading " << fileName << std::endl;
	return hSample;

}

HCHANNEL PlayGameSound(const char* fileName, bool loop) {

	HSAMPLE hSample = LoadSample(fileName, loop);
	
	HCHANNEL hSampleChannel;

	hSampleChannel = BASS_SampleGetChannel(hSample, false);
	BASS_ChannelPlay(hSampleChannel, true);

	return hSampleChannel;
}

