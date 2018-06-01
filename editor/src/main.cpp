#include <iostream>
#include <queue>
#include <stdio.h>
#include <string>
#include <vector>

#include <logger.h>

#define NOC_FILE_DIALOG_IMPLEMENTATION
#define NOC_FILE_DIALOG_WIN32
#include <noc_file_dialog.h>

#include <imgui.h>
#include <imgui-SFML.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

// Do a queue of commands within the context?

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
    std::vector<sf::Texture> texture_sheets{};
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

    for (const auto& file : files)
    {
        sf::Texture tex;
        tex.loadFromFile(file);
        ctx.texture_sheets.push_back(tex);
    }
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

    ImGui::ListBox("Layer", &ctx.current_layer, layers, IM_ARRAYSIZE(layers), 3);

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
    //texture_t& tex = ctx.texture_sheets[ctx.current_sheet];
    //ImGui::Image((ImTextureID*)tex.id, ImVec2(tex.width, tex.height), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
    ImGui::Image(ctx.texture_sheets[ctx.current_sheet]);

    ImVec2 cursor_pos_2 = ImGui::GetCursorPos();

    ImGui::Text("Size: %.0f, %.0f", size.x, size.y);

    ImGui::Text("Mousepos: %.0f, %.0f", mouse_pos.x, mouse_pos.y);
    ImGui::Text("Cursorpos: %.0f, %.0f", cursor_pos.x, cursor_pos.y);
    ImGui::Text("Cursorpos2: %.0f, %.0f", cursor_pos_2.x, cursor_pos_2.y);




    ImGui::End();
}

//void
//keyboard_update(SDL_Event& event,
//                editor_ctx_t& ctx)
//{
//
//}

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
    sf::RenderWindow window(sf::VideoMode(1280, 720), "YM Editor");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    bool window_open = true;

    editor_ctx_t editor_ctx;
    setup_textures(editor_ctx);

    sf::Clock clock;
    while (window_open)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
            {
                window_open = false;
                window.close();
            }
        }
        ImGui::SFML::Update(window, clock.restart());
        window.clear(sf::Color(115, 140, 153, 255));



        logic_update(editor_ctx);
        gui_update(editor_ctx);

        ImGui::ShowDemoWindow(nullptr);


        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}

