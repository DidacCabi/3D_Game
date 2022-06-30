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

void GUI::RenderAllGUI(bool canJump) {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	RenderGUI(25, 25, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/UI/jetpackFuel.png"));
	RenderGUI(25, 55, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/UI/hydrated.png"));
	RenderGUI(100, 25, 100, 25, Vector4(1, 1, 1, 1), Texture::Get("data/UI/red_button10.png"));

	for (int i = 0; i <= 50; i+=25) {

		RenderGUI(65 + i, 55, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/UI/blue_button06.png"));
	}
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE) && canJump) {
		
		RenderGUI(100, 25, jumpCounter * 56, 25, Vector4(1, 1, 1, 1), Texture::Get("data/UI/red_button11.png"));
	}
	if (!Input::isKeyPressed(SDL_SCANCODE_SPACE) && canJump) {
		RenderGUI(100, 25, jumpCounter * 56, 25, Vector4(1, 1, 1, 1), Texture::Get("data/UI/red_button11.png"));

	}
	
		
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	wasLeftMousePressed = false;
}

void GUI::fillWaterSquare(int level) {   //it will indicate which level is
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	switch(level){
	case 1:
		RenderGUI(65, 55, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/blue_button07.png"));
		break;
	case 2:
		RenderGUI(65, 55, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/blue_button07.png"));
		RenderGUI(90, 55, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/blue_button07.png"));
		break;
	case 3:
		RenderGUI(65, 55, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/blue_button07.png"));
		RenderGUI(90, 55, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/blue_button07.png"));
		RenderGUI(115, 55, 25, 25, Vector4(1, 1, 1, 1), Texture::Get("data/blue_button07.png"));
		break;
	}

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
	RenderGUI(width / 2, 300, 576, 77, Vector4(1, 1, 1, 1), Texture::Get("data/UI/tittle.png"));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void GUI::RenderMenuGUI() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	RenderGUI(width / 2, height / 2, width, height, Vector4(1, 1, 1, 1), Texture::Get("data/UI/background.tga"));
	RenderGUI(width / 2, height / 2, 165, 74, Vector4(1, 1, 1, 1), Texture::Get("data/UI/button_play.png"));
	RenderGUI(width / 2, (height / 2) + 100, 165, 74, Vector4(1, 1, 1, 1), Texture::Get("data/UI/how.png"));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void GUI::RenderTutoGUI() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	RenderGUI(width / 2, height / 2, width, height, Vector4(1, 1, 1, 1), Texture::Get("data/UI/background.tga"));
	RenderGUI(width / 2, (height / 2) - 100, 165, 74, Vector4(1, 1, 1, 1), Texture::Get("data/UI/button_play.png"));
	RenderGUI(100, 300, 200, 200, Vector4(1, 1, 1, 1), Texture::Get("data/UI/wasd.tga"));
	RenderGUI(100, 500, 200, 200, Vector4(1, 1, 1, 1), Texture::Get("data/UI/p.png"));
	RenderGUI(width / 2, 100, 600, 100, Vector4(1, 1, 1, 1), Texture::Get("data/UI/text.png"));
	RenderGUI(width - 136, height - 92, 273, 185, Vector4(1, 1, 1, 1), Texture::Get("data/UI/editor.png"));
	RenderGUI(400, height - 72, 250, 144, Vector4(1, 1, 1, 1), Texture::Get("data/UI/editor1.png"));

	drawText(30,350,"To move", Vector3(0,0,0), 3.0f);
	drawText(30, 530, "To change between", Vector3(0, 0, 0), 1.5f);
	drawText(35, 550, "play and editor", Vector3(0, 0, 0), 1.5f);

	drawText(280, 420, "On the editor:", Vector3(0, 0, 0), 3);


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


void GUI::RenderEndWinGUI(bool isWin) {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int width = Game::instance->window_width;
	int height = Game::instance->window_height;

	if (isWin) {
		RenderGUI(width / 2, height / 2, width, height, Vector4(1, 1, 1, 1), Texture::Get("data/UI/pool.tga"));
		RenderGUI(width / 2, height / 2, 582, 145, Vector4(1, 1, 1, 1), Texture::Get("data/UI/win.png"));
	}
	else {
		RenderGUI(width / 2, height / 2, width, height, Vector4(1, 1, 1, 1), Texture::Get("data/UI/lose.tga"));
		RenderGUI(width / 2, (height / 2)-50, 322, 90, Vector4(1, 1, 1, 1), Texture::Get("data/UI/loseText.png"));
	}

	RenderGUI(width / 2, (height / 2) + 250, 245, 71, Vector4(1, 1, 1, 1), Texture::Get("data/UI/playAgain.png"));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}



