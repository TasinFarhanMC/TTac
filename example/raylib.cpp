#include "raylib.h"
#include <iostream>

extern "C" {
#include "ttac.h"
extern void ttac_create_game(TTacGame *game, TTacBool starting_player);
extern TTacCell ttac_play(TTacGame *game, TTacCell move);
}

// Board setup
constexpr int CELL_SIZE = 150;
constexpr int GRID_SIZE = 3; // 3x3

// Map TTacCell constants to grid coordinates
std::pair<int, int> cell_to_coords(TTacCell cell) {
  switch (cell) {
  case TTAC_TOP_LEFT:
    return {0, 0};
  case TTAC_TOP:
    return {1, 0};
  case TTAC_TOP_RIGHT:
    return {2, 0};
  case TTAC_LEFT:
    return {0, 1};
  case TTAC_CENTER:
    return {1, 1};
  case TTAC_RIGHT:
    return {2, 1};
  case TTAC_BOTTOM_LEFT:
    return {0, 2};
  case TTAC_BOTTOM:
    return {1, 2};
  case TTAC_BOTTOM_RIGHT:
    return {2, 2};
  default:
    return {-1, -1}; // invalid cell
  }
}

// Convert mouse coordinates to TTacCell
TTacCell mouse_to_cell(int x, int y) {
  int col = x / CELL_SIZE;
  int row = y / CELL_SIZE;

  if (row == 0 && col == 0) return TTAC_TOP_LEFT;
  if (row == 0 && col == 1) return TTAC_TOP;
  if (row == 0 && col == 2) return TTAC_TOP_RIGHT;
  if (row == 1 && col == 0) return TTAC_LEFT;
  if (row == 1 && col == 1) return TTAC_CENTER;
  if (row == 1 && col == 2) return TTAC_RIGHT;
  if (row == 2 && col == 0) return TTAC_BOTTOM_LEFT;
  if (row == 2 && col == 1) return TTAC_BOTTOM;
  if (row == 2 && col == 2) return TTAC_BOTTOM_RIGHT;

  return 255; // invalid
}

// Draw a cross or circle
void draw_symbol(TTacCell cell, int player) {
  auto [x, y] = cell_to_coords(cell);
  if (x < 0) return;

  int px = x * CELL_SIZE + CELL_SIZE / 2;
  int py = y * CELL_SIZE + CELL_SIZE / 2;
  int radius = CELL_SIZE / 3;

  if (player == 1)
    DrawCircle(px, py, radius, RED);
  else
    DrawCircle(px, py, radius, BLUE);
}

int main() {
  InitWindow(GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE, "TTac AI Tester");
  SetTargetFPS(60);

  TTacGame game;
  ttac_create_game(&game, 1); // human starts

  bool game_over = false;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw grid
    for (int i = 0; i <= GRID_SIZE; ++i) {
      DrawLine(i * CELL_SIZE, 0, i * CELL_SIZE, CELL_SIZE * GRID_SIZE, BLACK);
      DrawLine(0, i * CELL_SIZE, CELL_SIZE * GRID_SIZE, i * CELL_SIZE, BLACK);
    }

    // Draw moves
    for (int i = 0; i < 9; ++i) {
      if (game.board[i] == 1)
        draw_symbol(static_cast<TTacCell>(i), 1);
      else if (game.board[i] == 2)
        draw_symbol(static_cast<TTacCell>(i), 2);
    }

    EndDrawing();

    if (!game_over && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      TTacCell move = mouse_to_cell(GetMouseX(), GetMouseY());
      TTacCell ai_move = ttac_play(&game, move);

      if (ai_move >= TTAC_GAME_LOSE && ai_move <= TTAC_GAME_WIN) {
        game_over = true;
        std::cout << "Game ended: " << int(ai_move) << "\n";
      } else if (ai_move != 255) {
        std::cout << "AI played: " << int(ai_move) << "\n";
      }
    }
  }

  CloseWindow();
  return 0;
}
