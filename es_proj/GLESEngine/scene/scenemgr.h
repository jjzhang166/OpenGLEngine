//
//  scenemgr.h
//  OpenGLEngine
//
//  Created by 彭怀亮 on 5/16/19.
//  Copyright © 2019 彭怀亮. All rights reserved.
//

#ifndef scenemgr_h
#define scenemgr_h

#include "scene.h"
#include "arscene.h"
#include "scene1.h"
#include "scene2.h"
#include "scene3.h"
#include "scene4.h"
#include "scene5.h"
#include "scene6.h"
#include "texmgr.hpp"
#include "IARInterface.h"

using namespace engine;

class SceneMgr : iSceneMgr
{
    DeclareSington(SceneMgr)
    
private:
    ~SceneMgr()
    {
        LeaveScene();
        SAFE_DELETE(lb_scene1);
        SAFE_DELETE(lb_scene2);
        SAFE_DELETE(lb_scene3);
        SAFE_DELETE(lb_scene4);
        SAFE_DELETE(lb_scene5);
        SAFE_DELETE(lb_scene6);
        SAFE_DELETE(lb_fps);
        SAFE_DELETE(lb_copy);
    }
    
    void ClickScene(UIObject* contex)
    {
        instance.ChangeTo(TY_Scene1 + contex->uid);
    }

    
public:
    
    void Init()
    {
        ChangeTo(TY_Scene2);
        lb_scene1 = new engine::UILabel(vec2(60,450), vec3(1), 1, "Scene1", TY_Scene1);
        lb_scene2 = new engine::UILabel(vec2(60,405), vec3(1), 1, "Scene2", TY_Scene2);
        lb_scene3 = new engine::UILabel(vec2(60,360), vec3(1), 1, "Scene3", TY_Scene3);
        lb_scene4 = new engine::UILabel(vec2(60,315), vec3(1), 1, "Scene4", TY_Scene4);
        lb_scene5 = new engine::UILabel(vec2(60,270), vec3(1), 1, "Scene5", TY_Scene5);
        lb_scene6 = new engine::UILabel(vec2(60,225), vec3(1), 1, "Scene6", TY_Scene6);
        lb_fps = new engine::UILabel(vec2(740,580), vec3(1,0,0), 0.5f);
        lb_copy = new engine::UILabel(vec2(20), vec3(1), 0.4f);
        auto f  = Bindfunc(SceneMgr::ClickScene);
        lb_scene1->RegistCallback(f);
        lb_scene2->RegistCallback(f);
        lb_scene3->RegistCallback(f);
        lb_scene4->RegistCallback(f);
        lb_scene5->RegistCallback(f);
        lb_scene6->RegistCallback(f);
    }
    
    void LeaveScene()
    {
        SAFE_DELETE(current);
    }
    
    void Draw(float delta)
    {
        if(current) current->DrawScenes();
        delay++;
        if(delay % 4 == 0)
        {
            lb_fps->setText("FPS: " + to_string_with_precision(1/delta, 4));
            lb_copy->setText("@copyright (c) penghuailiang");
        }
    }
    
    bool ChangeTo(int type)
    {
        if(current && type == current->getType())
        {
            std::cout<<"You are already enter scene "<<(current->getType() + 1)<<std::endl;
            return false;
        }
        else
        {
            LeaveScene();
            if(type == TY_Scene1)   current = new Scene1();
            if(type == TY_Scene2)   current = new Scene2();
            if(type == TY_Scene3)   current = new Scene3();
            if(type == TY_Scene4)   current = new Scene4();
            if(type == TY_Scene5)   current = new Scene5();
            if(type == TY_Scene6)   current = new Scene6();
            if(current)
            {
                current->Initial();
                if(current->isARScene())
                {
                    auto vr = (ARScene*)current;
                    vr->Process(ptr_ar);
                }
                setEngineScene();
            }
            return true;
        }
    }
    
    void setEngineScene()
    {
        scene = (iScene*)(current);
    }



public:
    Scene *current = nullptr;
    IARInterface* ptr_ar;
    
private:
    engine::UILabel *lb_scene1, *lb_scene2, *lb_scene3, *lb_scene4, *lb_scene5, *lb_scene6;
    engine::UILabel *lb_fps, *lb_copy;
    uint delay;
};

#endif /* scenemgr_h */
