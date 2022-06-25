#include "gui.h"


bool wasLeftMousePressed = false;
int waterCounter = 0;
//extern sPlayer playerStruct;
extern float jumpCounter;
//extern PlayStage::sPlayer playerStruct;

void GUI::RenderGUI(float start_x, float start_y, float w, float h, Vector4 color, Texture* tex ){
	int windowWidth = Game::instance->window_width;
	int windowHeight = Game::instance->window_height;

	Mesh quad;
	quad.createQuad(start_x, start_y, w, h, true);
	Camera cam2D;

	cam2D.setOrthographic(0, windowWidth, windowHeight, 0, -1, 1);

	Shader* jpshader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs" );
	//Texture* jptex = Texture::Get("data/jetpackFuel.png");
	if (!jpshader) return;

	jpshader->enable();
	jpshader->setUniform("u_color", color);
	jpshader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);

	if (tex != NULL) {
		jpshader->setUniform("u_texture", tex, 0);
	}
	jpshader->setUniform("u_time", time);
	jpshader->setUniform("u_tex_tiling", 1.0f);
	jpshader->setUniform("u_model", Matrix44());
	quad.render(GL_TRIANGLES);

	jpshader->disable();
}

bool GUI::RenderButton(float x, float y,  Texture* buttonTex) {
	Vector2 mouse = Input::mouse_position;
	float halfWidth = Game::instance->window_width * 0.5;
	float halfHeight = Game::instance->window_height * 0.5;
	float min_x = x - halfWidth;
	float max_x = x + halfWidth;
	float min_y = y - halfHeight;
	float max_y = y + halfHeight;

	bool hover = (mouse.x >= min_x) && (mouse.x <= max_x) && (mouse.y >= min_y) && (mouse.y <= max_y);
	Vector4 buttonColor = hover ? Vector4(1, 1, 1, 1) : Vector4(1, 1, 1, 0.7f);
	RenderGUI(100, 100, 25, 25, buttonColor,buttonTex);
	return wasLeftMousePressed && hover;
}

void GUI::RenderAllGUI() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	RenderGUI(25, 25, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/jetpackFuel.png"));
	RenderGUI(25, 55, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/hydrated.png"));
	//RenderGUI(100, 25, 100, 25, Vector4(1, 1, 1, 1), Texture::Get("data/red_button11.png"));
	RenderGUI(100, 25, 100, 25, Vector4(1, 1, 1, 1), Texture::Get("data/red_button10.png"));

	for (int i = 0; i <= 50; i+=25) {

		RenderGUI(65 + i, 55, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/blue_button06.png"));

		/*if (loadLevel(playerStruct.pos) == true) {
			RenderGUI(65 + i, 55, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/blue_button07.png"));
			waterCounter++;
		}*/
	}
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE)) {
		
		RenderGUI(100, 25, jumpCounter * 40, 25, Vector4(1, 1, 1, 1), Texture::Get("data/red_button11.png"));
		//PlayGameSound("data/jetpack.mp3");
	}
	if (!Input::isKeyPressed(SDL_SCANCODE_SPACE)) {
		RenderGUI(100, 25, jumpCounter * 40, 25, Vector4(1, 1, 1, 1), Texture::Get("data/red_button11.png"));

	}
	
		
	//if()
	//RenderButton(500, 500, Texture::Get("data/blue_panel.png"));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	wasLeftMousePressed = false;
}

void GUI::fillWaterSquare() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	RenderGUI(65, 55, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/blue_button07.png"));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void GUI::RenderIntroGUI() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	RenderGUI(width / 2, height / 2, width, height, Vector4(1, 1, 1, 1), Texture::Get("data/UI/background.tga"));
	RenderGUI(width / 2, 200, 576, 77, Vector4(1, 1, 1, 1), Texture::Get("data/UI/tittle.png"));
	RenderGUI(width/2, (height / 2) + 100, 165, 74, Vector4(1, 1, 1, 1), Texture::Get("data/UI/button_play.png"));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void GUI::RenderLoadingGUI() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	RenderGUI((width / 2) + 40, height / 2, 623, 145, Vector4(1, 1, 1, 1), Texture::Get("data/UI/loading.png"));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}


void GUI::RenderEndWinGUI() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	RenderGUI(width / 2, height / 2, width, height, Vector4(1, 1, 1, 1), Texture::Get("data/UI/pool.tga"));
	RenderGUI(width / 2, height / 2, 582, 145, Vector4(1, 1, 1, 1), Texture::Get("data/UI/win.png"));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}



