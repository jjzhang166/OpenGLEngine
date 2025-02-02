//
//  light.cpp
//  OpenGLEngine
//
//  Created by 彭怀亮 on 5/28/19.
//  Copyright © 2019 彭怀亮. All rights reserved.
//

#include "light.hpp"
#include "iScene.h"
#include "camera.hpp"

namespace engine
{

    Light::Light(vec3 color,vec3 direction)
    {
        this->color = color;
        this->direction = direction;
    }


    void Light::UpdateX(float dx)
    {
        if(direction.x + dx < radians(60.0f))
        {
            direction.x += dx;
        }
    }

    void Light::UpdateY(float dy)
    {
        if(direction.y + dy < radians(60.0f))
        {
            direction.y += dy;
        }
    }

    void DirectLight::Apply(const Material* mat)
    {
        if(mat && mat->shader) Apply(mat->shader);
    }
    
    void DirectLight::Apply(const Shader* shader)
    {
        shader->setVec3("light.color", color);
        shader->setVec3("light.direction", direction);
    }

    mat4 DirectLight::GetLigthSpaceMatrix(float near, float far, float up, float left)
    {
        error_stop(scene, "not implement iScene in your scenemgr & scene");
        Camera* camera = scene->getCamera();
        glm::vec3 pos = camera->Position;
        glm::vec3 frt = vec3(4) * glm::normalize(camera->Front);
        glm::vec3 target = pos + frt;
        
        vec3 dir = normalize(direction);
        int sc = 4;
        vec3 pos2 = target - vec3(sc * dir.x, sc * dir.y, sc * dir.z);
        mat4 view =  lookAt(pos2, target, vec3(0,1,0));
        mat4 proj = glm::ortho(-left, left, -up, up, near, far);
        return proj * view;
    }

    LightType  DirectLight::getType() const
    {
        return LightDirect;
    }

    std::string DirectLight::getMacro() const
    {
        return "_DirectLight_";
    }
    
    PointLight::PointLight(vec3 color, vec3 direction, vec3 pos, vec3 constant)
    :Light(color, direction)
    {
        this->pos=pos;
        this->constant=constant;
    }
    
    mat4 PointLight::GetLigthSpaceMatrix(float near, float far, float up, float left)
    {
        vec3 center = pos + direction;
        mat4 view = lookAt(pos, center, vec3(0,1,0));
        mat4 proj = glm::ortho(-left, left, -up, up, near, far);
        return proj * view;
    }
    
    void PointLight::Apply(const Material* mat)
    {
        if(mat && mat->shader) Apply(mat->shader);
    }

    void PointLight::Apply(const Shader* shader)
    {
        shader->setVec3("light.color", this->color);
        shader->setVec3("light.direction", this->direction);
        shader->setVec3("light.constant", this->constant);
        shader->setVec3("light.position", this->pos);
    }

    LightType PointLight::getType() const
    {
        return LightPoint;
    }

    std::string PointLight::getMacro() const
    {
        return "_PointLight_";
    }
    
    SpotLight::SpotLight(vec3 color, vec3 direction, vec3 pos, vec3 constant,float cutOff,float outerCutOff)
    :PointLight(color, direction, pos,constant)
    {
        this->cutOff= cos(glm::radians(cutOff));
        this->outerCutOff = cos(glm::radians(outerCutOff));
    }
    
    void SpotLight::Apply(const Material* mat)
    {
        if(mat && mat->shader) Apply(mat->shader);
    }

    void SpotLight::Apply(const Shader* shader)
    {
        PointLight::Apply(shader);
        shader->setFloat("light.cutOff", this->cutOff);
        shader->setFloat("light.outerCutOff", this->outerCutOff);
    }

    LightType SpotLight::getType() const
    {
        return LightSpot;
    }

    std::string SpotLight::getMacro() const
    {
        return "_SpotLight_";
    }

}
