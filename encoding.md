# Encoding

TTac uses **4-bit** cell encoding that encapsulates **relative symmetries**.  
Throughout this section, I describe the method and show the encoding used by TTac side by side as examples.

> [!NOTE]  
> The encoding only applies to **non-center** cells. The center cell can be any value not used in the encoding. TTac uses `1111`.

Let the 4 bits of a cell be `A B C D`. They can be ordered in any way (e.g., `DCBA`, `ADBC`). TTac uses **`ACDB`** ordering.

> [!WARNING]  
> Since C only has minimum **8-bit** integer types (`char`), we use the lower 4 bits and set the **upper 4 bits** to **zero**.

## Bit Generation 

### Bit A

Bit A represents the **cell type**: corner or edge. 

TTac sets Bit A for corners and unsets it for edges:

| **1---** | **0---** | **1---** |
| --- | --- | --- |
| **0---** | **1111** | **0---** |
| **1---** | **0---** | **1---** |

### Bit B

Bit B represents **adjacency/opposition** among **same-type** cells **(edge ↔ edge or corner ↔ corner)** . 

- **Opposite** cells have the **same** B value.  
- **Adjacent** cells have **different** B values.

To create an encoding, select a corner and an edge, set their B bits, propagate to opposite cells, and unset B for all others.  

TTac selects the `TOP_RIGHT` corner and `RIGHT` edge:

| **1--0** | **0--0** | **1--1** |
| --- | --- | --- |
| **0--1** | **1111** | **0--1** |
| **1--1** | **0--0** | **1--0** |

### Bits C & D

Bits C and D represent **adjacency/opposition** between **different-type** cells **(edge ↔ corner)**. 

- Select **C/D bit** based on **B bit**.  
 TTac selects C bit if B is set and D bit if B is unset.

- Pick a corner to define the bit:  
  - Set the bit for the selected corner  
  - Unset the bit for its opposite corner  
  - Set the bit for edges opposite to the selected corner  
  - Unset the bit for all other edges  
  - Set the bit for the corner that shares an opposite edge with the selected corner. 
Considering the edge with the same B bit as the selected corner.
  - Unset the bit for the remaining corner  
- Repeat for another corner with the other bit (C or D).

TTac selects the `TOP_RIGHT` corner for C bit:

| **10-0** | **00-0** | **11-1** |
| --- | --- | --- |
| **01-1** | **11-1** | **00-1** |
| **10-1** | **01-0** | **11-0** |

TTac selects the `TOP_LEFT` corner for D bit:

| **1000** | **0010** | **1101** |
| --- | --- | --- |
| **0111** | **1111** | **0001** |
| **1011** | **0100** | **1110** |

## Checks (Invariants)

### Same-Type Cells
- **Opposite** → same B bit  
- **Adjacent** → different B bit

### Different-Type Cells
- **Opposite** → same C/D bit (selected using B of reference cell)  
- **Adjacent** → different C/D bit (selected using B of reference cell)

## Generation Rules

### Same-Type

| Relation   | Bits to Flip |
|-----------|--------------|
| Opposite  | C, D         |
| Adjacent  | B, C or B, D |

### Different-Type

| Relation   | Bits to Flip |
|-----------|--------------|
| Opposite  | A, B or A, C/D (selected using inverted B) |
| Adjacent  | all bits or A, C/D (selected using B) |

