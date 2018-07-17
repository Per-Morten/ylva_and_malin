#include <iostream>
#include <queue>
#include <cstdio>
#include <string>
#include <vector>
#include <experimental/filesystem>
#include <fstream>

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
///      - Select cell for editing arguments
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
/// - Create a number mapping each cell to its texture.
///     - Maybe just give 128 possible sprites to each sheet or something
///       need to tweak the number to figure out what is fitting.
///       Will waste numbers for the int, but shouldn't be a problem.
///
///////////////////////////////////////////////////////////////////////////////

/// PLAN FOR NEXT TIME
/// MAJOR REFACTORING!

namespace UI
{
    //const sf::Vector2f sheet_size(256.0f, 256.0f);
    //const sf::Vector2f sheet_size(256.0f, 256.0f);
    const sf::Vector2f sheet_size(400.0f, 400.0f);


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

namespace FileFormat
{
    constexpr int interval = 128;
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
        toggle_grid,
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
    select,
    insert,
    erase,
};

struct TextureSheet
{
    sf::Texture texture;
    int col_count;
    int row_count;
    int first_id;

    // Filename for the TextureSheet
    std::string filename;
};

struct Cell
{
    int texture_id;
    int sheet_id;
};

struct Command
{
    int id;
    std::string command;
    std::string arguments;
};

struct EditorCtx
{
    // Drawing related
    std::vector<TextureSheet> texture_sheets{};
    std::size_t current_sheet{};

    // Cell related
    std::size_t current_sprite_id{};
    sf::Vector2<std::size_t> current_cell{};

    // Mode related
    Mode current_mode{ Mode::insert };
    sf::RectangleShape mark_area{};
    bool mouse_held{false};
    bool mark_enabled{false};

    bool grid_enabled{true};

    // Grid, not 2D, first dimension is which layer we are currently on.
    // TODO: Move to 2D representation, i.e. Three vectors inside each other,
    //       No reason to stay in 1D, and 2D will probably make resizing logic easier.
    std::vector<std::vector<std::vector<Cell>>> grid;
    int width{10};
    int height{10};
    int current_layer{};

    std::vector<Command> commands{};

    // Events
    std::queue<Event> events{};

    // Utility
    sf::RenderWindow* window;
    sf::View* view;

