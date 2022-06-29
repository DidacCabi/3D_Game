#pragma once
#ifndef STAGE_H
#define STAGE_H

#include "game.h"
#include "includes.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "entityMesh.h"
#include "animation.h"
#include <bass.h>

enum STAGE_ID {
	INTRO,
	MENU,
	TUTORIAL,
	LOADING,
	PLAY,
	EDITOR,
	END
};


HSAMPLE LoadSample(const char* fileName, bool loop);
HCHANNEL PlayGameSound(const char* fileName, bool loop);

class Stage {
public:
	virtual STAGE_ID GetId() = 0;
	virtual void render() = 0;
	virtual void update(float seconds_elapsed) = 0;
};

class IntroStage : public Stage {
public:
	STAGE_ID GetId();
	void render();
	void update(float seconds_elapsed);
};

class MenuStage : public Stage {
public:
	STAGE_ID GetId();
	void render();
	void update(float seconds_elapsed);
};

class TutoStage : public Stage {
public:
	STAGE_ID GetId();
	void render();
	void update(float seconds_elapsed);
};

class LoadingStage : public Stage {
public:
	STAGE_ID GetId();
	void render();
	void update(float seconds_elapsed);
};

class PlayStage : public Stage {
public:
	bool readedDecoration = false;
	const float jumpTime = 3.2f;
	bool canJump = true;
	bool isJumping = false;

	STAGE_ID GetId();
	void render();
	void update(float seconds_elapsed);
};

class EditorStage : public Stage {
public:
	int objectNum = 0;  //to choose which object we will render on the editor
	int decorationNum = 0;
	bool mode = false;   //switch between decoration and platforms mode
	EntityMesh* selected = NULL;

	int objectsPerLevel[5] = {1,2,4,2,5};
	int objectsLeft[5] = {1,2,4,2,5};

	STAGE_ID GetId();
	void render();
	void update(float seconds_elapsed);
	void renderInFront(std::string meshPath, std::string texPath);
	void selectObject();
};

class EndStage : public Stage {
public:
	STAGE_ID GetId();
	void render();
	void update(float seconds_elapsed);
};

void cameraMove(Camera* camera, float speed);

Stage* GetStage(STAGE_ID id);
Stage* GetCurrentStage();

void SetStage(STAGE_ID id);
void InitStages();

enum PLAYER_DIR {
	DOWN = 0,
	RIGHT = 1,
	LEFT = 2,
	UP = 3
};


struct sPlayer {
	Vector3 pos;
	float yaw;
	float scale;
};

//struct sPlayer {
//	Vector2 pos;
//	bool isMoving;
//	PLAYER_DIR dir;
//	bool isDead = 0;
//};


void GroundRendering();
void saveScene(const char* fileName);
void readScene(const char* fileName, std::vector<EntityMesh*>* vector);

bool loadLevel(Vector3 playerPos);
#endif 