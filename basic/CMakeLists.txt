cmake_minimum_required(VERSION 3.12)
project(boids-basic VERSION 1.0.0 LANGUAGES CXX)
find_package(atta 0.0.0.3 REQUIRED)

# Create project script target
atta_add_target(project_script "src/projectScript.cpp")
target_link_libraries(project_script boid_component settings_component)

# Create boid script target
atta_add_target(boid_script "src/boidScript.cpp")
target_link_libraries(boid_script boid_component settings_component)

# Create boid component target
atta_add_target(boid_component "src/boidComponent.cpp")

# Create settings component target
atta_add_target(settings_component "src/settingsComponent.cpp")
