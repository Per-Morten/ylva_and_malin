#include <iostream>
#include <queue>
#include <stdio.h>
#include <string>
#include <vector>
#include <experimental/filesystem>

#include <logger.h>

#define NOC_FILE_DIALOG_IMPLEMENTATION
#include <noc_file_dialog.h>

#include <imgui.h>
#include <imgui-SFML.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

///////////////////////////////////////////////////////////////////////////////
/// Notes
///////////////////////////////////////////////////////////////////////////////
/// Functionality needed:
/// - Core
///      - New File
///      - Save/Load
///      - Resize Room
/// - Convenience
///      - Copy Paste
///      - Resize sprite sheet (hard to see all sprites)
///         - Potential other solution for this: Just write program that
///           splits up the sprite sheets?
///         - Although, it might still be better to be able to resize.
///             - Or at least have the sprite sheet resize with the size of
///               the window.
///             - Heads up, resizing the ImGUI stuff will require me to
///               resize the view.
///      - Hotkeys
///          - Change current layer
///          - Change current sheet
///      - Hide Layer
///      - Mark within sprite sheet, combining several adjacent sprites.
///
/// TODOs:
/// - Major refactor of codebase
///     - Think of creating own event system to avoid having to deal with both
///       ImGui and SFML stuff in different places
///         - Could just be a struct containing my own event enum, and
///           SFML events.
///     - Moving over to own event system, only interaction with ImGUI logic
///       and SFML event system will then be to translate into my own event
///       system.
/// - Load all sprites from resources folder
///     - All dimensions can be calculated from size of image,
///       as tiles are always 32 pixels high and wide.
/// - Refactor tile placement logic.
/// - Select only one tile when clicking on tile borders.
///     - Should probably also shrink hit-space or something, to avoid
///
///////////////////////////////////////////////////////////////////////////////

namespace UI
{
    //const sf::Vector2f sheet_size(256.0f, 256.0f);
    const sf::Vector2f sheet_size(256.0f, 256.0f);

    const sf::Vector2f imgui_offset(312.5f, 20.0f);

    sf::Vector2f get_sheet_position(const sf::RenderWindow& window)
    {
        float y = window.getSize().y - sheet_size.y - 9.0f;
        return sf::Vector2f(10.0f, y);
    }
}

namespace Sprite
{
    const sf::Vector2f sprite_size(32.0f, 32.0f);
}

struct Event
{
    enum class Type
    {
        save,
        save_as,
        load,
        undo,
        redo,
        cut,
        copy,
        paste,
        sfml_event,
    };

    Type type;

    sf::Event event;

    Event() = delete;

    Event(Type t)
        : type(t)
    {

    }
};

enum class Mode
{
    insert,
    erase,
};

struct TextureSheet
{
    sf::Texture texture;
    int col_count;
    int row_count;
};

// TODO: Find out how to re-factor this.
struct EditorCtx
{
    // Drawing related
    std::vector<TextureSheet> texture_sheets{};
    std::size_t current_sheet{};
    sf::Sprite current_sprite{};

    // Mode related
    Mode current_mode{ Mode::insert };
    sf::RectangleShape mark_area{};
    bool mouse_held{false};
    bool mark_enabled{false};

    // Grid, not 2D, first dimension is which layer we are currently on.
    std::vector<std::vector<sf::RectangleShape>> grid;
    int width{10};
    int height{10};
    int current_layer{};

    // Events
    std::queue<Event> events{};

    // Utility
    sf::RenderWindow* window;
    sf::View* view;
};

void
setup_textures(EditorCtx& ctx)
{
    namespace fs = std::experimental::filesystem;

    for (auto& p : fs::recursive_directory_iterator("resources/sprites/"))
    {
        ctx.texture_sheets.emplace_back();

        auto& tmp = ctx.texture_sheets.back();
        tmp.texture.loadFromFile(p.path().generic_string());
        tmp.col_count = tmp.texture.getSize().x / Sprite::sprite_size.x;
        tmp.row_count = tmp.texture.getSize().y / Sprite::sprite_size.y;
    }

    ctx.current_sprite = sf::Sprite(ctx.texture_sheets[ctx.current_sheet].texture, sf::IntRect(0, 0, 32, 32));
}

