#include "collision.h"
#include "input.h"
#include "game.h"

extern std::vector<EntityMesh*> editorPlatforms;

EntityMesh* Collision::RayPick(Camera* cam) {

	Vector2 mouse = Input::mouse_position;
	Game* g = Game::instance;
	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, g->window_width, g->window_height);
	Vector3 rayOrigin = cam->eye;

	for (size_t i = 0; i < editorPlatforms.size(); i++) {
		EntityMesh* entity = editorPlatforms[i];
		Vector3 pos, normal;

		if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal)) {
			std::cout << "selected" << std::endl;
			return entity;
		}
	}
	return NULL;
}

void Collision::testSidePlayerColl(EntityMesh* player, Vector3 nextPos, float elapsed_time) {
	for (size_t i = 0; i < editorPlatforms.size(); i++)
	{
		Vector3 coll, collnorm;
		EntityMesh* platform = editorPlatforms[i];
		Vector3 playerPos = player->getPosition();
		if (!platform->mesh->testSphereCollision(platform->model, playerPos, 1.0f, coll, collnorm)) continue;
		Vector3 pushAway = normalize(coll - playerPos) * elapsed_time;
		nextPos = playerPos - pushAway;
	}
}

bool Collision::testBelowPlayerColl(EntityMesh* player) {
	
	Vector3 coll, collnorm;

	for (size_t i = 0; i < editorPlatforms.size(); i++)
	{
		Vector3 coll, collnorm;
		EntityMesh* platform = editorPlatforms[i];
		Vector3 playerPos = player->getPosition();
		if (platform->mesh->testRayCollision(platform->model, playerPos, Vector3(0, -1, 0), coll, collnorm, 0.2f, true)) return true;
	}
	return false; 

}
