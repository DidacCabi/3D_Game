#include "collision.h"
#include "input.h"
#include "entityMesh.h"
#include "game.h"

extern std::vector<EntityMesh*> editorPlatforms;

void Collision::RayPick(Camera* cam) {

	Vector2 mouse = Input::mouse_position;
	Game* g = Game::instance;
	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, g->window_width, g->window_height);
	Vector3 rayOrigin = cam->eye;

	for (size_t i = 0; i < editorPlatforms.size(); i++) {
		EntityMesh* entity = editorPlatforms[i];
		Vector3 pos, normal;

		if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal)) {
			std::cout << "col" << std::endl;
		}
	}
}