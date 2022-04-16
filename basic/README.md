---
title: Boids Basic
description: Basic implementation of 2D boids with obstacle avoidance
image: https://storage.googleapis.com/atta-images/docs/boids/basic/boids-basic.gif
---
# Boids Basic

<div align="center">
  <img src="https://storage.googleapis.com/atta-images/docs/boids/basic/boids-basic.gif" height="400">
</div>

Basic implementation of 2D boids with obstacle avoidance. Each boid is represented as a 2D vector. The force is given by:

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
 - **view radius**: How big is the agent view radius. Boids inside the view radius are considered neighbors.
 - **view expoent**: Not being used.
 - **noise**: Add random noise to neighbors readings.

**Obs:** Obstacle avoidance was implemented to avoid two types of objects:
 - Walls (mesh component set to box or plane)
 - Disks (entities with mesh component set as disk)
