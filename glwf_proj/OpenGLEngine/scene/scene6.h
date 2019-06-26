//
//  scene6.h
//  OpenGLEngine
//
//  Created by 彭怀亮 on 5/26/19.
//  Copyright © 2019 彭怀亮. All rights reserved.
//

#ifndef scene6_h
#define scene6_h

#include "scene.h"

class Scene6 : public Scene
{
    
public:
    
    ~Scene6()
    {
        SAFE_DELETE(halo);
        SAFE_DELETE(shader);
    }
    
    int getType() { return TY_Scene6; }
    
    std::string getSkybox() { return "newport_loft"; }
    
    virtual bool isEquirectangularMap() { return true; }
    
    glm::vec3 getCameraPos() { return glm::vec3(0.0f,0.0f,16.0f); }
    
    static void OnClick(UIEvent* e, void* arg)
    {
        Scene6* scene = (Scene6*)(arg);
        int evtid = e->evtid;
        scene->Click(evtid);
    }
        
    void InitLight()
    {
        light = new DirectLight(vec3(1.0f), vec3(0.0f,0.0f,-2.0f));
    }
    
    void InitScene()
    {
        shader = new LightShader("model.vs", "model.fs");
        ApplyCamera(shader);
        halo = new Avatar("halo", vec3(-1.0f, -4.0f, -1.5f), vec3(1.0f), -60);
    }

    void DrawScene()
    {
        halo->Rotate(0.2f);
        halo->Draw(shader, light);
    }
    
    void Click(int anim)
    {
        switch (anim) {
            case 0:
                halo->PlayAnim("something");
                break;
            case 1:
                halo->PlayAnim("idle");
                break;
            case 2:
                halo->PauseAnim();
                break;
            case 3:
                halo->ResumeAnim();
                break;
            case 4:
                halo->SetTPose();
                break;
            default:
                break;
        }
    }
    
private:
    LightShader* shader;
    Avatar *halo;
};


#endif /* scene6_h */