#ifndef ENTITY_H
#define ENTITY_H

#include "includes.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"

class Entity
{
public:
    Entity(); //constructor
    virtual ~Entity(); //destructor

    //some attributes 
    std::string name;
    Matrix44 model;

    //methods overwritten by derived classes 
    virtual void render() = 0;
    virtual void update(float elapsed_time) = 0;

    //some useful methods...
    Vector3 getPosition();
};

#endif 