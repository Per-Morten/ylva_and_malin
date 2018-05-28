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
#include <lodepng.h>

#define NOC_FILE_DIALOG_IMPLEMENTATION
#define NOC_FILE_DIALOG_WIN32
#include <noc_file_dialog.h>

// Because, come on SDL :/
#undef main

// TODO: Fix cleaner startup.

// Do a queue of commands within the context?
// Should make it possible to

struct editor_context
{
    bool eraser_mode{ false };
    GLuint sprite_sheet_id{};
    unsigned int width;
    unsigned int height;
};

void
setup_texture(editor_context& ctx)
{
    GLubyte* image;
    unsigned int width;
    unsigned int height;

    // TODO: Go through and load all sprites from the sprites folder!
    const auto filename = "resources/sprites/malin_regular.png";


    unsigned error = lodepng_decode32_file(&image,
                                           &width,
                                           &height,
                                           filename);

    ctx.height = height;
    ctx.width = width;

    // Create texture slot
    glActiveTexture(GL_TEXTURE0);
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        LOG_WARN("Could not set active texture for %s",
                filename);
        free(image);

        return;
    }

    glGenTextures(1, &ctx.sprite_sheet_id);
    glBindTexture(GL_TEXTURE_2D, ctx.sprite_sheet_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image);

    err = glGetError();
    if (err != GL_NO_ERROR)
    {
        LOG_WARN("glTexImage2D failed for %s",
                filename);
        return;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void
gui_update(editor_context& ctx)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save", "CTRL+S"))
            {
                LOG_DEBUG("Save clicked");

                // Either save with filename that is inside ctx.
                // If that is nullptr, then open proper saver.
                const auto filename = noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, nullptr,
                                                           nullptr, nullptr);
            }
            if (ImGui::MenuItem("Save as", "CTRL+Shift+S"))
            {
                const auto filename = noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, nullptr,
                                                           nullptr, nullptr);
                LOG_DEBUG("Save As clicked");
            }
            if (ImGui::MenuItem("Load", "CTRL+O"))
            {
                const auto filename = noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, nullptr,
                                                           nullptr, nullptr);
                LOG_DEBUG("Load clicked");
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z"))
            {

            }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
            {

            }  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X"))
            {

            }
            if (ImGui::MenuItem("Copy", "CTRL+C"))
            {

            }
            if (ImGui::MenuItem("Paste", "CTRL+V"))
            {

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

    // Layer
    // Unsure how to do this. Maybe A Slider Int?
    // Or have max number of layers, and radio buttons?
    // Or just int entry field?
    // Also should indicate that something is logic.

    // Eraser
    if (ImGui::Checkbox("Eraser mode", &ctx.eraser_mode))
        LOG_DEBUG("Eraser mode selected");

    ImTextureID my_tex_id = (ImTextureID*)ctx.sprite_sheet_id;
    ImGuiIO& io = ImGui::GetIO();
    float my_tex_w = (float)ctx.width;
    float my_tex_h = (float)ctx.height;

    //ImGui::Text("%.0fx%.0f", my_tex_w, my_tex_h);
    ImVec2 mouse_pos = ImGui::GetMousePos();
    ImVec2 cursor_pos = ImGui::GetCursorPos();

    ImVec2 size = ImGui::GetItemRectSize();

    ImGui::SetCursorScreenPos(ImVec2(6.0f, 75.0f));
    //ImGui::SetCursorPosY(75.0f);
    ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), ImVec2(0,0), ImVec2(1,1), ImColor(255,255,255,255), ImColor(255,255,255,128));

    ImVec2 cursor_pos_2 = ImGui::GetCursorPos();

    ImGui::Text("Size: %.0f, %.0f", size.x, size.y);

    ImGui::Text("Mousepos: %.0f, %.0f", mouse_pos.x, mouse_pos.y);
    ImGui::Text("Cursorpos: %.0f, %.0f", cursor_pos.x, cursor_pos.y);
    ImGui::Text("Cursorpos2: %.0f, %.0f", cursor_pos_2.x, cursor_pos_2.y);


    // Need a copy or something as well, in the case were we add custom attributes.

    // Sprite Sheet
    // See Demo window about images.
    // Also, remember to be able to switch between sprite sheets.
    // And also, add and remove sprite sheets.

    ImGui::End();
}

void
logic_update(SDL_Event& event,
             editor_context& ctx)
{

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
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplSdlGL3_Init(window);

    ImGui::StyleColorsDark();

    bool window_open = true;

    editor_context editor_ctx;
    setup_texture(editor_ctx);

    while (window_open)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdlGL3_ProcessEvent(&event);
            logic_update(event, editor_ctx);

            if (event.type == SDL_QUIT)
                window_open = false;
        }
        ImGui_ImplSdlGL3_NewFrame(window);
        // Draw my own stuff!

        gui_update(editor_ctx);

        // ImGui::ShowDemoWindow(nullptr);




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

