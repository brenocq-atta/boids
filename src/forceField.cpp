//--------------------------------------------------
// Boids Basic
// forceField.cpp
// Date: 2022-08-02
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "forceField.h"
#include <atta/component/components/mesh.h>
#include <atta/component/components/relationship.h>
#include <atta/component/components/transform.h>

namespace cmp = atta::component;

// Fixed entities
#include "common.h"

atta::vec2 getForceField(atta::vec2 position) {
    // Walls
    static cmp::Transform* tw = topWall.get<cmp::Transform>();
    static cmp::Transform* bw = bottomWall.get<cmp::Transform>();
    static cmp::Transform* lw = leftWall.get<cmp::Transform>();
    static cmp::Transform* rw = rightWall.get<cmp::Transform>();

    atta::vec2 forceField;

    // Deal with fixed walls
    if (tw) // Top wall
    {
        float dist = tw->position.y - position.y;
        atta::vec2 normal(0, -1);
        forceField += dist > 0.05f ? normal / (dist * dist) : normal * 1000.0f;
    }
    if (bw) // Bottom wall
    {
        float dist = position.y - bw->position.y;
        atta::vec2 normal(0, 1);
        forceField += dist > 0.05f ? normal / (dist * dist) : normal * 1000.0f;
    }
    if (lw) // Left wall
    {
        float dist = position.x - lw->position.x;
        atta::vec2 normal(1, 0);
        forceField += dist > 0.05f ? normal / (dist * dist) : normal * 1000.0f;
    }
    if (rw) // Right wall
    {
        float dist = rw->position.x - position.x;
        atta::vec2 normal(-1, 0);
        forceField += dist > 0.05f ? normal / (dist * dist) : normal * 1000.0f;
    }

    // Deal with dynamic obstacle
    cmp::Relationship* obsr = obstacles.get<cmp::Relationship>();
    for (cmp::EntityId eid : obsr->getChildren()) {
        cmp::Entity obstacle{eid};
        cmp::Mesh* obsM = obstacle.get<cmp::Mesh>();
        cmp::Transform* obsT = obstacle.get<cmp::Transform>();
        switch (obsM->sid.getId()) {
        case "meshes/disk.obj"_sid:
        case "meshes/sphere.obj"_sid: {
            atta::vec2 distDir = atta::vec2(position) - atta::vec2(obsT->position);
            float radius = obsT->scale.x;
            forceField += 0.4f * (atta::normalize(distDir) * radius) / distDir.squareLength();
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

