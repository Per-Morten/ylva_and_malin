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

    // Logic layer
    // <cid> = command_id
    // <tid> = texture_id
    <tid> <cid> <tid> <cid> <tid> <cid> <tid> <cid> <tid> <cid>
    <tid> <cid> <tid> <cid> <tid> <cid> <tid> <cid> <tid> <cid>
    <tid> <cid> <tid> <cid> <tid> <cid> <tid> <cid> <tid> <cid>
    <tid> <cid> <tid> <cid> <tid> <cid> <tid> <cid> <tid> <cid>
    <tid> <cid> <tid> <cid> <tid> <cid> <tid> <cid> <tid> <cid>
    <tid> <cid> <tid> <cid> <tid> <cid> <tid> <cid> <tid> <cid>

    // Graphics layer
    <tid> <tid> <tid> <tid> <tid>
    <tid> <tid> <tid> <tid> <tid>
    <tid> <tid> <tid> <tid> <tid>
    <tid> <tid> <tid> <tid> <tid>
    <tid> <tid> <tid> <tid> <tid>
    <tid> <tid> <tid> <tid> <tid>


```

## Example:
```
    10 10
    128     // 128 Between each layer
    
    5
    0 WALKABLE 
    1 UNWALKABLE 
    2 MOVE_TO 1 // Move to scene 1 upon touching this tile.
    3 MOVE_TO 2
    4 MOVE_TO 3
    
    7
    resources/sprites/floor_0.png 128
    resources/sprites/malin_regular.png 256
    resources/sprites/ylva_regular.png 384
    resources/sprites/walls_0.png 512
    resources/sprites/furniture_1.png 640
    resources/sprites/logic.png 0
    resources/sprites/furniture_0.png 768
    
    2
    5
    2 2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    2 3 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    2 4 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 2 3 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
    
    129 129 129 129 129 129 129 129 129 129 
    129 129 129 129 129 129 129 129 129 129 
    129 129 129 129 129 129 129 129 129 129 
    129 129 129 129 129 129 129 129 129 129 
    129 129 129 129 129 129 129 129 129 129 
    129 129 129 129 129 129 129 129 129 129 
    129 129 129 129 129 129 129 129 129 129 
    129 129 129 129 129 129 129 129 129 129 
    129 129 129 129 129 129 129 129 129 129 
    129 129 129 129 129 129 129 129 129 129 
    
    0 0 0 128 128 128 128 128 128 128 
    0 0 0 128 128 128 128 128 128 128 
    0 128 0 128 128 128 128 128 128 128 
    0 0 0 128 0 0 0 0 0 0 
    0 0 0 0 128 0 128 0 0 0 
    0 0 0 0 0 0 128 0 0 0 
    0 0 0 0 0 128 0 0 0 0 
    0 128 128 128 128 128 0 0 0 0 
    0 128 128 128 128 128 0 0 0 0 
    0 128 128 128 128 128 0 0 0 0 
    
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 
    0 0 0 0 0 0 0 0 0 0 

```
