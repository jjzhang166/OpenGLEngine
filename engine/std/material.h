//
//  xmesh.hpp
//  OpenGLEngine
//
//  Created by 彭怀亮 on 6/4/19.
//  Copyright © 2019 彭怀亮. All rights reserved.
//

#ifndef material_h
#define material_h

#include "common.h"
#include "shader.h"

#define TEXTURE_NUM 4

struct Vertex
{
    glm::vec3 Position;
    //uv
    glm::vec2 TexCoords;
    //normal
    glm::vec3 Normal;
};

struct MeshData
{
    unsigned int num_indice;
    unsigned int *indices;
    unsigned int num_vert;
    Vertex* vertices;
    
    GLuint diffuse_texture;
    GLuint normal_texure;
    GLuint ambient_texture;
    GLuint specul_texture;
    
    
    ~MeshData()
    {
        delete []indices;
        delete []vertices;
    }
};


class Material
{
public:
    
    Material(MeshData* data);
    
    ~Material();
    
    void SetupMesh();
    
    void Draw(Shader* shader);
    
    void DrawMesh();
    
private:
    MeshData* data;
    unsigned int ebo, vao, vbo;
};

#endif /* material_h */