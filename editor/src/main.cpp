#include <iostream>
#include <queue>
#include <stdio.h>
#include <string>
#include <vector>

#include <GL/GLEW.h>
#include <gl/GLU.h>
#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>
#include <lodepng.h>
#include <SDL.h>

#include <logger.h>
#include <texture.h>

#define NOC_FILE_DIALOG_IMPLEMENTATION
#define NOC_FILE_DIALOG_WIN32
#include <noc_file_dialog.h>

// Because, come on SDL :/
#undef main

// TODO: Fix cleaner startup.

// Do a queue of commands within the context?
// Should make it possible to

enum class event_t
{
    save,
    save_as,
    load,
    undo,
    redo,
    cut,
    copy,
    paste,
};

struct editor_ctx_t
{
    // Drawing related
    std::vector<texture_t> texture_sheets{};
    std::size_t current_sheet{};
    int current_layer{};
    bool eraser_mode{ false };

    // Events
    std::queue<event_t> events{};
};

void
setup_textures(editor_ctx_t& ctx)
{
    // TODO: Go through and load all sprites from the sprites folder!
    const char* files[] =
    {
        "resources/sprites/malin_regular.png",
        "resources/sprites/ylva_regular.png",
        "resources/sprites/floor_0.png",
        "resources/sprites/furniture_0.png",
        "resources/sprites/furniture_1.png",
        "resources/sprites/walls_0.png",
        "resources/sprites/logic.png",
    };

    ctx.texture_sheets.resize(std::size(files));
    const auto res = create_textures(files,
                                     ctx.texture_sheets.size(),
                                     ctx.texture_sheets.data());

    if (res < ctx.texture_sheets.size())
    {
        for (int i = res; i < ctx.texture_sheets.size(); i++)
        {
            LOG_WARN("Could not load texture: %s", files[i]);
        }
    }

    ctx.texture_sheets.resize(res);
    ctx.current_sheet = 0;
}

void
gui_update(editor_ctx_t& ctx)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save", "CTRL+S"))
            {
                ctx.events.push(event_t::save);
            }
            if (ImGui::MenuItem("Save as", "CTRL+Shift+S"))
            {
                ctx.events.push(event_t::save_as);
            }
            if (ImGui::MenuItem("Load", "CTRL+O"))
            {
                ctx.events.push(event_t::load);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z"))
            {
                ctx.events.push(event_t::undo);
            }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
            {
                ctx.events.push(event_t::redo);
            }  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X"))
            {
                ctx.events.push(event_t::cut);
            }
            if (ImGui::MenuItem("Copy", "CTRL+C"))
            {
                ctx.events.push(event_t::copy);
            }
            if (ImGui::MenuItem("Paste", "CTRL+V"))
            {
                ctx.events.push(event_t::paste);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }


    // Think if this should be a normal menu style, not just the "menu button"
    ImGui::SetNextWindowPos(ImVec2(0.0f, 20.0f));
    ImGui::Begin("Menu", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_AlwaysAutoResize |
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoResize);



    // Eraser
    if (ImGui::Checkbox("Eraser mode", &ctx.eraser_mode))
        LOG_DEBUG("Eraser mode selected");

    // Layer
    // Unsure how to do this. Maybe A Slider Int?
    // Or have max number of layers, and radio buttons?
    // Or just int entry field?
    // Also should indicate that something is logic.
    const char* layers[] =
    {
        "logic_0",
        "logic_1",
        "floor_0",
        "floor_1",
        "furniture_0",
        "furniture_1",
        "furniture_2",
    };
    // ImGui::Combo("Layer", &ctx.current_layer, layers, IM_ARRAYSIZE(layers));
    ImGui::ListBox("Layer", &ctx.current_layer, layers, IM_ARRAYSIZE(layers), 3);
    // ImGui::SliderInt("Layer", &ctx.current_layer, )

    // Sprite Sheet
    // See Demo window about images.
    // Also, remember to be able to switch between sprite sheets.
    // And also, add and remove sprite sheets.
    auto curr_sheet = (int*)&ctx.current_sheet;
    ImGui::SliderInt("Sprite Sheet", curr_sheet, 0, ctx.texture_sheets.size() - 1);

    //ImGui::Text("%.0fx%.0f", my_tex_w, my_tex_h);
    ImVec2 mouse_pos = ImGui::GetMousePos();
    ImVec2 cursor_pos = ImGui::GetCursorPos();

    ImVec2 size = ImGui::GetItemRectSize();

    //ImGui::SetCursorScreenPos(ImVec2(6.0f, 75.0f));
    //ImGui::SetCursorPosY(75.0f);
    texture_t& tex = ctx.texture_sheets[ctx.current_sheet];
    ImGui::Image((ImTextureID*)tex.id, ImVec2(tex.width, tex.height), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));

    ImVec2 cursor_pos_2 = ImGui::GetCursorPos();

    ImGui::Text("Size: %.0f, %.0f", size.x, size.y);

    ImGui::Text("Mousepos: %.0f, %.0f", mouse_pos.x, mouse_pos.y);
    ImGui::Text("Cursorpos: %.0f, %.0f", cursor_pos.x, cursor_pos.y);
    ImGui::Text("Cursorpos2: %.0f, %.0f", cursor_pos_2.x, cursor_pos_2.y);




    ImGui::End();
}

void
keyboard_update(SDL_Event& event,
                editor_ctx_t& ctx)
{

}

void
logic_update(editor_ctx_t& ctx)
{
    while (!ctx.events.empty())
    {
        auto event = ctx.events.front();
        ctx.events.pop();
        if (event == event_t::save)
        {
            LOG_DEBUG("Save");
        }
        if (event == event_t::save_as)
        {
            LOG_DEBUG("Save As");
            const auto filename = noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, nullptr,
                                                       nullptr, nullptr);
        }
        if (event == event_t::load)
        {
            LOG_DEBUG("Load");
            const auto filename = noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, nullptr,
                                                       nullptr, nullptr);
        }
    }
}


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
    //ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplSdlGL3_Init(window);

    ImGui::StyleColorsDark();

    bool window_open = true;

    editor_ctx_t editor_ctx;
    setup_textures(editor_ctx);

    while (window_open)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
            keyboard_update(event, editor_ctx);

            if (event.type == SDL_QUIT)
                window_open = false;
        }
        ImGui_ImplSdlGL3_NewFrame(window);


        // Draw my own stuff!




        logic_update(editor_ctx);
        gui_update(editor_ctx);

        ImGui::ShowDemoWindow(nullptr);




        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    delete_textures(editor_ctx.texture_sheets.data(), editor_ctx.texture_sheets.size());
    ImGui_ImplSdlGL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

