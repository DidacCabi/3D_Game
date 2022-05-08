#include "entityMesh.h"

EntityMesh::EntityMesh(Mesh* mesh, Texture* tex, Shader* shader, Vector4 color) {
    this->mesh = mesh;
    this->model = model;
    this->color = color;
    this->shader = shader;
    this->texture = tex;
}

void EntityMesh::render() {

    assert(mesh != null, "mesh in RenderMesh was null");
    if (!shader) return;

    //get the last camera that was activated
    Camera* camera = Camera::current;
    Matrix44 model = this->model;

    //enable shader and pass uniforms
    shader->enable();
    shader->setUniform("u_color", color);
    shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
    shader->setUniform("u_texture", texture, 0);
    shader->setUniform("u_time", time);
    shader->setUniform("u_model", model);

    //render the mesh using the shader
    mesh->render(GL_TRIANGLES);

    //disable the shader after finishing rendering
    shader->disable();
}

void EntityMesh::update(float elapsed_time) {

}
