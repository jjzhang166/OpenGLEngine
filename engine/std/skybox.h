//
//  skybox.h
//  OpenGLEngine
//
//  Created by 彭怀亮 on 5/13/19.
//  Copyright © 2019 彭怀亮. All rights reserved.
//

#ifndef skybox_h
#define skybox_h

#include <vector>
#include <glm/glm.hpp>
#include "shader.h"
#include "camera.h"

namespace engine
{
    
    class Skybox
    {
        
    public:
        
        Skybox(Camera* camera,std::string name,bool hdr=false);
        
        ~Skybox();
        
        void MakeCube(unsigned int* map, int size, bool mipmap);
        
        void Draw();
        
        
    private:
        
        void Equirect2Cube();
        
        void GenerateEnvmap(glm::mat4 captureViews[], glm::mat4 captureProjection);
        
        void GenerateIrradiance(glm::mat4 captureViews[], glm::mat4 captureProjection);
        
        void GeneratePrefilter(glm::mat4 captureViews[], glm::mat4 captureProjection);
        
        void GenerateLut();
        
        void RenderCube();
        
        void RenderQuad();
        
        void init_tex(bool hdr);
        
        void init_buff();
        
        
    public:
        GLuint irradianceMap,envCubemap,prefilterMap,brdfLUTTexture;
        GLuint cubemapTexture,hdrTexture;
        
    private:
        GLuint vao, vbo;
        GLuint captureFBO, captureRBO;
        std::vector<std::string> faces;
        Shader* shader=nullptr;
        Shader* equirectangularToCubemapShader=nullptr;
        Shader* irradianceShader=nullptr;
        Shader* prefilterShader=nullptr;
        Shader* brdfShader=nullptr;
        Camera* camera=nullptr;
        std::string name;
        
    };
    
}

#endif /* skybox_h */