void
gui_update(EditorCtx& ctx)
{
    // Dropdown menu
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save", "CTRL+S"))
            {
                ctx.events.push(Event::Type::save);
            }
            if (ImGui::MenuItem("Save as", "CTRL+Shift+S"))
            {
                ctx.events.push(Event::Type::save_as);
            }
            if (ImGui::MenuItem("Load", "CTRL+O"))
            {
                ctx.events.push(Event::Type::load);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z"))
            {
                ctx.events.push(Event::Type::undo);
            }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
            {
                ctx.events.push(Event::Type::redo);
            }  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X"))
            {
                ctx.events.push(Event::Type::cut);
            }
            if (ImGui::MenuItem("Copy", "CTRL+C"))
            {
                ctx.events.push(Event::Type::copy);
            }
            if (ImGui::MenuItem("Paste", "CTRL+V"))
            {
                ctx.events.push(Event::Type::paste);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // Tools window
    ImGui::SetNextWindowPos(ImVec2(1.0f, 20.0f));
    ImGui::Begin("Tools", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_AlwaysAutoResize |
                 ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoResize);


    // Mode
    auto mode = (int*)&ctx.current_mode;
    ImGui::RadioButton("erase", mode, (int)Mode::erase);
    ImGui::SameLine();
    ImGui::RadioButton("insert", mode, (int)Mode::insert);

    ImGui::Checkbox("mark", &ctx.mark_enabled);

    // Layer
    static const char* layers[] =
    {
        "logic_0",
        "logic_1",
        "floor_0",
        "floor_1",
        "furniture_0",
        "furniture_1",
        "furniture_2",
    };

    for (int i = 0; i < sizeof(layers) / sizeof(char*); i++)
    {
        if (ImGui::Selectable(layers[i], ctx.current_layer == i))
            ctx.current_layer = i;
    }

    // Sprite Sheet
    auto curr_sheet = (int*)&ctx.current_sheet;
    ImGui::SliderInt("Sprite Sheet", curr_sheet, 0, ctx.texture_sheets.size() - 1);

    ImVec2 mouse_pos = ImGui::GetMousePos();

    ImGui::Text("Mousepos: %.0f, %.0f", mouse_pos.x, mouse_pos.y);

    ImGui::SetCursorScreenPos(UI::get_sheet_position(*ctx.window));
    ImGui::Image(ctx.texture_sheets[ctx.current_sheet].texture, { UI::sheet_size.x, UI::sheet_size.y });


    ImGui::End();
}

void
logic_update(EditorCtx& ctx)
{
    while (!ctx.events.empty())
    {
        auto event = ctx.events.front();
        ctx.events.pop();

        if (event.type == Event::Type::save)
        {
            LOG_DEBUG("Save");
        }
        if (event.type == Event::Type::save_as)
        {
            LOG_DEBUG("Save As");
            //const auto filename = noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, nullptr,
            //                                           nullptr, nullptr);
        }
        if (event.type == Event::Type::load)
        {
            LOG_DEBUG("Load");
            //const auto filename = noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, nullptr,
            //                                           nullptr, nullptr);
        }
    }
}

void
tools_update(EditorCtx& ctx,
             const sf::Event& event,
             const sf::RenderWindow& window)
{
    auto sheet_rect = sf::RectangleShape({UI::sheet_size.x, UI::sheet_size.y});
    const auto sheet_position = sf::Vector2f(UI::get_sheet_position(window).x, UI::get_sheet_position(window).y);
    sheet_rect.setPosition(sheet_position);

    if (event.type == sf::Event::MouseButtonReleased &&
        sheet_rect.getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
    {
        const auto screen_x = event.mouseButton.x;
        const auto screen_y = event.mouseButton.y;

        const auto screen_img_x = screen_x - sheet_position.x;
        const auto screen_img_y = screen_y - sheet_position.y;
        const auto sheet = &ctx.texture_sheets[ctx.current_sheet];
        const auto ratio_x = (float)sheet->texture.getSize().x / UI::sheet_size.x;
        const auto ratio_y = (float)sheet->texture.getSize().y / UI::sheet_size.y;
        const auto adjusted_ms_x = screen_img_x * ratio_x;
        const auto adjusted_ms_y = screen_img_y * ratio_y;
        const auto sprite_size_x = (float)sheet->texture.getSize().x / sheet->col_count;
        const auto sprite_size_y = (float)sheet->texture.getSize().y / sheet->row_count;
        const int col = adjusted_ms_x / sprite_size_x;
        const int row = adjusted_ms_y / sprite_size_y;

        ctx.current_sprite = sf::Sprite(ctx.texture_sheets[ctx.current_sheet].texture, sf::IntRect(col * 32, row * 32, 32, 32));
    }

    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::E)
            ctx.current_mode = Mode::erase;
        if (event.key.code == sf::Keyboard::M)
            ctx.mark_enabled = !ctx.mark_enabled;
        if (event.key.code == sf::Keyboard::I || event.key.code == sf::Keyboard::Q)
            ctx.current_mode = Mode::insert;
        if (event.key.code == sf::Keyboard::S && event.key.control)
            ctx.events.push(Event::Type::save);
        if (event.key.code == sf::Keyboard::O && event.key.control)
            ctx.events.push(Event::Type::load);

    }


}

void
grid_update(EditorCtx& ctx,
            const sf::Event& event,
            const sf::RenderWindow& window)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (!ctx.mark_enabled)
        {
            const auto alpha = (ctx.current_mode == Mode::insert) ? 255 : 0;
            const auto texture = (ctx.current_mode == Mode::insert) ? ctx.current_sprite.getTexture() : nullptr;

            for (auto& item : ctx.grid[ctx.current_layer])
            {
                if (item.getGlobalBounds().contains(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y))))
                {
                    item.setFillColor(sf::Color(255, 255, 255, alpha));
                    item.setTexture(texture);
                    item.setTextureRect(ctx.current_sprite.getTextureRect());
                }
            }
        }
        else
        {
            ctx.mouse_held = true;
            ctx.mark_area.setPosition(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
            ctx.mark_area.setSize(sf::Vector2f(0.0f, 0.0f));
            ctx.mark_area.setFillColor(sf::Color(0, 128, 255, 128));
            ctx.mark_area.setOutlineColor(sf::Color(0, 128, 255, 255));

            ctx.mark_area.setOutlineThickness(2.0f);
        }
    }

    // TODO: Placing tiles on grid logic needs to be re-factored. This isn't good.
    if (event.type == sf::Event::MouseButtonReleased && ctx.mark_enabled)
    {
        ctx.mouse_held = false;
        for (auto& item : ctx.grid[ctx.current_layer])
        {
            if (ctx.mark_area.getGlobalBounds().intersects(item.getGlobalBounds()) ||
                item.getGlobalBounds().contains(ctx.mark_area.getPosition())) // extra for allowing not only to mark, but also to single click in mark mode.
            {
                auto alpha = (ctx.current_mode == Mode::insert) ? 255 : 0;
                auto texture = (ctx.current_mode == Mode::insert) ? ctx.current_sprite.getTexture() : nullptr;

                item.setFillColor(sf::Color(255, 255, 255, alpha));
                item.setTexture(texture);
                item.setTextureRect(ctx.current_sprite.getTextureRect());
            }
        }
    }

    if (event.type == sf::Event::MouseMoved && ctx.mark_enabled && ctx.mouse_held)
    {
        const auto position = ctx.mark_area.getPosition();
        ctx.mark_area.setSize(window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)) - position);
    }

    if (event.type == sf::Event::MouseWheelScrolled)
    {
        if (event.mouseWheelScroll.delta < 0)
        {
            ctx.view->zoom(1.25f);
        }
        else
        {
            ctx.view->zoom(0.75f);
        }
    }
}

