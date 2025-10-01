#include "raylib.h"
#include "ttac.h"
#include <array>
#include <iostream>
#include <string>

// Board setup
constexpr int CELL_SIZE = 150;
constexpr int GRID_SIZE = 3;

// Convert TTacCell to readable name
std::string cell_name(TTacCell cell) {
  switch (cell) {
  case TTAC_TOP_LEFT:
    return "TOP_LEFT";
  case TTAC_TOP:
    return "TOP";
  case TTAC_TOP_RIGHT:
    return "TOP_RIGHT";
  case TTAC_LEFT:
    return "LEFT";
  case TTAC_CENTER:
    return "CENTER";
  case TTAC_RIGHT:
    return "RIGHT";
  case TTAC_BOTTOM_LEFT:
    return "BOTTOM_LEFT";
  case TTAC_BOTTOM:
    return "BOTTOM";
  case TTAC_BOTTOM_RIGHT:
    return "BOTTOM_RIGHT";
  default:
    return "INVALID";
  }
}

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
    return -1;
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

// Print winner
void print_winner(TTacGame &game) {
  switch (ttac_game_state(game)) {
  case TTAC_GAME_AI_WIN:
    std::cout << ">>> AI wins!\n";
    break;
  case TTAC_GAME_DRAW:
    std::cout << ">>> Draw game.\n";
    break;
  case TTAC_GAME_PENDING:
    std::cout << ">>> Game still pending.\n";
    break;
  default:
    std::cout << ">>> Unknown state.\n";
    break;
  }
}

int main() {
  InitWindow(GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE, "TTac AI Tester");
  SetTargetFPS(60);

  TTacGame game;
  std::array<int, 9> board {}; // 0=empty, 1=human, 2=AI
  bool game_over = false;
  bool ai_starts = true;

  auto reset_game = [&](bool ai_first) {
    board.fill(0);
    game_over = false;
    ai_starts = ai_first;
    ttac_create_game(&game, ai_first);

    if (ai_first) {
      TTacCell first_ai_move = ttac_play(game, 255); // sentinel
      int ai_idx = cell_to_index(first_ai_move);
      if (ai_idx >= 0) board[ai_idx] = 2;
      std::cout << "[AI] plays first: " << cell_name(first_ai_move) << "\n";
    } else {
      std::cout << "Human plays first.\n";
    }
  };

  reset_game(true); // default: AI starts

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
      if (idx < 0 || board[idx] != 0) continue;

      board[idx] = 1;
      std::cout << "[Human] plays: " << cell_name(move) << "\n";

      TTacCell ai_move = ttac_play(game, move);

      if (ttac_game_state(game) != TTAC_GAME_PENDING) {
        game_over = true;
        print_winner(game);
      }

      int ai_idx = cell_to_index(ai_move);
      if (ai_idx >= 0) {
        board[ai_idx] = 2;
        std::cout << "[AI] plays: " << cell_name(ai_move) << "\n";
      }
    }

    // Reset controls
    if (IsKeyPressed(KEY_R)) {
      reset_game(true); // AI starts
    }
    if (IsKeyPressed(KEY_H)) {
      reset_game(false); // Human starts
    }
  }

  CloseWindow();
  return 0;
}
