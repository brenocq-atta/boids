//--------------------------------------------------
// Boids Basic
// boidScript.cpp
// Date: 2022-01-29
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "boidScript.h"
#include "boidComponent.h"
#include "settingsComponent.h"
#include <atta/componentSystem/components/transformComponent.h>
#include <atta/componentSystem/components/relationshipComponent.h>
#include <atta/componentSystem/components/meshComponent.h>
#include <random>
using namespace atta;

#define OBSTACLES_EID 2// Obstacles entity id
#define SETTINGS_EID 5// Settings entity id

// Walls
#define TOP_WALL_EID 6
#define BOTTOM_WALL_EID 7
#define RIGHT_WALL_EID 8
#define LEFT_WALL_EID 9

void BoidScript::update(Entity entity, float dt)
{
    SettingsComponent* s = ComponentManager::getEntityComponent<SettingsComponent>(SETTINGS_EID);
    BoidComponent* b = entity.getComponent<BoidComponent>();
    TransformComponent* t = entity.getComponent<TransformComponent>();

    std::vector<vec2> neighbourVecs;
    for(EntityId neighbour : b->neighbors)
        neighbourVecs.push_back(getNeighbourVec(entity, neighbour));

    vec2 force {};
    force += collisionAvoidance(entity, neighbourVecs) * s->collisionAvoidanceFactor;
    force += velocityMatching(entity) * s->velocityMatchingFactor;
    force += flockCentering(entity, neighbourVecs) * s->flockCenteringFactor;
    force += obstacleAvoidance(entity) * 30.0f;

    b->acceleration = force;
}

vec2 BoidScript::collisionAvoidance(Entity entity, const std::vector<vec2>& neighbourVecs)
{
    SettingsComponent* s = ComponentManager::getEntityComponent<SettingsComponent>(SETTINGS_EID);
    BoidComponent* b = entity.getComponent<BoidComponent>();
    TransformComponent* t = entity.getComponent<TransformComponent>();

    vec2 avoidanceVector = vec2(0.0f);
    for(vec2 neighVec: neighbourVecs)
    {
        vec2 avoidVec = -neighVec;
        if(avoidVec == vec2(0.0f)) continue;// Ignore if they are overlapping

        vec2 dir = normalize(avoidVec);
        float dist = std::max(avoidVec.length(), 0.00001f);
        avoidanceVector += dir/(dist*dist);
    }
    if(neighbourVecs.size())
        avoidanceVector /= neighbourVecs.size();

    return avoidanceVector;
}

vec2 BoidScript::velocityMatching(Entity entity)
{
    SettingsComponent* s = ComponentManager::getEntityComponent<SettingsComponent>(SETTINGS_EID);
    BoidComponent* b = entity.getComponent<BoidComponent>();

    std::default_random_engine generator;
    std::normal_distribution<float> distribution(0.0f, s->noise);

    vec2 velVector = b->velocity;
    for(EntityId neighbor : b->neighbors)
    {
        BoidComponent* bo = ComponentManager::getEntityComponent<BoidComponent>(neighbor);
        velVector += bo->velocity;

        float rx = distribution(generator);
        float ry = distribution(generator);
        velVector += vec2(rx, ry);
    }
    velVector /= (b->neighbors.size() + 1);
    
    // Steering force
    return velVector-b->velocity;
}

vec2 BoidScript::flockCentering(Entity entity, const std::vector<vec2>& neighbourVecs)
{
    SettingsComponent* s = ComponentManager::getEntityComponent<SettingsComponent>(SETTINGS_EID);
    TransformComponent* t = entity.getComponent<TransformComponent>();

    // Average neighbours positions
    vec2 avgLoc = vec2(0.0f);
    for(vec2 neighVec : neighbourVecs)
        avgLoc += vec2(t->position)+neighVec;
    if(neighbourVecs.size())
        avgLoc /= neighbourVecs.size();
    
    return avgLoc-vec2(t->position);
}

vec2 BoidScript::getNeighbourVec(Entity entity, EntityId neighbour)
{
    SettingsComponent* s = ComponentManager::getEntityComponent<SettingsComponent>(SETTINGS_EID);
    TransformComponent* t = entity.getComponent<TransformComponent>();
    TransformComponent* tn = ComponentManager::getEntityComponent<TransformComponent>(neighbour);

    vec2 neighVec = vec2(tn->position-t->position);
    vec2 norm = normalize(neighVec);
    float dist = neighVec.length();

    std::default_random_engine generator;
    std::normal_distribution<float> distribution(0.0f, s->noise);
    float r = distribution(generator);

    return norm*(dist+r);
}

atta::vec2 BoidScript::worldForceField(atta::vec2 position)
{
    // Walls
    static TransformComponent* tw = ComponentManager::getEntityComponent<TransformComponent>(TOP_WALL_EID);
    static TransformComponent* bw = ComponentManager::getEntityComponent<TransformComponent>(BOTTOM_WALL_EID);
    static TransformComponent* lw = ComponentManager::getEntityComponent<TransformComponent>(LEFT_WALL_EID);
    static TransformComponent* rw = ComponentManager::getEntityComponent<TransformComponent>(RIGHT_WALL_EID);

    vec2 forceField;

    // Deal with fixed walls
    if(tw)// Top wall
    {
        float dist = tw->position.y - position.y;
        vec2 normal(0, -1);
        forceField += dist > 0.05f ? normal/(dist*dist) : normal*1000.0f;
    }
    if(bw)// Bottom wall
    {
        float dist = position.y - bw->position.y;
        vec2 normal(0, 1);
        forceField += dist > 0.05f ? normal/(dist*dist) : normal*1000.0f;
    }
    if(lw)// Left wall
    {
        float dist = position.x - lw->position.x;
        vec2 normal(1, 0);
        forceField += dist > 0.05f ? normal/(dist*dist) : normal*1000.0f;
    }
    if(rw)// Right wall
    {
        float dist = rw->position.x - position.x;
        vec2 normal(-1, 0);
        forceField += dist > 0.05f ? normal/(dist*dist) : normal*1000.0f;
    }

    // Deal with dynamic obstacle
    RelationshipComponent* obsr = ComponentManager::getEntityComponent<RelationshipComponent>(OBSTACLES_EID);
    for(EntityId obstacle : obsr->getChildren())
    {
        MeshComponent* obsM = ComponentManager::getEntityComponent<MeshComponent>(obstacle);
        TransformComponent* obsT = ComponentManager::getEntityComponent<TransformComponent>(obstacle);
        switch(obsM->sid.getId())
        {
            case "meshes/disk.obj"_sid:
            case "meshes/sphere.obj"_sid:
                {
                    vec2 distDir = vec2(position) - vec2(obsT->position);
                    float radius = obsT->scale.x;
                    forceField += 0.4f * (normalize(distDir) * radius)/distDir.squareLength();
                    break;
                }
            case "meshes/plane.obj"_sid:
            case "meshes/box.obj"_sid:
                break;
            default:
                LOG_WARN("BoidScript", "Trying to avoid unknown obstacle [w]$0[]", obsM->sid.getString());
        }
    }
    return forceField;
}

vec2 BoidScript::obstacleAvoidance(Entity entity)
{
    // Boid info
    BoidComponent* b = entity.getComponent<BoidComponent>();
    TransformComponent* t = entity.getComponent<TransformComponent>();

    vec2 forceField = worldForceField(vec2(t->position));
    vec2 steering = normalize(normalize(forceField) - normalize(b->velocity));

    return forceField;
}
