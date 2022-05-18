#ifndef ENTITYMESH_H
#define ENTITYMESH_H

#include "includes.h"
#include "utils.h"
#include "entity.h"
#include "shader.h"
#include "camera.h"

class EntityMesh : public Entity {
public:
    EntityMesh::EntityMesh(Mesh* mesh, Texture* tex,  Shader* shader, Vector4 color, std::string meshPath, std::string texPath);
    EntityMesh::EntityMesh(Mesh* mesh, Texture* tex, Shader* shader, Vector4 color);
    //Attributes of this class 
    Mesh* mesh;
    Texture* texture;
    std::string meshPath; 
    std::string texPath;
    Shader* shader;
    Vector4 color;

    //methods overwritten 
    void render();
    void update(float dt);
};


#endif