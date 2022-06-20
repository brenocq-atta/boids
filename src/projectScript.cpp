//--------------------------------------------------
// Boid Basic
// projectScript.cpp
// Date: 2022-01-29
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <atta/componentSystem/componentManager.h>
#include <atta/componentSystem/components/transformComponent.h>
#include <atta/componentSystem/components/materialComponent.h>
#include <imgui.h>
#include <imgui_internal.h>// Disable items
//#include <implot.h>

#include "projectScript.h"
#include "boidComponent.h"
#include "settingsComponent.h"

#define PROT_BOID_EID 1// Prototype boid entity id
#define SETTINGS_EID 5// Settings entity id

// Walls
#define TOP_WALL_EID 6
#define BOTTOM_WALL_EID 7
#define RIGHT_WALL_EID 8
#define LEFT_WALL_EID 9
#define BACKGROUND_EID 4

Project::Project():
    _running(false)
{

}

void Project::onStart()
{
    _running = true;

    srand(42);
    initBoids();

    // TODO assign image to background and update with force vector
    //atta::Image::CreateInfo info {};
    //info.width = 100;
    //info.height = 100;
    //std::shared_ptr<atta::Image> _bgImage = atta::GraphicsManager::create<atta::Image>(info);
}

void Project::initBoids()
{
    // Initialize boids randomly
    atta::Factory* factory = atta::ComponentManager::getPrototypeFactory(PROT_BOID_EID);
    for(atta::EntityId boid : factory->getCloneIds())
    {
        atta::TransformComponent* t = atta::ComponentManager::getEntityComponent<atta::TransformComponent>(boid);
        BoidComponent* b = atta::ComponentManager::getEntityComponent<BoidComponent>(boid);

        // Calculate size and offset
        vec3& tp = ComponentManager::getEntityComponent<TransformComponent>(TOP_WALL_EID)->position;
        vec3& bp = ComponentManager::getEntityComponent<TransformComponent>(BOTTOM_WALL_EID)->position;
        vec3& lp = ComponentManager::getEntityComponent<TransformComponent>(LEFT_WALL_EID)->position;
        vec3& rp = ComponentManager::getEntityComponent<TransformComponent>(RIGHT_WALL_EID)->position;
        vec2 offset((rp.x+lp.x)/2.0f, (tp.y+bp.y)/2.0f);
        vec2 size(rp.x-lp.x, tp.y-bp.y);

        // Initialize each boid position
        t->position.x = ((rand()%1000)/1000.0f - 0.5f)*size.x+offset.x;
        t->position.y = ((rand()%1000)/1000.0f - 0.5f)*size.y+offset.y;

        float rAngle = (rand()%1000)/1000.0f;
        b->velocity.x = cos(rAngle);
        b->velocity.y = sin(rAngle);
    }
}

void Project::onStop()
{
    _running = false;
}

void Project::onUpdateBefore(float)
{
    SettingsComponent* s = atta::ComponentManager::getEntityComponent<SettingsComponent>(SETTINGS_EID);

    updateWalls();
    // Update neighbors
    atta::Factory* factory = atta::ComponentManager::getPrototypeFactory(PROT_BOID_EID);
    for(atta::EntityId boid : factory->getCloneIds())
    {
        atta::TransformComponent* t = atta::ComponentManager::getEntityComponent<atta::TransformComponent>(boid);
        BoidComponent* boidInfo = atta::ComponentManager::getEntityComponent<BoidComponent>(boid);
        boidInfo->neighbors.clear();

        for(atta::EntityId other : factory->getCloneIds())
        {
            if(other == boid) continue;

            // Test if other is a neighbor
            atta::TransformComponent* ot = atta::ComponentManager::getEntityComponent<atta::TransformComponent>(other);
            if((vec2(ot->position)-vec2(t->position)).length() <= s->viewRadius)
                boidInfo->neighbors.push_back(other);
        }
    }
}

void Project::onUpdateAfter(float dt)
{
    float maxAcc = 3.0;
    float maxVel = 10;

    // Update positions, velocities and accelerations
    atta::Factory* factory = atta::ComponentManager::getPrototypeFactory(PROT_BOID_EID);
    for(atta::EntityId boid : factory->getCloneIds())
    {
        atta::TransformComponent* t = atta::ComponentManager::getEntityComponent<atta::TransformComponent>(boid);
        BoidComponent* boidInfo = atta::ComponentManager::getEntityComponent<BoidComponent>(boid);

        // Limit vectors
        if(boidInfo->acceleration.length() > maxAcc)
            boidInfo->acceleration = normalize(boidInfo->acceleration) * maxAcc;
        if(boidInfo->velocity.length() > maxVel)
            boidInfo->velocity = normalize(boidInfo->velocity) * maxVel;

        // Update velocity
        boidInfo->velocity += boidInfo->acceleration*dt;
        boidInfo->velocity.normalize();
        boidInfo->acceleration = vec2(0.0f);

        // Apply velocity to boid
        t->position += vec3(boidInfo->velocity*dt, 0.0f);
        if(boidInfo->velocity.length() > 0)
            t->orientation.rotationFromVectors( 
                    normalize(vec3(boidInfo->velocity, 0.0f)), vec3(0, -1, 0));
    }
}

