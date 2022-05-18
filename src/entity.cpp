#include "entity.h"

Entity::Entity() {

}
Entity::~Entity() {

}

Vector3 Entity::getPosition() {
	return model.getTranslation();
}