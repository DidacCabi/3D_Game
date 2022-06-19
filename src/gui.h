#ifndef GUI_H
#define GUI_H

#include "texture.h"
#include "mesh.h"
#include "camera.h"
#include <GL/glew.h>
#include "game.h"
#include "shader.h"
#include "input.h"

class GUI {
	public:
		
		static void RenderGUI(float start_x, float start_y, float w, float h, Vector4 color, Texture* tex);
		static void RenderAllGUI();
		static bool RenderButton(float x, float y, Texture* buttonTex);
		//static void RenderIntroGUI();
};





#endif