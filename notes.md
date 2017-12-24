# Notes
Different notes that I should come back to if I wonder why I have done something
particular, and it doesn't really belong in a comment.

## Positional spaces
### Logical
Want to keep the game logic space more like pixel space, easier to calculate with, and makes more sense in 2D,
for example when looking up tile position based on on-screen position.
However, need to translate this into OpenGL space when drawing.
OpenGL space also currently goes from 0 to 2, rather than -1 to 1, unsure if I will move back again.
Will let it be in [0->2) for the moment.

## Unsigned vs Signed in Shaders
Have had some problems with unsigned values in shaders stopping the shader from compiling,
with no really good error messages, might just be that the shader compiler is a bit touchy,
which wouldn't be surprising.
Will therefore always use int, even when values cant be < 0.

##### Edit
The problems wasn't really the signed vs unsigned, it was related to a memory issue when loading shaders.
The shader source didn't necessarily end with a '\0', which I wrongfully blamed on uint vs int.
