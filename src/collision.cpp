#include "collision.h"
#include "input.h"
#include "game.h"

extern std::vector<EntityMesh*> staticObjects;

EntityMesh* Collision::RayPick(Camera* cam) {

	Vector2 mouse = Input::mouse_position;
	Game* g = Game::instance;
	Vector3 dir = cam->getRayDirection(mouse.x, mouse.y, g->window_width, g->window_height);
	Vector3 rayOrigin = cam->eye;

	for (size_t i = 0; i < staticObjects.size(); i++) {
		EntityMesh* entity = staticObjects[i];
		Vector3 pos, normal;

		if (entity->mesh->testRayCollision(entity->model, rayOrigin, dir, pos, normal)) {
			std::cout << "selected" << std::endl;
			return entity;
		}
	}
	return NULL;
}

Vector3 Collision::testSidePlayerColl(EntityMesh* player, Vector3 playerPos, Vector3 nextPos, float elapsed_time, EntityMesh* aiSun, int level) {
	for (size_t i = 0; i < staticObjects.size(); i++)
	{
		EntityMesh* platform = staticObjects[i];

		if (level == 3) {
			Vector3 sunColl, sunCollnorm;
			if (aiSun->mesh->testSphereCollision(aiSun->model, platform->getPosition(), 1.5f, sunColl, sunCollnorm)) {
				staticObjects.erase(staticObjects.begin() + i);
				continue;
			}
		}

		Vector3 coll, collnorm;
		if (!player->mesh->testSphereCollision(player->model, platform->getPosition(), 2.0f, coll, collnorm)) continue;
		Vector3 pushAway = normalize(coll - playerPos) * elapsed_time;
		nextPos = playerPos - pushAway;
		return nextPos;
	}
	return nextPos;
}

bool Collision::testBelowPlayerColl(EntityMesh* player) {
	
	Vector3 coll, collnorm;

	for (size_t i = 0; i < staticObjects.size(); i++)
	{
		Vector3 coll, collnorm;
		EntityMesh* platform = staticObjects[i];
		Vector3 playerPos = player->getPosition();
		if (platform->mesh->testRayCollision(platform->model, playerPos, Vector3(0, -1, 0), coll, collnorm, 0.21f, true)) return true;
	}
	return false; 

}
