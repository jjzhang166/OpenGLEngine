//
//  scene1.h
//  OpenGLEngine
//
//  Created by 彭怀亮 on 5/16/19.
//  Copyright © 2019 彭怀亮. All rights reserved.
//

#ifndef scene_h
#define scene_h
#include "../engine/engine.h"
#include "../engine/std/camera.hpp"
#include "../engine/std/light.hpp"
#include "../engine/gui/uimgr.hpp"
#include "../engine/gui/label.hpp"
#include "../engine/gui/button.hpp"
#include "../engine/gui/uiobject.hpp"
#include "../engine/std/shader.hpp"
#include "../engine/std/texture.hpp"
#include "../engine/std/iScene.h"
#include "../engine/std/skybox.hpp"

#define TY_Scene1 0
#define TY_Scene2 1
#define TY_Scene3 2
#define TY_Scene4 3
#define TY_Scene5 4
#define TY_Scene6 5
#define TY_Scene7 6

using namespace engine;

class Scene : iScene
{

public:

    virtual ~Scene()
    {
        SAFE_DELETE(camera);
        SAFE_DELETE(skybox);
        SAFE_DELETE(light);
        SAFE_DELETE(depthShader);
        SAFE_DELETE(debugShader);
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
        glDeleteFramebuffers(1, &depthMapFBO);
        glCheckError();
    }

    virtual bool drawShadow() { return true; }

    virtual glm::vec3 getCameraPos() { return glm::vec3(0.0f,0.0f,3.0f); }

    virtual std::string getSkybox() { return "lake"; }

    virtual bool isEquirectangularMap() { return false; }

    virtual uint bufferBit() { return GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ; }

    Camera* getCamera() { return camera; }

    /*
     init: camera-> skybox-> light-> scene-> ui
     */
    void Initial()
    {
        camera = new Camera(getCameraPos());
        skybox = new Skybox(camera, getSkybox(), isEquirectangularMap());
        lightMatrix = glm::mat4(1);
        InitLight();
        InitScene();

        if(drawShadow())
        {
            depthShader  = new Shader("depth.vs","depth.fs");
            debugShader = new Shader("debug.vs", "debug.fs");
            debugShader->attach("_DEBUG_DEPTH_");
            InitDepthBuffer();
            InitQuad(&quadVAO, &quadVBO, debugShader);
        }
        DrawUI();
    }

    virtual int getType() = 0;

    virtual void InitLight() = 0;

    virtual void InitScene() { }

    virtual void DrawUI() { }

    virtual void DrawShadow(Shader *depthShader)
    {
        float near_plane = 0.1f, far_plane = 7.5f, bound = 4.0;
        lightMatrix = light->GetLigthSpaceMatrix(near_plane, far_plane, bound, bound);
        depthShader->use();
        depthShader->setMat4("lightSpaceMatrix", lightMatrix);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_WIDTH);
    }

    virtual void DrawScene() { }

    virtual void OnLightChange(int key) { }

    void ClearScene()
    {
        uint bit = bufferBit();
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        if(bit & GL_STENCIL_BUFFER_BIT)
        {
            glEnable(GL_STENCIL_TEST);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            glStencilMask(0x00);
        }
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(bit);
    }

    void DrawScenes()
    {
        timeValue = GetRuntime();
        if(drawShadow())
        {
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_DEPTH_BUFFER_BIT);
            DrawShadow(depthShader);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        ClearScene();
        glViewport(0, 0, RENDER_WIDTH, RENDER_HEIGTH);
        DrawScene();
        if(drawShadow() && debug) RenderQuad();
        if(skybox) skybox->Draw();
    }

    void RebuildSky()
    {
        delete skybox;
        skybox = new Skybox(camera, getSkybox(), isEquirectangularMap());
    }

    void ProcessKeyboard(int key, float deltatime)
    {
        if(camera)
        {
            if (Qt::Key_A == key)
                camera->ProcessKeyboard(LEFT, deltatime);
            if (Qt::Key_D == key)
                camera->ProcessKeyboard(RIGHT, deltatime);
            if (Qt::Key_W == key)
                camera->ProcessKeyboard(FORWARD, deltatime);
            if (Qt::Key_S == key)
                camera->ProcessKeyboard(BACKWARD, deltatime);
            if (Qt::Key_Space == key)
            {
                float timeValue = GetRuntime()*0.04f;
                float ang = radians(timeValue);
                vec3 center = vec3(0.0f, 0.0f, -2.0f);
                vec3 pos = camera->Position;
                vec3 npos = pos;
                npos.x = (pos.x - center.x) * cos(ang) - (pos.z - center.z) * sin(ang) + center.x;
                npos.z = (pos.z - center.z) * cos(ang) + (pos.x - center.x) * sin(ang) + center.z;
                camera->RotateAt(npos, center);
            }
            if (Qt::Key_K == key)
                skybox->Equirect2Cube();
        }
        if(light)
        {
            if (Qt::Key_Left == key)
            {
                light->UpdateX(-deltatime);
                OnLightChange(GLFW_KEY_LEFT);
            }
            if (Qt::Key_Right == key)
            {
                light->UpdateX(deltatime);
                OnLightChange(GLFW_KEY_RIGHT);
            }
            if (Qt::Key_Up == key)
            {
                light->UpdateY(deltatime);
                OnLightChange(GLFW_KEY_UP);
            }
            if (Qt::Key_Down == key)
            {
                light->UpdateY(-deltatime);
                OnLightChange(GLFW_KEY_DOWN);
            }
        }
    }

    void ProcessMouseMove(double xoffset,double yoffset)
    {
        if(camera)
        {
            camera->ProcessMouseMovement(xoffset, yoffset);
        }
    }

    void ProcessMouseScroll(double xoffset, double yoffset)
    {
        if(camera)
        {
            camera->ProcessMouseScroll(yoffset);
        }
    }


protected:

    void ApplyCamera(Material* mat)
    {
        if(mat && mat->shader)
        {
            camera->Attach(mat->shader);
        }
    }

    void ApplyCamera(Shader* shader)
    {
        camera->Attach(shader);
    }

    void RenderQuad(GLuint map)
    {
        debugShader->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, map);
        DrawQuad();
    }

private:
    void InitDepthBuffer()
    {
        glGenFramebuffers(1, &depthMapFBO);
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_WIDTH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderQuad()
    {
        debugShader->use();
        debugShader->setFloat("near_plane", 1.0f);
        debugShader->setFloat("far_plane", 7.5f);
        RenderQuad(depthMap);
    }

    void DrawQuad()
    {
        glBindVertexArray(quadVAO);
        glDrawArrays(DRAW_MODE, 0, 6);
        glBindVertexArray(0);
    }


protected:
    Camera* camera = nullptr;
    Light*  light = nullptr;
    Skybox* skybox = nullptr;
    uint SHADOW_WIDTH = 1024;
    GLuint depthMap;
    float timeValue;
    mat4 lightMatrix;
    bool debug;
    GLuint quadVAO, quadVBO;
    Shader* depthShader = nullptr;
    Shader* debugShader = nullptr;
    GLuint depthMapFBO;
};


#endif /* scene_h */
