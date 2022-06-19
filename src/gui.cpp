#include "gui.h"
#include "mesh.h"
#include "camera.h"
#include <GL/glew.h>
#include "game.h"
#include "texture.h"
#include "shader.h"

void GUI::RenderGUI() {
	int windowWidth = Game::instance->window_width;
	int windowHeight = Game::instance->window_height;

	Mesh quad;
	quad.createQuad(50, 50, 25, 25, true);
	Camera cam2D;

	cam2D.setOrthographic(0, windowWidth, windowHeight, 0, -1, 1);

	Shader* jpshader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs" );
	Texture* jptex = Texture::Get("data/jetpackFuel.png");
	if (!jpshader) return;

	jpshader->enable();
	jpshader->setUniform("u_color", Vector4(1, 1, 1, 1));
	jpshader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);

	if (jptex != NULL) {
		jpshader->setUniform("u_texture", jptex, 0);
	}
	jpshader->setUniform("u_time", time);
	jpshader->setUniform("u_tex_tiling", 1.0f);
	jpshader->setUniform("u_model", Matrix44());
	quad.render(GL_TRIANGLES);

	jpshader->disable();
}

void GUI::RenderAllGUI() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	RenderGUI();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);


}
