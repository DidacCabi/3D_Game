#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"
#include "entityMesh.h"
#include "entity.h"
#include "stage.h"
#include "collision.h"
#include <cmath>
#include <bass.h>

//some globals
extern std::vector<Stage*> stages;
extern STAGE_ID currentStage;

EntityMesh* ground;

EntityMesh* jetpack;

std::vector<EntityMesh*> staticObjects;
Mesh* mesh = NULL;
Texture* texture = NULL;
Shader* shader = NULL;

EntityMesh* player;
Mesh* playerMesh = NULL;
Texture* playerTex = NULL;

bool cameraLocked = true;

Animation* anim = NULL;
float angle = 0;
float mouse_speed = 100.0f;
FBO* fbo = NULL;

Game* Game::instance = NULL;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f,100.f, 100.f),Vector3(0.f,0.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f,window_width/(float)window_height,0.1f,10000.f); //set the projection, we want to be perspective

	InitStages();

	//loading Mesh from Mesh Manager
	playerMesh = Mesh::Get("data/Chr_Dungeon_Skeleton_01_34.obj");
	playerTex = Texture::Get("data/PolygonMinis_Texture.png");

	
	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	
	jetpack = new EntityMesh(Mesh::Get("data/Jetpack.obj"), Texture::Get("data/Jetpack_BaseColor.png"), shader, Vector4(1,1,1,1));
	ground = new EntityMesh(Mesh::Get("data/platforms/ground.obj"), Texture::Get("data/platforms/ground.png"), shader, Vector4(1,1,1,1));

	staticObjects.reserve(50);
	staticObjects.push_back(new EntityMesh(Mesh::Get("data/sky.ASE"), Texture::Get("data/sky.tga"), shader, Vector4(1, 1, 1, 1)));

	player = new EntityMesh(playerMesh, playerTex, shader, Vector4(1, 1, 1, 1));

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse

	//Inicializamos BASS al arrancar el juego (id_del_device, muestras por segundo, ...)
	if (BASS_Init(-1, 44100, 0, 0, NULL) == false) //-1 significa usar el por defecto del sistema operativo
	{
		//error abriendo la tarjeta de sonido...
	}

}

void RenderMesh(Matrix44& model, Mesh* a_mesh, Texture* tex, Shader* a_shader, Camera* cam) {
	assert(a_mesh != NULL, "mesh in RenderMesh was null");
	if (!a_shader) return;

	//enable shader
	shader->enable();

	//upload uniforms
	a_shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	a_shader->setUniform("u_viewprojection", cam->viewprojection_matrix);
	a_shader->setUniform("u_texture", tex, 0);
	a_shader->setUniform("u_time", time);

	a_shader->setUniform("u_model", model);
	a_mesh->render(GL_TRIANGLES);

	shader->disable();
}

void RenderIslands() {

	if (shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", Game::instance->camera->viewprojection_matrix);
		shader->setUniform("u_texture", texture, 0);
		shader->setUniform("u_time", time);

		Matrix44 m;
		for (size_t i = 0; i < 10; i++)
		{
			for (size_t j = 0; j < 10; j++)
			{
				Vector3 size = mesh->box.halfsize * 2;
				m.setTranslation(size.x * i, 0.0f, size.z * j);
				shader->setUniform("u_model", m);
				mesh->render(GL_TRIANGLES);
			}
		}

		//disable shader
		shader->disable();
	}
}

//what to do when the image has to be draw
void Game::render(void)
{
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	camera->enable();

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
   
	//m.rotate(angle*DEG2RAD, Vector3(0, 1, 0));


	GetCurrentStage()->render();

	//island->render();

	//RenderMesh(islandModel, mesh, texture, shader, camera);
	//RenderMesh(planeModel, planeMesh, planeTexture, shader, camera);
	//RenderMesh(bombModel, bombMesh, bombTexture, shader, camera);
	//mesh->renderBounding(islandModel);
	//RenderIslands();

	//Draw the floor grid
	//drawGrid();

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	GetCurrentStage()->update(seconds_elapsed);
	

	if (Input::wasKeyPressed(SDL_SCANCODE_P)) { 
		int nextStageIndex = ((int)currentStage + 1) % stages.size();
		SetStage((STAGE_ID)nextStageIndex);
	}
	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_F1: Shader::ReloadAll(); break; 
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	mouse_speed *= event.y > 0 ? 1.1 : 0.9;
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

