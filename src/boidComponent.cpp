//--------------------------------------------------
// Boids Basic
// boidComponent.h
// Date: 2022-01-29
// By Breno Cunha Queiroz
//--------------------------------------------------
#include "boidComponent.h"

using namespace atta;
namespace cmp = atta::component;

template <>
cmp::ComponentDescription& cmp::TypedComponentRegistry<BoidComponent>::getDescription() {
    static cmp::ComponentDescription desc = {"Boid",
                                             {{AttributeType::VECTOR_FLOAT32, offsetof(BoidComponent, velocity), "velocity"},
                                              {AttributeType::VECTOR_FLOAT32, offsetof(BoidComponent, acceleration), "acceleration"},
                                              {AttributeType::CUSTOM, offsetof(BoidComponent, neighbors), "neighbors"}},
                                             // Max instances
                                             1024,
                                             // Serialize
                                             {{"neighbors",
                                               [](std::ostream& os, void* data) {
                                                   // Do not serialize neighbors
                                               }}},
                                             // Deserialize
                                             {{"neighbors", [](std::istream& is, void* data) {
                                                   // Do not deserialize neighbors
                                               }}}};

    return desc;
}