void
setup_grid(EditorCtx& ctx)
{
    // Number of layers.
    ctx.grid.resize(7);

    for (std::size_t i = 0; i < ctx.grid.size(); i++)
    {
        ctx.grid[i].resize(ctx.width * ctx.height);

        for (int row = 0; row < ctx.height; row++)
        {
            for (int col = 0; col < ctx.width; col++)
            {
                auto& cell = ctx.grid[i][std::size_t(row * ctx.width + col)];

                cell.setSize({32.0f, 32.0f});
                cell.setOutlineColor(sf::Color::Black);
                cell.setOutlineThickness(2.0f);
                cell.setFillColor(sf::Color(255, 255, 255, 0));

                float xPos = col * 32.0f;
                float yPos = row * 32.0f;

                cell.setPosition({xPos, yPos});

            }
        }
    }
}

// TODO: Setup function for outputting all information related to current context.
void
output_ctx_info(EditorCtx& ctx)
{

}

// TODO: Create function that translates sfml events to internal events, so I can have centralized event queue.

int
main([[maybe_unused]] int argc,
     [[maybe_unused]] char** argv)
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "YM Editor");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    sf::View view(window.getView());
    view.setSize(window.getSize().x - UI::imgui_offset.x, window.getSize().y - UI::imgui_offset.y);
    sf::Vector2f size = {UI::imgui_offset.x / window.getSize().x, UI::imgui_offset.y / window.getSize().y};
    view.setViewport(sf::FloatRect(size.x, size.y, 1 - size.x, 1 - size.y));

    EditorCtx editor_ctx;
    editor_ctx.window = &window;
    editor_ctx.view = &view;
    setup_textures(editor_ctx);
    setup_grid(editor_ctx);

    sf::Clock clock;
    bool window_open = true;
    while (window_open)
    {
        window.setView(view);
        ImGui::SFML::Update(window, clock.restart());
        window.clear(sf::Color(115, 140, 153, 255));
        sf::Event event;

        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
            {
                window_open = false;
                window.close();
            }
            if (event.type == sf::Event::Resized)
            {
                view.setSize(event.size.width - UI::imgui_offset.x, event.size.height - UI::imgui_offset.y);
                sf::Vector2f size = {UI::imgui_offset.x / event.size.width, UI::imgui_offset.y / event.size.height};
                view.setViewport(sf::FloatRect(size.x, size.y, 1 - size.x, 1 - size.y));
            }
            tools_update(editor_ctx, event, window);
            grid_update(editor_ctx, event, window);
        }


        // Fix tool view
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        {
            view.move(5.0f, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            view.move(-5.0f, 0.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            view.move(0.0f, -5.0f);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            view.move(0.0f, 5.0f);
        }

        logic_update(editor_ctx);
        gui_update(editor_ctx);

        for (auto& item : editor_ctx.grid)
        {
            for (auto& cell : item)
                window.draw(cell);
        }




        //ImGui::ShowDemoWindow(nullptr);



        if (editor_ctx.current_mode == Mode::insert)
        {
            auto pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            editor_ctx.current_sprite.setPosition(pos);
            editor_ctx.current_sprite.setOrigin(16, 16);
            window.draw(editor_ctx.current_sprite);
        }
        //window.setMouseCursorVisible(editor_ctx.current_mode != Mode::insert);

        if (editor_ctx.mark_enabled && editor_ctx.mouse_held)
        {
            window.draw(editor_ctx.mark_area);
        }

        ImGui::SFML::Render(window);

        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}

