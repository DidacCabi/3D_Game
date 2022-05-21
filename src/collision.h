#ifndef COLLISION_H
#define COLLISION_H

#include "camera.h"
#include "entityMesh.h"

class Collision {
	public:

		static EntityMesh* RayPick(Camera* cam);

};


#endif 