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

vec2 BoidScript::obstacleAvoidance(Entity entity)
{
    BoidComponent* b = entity.getComponent<BoidComponent>();
    TransformComponent* t = entity.getComponent<TransformComponent>();
    RelationshipComponent* obsr = ComponentManager::getEntityComponent<RelationshipComponent>(OBSTACLES_EID);

    vec2 avoidanceForce {};

    // Deal with each obstacle
    for(EntityId obstacle : obsr->getChildren())
    {
        MeshComponent* obsM = ComponentManager::getEntityComponent<MeshComponent>(obstacle);
        TransformComponent* obsT = ComponentManager::getEntityComponent<TransformComponent>(obstacle);
        switch(obsM->sid.getId())
        {
            case "meshes/disk.obj"_sid:
                {
                    // TODO only working with circles (not ellipsis)
                    vec2 obsDir = vec2(obsT->position)-vec2(t->position);
                    vec2 steering = normalize(normalize(b->velocity)-normalize(obsDir));
                    float radius = obsT->scale.x;
                    float dist = obsDir.length();

                    if(dist > 0)
                        avoidanceForce += steering * (radius/(dist*dist));
                    break;
                }
            case "meshes/plane.obj"_sid:
            case "meshes/cube.obj"_sid:
                {
                    // Cubes will be ignored, the walls are made of cubes but are handled as half space planes
                    vec2 halfSpaceNormal {};
                    float dist = 0.0f;
                    if(obsT->position.x > 0)
                    {
                        halfSpaceNormal = vec2(-1, 0);
                        dist = obsT->position.x - t->position.x;
                    }
                    else if(obsT->position.x < 0)
                    {
                        halfSpaceNormal = vec2(1, 0);
                        dist = t->position.x - obsT->position.x;
                    }
                    else if(obsT->position.y > 0)
                    {
                        halfSpaceNormal = vec2(0, -1);
                        dist = obsT->position.y - t->position.y;
                    }
                    else if(obsT->position.y < 0)
                    {
                        halfSpaceNormal = vec2(0, 1);
                        dist = t->position.y - obsT->position.y;
                    }
    
                    vec2 steering = normalize(normalize(vec2(b->velocity))-halfSpaceNormal);
                    vec2 force {};
                    if(dist > 0.5f)
                        force = halfSpaceNormal / (dist*dist);
                    else
                        force = halfSpaceNormal * 1000.0f;

                    //if(entity.getCloneId() == 0)
                    //    LOG_DEBUG("BOidScript", "Normal $0, force: $1, dist: $2", halfSpaceNormal, force, dist);

                    avoidanceForce += force;
                    break;
                }
            default:
                LOG_WARN("BoidScript", "Trying to avoid unknown obstacle [w]$0[]", obsM->sid.getString());
        }
    }
    
    
    return avoidanceForce;
}
