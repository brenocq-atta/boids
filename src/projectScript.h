//--------------------------------------------------
// Boids Basic
// projectScript.h
// Date: 2022-01-29
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef PROJECT_SCRIPT_H
#define PROJECT_SCRIPT_H
#include <atta/pch.h>
#include <atta/resource/resources/image.h>
#include <atta/script/projectScript.h>

namespace scr = atta::script;
namespace rsc = atta::resource;

class Project : public scr::ProjectScript {
  public:
    Project();

    //---------- Simulation ----------//
    void onLoad() override;
    void onStart() override;
    void onStop() override;
    void onUpdateBefore(float) override;
    void onUpdateAfter(float dt) override;

    //---------- UI ----------//
    void onUIRender() override;

  private:
    void initBoids();
    void updateWalls();
    void updateBackground();

    // UI
    void mainParemeters();
    void boidParemeters();
    void boidInspect();

    bool _running;
    rsc::Image* _bgImage;
};

ATTA_REGISTER_PROJECT_SCRIPT(Project)

#endif // PROJECT_SCRIPT_H
