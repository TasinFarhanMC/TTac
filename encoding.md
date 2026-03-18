# Encoding

TTac uses 4-bit cell encoding that encapsulates relative symmetries.  
Throughout this section, I describe the method and show the TTac encoding side by side as examples.

> [!NOTE]
> The encoding only applies to non-center cells. The center cell can be any value not used in the encoding. TTac uses `1111`.

Let the 4 bits of a cell be `A B C D`. They can be ordered in any way (e.g., `DCBA`, `ADBC`). TTac uses `ACDB` ordering.

> [!WARNING]
> Since C only has minimum 8-bit integer types (`char`), we use the lower 4 bits and set the top 4 bits to zero.

## Bit A
Bit A represents the cell type: corner or edge.  
TTac sets Bit A for corners and unsets it for edges.

| **1---** | **0---** | **1---** |
| --- | --- | --- |
| **0---** | **1111** | **0---** |
| **1---** | **0---** | **1---** |

## Bit B
Bit B represents adjacency/opposition among **same-type cells**.  
Opposite cells share the same B value, and adjacent cells have different B values.  

TTac selects a corner and an edge, sets their B bits, sets the B bits of their opposite cells, and unsets B for all others:

| **1--0** | **0--0** | **1--1** |
| --- | --- | --- |
| **0--1** | **1111** | **0--1** |
| **1--1** | **0--0** | **1--0** |

## Bits C & D
Bits C and D represent adjacency/opposition between **different-type cells** (edge ↔ corner).  
- If B is set, C is used; if B is unset, D is used.  
- If the C/D bit of the other cell matches the current cell, it is opposite (`TTAC_IS_OPP_DIFF = true`), otherwise it is adjacent (`TTAC_IS_ADJ_DIFF = true`).

TTac uses the following configuration:

| **1000** | **0010** | **1101** |
| --- | --- | --- |
| **0111** | **1111** | **0001** |
| **1011** | **0100** | **1110** |