    std::string filename;
};

bool
is_command_w_arguments(const char* str)
{
    static const char* commands[] =
    {
        "MOVE_TO",
    };

    return std::any_of(std::begin(commands), std::end(commands),
                       [str](const auto& item)
                       {
                            return std::strcmp(item, str) == 0;
                       });
}

void
setup_textures(EditorCtx& ctx)
{
    namespace fs = std::experimental::filesystem;

    int first_id = FileFormat::interval;

    for (auto& p : fs::recursive_directory_iterator("resources/sprites/"))
    {
        ctx.texture_sheets.emplace_back();

        auto& tmp = ctx.texture_sheets.back();
        tmp.texture.loadFromFile(p.path().generic_string());
        tmp.col_count = tmp.texture.getSize().x / Sprite::sprite_size.x;
        tmp.row_count = tmp.texture.getSize().y / Sprite::sprite_size.y;
        tmp.filename = p.path().filename().generic_string();

        if (p.path().generic_string().find("logic") != std::string::npos)
        {
            tmp.first_id = 0;
        }
        else
        {
            tmp.first_id = first_id;
            first_id += FileFormat::interval;
        }
    }
}

void
setup_grid(EditorCtx& ctx)
{
    ctx.grid.resize(7);
    for (std::size_t i = 0; i < ctx.grid.size(); i++)
    {
        ctx.grid[i].resize(ctx.height);
        for (std::size_t row = 0; row < ctx.grid[i].size(); row++)
        {
            ctx.grid[i][row].resize(ctx.width);
        }
    }
}

void
setup_default_context(EditorCtx& ctx)
{
    setup_textures(ctx);
    setup_grid(ctx);
}

void
save_map(EditorCtx& ctx)
{
    auto file = std::fopen(ctx.filename.c_str(), "w");
    if (!file)
    {
        LOG_WARN("Could not open file: %s", ctx.filename.c_str());
        ctx.filename = "";
        return;
    }

    std::fprintf(file, "%d %d\n%d\n\n", ctx.width, ctx.height, FileFormat::interval);

    // Isolate commands
    std::vector<std::pair<int, std::string>> commands;
    for (int i = 0; i < 2; i++)
    {
        // for (const auto& item : ctx.grid[i])
        // {
        //     commands.emplace_back(item.texture_id, item.arguments);
        // }
    }

    commands.erase(std::unique(commands.begin(),
                               commands.end()),
                   commands.end());

    commands.erase(std::remove_if(commands.begin(),
                                  commands.end(),
                                  [](const auto& item)
                                  {
                                    return item.second.empty() || item.first == 0 || item.first == 1;
                                  }),
                   commands.end());


    // Print logic definitions
    std::fprintf(file, "%zu\n0 WALKABLE\n1 UNWALKABLE\n", commands.size() + 2);
    for (const auto& command : commands)
        std::fprintf(file, "%d %s\n", command.first, command.second.c_str());

    std::fprintf(file, "\n");

    // Print graphics paths
    std::fprintf(file, "%zu\n", ctx.texture_sheets.size());
    for (const auto& texture : ctx.texture_sheets)
        std::fprintf(file, "resources/sprites/%s %d\n", texture.filename.c_str(), texture.first_id);

    std::fprintf(file, "\n");


    // Print graphics layers
    const auto print_layer = [file](const auto& grid)
    {
        for (const auto& row : grid)
        {
            for (const auto& col : row)
            {
                std::fprintf(file, "%d ", col.sheet_id + col.texture_id);
            }
            std::fprintf(file, "\n");
        }
    };

    std::fprintf(file, "%zu\n%zu\n", 2, ctx.grid.size() - 2);

    // Print logic layers
    for (std::size_t i = 0; i < 2; i++)
    {
        print_layer(ctx.grid[i]);
        std::fprintf(file, "\n");
    }

    // Print graphics layers
    for (std::size_t i = 2; i < ctx.grid.size(); i++)
    {
        print_layer(ctx.grid[i]);
        std::fprintf(file, "\n");
    }

    std::fclose(file);
}

void
load_map(EditorCtx& ctx)
{
    std::ifstream inn(ctx.filename);
    if (!inn)
    {
        LOG_WARN("Failed to load file: %s", ctx.filename.c_str());
        ctx.filename = "";
        return;
    }

    {
        int width, height, interval;
        inn >> width >> height >> interval;
        ctx.width = width;
        ctx.height = height;
    }

    // Commands
    {
        ctx.commands.clear();
        int command_count;
        inn >> command_count;
        for (int i = 0; i != command_count; i++)
        {
            std::string buffer;
            int command;
            inn >> command >> buffer;
            ctx.commands.push_back({command, buffer});
            if (is_command_w_arguments(buffer.c_str()))
            {
                inn >> ctx.commands.back().arguments;
            }
        }
    }

    // Textures
    {
        ctx.texture_sheets.clear();
        ctx.current_sheet = 0;
        int texture_count;
        inn >> texture_count;
        for (int i = 0; i != texture_count; i++)
        {
            ctx.texture_sheets.emplace_back();
            auto& tmp = ctx.texture_sheets.back();
            std::string path;
            inn >> path;

            tmp.filename = path.substr(path.rfind("/") + 1);
            tmp.texture.loadFromFile(path);
            tmp.col_count = tmp.texture.getSize().x / Sprite::sprite_size.x;
            tmp.row_count = tmp.texture.getSize().y / Sprite::sprite_size.y;

            inn >> tmp.first_id;
        }
    }

    // Graphics
    {
        int logic_count;
        int graphics_count;
        inn >> logic_count >> graphics_count;

        ctx.grid.clear();
        ctx.grid.resize(logic_count + graphics_count);

        for (std::size_t i = 0; i < ctx.grid.size(); i++)
        {

            ctx.grid[i].resize(ctx.height);

            for (int row = 0; row < ctx.height; row++)
            {
                ctx.grid[i][row].resize(ctx.width);

                for (int col = 0; col < ctx.width; col++)
                {
                    int texture_id;
                    inn >> texture_id;

                    if (texture_id > 0)
                    {
                        int c = 0;
                        while (texture_id < ctx.texture_sheets[c].first_id)
                            c++;

                        const auto& sheet = ctx.texture_sheets[c];

                        auto& new_cell = ctx.grid[i][row][col];
                        new_cell.sheet_id = sheet.first_id;
                        new_cell.texture_id = (sheet.first_id == 0)
                                            ? texture_id
                                            : texture_id % sheet.first_id;

                    }
                }
            }
        }
    }
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

        if (ImGui::BeginMenu("Canvas"))
        {
            if (ImGui::MenuItem("Resize"))
            {
                LOG_DEBUG("Resize pressed");
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
    ImGui::SameLine();
    ImGui::RadioButton("select", mode, (int)Mode::select);

    ImGui::Checkbox("mark", &ctx.mark_enabled);

    if (ImGui::Button("Toggle Grid"))
        ctx.events.push(Event::Type::toggle_grid);

    // Layer
    static const char* layers[] =
    {
        "logic_0", // DO NOT REORDER logic_0 and logic_1
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
        {
            ctx.current_layer = i;
        }
    }

    // Sprite Sheet
    auto curr_sheet = (int*)&ctx.current_sheet;
    ImGui::Text("Sprite Sheet");
    if (ImGui::SliderInt("##sprite_sheet", curr_sheet, 0, ctx.texture_sheets.size() - 1))
        ctx.current_sprite_id = 0;

    {
        char args[256] = {0};
        ImGui::Text("Arguments:");

        if (ImGui::InputText("##arguments", args, sizeof(args)))
        {
            //ctx.grid[ctx.current_layer][ctx.current_cell].arguments = args;
            LOG_DEBUG("Input Argument: %s", args);
        }
    }

    const auto mouse_pos = ImGui::GetMousePos();

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

        switch (event.type)
        {
            case Event::Type::save:
            {
                if (ctx.filename.empty())
                    ctx.events.push(Event::Type::save_as);
                else
                    save_map(ctx);
            }
            break;

            case Event::Type::save_as:
            {
                const auto filename = noc_file_dialog_open(NOC_FILE_DIALOG_SAVE, nullptr,
                                                           nullptr, nullptr);
                if (filename)
                {
                    ctx.filename = filename;
                    save_map(ctx);
                }
            }
            break;

            case Event::Type::load:
            {
                const auto filename = noc_file_dialog_open(NOC_FILE_DIALOG_OPEN, nullptr,
                                                           nullptr, nullptr);

                if (filename)
                {
                    ctx.filename = filename;
                    load_map(ctx);
                }
            }
            break;

            case Event::Type::undo:

            break;

            case Event::Type::redo:

            break;

            case Event::Type::cut:

            break;

            case Event::Type::copy:

            break;

            case Event::Type::paste:

            break;

            case Event::Type::toggle_grid:
            {
                ctx.grid_enabled = !ctx.grid_enabled;
            }

            break;

            case Event::Type::sfml_event:

            break;
        }
    }
}

void
tools_update(EditorCtx& ctx,
             const sf::Event& event,
             const sf::RenderWindow& window)
{
    const auto sheet_position = UI::get_sheet_position(window);

    if (event.type == sf::Event::MouseButtonReleased &&
        sf::FloatRect(sheet_position, UI::sheet_size).contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)))
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

