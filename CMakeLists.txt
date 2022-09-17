cmake_minimum_required(VERSION 3.12)
project(boids VERSION 1.0.0 LANGUAGES CXX)
find_package(atta 0.2.0.0 REQUIRED)

atta_add_target(project_base "src/forceField.cpp")

# Create project script target
atta_add_target(project_script "src/projectScript.cpp")
target_link_libraries(project_script PUBLIC boid_component settings_component project_base)

# Create boid script target
atta_add_target(boid_script "src/boidScript.cpp")
target_link_libraries(boid_script PUBLIC boid_component settings_component project_base)

# Create boid component target
atta_add_target(boid_component "src/boidComponent.cpp")

# Create settings component target
atta_add_target(settings_component "src/settingsComponent.cpp")
