//--------------------------------------------------
// Boids Basic
// common.h
// Date: 2022-08-13
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef COMMON_H
#define COMMON_H
#include <atta/component/interface.h>

namespace cmp = atta::component;

const cmp::Entity boidPrototype(1);
const cmp::Entity obstacles(2);
const cmp::Entity settings(5);
const cmp::Entity topWall(6);
const cmp::Entity bottomWall(7);
const cmp::Entity rightWall(8);
const cmp::Entity leftWall(9);
const cmp::Entity background(4);

#endif // COMMON_H
