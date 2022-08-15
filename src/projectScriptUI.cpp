//--------------------------------------------------
// Boid Basic
// projectScriptUI.cpp
// Date: 2022-08-14
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <imgui.h>
#include <imgui_internal.h> // Disable items
#include <implot.h>

void Project::onUIRender() {
    ImGui::Begin("Configure");
    {
        mainParemeters();
        ImGui::Separator();
        boidParemeters();
    }
    ImGui::End();

    ImGui::Begin("Inspect agent");
    boidInspect();
    ImGui::End();
}

void Project::mainParemeters() {
    if (ImGui::Button("Reset"))
        initBoids();

    ImGui::Text("Main parameters");

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    float min = 0.0f;
    float max = 5.0f;

    const char* names[3] = {"Collision avoidance factor", "Velocity matching factor", "Flock centering factor"};
    static bool active[3] = {true, true, true};
    SettingsComponent* s = settings.get<SettingsComponent>();
    static float lastVal[3] = {s->collisionAvoidanceFactor, s->velocityMatchingFactor, s->flockCenteringFactor};
    float* paramPtr[3] = {&s->collisionAvoidanceFactor, &s->velocityMatchingFactor, &s->flockCenteringFactor};

    // Render checkbox and sliders
    for (unsigned i = 0; i < 3; i++) {
        ImGui::Text("%s", names[i]);

        if (ImGui::Checkbox((std::string("###Active") + names[i]).c_str(), &active[i])) {
            if (active[i] == false) {
                lastVal[i] = *paramPtr[i];
                *paramPtr[i] = 0.0f;
            } else {
                *paramPtr[i] = lastVal[i];
            }
        }

        ImGui::SameLine();
        if (!active[i])
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);

        ImGui::SliderScalar((std::string("###Slider") + names[i]).c_str(), ImGuiDataType_Float, paramPtr[i], &min, &max, "%.6f",
                            ImGuiSliderFlags_None);

        if (!active[i])
            ImGui::PopItemFlag();
    }
}

void Project::boidParemeters() {
    SettingsComponent* s = settings.get<SettingsComponent>();

    ImGui::Text("Boid parameters");

    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    ImGui::Text("View Radius");
    ImGui::DragFloat("###DragViewRadius", &s->viewRadius, 0.05f, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_None);
    ImGui::SameLine();

    static bool showViewRadius = false;
    if(ImGui::Checkbox("Show##showViewRadius", &showViewRadius) && !showViewRadius)
        gfx::Drawer::clear<gfx::Drawer::Line>("boidView");
        
    if (showViewRadius) {
        if (_running) {
            // Clear lines
            gfx::Drawer::clear<gfx::Drawer::Line>("boidView");
            // Add lines for each circle
            for (cmp::Entity boid : cmp::getFactory(boidPrototype)->getClones()) {
                atta::vec3 pos = boid.get<cmp::Transform>()->position;
                const int numSections = 50;
                // Add circle of lines
                for (int i = 0; i < numSections; i++) {
                    float angle0 = M_PI * 2 * i / float(numSections);
                    float angle1 = M_PI * 2 * (i + 1) / float(numSections);
                    float vr = s->viewRadius;
                    gfx::Drawer::Line line;
                    line.p0 = pos + atta::vec3(vr * cos(angle0), vr * sin(angle0), 1.0f);
                    line.p1 = pos + atta::vec3(vr * cos(angle1), vr * sin(angle1), 1.0f);
                    line.c0 = line.c1 = {1, 0, 0, 1};
                    // Add line
                    gfx::Drawer::add(line, "boidView");
                }
            }
        }
    }

    ImGui::Text("Noise");
    ImGui::DragFloat("###DragNoise", &s->noise, 0.01f, 0.0f, 5.0f, "%.2f", ImGuiSliderFlags_None);

    ImGui::Text("Tip: You can move the walls");
    ImGui::Text("Tip: You can add more circles");
}

void Project::boidInspect() {
    static std::vector<atta::vec2> pos;
    static std::array<atta::vec2, 20> vel; // Circular buffer
    static std::vector<atta::vec2> acc;
    static int velOffset = 0;
    static cmp::Entity lastSelected{-1};

    if (_running) {
        cmp::Factory* factory = cmp::getFactory(boidPrototype);

        // Choose which boid to inspect
        cmp::Entity selected = cmp::getSelectedEntity();
        if (!(selected >= factory->getFirstClone() && selected <= factory->getLastClone()))
            selected = lastSelected; // Change if selected another boid
        if (selected == -1)
            selected = factory->getFirstClone(); // Set first boid to be selected

        // Clear data if started inspecting another boid
        if (selected != lastSelected) {
            pos.clear();
            acc.clear();
            std::fill(vel.begin(), vel.end(), atta::vec2{});
            velOffset = 0;
        }
        lastSelected = selected;

        cmp::Transform* t = selected.get<cmp::Transform>();
        BoidComponent* b = selected.get<BoidComponent>();

        // Update selected boid data
        pos.push_back(t->position);
        acc.push_back(b->acceleration);
        vel[velOffset] = b->velocity;
        velOffset = (velOffset + 1) % vel.size();

        // Show boid info
        ImGui::Text("Select the boid that you want to inspect");
        ImGui::Separator();
        ImGui::Text("Info");
        ImGui::Text("EntityId: %d", int(selected.getId()));
        ImGui::Text("Num neighbors: %zu", b->neighbors.size());
        ImGui::Text("Position: %s", atta::vec2(t->position).toString().c_str());
        ImGui::Text("Velocity: %s", b->velocity.toString().c_str());
        ImGui::Text("Acceleration: %s", b->acceleration.toString().c_str());

        // Plot position
        ImGui::Separator();
        ImGui::Text("Position");
        if (ImPlot::BeginPlot("##Position", ImVec2(-1, 250), ImPlotFlags_Equal)) {
            ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
            ImPlot::PlotLine("##BoidPosition", &pos[0].x, &pos[0].y, pos.size(), 0, sizeof(atta::vec2));
            ImPlot::EndPlot();
        }

        // Plot velocity
        ImGui::Separator();
        ImGui::Text("Velocity");
        if (ImPlot::BeginPlot("##Velocity", ImVec2(-1, 250), ImPlotFlags_Equal)) {
            ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_Lock, ImPlotAxisFlags_Lock);
            ImPlot::SetupAxisLimits(ImAxis_X1, -1, 1);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 1);
            ImPlot::PlotLine("##BoidVelocity", &vel[0].x, &vel[0].y, vel.size(), velOffset, sizeof(atta::vec2));
            ImPlot::EndPlot();
        }

        // Plot acceleration
        ImGui::Separator();
        ImGui::Text("Acceleration");
        if (ImPlot::BeginPlot("##Acceleration", ImVec2(-1, 250), ImPlotFlags_Equal)) {
            ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
            ImPlot::PlotLine("##BoidAcceleration", &acc[0].x, &acc[0].y, pos.size(), 0, sizeof(atta::vec2));
            ImPlot::EndPlot();
        }

    } else {
        pos.clear();
        std::fill(vel.begin(), vel.end(), atta::vec2{});
        acc.clear();
        velOffset = 0;
    }
}
