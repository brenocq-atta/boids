//--------------------------------------------------
// Boids Basic
// boidScript.cpp
// Date: 2022-01-29
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "boidScript.h"
#include "boidComponent.h"
#include "common.h"
#include "forceField.h"
#include "settingsComponent.h"
#include <atta/component/components/mesh.h>
#include <atta/component/components/relationship.h>
#include <atta/component/components/transform.h>
#include <random>

void BoidScript::update(cmp::Entity entity, float dt) {
    BoidComponent* b = entity.get<BoidComponent>();
    cmp::Transform* t = entity.get<cmp::Transform>();
    SettingsComponent* s = settings.get<SettingsComponent>();

    std::vector<atta::vec2> neighbourVecs;
    for (cmp::EntityId neighbour : b->neighbors)
        neighbourVecs.push_back(getNeighbourVec(entity, neighbour));

    atta::vec2 force{};
    force += collisionAvoidance(entity, neighbourVecs) * s->collisionAvoidanceFactor;
    force += velocityMatching(entity) * s->velocityMatchingFactor;
    force += flockCentering(entity, neighbourVecs) * s->flockCenteringFactor;
    force += obstacleAvoidance(entity) * 30.0f;

    b->acceleration = force;
}

atta::vec2 BoidScript::collisionAvoidance(cmp::Entity entity, const std::vector<atta::vec2>& neighbourVecs) {
    BoidComponent* b = entity.get<BoidComponent>();
    cmp::Transform* t = entity.get<cmp::Transform>();

    atta::vec2 avoidanceVector = atta::vec2(0.0f);
    for (atta::vec2 neighVec : neighbourVecs) {
        atta::vec2 avoidVec = -neighVec;
        if (avoidVec == atta::vec2(0.0f))
            continue; // Ignore if they are overlapping

        atta::vec2 dir = normalize(avoidVec);
        float dist = std::max(avoidVec.length(), 0.00001f);
        avoidanceVector += dir / (dist * dist);
    }
    if (neighbourVecs.size())
        avoidanceVector /= neighbourVecs.size();

    return avoidanceVector;
}

atta::vec2 BoidScript::velocityMatching(cmp::Entity entity) {
    BoidComponent* b = entity.get<BoidComponent>();

    std::default_random_engine generator;
    std::normal_distribution<float> distribution(0.0f, settings.get<SettingsComponent>()->noise);

    atta::vec2 velVector = b->velocity;
    for (cmp::EntityId neighbor : b->neighbors) {
        BoidComponent* bo = cmp::getComponent<BoidComponent>(neighbor);
        velVector += bo->velocity;

        float rx = distribution(generator);
        float ry = distribution(generator);
        velVector += atta::vec2(rx, ry);
    }
    velVector /= (b->neighbors.size() + 1);

    // Steering force
    return velVector - b->velocity;
}

atta::vec2 BoidScript::flockCentering(cmp::Entity entity, const std::vector<atta::vec2>& neighbourVecs) {
    cmp::Transform* t = entity.get<cmp::Transform>();

    // Average neighbours positions
    atta::vec2 avgLoc = atta::vec2(0.0f);
    for (atta::vec2 neighVec : neighbourVecs)
        avgLoc += atta::vec2(t->position) + neighVec;
    if (neighbourVecs.size())
        avgLoc /= neighbourVecs.size();

    return avgLoc - atta::vec2(t->position);
}

atta::vec2 BoidScript::getNeighbourVec(cmp::Entity entity, cmp::Entity neighbour) {
    // Calculate vector from entity to neighbour with noise
    cmp::Transform* t = entity.get<cmp::Transform>();
    cmp::Transform* tn = neighbour.get<cmp::Transform>();

    atta::vec2 neighVec = atta::vec2(tn->position - t->position);
    atta::vec2 norm = atta::normalize(neighVec);
    float dist = neighVec.length();

    std::default_random_engine generator;
    std::normal_distribution<float> distribution(0.0f, settings.get<SettingsComponent>()->noise);
    float r = distribution(generator);

    return norm * (dist + r);
}

atta::vec2 BoidScript::obstacleAvoidance(cmp::Entity entity) {
    // Boid info
    BoidComponent* b = entity.get<BoidComponent>();
    cmp::Transform* t = entity.get<cmp::Transform>();

    atta::vec2 forceField = getForceField(atta::vec2(t->position));
    atta::vec2 steering = atta::normalize(atta::normalize(forceField) - atta::normalize(b->velocity));

    return forceField;
}
