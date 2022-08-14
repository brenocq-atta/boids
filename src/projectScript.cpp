//--------------------------------------------------
// Boid Basic
// projectScript.cpp
// Date: 2022-01-29
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "projectScript.h"
#include "boidComponent.h"
#include "common.h"
#include "forceField.h"
#include <atta/component/components/material.h>
#include <atta/component/components/transform.h>
#include <atta/component/components/prototype.h>
#include <atta/component/interface.h>
#include <atta/resource/interface.h>

namespace scr = atta::script;
namespace rsc = atta::resource;

Project::Project() : _running(false), _bgImage(nullptr) {}

void Project::onLoad() {
    if (!_bgImage) {
        // Create image
        rsc::Image::CreateInfo info{};
        info.width = 100;
        info.height = 100;
        info.format = rsc::Image::Format::RGBA8;
        _bgImage = rsc::create<rsc::Image>("background.png", info);

        // Set image to all white
        uint8_t* data = _bgImage->getData();
        for (unsigned i = 0; i < info.width * info.height * 4; i++)
            data[i] = 255;
        _bgImage->update();
    }
}

void Project::onStart() {
    _running = true;
    srand(42);// Repeatable simulations
    initBoids();
}

void Project::initBoids() {
    // Initialize boids randomly
    for (cmp::Entity boid : cmp::getFactory(boidPrototype)->getClones()) {
        cmp::Transform* t = boid.get<cmp::Transform>();
        BoidComponent* b = boid.get<BoidComponent>();

        // Calculate size and offset
        atta::vec3& tp = topWall.get<cmp::Transform>()->position;
        atta::vec3& bp = bottomWall.get<cmp::Transform>()->position;
        atta::vec3& lp = leftWall.get<cmp::Transform>()->position;
        atta::vec3& rp = rightWall.get<cmp::Transform>()->position;
        atta::vec2 offset((rp.x + lp.x) / 2.0f, (tp.y + bp.y) / 2.0f);
        atta::vec2 size(rp.x - lp.x, tp.y - bp.y);

        // Initialize each boid position
        t->position.x = ((rand() % 1000) / 1000.0f - 0.5f) * size.x + offset.x;
        t->position.y = ((rand() % 1000) / 1000.0f - 0.5f) * size.y + offset.y;

        float rAngle = (rand() % 1000) / 1000.0f;
        b->velocity.x = cos(rAngle);
        b->velocity.y = sin(rAngle);
    }
}

void Project::onStop() { _running = false; }

void Project::onUpdateBefore(float) {
    updateWalls();
    updateBackground();

    // Update neighbors
    for (cmp::Entity boid : cmp::getFactory(boidPrototype)->getClones()) {
        cmp::Transform* t = boid.get<cmp::Transform>();
        BoidComponent* boidInfo = boid.get<BoidComponent>();
        boidInfo->neighbors.clear();

        // For each other boid -> O(n^2)
        for (cmp::Entity other : cmp::getFactory(boidPrototype)->getClones()) {
            if (boid == other)
                continue;

            // Test if other is a neighbor
            cmp::Transform* ot = other.get<cmp::Transform>();
            if ((atta::vec2(ot->position) - atta::vec2(t->position)).length() <= settings.get<SettingsComponent>()->viewRadius)
                boidInfo->neighbors.push_back(other);
        }
    }
}

void Project::onUpdateAfter(float dt) {
    const float maxAcc = 3.0;
    const float maxVel = 10;

    // Update positions, velocities and accelerations
    for (cmp::Entity boid : cmp::getFactory(boidPrototype)->getClones()) {
        cmp::Transform* t = boid.get<cmp::Transform>();
        BoidComponent* boidInfo = boid.get<BoidComponent>();

        // Limit vectors
        if (boidInfo->acceleration.length() > maxAcc)
            boidInfo->acceleration = atta::normalize(boidInfo->acceleration) * maxAcc;
        if (boidInfo->velocity.length() > maxVel)
            boidInfo->velocity = atta::normalize(boidInfo->velocity) * maxVel;

        // Update velocity
        boidInfo->velocity += boidInfo->acceleration * dt;
        boidInfo->velocity.normalize();
        boidInfo->acceleration = atta::vec2(0.0f);

        // Apply velocity to boid
        t->position += atta::vec3(boidInfo->velocity * dt, 0.0f);
        if (boidInfo->velocity.length() > 0)
            t->orientation.rotationFromVectors(atta::normalize(atta::vec3(boidInfo->velocity, 0.0f)), atta::vec3(0, -1, 0));
    }
}

void Project::updateWalls() {
    static cmp::Transform* t = topWall.get<cmp::Transform>();
    static cmp::Transform* b = bottomWall.get<cmp::Transform>();
    static cmp::Transform* l = leftWall.get<cmp::Transform>();
    static cmp::Transform* r = rightWall.get<cmp::Transform>();
    static cmp::Transform* bg = background.get<cmp::Transform>();

    if (t && b && l && r && bg) {
        // Positions
        atta::vec3& tp = t->position;
        atta::vec3& bp = b->position;
        atta::vec3& lp = l->position;
        atta::vec3& rp = r->position;
        atta::vec3& bgp = bg->position;

        // Scales
        atta::vec3& ts = t->scale;
        atta::vec3& bs = b->scale;
        atta::vec3& ls = l->scale;
        atta::vec3& rs = r->scale;
        atta::vec3& bgs = bg->scale;

        // Offsets
        atta::vec2 offset((rp.x + lp.x) / 2.0f, (tp.y + bp.y) / 2.0f);
        atta::vec2 size(rp.x - lp.x, tp.y - bp.y);

        // Update background position/scale
        bgp = atta::vec3(offset, -1.0f);
        bgs = atta::vec3(size, 1.0f);

        // Update wall positions
        tp.x = bp.x = offset.x;
        rp.y = lp.y = offset.y;

        // Update wall scales
        ts.x = bs.x = size.x;
        rs.y = ls.y = size.y;
    }
}

void Project::updateBackground() {
    if (!_bgImage)
        return;

    static cmp::Transform* bg = background.get<cmp::Transform>();
    float relW = 5;
    float relH = 5;
    uint32_t width = bg->scale.x * relW;
    uint32_t height = bg->scale.y * relH;

    // Resize image if necessary
    if (width != _bgImage->getWidth() || height != _bgImage->getHeight())
        _bgImage->resize(width, height);

    // Change image color
    uint8_t* data = _bgImage->getData();
    for (int i = 0; i < width; i++)
        for (int j = 0; j < height; j++) {
            atta::vec2 start{bg->scale.x / 2, bg->scale.y / 2};
            atta::vec2 pos = atta::vec2(bg->position) - start + atta::vec2((i + 0.5f) / relW, (j + 0.5f) / relH);
            atta::vec2 force = getForceField(pos);
            float scale = atta::length(force);
            if (scale > 0)
                scale = (log(scale) + 2.0f) / 3.0f;
            if (scale < 0)
                scale = 0;
            if (scale > 1)
                scale = 1;

            unsigned index = (i + (height - 1 - j) * width) * 4;
            data[index + 0] = 255 * scale;
            data[index + 1] = 255 * scale;
            data[index + 2] = 255 * scale;
            data[index + 3] = 255;
        }
    _bgImage->update();
}

#include "projectScriptUI.cpp"
