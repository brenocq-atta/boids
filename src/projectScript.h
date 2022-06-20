//--------------------------------------------------
// Boids Basic
// projectScript.h
// Date: 2022-01-29
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef PROJECT_SCRIPT_H
#define PROJECT_SCRIPT_H
#include <atta/pch.h>
#include <atta/scriptSystem/projectScript.h>

class Project : public atta::ProjectScript
{
public:
    Project();

    //---------- Simulation ----------//
	void onStart() override;
	void onStop() override;
	void onUpdateBefore(float) override;
	void onUpdateAfter(float dt) override;
    void onAttaLoop() override;

    //---------- UI ----------//
    void onUIRender() override;

private:
    void initBoids();
    void mainParemeters();
    void boidParemeters();
    void updateWalls();

    bool _running;
    //std::shared_ptr<atta::Image> _bgImage;
};

ATTA_REGISTER_PROJECT_SCRIPT(Project)

#endif// PROJECT_SCRIPT_H
