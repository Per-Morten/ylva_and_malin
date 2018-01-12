# Plans
Moving plans over to markdown, continuous process. 

## Architecture
### Main Game Loop
Unsure if this will work, but at least think of it for now.


|Time Step      | Thread 0               | Thread 1             |
|:--------------|:-----------------------|:---------------------|
| 0             | Update Static Env      |                      |
| 1             | Update Dynamic Env     | Draw Static Env      |
| 2             | Poll Controllers       | Draw Dynamic Env     |
| 3             | Update Interact-able   |                      |
| 4             | Update UI              | Draw Interact-able   |
| 5             |                        | Draw UI              |

#### Thoughts
* How is this affected by resizing the screen?
* Need to flesh this out with how we can get resources needed for future computations, like reading text to display from file etc.
* This doesn't need to be thread 0 and thread 1, can create a job system, the only thing that then becomes important is to list dependent tasks, for example that no graphics processing can happen at the same time (to avoid heavy synchronization).

#### Limitations
* Cannot have interact-ables in the background.
    * Can potentially hack this by still employing the layered approach to drawing.

## Turn Regions into allocators
Regions should be turned into allocators, as we allocate from them, and there is no real reason for them to have separate logic.
Only thing extra they need is id, which we can find some other way of storing,
to avoid wasting the id space on all other allocators.
Regions can just be their own id's. no reason to use anything else then an enum.
Region  is enum, used to look up in table of allocators, that one allocate from.
