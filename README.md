# TTac

**TTac** is a **Perfect Tic-Tac-Toe** engine written using **bit-encoded group symmetry** as a compiled **Finite State Machine (FSM)**.

It encodes cell symmetry in bits to minimize logic, allowing **constant-time moves** and **tiny memory usage**. The engine works for both **AI-first** and **player-first** games. All logic is deterministic and can be directly translated into hardware.

*Note: Tic-Tac-Toe has multiple equivalent optimal move trees, but this engine uses only the **one** with the most **threats**.*

Try it now in the [**Web Example**](https://tasinfarhanmc.github.io/TTac/)
## Setup

Include the header and define the implementation in a source file:

```c
#define TTAC_IMPLEMENTATION
#include "ttac.h"
```

Use random from libc for randomness

```c
#define TTAC_C_RANDOM
#define TTAC_IMPLEMENTATION
#include "ttac.h"
```

Optionally, provide a custom random function:

```c
#define TTAC_USER_RANDOM my_random
#define TTAC_IMPLEMENTATION
#include "ttac.h"
```

### CMake Integration

#### Options

* `TTAC_C_RANDOM` — use `random()` from libc
* `TTAC_USER_RANDOM` — define your own random function

#### Using FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    TTac
    GIT_REPOSITORY https://github.com/TasinFarhanMC/TTac
    GIT_TAG v0.1.1
)
FetchContent_MakeAvailable(TTac)
```

#### Using [CPM](https://github.com/cpm-cmake/CPM.cmake)

```cmake
CPMAddPackage("gh:TasinFarhanMC/TTac@0.1.1")
```

#### Linking

```cmake
target_link_libraries(target PRIVATE TTac)
```
## Documentation

### Naming Conventions

* **opp** → opposite cell (corner ↔ corner or edge ↔ edge across the board)
* **adj** → adjacent cell (next to the reference cell)
* **diff** → different type (edge vs corner or corner vs edge)
* **same** → same type (edge vs edge or corner vs corner)

---

### Types

| Type         | Description                                                          |
| ------------ | -------------------------------------------------------------------- |
| `TTacCell`   | 4-bit encoded cell position (corner, edge, center); *Type:* **char** |
| `TTacBool`   | Boolean value (0 or non-0); *Type:* **char**                             |
| `TTacGame`   | Game instance with state and branch logic; *Type:* **struct**        |
| `TTacBranch` | Function pointer type for tree branches; *Type:* **pointer**         |

---

### Constants

**Cells:**

```c
// Corners
TTAC_TOP_LEFT, TTAC_TOP_RIGHT, 
TTAC_BOTTOM_LEFT, TTAC_BOTTOM_RIGHT

// Edges
TTAC_TOP, TTAC_BOTTOM, 
TTAC_LEFT, TTAC_RIGHT

// Center & NULL
TTAC_CENTER, TTAC_NULL
```

**Game states:**

```c
TTAC_GAME_PENDING
TTAC_GAME_DRAW
TTAC_GAME_AI_WIN // == TTAC_GAME_PLAYER_LOSS
TTAC_GAME_AI_LOSS // == TTAC_GAME_PLAYER_WIN
```

---

### Macros

**Symmetry operations (bitwise):**

```c
TTAC_OPP_SAME(x)    // Returns the opposite cell of the same type
TTAC_ADJ1_SAME(x)   // Returns first adjacent cell of same type
TTAC_ADJ2_SAME(x)   // Returns second adjacent cell of same type

TTAC_OPP1_DIFF(x)   // Returns first opposite cell of different type
TTAC_OPP2_DIFF(x)   // Returns second opposite cell of different type
TTAC_ADJ1_DIFF(x)   // Returns first adjacent cell of different type
TTAC_ADJ2_DIFF(x)   // Returns second adjacent cell of different type
```

**Cell type checks:**

```c
TTAC_IS_CORNER(x)  
TTAC_IS_EDGE(x)    
TTAC_IS_CENTER(x)
```

**Adjacency and difference checks:**

```c
TTAC_IS_OPP_SAME(a,b)  // True if a and b are opposite of same type
TTAC_IS_ADJ_SAME(a,b)  // True if a and b are adjacent of same type
TTAC_IS_OPP_DIFF(a,b)  // True if a and b are opposite of different type
TTAC_IS_ADJ_DIFF(a,b)  // True if a and b are adjacent of different type
TTAC_IS_SAME(a,b)      // True if a and b are of same type
TTAC_IS_DIFF(a,b)      // True if a and b are of different type
```

**Middle / adjacency generation:**

```c
TTAC_MIDDLE(a,b)      // Returns the middle cell between a and b (corners/edges)
TTAC_ADJ_GEN_ADJ(a,b) // Generates an adjacent cell to b that is adjacent to a
TTAC_OPP_GEN_ADJ(a,b) // Generates an opposite cell to b that is opposite to a
```

---

### API Functions

| Function                                              | Description                                                    |
| ----------------------------------------------------- | -------------------------------------------------------------- |
| `ttac_create_game(TTacGame *game, TTacBool ai_start)` | Initializes a new game. If `ai_start` is true, AI moves first. |
| `ttac_play(game, move)`                               | Plays a move on a game instance (by value).                    |
| `ttac_play_ptr(game, move)`                           | Plays a move on a pointer to a game instance.                  |

**Predefined arrays:**

```c
extern const TTacCell ttac_corners[4];
extern const TTacCell ttac_edges[4];
```

---

### How It Works

1. **FSM-based engine:**
   Each game instance maintains a `branch` pointer representing the current FSM state. Moves trigger state changes, and outputs are computed using bit operations on state and move.

2. **Bit-encoded symmetry:**
   Each cell encodes its position and relationships to other cells in **4 bits**, allowing adjacency and opposition calculations using **pure bitwise logic**.

3. **Deterministic Engine:**
   No searching is performed; the precompiled FSM produces the **optimal move in constant time**.
## Example

A minimal C example using TTac:

```c
#include <stdio.h>
#define TTAC_IMPLEMENTATION
#include "ttac.h"

int main() {
    TTacGame game;
    ttac_create_game(&game, 1); // AI starts
    TTacCell move = TTAC_CENTER;

    TTacCell ai_move = ttac_play_ptr(&game, move);
    printf("AI plays: %d\n", ai_move);

    return 0;
}
```

* A full example using **raylib** can be found at [example/raylib.cpp](https://github.com/TasinFarhanMC/ttac/blob/main/example/raylib.cpp).
* Web version of the raylib example (built with **[emscripten](https://github.com/emscripten-core/emscripten)**): [tasinfarhanmc.github.io/TTac](https://tasinfarhanmc.github.io/TTac/).
