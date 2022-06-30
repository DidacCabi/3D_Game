#include "ai.h"

extern EntityMesh* aiSun;
extern EntityMesh* player;

struct sSun {
		Vector3 pos;
		float yaw;
	};
sSun sun;

float AI::sign(float value){
	return value >= 0.0f ? 1.0f : -1.0f;
}

void AI::following_AI(float seconds_elapsed) {   //orient and move the sun towards the player  
	sun.pos = aiSun->getPosition();

	Matrix44 aiModel = aiSun->model;
	Vector3 side = aiModel.rotateVector(Vector3(1, 0, 0)).normalize();
	Vector3 forward = aiModel.rotateVector(Vector3(0, 0, -1)).normalize();
	Vector3 updown = aiModel.rotateVector(Vector3(0, 1, 0)).normalize();

	Vector3 toTarget = player->getPosition() - sun.pos;   // Distance vector
	float dist = toTarget.length();						// Distance between player and Sun
	toTarget.normalize();

	float sideDot = side.dot(toTarget);
	float fwdDot = forward.dot(toTarget);
	float updownDot = updown.dot(toTarget);

	if (fwdDot < 0.98f) {
		sun.yaw += sign(sideDot) * 120.0f * seconds_elapsed;
	}
	if (dist > 0.5f) {
		float sunVelocity = 6.0f;
		sun.pos = sun.pos + (forward * sunVelocity * seconds_elapsed);
	}
	if (toTarget.y > 0.2f || toTarget.y < -0.2f) {
		sun.pos.y = sun.pos.y + (sign(updownDot) * seconds_elapsed * 10.0f);
	}


	aiSun->model.setTranslationVec(sun.pos);
	aiSun->model.rotate(sun.yaw * DEG2RAD, Vector3(0,1,0));
}
