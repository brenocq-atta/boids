cmake_minimum_required(VERSION 3.12)
project(boids VERSION 1.1.0 LANGUAGES CXX)
find_package(atta 0.3.0.0 REQUIRED)

# Create settings component target
atta_add_target(settings_component "src/settingsComponent.cpp")

# Create boid component target
atta_add_target(boid_component "src/boidComponent.cpp")

# Common functions
atta_add_target(common "src/forceField.cpp")

# Create project script target
atta_add_target(project_script "src/projectScript.cpp")
target_link_libraries(project_script PRIVATE boid_component settings_component common)

# Create boid script target
atta_add_target(boid_script "src/boidScript.cpp")
target_link_libraries(boid_script PRIVATE boid_component settings_component common)
