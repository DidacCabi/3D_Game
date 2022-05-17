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

//some globals
extern std::vector<Stage*> stages;
extern STAGE_ID currentStage;

EntityMesh* island;
Matrix44 islandModel;

std::vector<Mesh*> platformMeshes;
std::vector<Texture*> platformTexs;
std::vector<Matrix44> platformModels;
std::vector<EntityMesh*> platforms;

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

	// example of loading Mesh from Mesh Manager
	mesh = Mesh::Get("data/island.ASE");
	texture = Texture::Get("data/island_color.tga");

	playerMesh = Mesh::Get("data/Chr_Adventure_Viking_01_0.obj");
	playerTex = Texture::Get("data/PolygonMinis_Texture.png");

	platformMeshes.reserve(20);
	platformTexs.reserve(20);
	platformModels.reserve(20);
	platforms.reserve(20);
	platformMeshes.push_back(Mesh::Get("data/platforms/blockSnow.obj"));
	platformTexs.push_back(Texture::Get("data/platforms/blockSnow.png"));
	platformMeshes.push_back(Mesh::Get("data/platforms/blockSnowCliff.obj"));
	platformTexs.push_back(Texture::Get("data/platforms/blockSnowCliff.png"));
	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	staticObjects.push_back(new EntityMesh(Mesh::Get("data/sky.ASE"), Texture::Get("data/sky.tga"), shader, Vector4(1, 1, 1, 1)));
	platforms.push_back(new EntityMesh(platformMeshes[0], NULL, shader, Vector4(1, 1, 1, 1)));
	island = new EntityMesh(mesh, texture, shader, Vector4(1,1,1,1));
	player = new EntityMesh(playerMesh, playerTex, shader, Vector4(1, 1, 1, 1));

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
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
	drawGrid();

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
		case SDLK_h: Collision::RayPick(camera);   // TODO Temporary collision tester 
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

