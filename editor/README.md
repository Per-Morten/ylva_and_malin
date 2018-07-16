Editor for creating maps for ym project.

## Commands
```
    WALKABLE // Always valued at 0
    UNWALKABLE // Always valued at 1
    MOVE_TO <scene_id>
```

## File format
```
    <width> <height>

    // Intervals
    // The range between each numeric category.
    // For example, 128 would imply that the first category (logic) is in the range 0-127
    // While the first graphics category would be in the range 128-255
    <intervals>

    <number_of_commands>
    <id> <command>
    <id> <command>
    ...
    <id> <command>
    
    <number_of_textures>
    <texture_path> 
    <first_id> // Always starts as 128, 0-127 is reserved for logic layer. 
    <texture_path> 
    <first_id> // Due to this, to get the texture id, can just mod with 128.
    ...
    <texture_path> 
    <first_id>

    <graphics layer count>
    <logic layer count>

    <id> <id> <id> <id> <id>
    <id> <id> <id> <id> <id>
    <id> <id> <id> <id> <id>
    <id> <id> <id> <id> <id>
    <id> <id> <id> <id> <id>
    <id> <id> <id> <id> <id>

    <lid> <lid> <lid> <lid> <lid>
    <lid> <lid> <lid> <lid> <lid>
    <lid> <lid> <lid> <lid> <lid>
    <lid> <lid> <lid> <lid> <lid>
    <lid> <lid> <lid> <lid> <lid>
    <lid> <lid> <lid> <lid> <lid>

```

## Example:
```
    5 5
    128         // 128 between each layer
    3
    0 WALKABLE
    1 UNWALKABLE
    2 MOVE_TO 1 // Change to scene 1 upon moving onto this tile in the logic layer

    2
    resources/sprites/floor_0.png 128
    resources/sprites/furniture_0.png 256

    1
    128 129 128 156 288
    128 129 128 156 288
    128 129 128 156 288
    128 129 128 156 288
    128 129 128 156 288

    1
    1 1 1 1 1
    1 0 0 0 1
    1 0 0 2 1
    1 0 0 2 1
    1 1 1 1 1


```
