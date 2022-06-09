#ifndef COLLISION_H
#define COLLISION_H

#include "camera.h"
#include "entityMesh.h"

class Collision {
public:

	static EntityMesh* RayPick(Camera* cam);
	static Vector3 testSidePlayerColl(EntityMesh* player, Vector3 playerPos, Vector3 nextPos, float elapsed_time, EntityMesh* aiSun, int level);
	static bool testBelowPlayerColl(EntityMesh* player);
};
#endif 