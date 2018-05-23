#include <logger.h>
#include <string>
#include <iostream>
#include <vector>

#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>
#include <stdio.h>
#include <GL/GLEW.h>
#include <gl/GLU.h>
#include <SDL.h>

// Because, come on SDL :/
#undef main

// TODO: Fix cleaner startup.

int
main(int argc,
     char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    auto window = SDL_CreateWindow("YM Editor",
                                   SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   1280, 720,
                                   SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    auto gl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);
    glewInit();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplSdlGL3_Init(window);

    ImGui::StyleColorsDark();

    bool window_open = true;
    bool eraser_mode = false;

    while (window_open)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                window_open = false;
        }
        ImGui_ImplSdlGL3_NewFrame(window);

        ImGui::ShowDemoWindow(nullptr);

        // Think if this should be a normal menu style, not just the "menu button"
        ImGui::Begin("Menu", nullptr,
                     ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize);
        {
            const ImVec2 button_size = { 77.0f, 20.0f };
            // Save
            if (ImGui::Button("Save", button_size))
                LOG_DEBUG("Save clicked");

            // Save as
            ImGui::SameLine();
            if (ImGui::Button("Save As", button_size))
                LOG_DEBUG("Save As clicked");

            // Load
            ImGui::SameLine();
            if (ImGui::Button("Load", button_size))
                LOG_DEBUG("Load clicked");

            // Layer
            // Unsure how to do this. Maybe A Slider Int?
            // Or have max number of layers, and radio buttons?
            // Or just int entry field?
            // Also should indicate that something is logic.

            // Eraser
            if (ImGui::Checkbox("Eraser mode", &eraser_mode))
                LOG_DEBUG("Eraser mode selected");

            // Need a copy or something as well, in the case were we add custom attributes.

            // Sprite Sheet
            // See Demo window about images.
            // Also, remember to be able to switch between sprite sheets.
            // And also, add and remove sprite sheets.
        }
        ImGui::End();


        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplSdlGL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

