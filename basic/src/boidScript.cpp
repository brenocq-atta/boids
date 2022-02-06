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
#include <random>
using namespace atta;

void BoidScript::update(Entity entity, float dt)
{
    SettingsComponent* s = ComponentManager::getEntityComponent<SettingsComponent>(0);
    BoidComponent* b = entity.getComponent<BoidComponent>();
    TransformComponent* t = entity.getComponent<TransformComponent>();

    std::vector<vec2> neighbourVecs;
    for(EntityId neighbour : b->neighbors)
        neighbourVecs.push_back(getNeighbourVec(entity, neighbour));

    vec2 force {};
    force += collisionAvoidance(entity, neighbourVecs) * s->collisionAvoidanceFactor * 100.0f;
    force += velocityMatching(entity) * s->velocityMatchingFactor;
    force += flockCentering(entity, neighbourVecs) * s->flockCenteringFactor;

    b->acceleration = force;
}

vec2 BoidScript::collisionAvoidance(Entity entity, const std::vector<vec2>& neighbourVecs)
{
    SettingsComponent* s = ComponentManager::getEntityComponent<SettingsComponent>(0);
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
    SettingsComponent* s = ComponentManager::getEntityComponent<SettingsComponent>(0);
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
    SettingsComponent* s = ComponentManager::getEntityComponent<SettingsComponent>(0);
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
    SettingsComponent* s = ComponentManager::getEntityComponent<SettingsComponent>(0);
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
