//--------------------------------------------------
// Boids Basic
// boidScript.h
// Date: 2022-01-29
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef BOID_SCRIPT_H
#define BOID_SCRIPT_H
#include <atta/pch.h>
#include <atta/scriptSystem/script.h>

class BoidScript : public atta::Script
{
public:
    void update(atta::Entity entity, float dt) override;

private:
    atta::vec2 collisionAvoidance(atta::Entity entity, const std::vector<atta::vec2>& neighbourVecs);
    atta::vec2 velocityMatching(atta::Entity entity);
    atta::vec2 flockCentering(atta::Entity entity, const std::vector<atta::vec2>& neighbourVecs);
    atta::vec2 obstacleAvoidance(atta::Entity entity);

    atta::vec2 getNeighbourVec(atta::Entity entity, atta::EntityId neighbour);
};

ATTA_REGISTER_SCRIPT(BoidScript)

#endif// BOID_SCRIPT_H
