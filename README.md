<!--
title: Boids
description: Multiple implementations of boids
image: https://storage.googleapis.com/atta-images/docs/boids/basic/boids-basic.gif
-->

# Boids

<div align="center">
  <img src="https://storage.googleapis.com/atta-images/docs/boids/basic/boids-basic.gif" height="400">
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

## How to run
Install atta following the instructions [here](https://github.com/brenocq/atta).
```
  git clone https://github.com/brenocq-atta/boids
  atta ./basic/basic.atta
```

## References
- Craig Reynolds. **Flocks, herds and schools: A distributed behavioral model.** SIGGRAPH 87.
- [Craig Reynolds' website](https://www.red3d.com/cwr/boids/).
- [The Coding Train coding challange](https://www.youtube.com/watch?v=mhjuuHl6qHM).
