#include "entityMesh.h"

EntityMesh::EntityMesh(Mesh* mesh, Texture* tex, Shader * shader, Vector4 color = Vector4(1,1,1,1), std::string meshPath = "", std::string texPath = "") {
    if (mesh == NULL) this->mesh = Mesh::Get(meshPath.c_str());
    else this->mesh = mesh;
    this->model = model;
    this->color = color;
    this->shader = shader;
    if (tex == NULL) this->texture = Texture::Get(texPath.c_str());
    else this->texture = tex;
    this->texPath = texPath;
    this->meshPath = meshPath;
}

EntityMesh::EntityMesh(Mesh* mesh, Texture* tex, Shader* shader, Vector4 color) {
    this->mesh = mesh;
    this->model = model;
    this->color = color;
    this->shader = shader;
    this->texture = tex;
}

void EntityMesh::render(float tiling ) {

    assert(mesh != NULL, "mesh in RenderMesh was null");
    if (!shader) return;

    //get the last camera that was activated
    Camera* camera = Camera::current;
    Matrix44 model = this->model;

    //enable shader and pass uniforms
    shader->enable();
    shader->setUniform("u_color", color);
    shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
    shader->setUniform("u_tex_tiling", tiling);
    if(texture != NULL) shader->setUniform("u_texture", texture, 0);
    shader->setUniform("u_time", time);
    shader->setUniform("u_model", model);

    //render the mesh using the shader
    mesh->render(GL_TRIANGLES);

    //disable the shader after finishing rendering
    shader->disable();
}

void EntityMesh::update(float elapsed_time) {

}
