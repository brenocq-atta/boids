<!--
title: Boids
description: Basic implementation of 2D boids with obstacle avoidance
image: https://storage.googleapis.com/atta-images/docs/boids/boids-basic.gif
build: https://storage.googleapis.com/atta-repos/boids
-->
# Boids

<div align="center">
  <img src="https://storage.googleapis.com/atta-images/docs/boids/boids-basic.gif" height="400">
</div>

This repository will contain different implementations of boids. For now, only the basic 2D implementation has been done.

## The algorithm

The first publication of Boids was made in 1986 by Craig Reynolds, and it consists of multiple agents intecting with each other to generate a flocking behaviour. The algorithm consists of 3 simple rules: _Separation, Alignment, Cohesion_.

**Separation:**

Agents avoid being too close to each other.
<div align="center">
  <img src="https://storage.googleapis.com/atta-images/docs/boids/separation.png" height="200">
</div>

**Alignment:**

Agents align their angle with those of neighbors.
<div align="center">
  <img src="https://storage.googleapis.com/atta-images/docs/boids/alignment.png" height="200">
</div>

**Cohesion:**

Agents move to the center of their neighbors.
<div align="center">
  <img src="https://storage.googleapis.com/atta-images/docs/boids/cohesion.png" height="200">
</div>

Each agent is represented as a vector, and the each rule generate a vector that is summed to the agent's vector.

## Implementation
Implementation of 2D boids with obstacle avoidance. Each boid is represented as a 2D vector. The force is given by:

```
Force = collisionAvoidance * collisionAvoidanceFactor + 
        velocityMatching * velocityMatchingFactor + 
        flockCentering * flockCenteringFactor + 
        obstacleAvoidance
```

The following parameters can be changed from the UI: 
 - **collisionAvoidanceFactor**: How strong the collision avoidance will be. Can be used to adjust how close each agent will be from each other
 - **velocityMatchingFactor**: How strong the collision avoidance will be. If set to zero, agents will follow different directions.
 - **flockCenteringFactor**: How strong the flock centering will be. If set to zero, agents will always avoid each other.
 - **viewRadius**: How big is the agent view radius. Boids inside the view radius are considered neighbors.
 - **noise**: Add random noise to neighbors readings.

**Obs:** Obstacle avoidance was implemented to avoid two types of objects:
 - Walls (4 predefined entities)
 - Disks (entities with mesh component set as disk)

### Features
- You can move the walls while the simulation is running.
- Turn on world force field plot (obstacle avoidance force).
- Inspect position/velocity plot of selected boid.

## References
- Craig Reynolds. **Flocks, herds and schools: A distributed behavioral model.** SIGGRAPH 87.
- [Craig Reynolds' website](https://www.red3d.com/cwr/boids/).
- [The Coding Train coding challange](https://www.youtube.com/watch?v=mhjuuHl6qHM).