        ctx.current_sprite_id = row * ctx.texture_sheets[ctx.current_sheet].row_count + col;
    }

    if (event.type == sf::Event::KeyPressed && event.key.control)
    {
        if (event.key.code == sf::Keyboard::E)
            ctx.current_mode = Mode::erase;
        if (event.key.code == sf::Keyboard::M)
            ctx.mark_enabled = !ctx.mark_enabled;
        if (event.key.code == sf::Keyboard::I || event.key.code == sf::Keyboard::Q)
            ctx.current_mode = Mode::insert;
        if (event.key.code == sf::Keyboard::S)
            ctx.events.push(Event::Type::save);
        if (event.key.code == sf::Keyboard::O)
            ctx.events.push(Event::Type::load);

    }
}

// Note: Points into the grid supplied
// Be careful about using this for holding values, prefer for temporary functionality.
template<class Predicate>
std::vector<Cell*>
get_cells_where(std::vector<std::vector<Cell>>& grid,
                    Predicate pred)
{
    std::vector<Cell*> res;
    for (std::size_t row = 0; row != grid.size(); row++)
        for (std::size_t col = 0; col != grid[row].size(); col++)
            if (pred(row, col))
                res.push_back(&grid[row][col]);

    return res;

}

void
setup_sprite_texture(const TextureSheet& sheet,
                     int texture_id,
                     sf::RectangleShape& out_shape)
{
    const auto sheet_col = texture_id % sheet.row_count;
    const auto sheet_row = texture_id / sheet.col_count;

    out_shape.setTextureRect(sf::IntRect(sheet_col * Sprite::sprite_size.x,
                                         sheet_row * Sprite::sprite_size.y,
                                         Sprite::sprite_size.x,
                                         Sprite::sprite_size.y));

    out_shape.setTexture(&sheet.texture);
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
            const auto sheet_id = (ctx.current_mode == Mode::insert) ? ctx.texture_sheets[ctx.current_sheet].first_id : 0;
            const auto texture_id = (ctx.current_mode == Mode::insert) ? ctx.current_sprite_id : 0;

            const auto query = [&window, &event](auto row, auto col)
            {
                sf::FloatRect rect;
                rect.width = Sprite::sprite_size.x;
                rect.height = Sprite::sprite_size.y;
                rect.left = col * Sprite::sprite_size.x;
                rect.top = row * Sprite::sprite_size.y;

                return rect.contains(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
            };

            for (auto item : get_cells_where(ctx.grid[ctx.current_layer], query))
            {
                item->texture_id = texture_id;
                item->sheet_id = sheet_id;
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

        const auto sheet_id = (ctx.current_mode == Mode::insert) ? ctx.texture_sheets[ctx.current_sheet].first_id : 0;
        const auto texture_id = (ctx.current_mode == Mode::insert) ? ctx.current_sprite_id : 0;

        const auto query = [&ctx](auto row, auto col)
        {
            sf::FloatRect rect;
            rect.width = Sprite::sprite_size.x;
            rect.height = Sprite::sprite_size.y;
            rect.left = col * Sprite::sprite_size.x;
            rect.top = row * Sprite::sprite_size.y;

            return ctx.mark_area.getGlobalBounds().intersects(rect) ||
                   rect.contains(ctx.mark_area.getPosition());
        };

        for (auto item : get_cells_where(ctx.grid[ctx.current_layer], query))
        {
            item->texture_id = texture_id;
            item->sheet_id = sheet_id;
        }
    }

    if (event.type == sf::Event::MouseMoved && ctx.mark_enabled && ctx.mouse_held)
    {
        const auto position = ctx.mark_area.getPosition();
        ctx.mark_area.setSize(window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)) - position);
    }

    if (event.type == sf::Event::MouseWheelScrolled)
        ctx.view->zoom(event.mouseWheelScroll.delta < 0 ? 1.25f : 0.75f);

}

