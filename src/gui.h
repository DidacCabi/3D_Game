#ifndef GUI_H
#define GUI_H

#include "texture.h"
#include "mesh.h"
#include "camera.h"
#include <GL/glew.h>
#include "game.h"
#include "shader.h"
#include "input.h"
#include "stage.h"



class GUI {
	public:
		
		static void RenderGUI(float start_x, float start_y, float w, float h, Vector4 color, Texture* tex);
		static void RenderAllGUI(bool canJump);
		static bool RenderButton(float x, float y, Texture* buttonTex);
		static void RenderIntroGUI();
		static void RenderMenuGUI();
		static void RenderTutoGUI();
		static void RenderLoadingGUI();
		static void fillWaterSquare(int level);
		static void RenderEndWinGUI(bool isWin);
};





#endif