#pragma once
#ifndef STAGE_H
#define STAGE_H

#include "game.h"
#include "includes.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"

enum STAGE_ID {
	INTRO,
	TUTORIAL,
	PLAY,
	EDITOR,
	END
};

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

class TutorialStage : public Stage {
public:
	STAGE_ID GetId();
	void render();
	void update(float seconds_elapsed);
};

class PlayStage : public Stage {
public:
	STAGE_ID GetId();
	void render();
	void update(float seconds_elapsed);
};

class EditorStage : public Stage {
public:
	STAGE_ID GetId();
	void render();
	void update(float seconds_elapsed);
	void onKeyDown(SDL_KeyboardEvent event);
	void renderInFront(Mesh* mesh, Texture* tex);
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
	Vector2 pos;
	bool isMoving;
	PLAYER_DIR dir;
	bool isDead = 0;
	
};

#endif 