//--------------------------------------------------
// Boids Basic
// boidScript.h
// Date: 2022-01-29
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef BOID_SCRIPT_H
#define BOID_SCRIPT_H
#include <atta/pch.h>
#include <atta/script/script.h>

namespace cmp = atta::component;
namespace scr = atta::script;

class BoidScript : public scr::Script {
  public:
    void update(cmp::Entity entity, float dt) override;

  private:
    atta::vec2 collisionAvoidance(cmp::Entity entity, const std::vector<atta::vec2>& neighbourVecs);
    atta::vec2 velocityMatching(cmp::Entity entity);
    atta::vec2 flockCentering(cmp::Entity entity, const std::vector<atta::vec2>& neighbourVecs);
    atta::vec2 obstacleAvoidance(cmp::Entity entity);

    atta::vec2 getNeighbourVec(cmp::Entity entity, cmp::Entity neighbour);
};

ATTA_REGISTER_SCRIPT(BoidScript)

#endif // BOID_SCRIPT_H
