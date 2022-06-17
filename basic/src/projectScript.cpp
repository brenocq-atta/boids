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

#include "projectScript.h"
#include "boidComponent.h"
#include "settingsComponent.h"

#define PROT_BOID_EID 1// Prototype boid entity id
#define SETTINGS_EID 5// Settings entity id

Project::Project():
    _running(false)
{

}

void Project::onStart()
{
    _running = true;

    srand(31415);
    atta::Factory* factory = atta::ComponentManager::getPrototypeFactory(PROT_BOID_EID);
    for(atta::EntityId boid : factory->getCloneIds())
    {
        atta::TransformComponent* t = atta::ComponentManager::getEntityComponent<atta::TransformComponent>(boid);
        BoidComponent* b = atta::ComponentManager::getEntityComponent<BoidComponent>(boid);

        // Initialize each boid position
        t->position.x = ((rand()%1000)/1000.0f - 0.5f)*10.0f;
        t->position.y = ((rand()%1000)/1000.0f - 0.5f)*10.0f;

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

    mainParemeters();
    ImGui::Separator();
    boidParemeters();
    
    ImGui::End();
}

void Project::mainParemeters()
{
    SettingsComponent* s = atta::ComponentManager::getEntityComponent<SettingsComponent>(SETTINGS_EID);
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

    ImGui::Text("View Expoent");
    ImGui::DragFloat("###DragViewExpoent", &s->viewExpoent, 0.05f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_None);

    ImGui::Text("Noise");
    ImGui::DragFloat("###DragNoise", &s->noise, 0.01f, 0.0f, 5.0f, "%.2f", ImGuiSliderFlags_None);
}