void
draw_layer(const std::vector<std::vector<Cell>>& layer,
           const std::vector<TextureSheet>& textures,
           sf::RenderWindow& window)
{
    sf::RectangleShape shape(Sprite::sprite_size);

    for (std::size_t row = 0; row < layer.size(); row++)
    {
        for (std::size_t col = 0; col < layer[row].size(); col++)
        {
            const auto& cell = layer[row][col];

            if (cell.sheet_id + cell.texture_id > 0)
            {

                const auto& sheet = *std::find_if(textures.cbegin(),
                                                  textures.cend(),
                                                  [&cell](const auto& item)
                                                  {
                                                     return item.first_id == cell.sheet_id;
                                                  });

                setup_sprite_texture(sheet, cell.texture_id, shape);

                shape.setPosition(col * Sprite::sprite_size.x, row * Sprite::sprite_size.y);

                window.draw(shape);
            }
        }
    }
}

void
draw_grid(const std::vector<std::vector<std::vector<Cell>>>& grid,
          const std::vector<TextureSheet>& textures,
          sf::RenderWindow& window)
{
    for (const auto& layer : grid)
        draw_layer(layer, textures, window);
}

void
draw_grid_outline(int col_count,
                  int row_count,
                  sf::RenderWindow& window)
{
    sf::RectangleShape shape;
    shape.setSize(Sprite::sprite_size);
    shape.setFillColor(sf::Color(0, 0, 0, 0));
    shape.setOutlineColor(sf::Color::Black);
    shape.setOutlineThickness(2.0f);

    for (int row = 0; row < row_count; row++)
    {
        for (int col = 0; col < col_count; col++)
        {
            shape.setPosition(sf::Vector2f(col * Sprite::sprite_size.x,
                                           row * Sprite::sprite_size.y));

            window.draw(shape);
        }
    }
}

void
draw_sprite_below_mouse(const EditorCtx& ctx,
                        sf::RenderWindow& window)
{
    auto pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    sf::RectangleShape shape(Sprite::sprite_size);

    setup_sprite_texture(ctx.texture_sheets[ctx.current_sheet],
                         ctx.current_sprite_id,
                         shape);

    shape.setPosition(pos);
    shape.setOrigin(Sprite::sprite_size.x / 2, Sprite::sprite_size.y / 2);
    window.draw(shape);
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
    setup_default_context(editor_ctx);


    sf::Clock clock;
    while (window.isOpen())
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

        gui_update(editor_ctx);
        logic_update(editor_ctx);

        draw_grid(editor_ctx.grid,
                  editor_ctx.texture_sheets,
                  window);

        // Ensure drawing the layer I am currently on.
        // Prefer this to making everything transparent.
        draw_layer(editor_ctx.grid[editor_ctx.current_layer],
                   editor_ctx.texture_sheets,
                   window);

        if (editor_ctx.grid_enabled)
            draw_grid_outline(editor_ctx.width, editor_ctx.height, window);



        //ImGui::ShowDemoWindow(nullptr);



        if (editor_ctx.current_mode == Mode::insert)
        {
            draw_sprite_below_mouse(editor_ctx, window);
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