void Project::onAttaLoop()
{

}

void Project::onUIRender()
{
    ImGui::Begin("Configure");
    {
        mainParemeters();
        ImGui::Separator();
        boidParemeters();
    }
    ImGui::End();

    ImGui::Begin("Inspect agent");
    {
        // TODO inspect agent
        //std::vector<int> xs = {0, 1, 2, 3, 4};
        //std::vector<int> ys = {0, 1, 0, 1, 0};
        //if(ImPlot::BeginPlot("##Position"))
        //{
        //    ImPlot::PlotLine("X", xs.data(), ys.data(), xs.size());
        //    ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
        //    ImPlot::PlotLine("X", xs.data(), ys.data(), xs.size());
        //    ImPlot::EndPlot();
        //}
    }
    ImGui::End();
}

void Project::mainParemeters()
{
    SettingsComponent* s = atta::ComponentManager::getEntityComponent<SettingsComponent>(SETTINGS_EID);

    if(ImGui::Button("Reset"))
        initBoids();

    ImGui::Text("Main parameters");

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    float min = 0.0f;
    float max = 5.0f;

    const char* names[3] = {"Collision avoidance factor", "Velocity matching factor", "Flock centering factor"};
    static bool active[3] = { true, true, true };
    static float lastVal[3] = { s->collisionAvoidanceFactor, s->velocityMatchingFactor, s->flockCenteringFactor };
    float* paramPtr[3] = { &s->collisionAvoidanceFactor, &s->velocityMatchingFactor, &s->flockCenteringFactor };

    // Render checkbox and sliders
    for(unsigned i = 0; i < 3; i++)
    {
        ImGui::Text("%s", names[i]);

        if(ImGui::Checkbox((std::string("###Active")+names[i]).c_str(), &active[i]))
        {
            if(active[i] == false)
            {
                lastVal[i] = *paramPtr[i];
                *paramPtr[i] = 0.0f;
            }
            else
            {
                *paramPtr[i] = lastVal[i];
            }
        }

        ImGui::SameLine(); 
        if(!active[i])
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);

        ImGui::SliderScalar((std::string("###Slider")+names[i]).c_str(), ImGuiDataType_Float, paramPtr[i], &min, &max, "%.6f", ImGuiSliderFlags_None);

        if(!active[i])
            ImGui::PopItemFlag();
    }
}

void Project::boidParemeters()
{
    SettingsComponent* s = atta::ComponentManager::getEntityComponent<SettingsComponent>(SETTINGS_EID);
    ImGui::Text("Boid parameters");

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    ImGui::Text("View Radius");
    ImGui::DragFloat("###DragViewRadisu", &s->viewRadius, 0.05f, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_None);

    ImGui::Text("Noise");
    ImGui::DragFloat("###DragNoise", &s->noise, 0.01f, 0.0f, 5.0f, "%.2f", ImGuiSliderFlags_None);

    ImGui::Text("Tip: You can move the walls");
    ImGui::Text("Tip: You can add more circles");
}

void Project::updateWalls()
{
    static TransformComponent* t = ComponentManager::getEntityComponent<TransformComponent>(TOP_WALL_EID);
    static TransformComponent* b = ComponentManager::getEntityComponent<TransformComponent>(BOTTOM_WALL_EID);
    static TransformComponent* l = ComponentManager::getEntityComponent<TransformComponent>(LEFT_WALL_EID);
    static TransformComponent* r = ComponentManager::getEntityComponent<TransformComponent>(RIGHT_WALL_EID);
    static TransformComponent* bg = ComponentManager::getEntityComponent<TransformComponent>(BACKGROUND_EID);
    
    if(t && b && l && r && bg)
    {
        // Positions
        vec3& tp = t->position;
        vec3& bp = b->position;
        vec3& lp = l->position;
        vec3& rp = r->position;
        vec3& bgp = bg->position;

        // Scales
        vec3& ts = t->scale;
        vec3& bs = b->scale;
        vec3& ls = l->scale;
        vec3& rs = r->scale;
        vec3& bgs = bg->scale;

        // Offsets
        vec2 offset((rp.x+lp.x)/2.0f, (tp.y+bp.y)/2.0f);
        vec2 size(rp.x-lp.x, tp.y-bp.y);

        // Update background position/scale
        bgp = vec3(offset, -1.0f);
        bgs = vec3(size, 1.0f);

        // Update wall positions
        tp.x = bp.x = offset.x;
        rp.y = lp.y = offset.y;

        // Update wall scales
        ts.x = bs.x = size.x;
        rs.y = ls.y = size.y;
    }
}
