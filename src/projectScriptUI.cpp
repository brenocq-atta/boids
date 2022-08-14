//--------------------------------------------------
// Boid Basic
// projectScriptUI.cpp
// Date: 2022-08-14
// By Breno Cunha Queiroz
//--------------------------------------------------
#include <imgui.h>
#include <imgui_internal.h> // Disable items
//#include <implot.h>

void Project::onUIRender() {
    ImGui::Begin("Configure");
    {
        mainParemeters();
        ImGui::Separator();
        boidParemeters();
    }
    ImGui::End();

    ImGui::Begin("Inspect agent");
    {
        // TODO inspect agent
        // std::vector<int> xs = {0, 1, 2, 3, 4};
        // std::vector<int> ys = {0, 1, 0, 1, 0};
        // if(ImPlot::BeginPlot("##Position"))
        //{
        //    ImPlot::PlotLine("X", xs.data(), ys.data(), xs.size());
        //    ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
        //    ImPlot::PlotLine("X", xs.data(), ys.data(), xs.size());
        //    ImPlot::EndPlot();
        //}
    }
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

    ImGui::Text("Noise");
    ImGui::DragFloat("###DragNoise", &s->noise, 0.01f, 0.0f, 5.0f, "%.2f", ImGuiSliderFlags_None);

    ImGui::Text("Tip: You can move the walls");
    ImGui::Text("Tip: You can add more circles");
}
