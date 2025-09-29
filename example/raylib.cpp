#include "raylib.h"
#include <array>
#include <iostream>

extern "C" {
#include "ttac.h"
extern void ttac_create_game(TTacGame *game, TTacBool starting_player);
extern TTacCell ttac_play(TTacGame *game, TTacCell move);
}

// Board setup
constexpr int CELL_SIZE = 150;
constexpr int GRID_SIZE = 3;

// Map mouse coordinates to TTacCell
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

// Map TTacCell to board index 0..8
int cell_to_index(TTacCell cell) {
  switch (cell) {
  case TTAC_TOP_LEFT:
    return 0;
  case TTAC_TOP:
    return 1;
  case TTAC_TOP_RIGHT:
    return 2;
  case TTAC_LEFT:
    return 3;
  case TTAC_CENTER:
    return 4;
  case TTAC_RIGHT:
    return 5;
  case TTAC_BOTTOM_LEFT:
    return 6;
  case TTAC_BOTTOM:
    return 7;
  case TTAC_BOTTOM_RIGHT:
    return 8;
  default:
    return -1; // sentinel / invalid
  }
}

// Draw a symbol for a player
void draw_symbol(int index, int player) {
  int row = index / GRID_SIZE;
  int col = index % GRID_SIZE;
  int px = col * CELL_SIZE + CELL_SIZE / 2;
  int py = row * CELL_SIZE + CELL_SIZE / 2;
  int radius = CELL_SIZE / 3;

  if (player == 1)
    DrawCircle(px, py, radius, RED); // Human
  else if (player == 2)
    DrawCircle(px, py, radius, BLUE); // AI
}

int main() {
  InitWindow(GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE, "TTac AI Tester");
  SetTargetFPS(60);

  TTacGame game;
  ttac_create_game(&game, 1); // AI starts

  std::array<int, 9> board {}; // 0=empty, 1=human, 2=AI
  bool game_over = false;

  // Let AI make the first move
  TTacCell first_ai_move = ttac_play(&game, 255); // sentinel for no human move
  if (first_ai_move < TTAC_CENTER) {
    int ai_idx = cell_to_index(first_ai_move);
    if (ai_idx >= 0) board[ai_idx] = 2;
    std::cout << "AI played first: " << int(first_ai_move) << "\n";
  }

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw grid
    for (int i = 0; i <= GRID_SIZE; ++i) {
      DrawLine(i * CELL_SIZE, 0, i * CELL_SIZE, CELL_SIZE * GRID_SIZE, BLACK);
      DrawLine(0, i * CELL_SIZE, CELL_SIZE * GRID_SIZE, i * CELL_SIZE, BLACK);
    }

    // Draw symbols
    for (int i = 0; i < 9; ++i) {
      if (board[i] != 0) draw_symbol(i, board[i]);
    }

    EndDrawing();

    if (!game_over && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      TTacCell move = mouse_to_cell(GetMouseX(), GetMouseY());
      int idx = cell_to_index(move);
      if (idx < 0 || board[idx] != 0) continue; // invalid / already played

      // Update human move first
      board[idx] = 1;

      // Call AI
      TTacCell ai_move = ttac_play(&game, move);

      // Check game end
      if (ai_move >= TTAC_GAME_LOSE && ai_move <= TTAC_GAME_WIN) {
        game_over = true;
        std::cout << "Game ended: " << int(ai_move) << "\n";
      }
      // Update AI move
      int ai_idx = cell_to_index(ai_move);
      if (ai_idx >= 0) board[ai_idx] = 2;
      std::cout << "AI played: " << int(ai_move) << "\n";
    }

    if (IsKeyPressed(KEY_R)) {
      board.fill(0);
      game_over = false;
      ttac_create_game(&game, 1); // AI starts

      TTacCell first_ai_move = ttac_play(&game, 255); // AI first move
      if (first_ai_move < TTAC_CENTER) {
        int ai_idx = cell_to_index(first_ai_move);
        if (ai_idx >= 0) board[ai_idx] = 2;
        std::cout << "AI played first: " << int(first_ai_move) << "\n";
      }
    }
  }

  CloseWindow();
  return 0;
}
