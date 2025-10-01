#include "raylib.h"
#include "ttac.h"
#include <array>
#include <iostream>
#include <string>

constexpr int GRID_SIZE = 3;

enum Mode { MODE_AI_START, MODE_HUMAN_START, MODE_BOX };

// Map TTacCell to index
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

// Map mouse coords to TTacCell
TTacCell mouse_to_cell(int x, int y, int cell_w, int cell_h) {
  int col = x / cell_w;
  int row = y / cell_h;

  if (row == 0 && col == 0) return TTAC_TOP_LEFT;
  if (row == 0 && col == 1) return TTAC_TOP;
  if (row == 0 && col == 2) return TTAC_TOP_RIGHT;
  if (row == 1 && col == 0) return TTAC_LEFT;
  if (row == 1 && col == 1) return TTAC_CENTER;
  if (row == 1 && col == 2) return TTAC_RIGHT;
  if (row == 2 && col == 0) return TTAC_BOTTOM_LEFT;
  if (row == 2 && col == 1) return TTAC_BOTTOM;
  if (row == 2 && col == 2) return TTAC_BOTTOM_RIGHT;
  return 255;
}

// Draw a symbol
void draw_symbol(int index, int player, int cell_w, int cell_h) {
  int row = index / GRID_SIZE;
  int col = index % GRID_SIZE;
  int px = col * cell_w + cell_w / 2;
  int py = row * cell_h + cell_h / 2;
  int radius = std::min(cell_w, cell_h) / 3;

  if (player == 1) DrawCircle(px, py, radius, RED);
  if (player == 2) DrawCircle(px, py, radius, BLUE);
}

int main() {
  int win_w = 600;
  int win_h = 600;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(win_w, win_h, "TTac AI / Human / Box Mode");
  SetTargetFPS(60);

  TTacGame game;
  std::array<int, 9> board {};
  bool game_over = false;
  Mode mode = MODE_AI_START;

  auto start_game = [&](Mode start_mode) {
    board.fill(0);
    game_over = false;
    mode = start_mode;

    if (start_mode == MODE_AI_START) {
      ttac_create_game(&game, true);
      TTacCell ai_move = ttac_play(game, 255);
      int idx = cell_to_index(ai_move);
      if (idx >= 0) board[idx] = 2;
    } else if (start_mode == MODE_HUMAN_START) {
      ttac_create_game(&game, false);
    }
  };

  start_game(MODE_AI_START);

  while (!WindowShouldClose()) {
    win_w = GetScreenWidth();
    win_h = GetScreenHeight();

    int cell_w = win_w / GRID_SIZE;
    int cell_h = win_h / GRID_SIZE;

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Draw grid
    for (int i = 0; i <= GRID_SIZE; ++i) {
      DrawLine(i * cell_w, 0, i * cell_w, win_h, BLACK);
      DrawLine(0, i * cell_h, win_w, i * cell_h, BLACK);
    }

    // Draw symbols
    for (int i = 0; i < 9; ++i)
      if (board[i] != 0) draw_symbol(i, board[i], cell_w, cell_h);

    // Draw current mode
    std::string mode_text = (mode == MODE_AI_START) ? "Mode: AI Start" : (mode == MODE_HUMAN_START) ? "Mode: Human Start" : "Mode: Box";
    DrawText(mode_text.c_str(), 10, 10, 20, DARKGRAY);

    EndDrawing();

    // ===== INPUT HANDLING =====
    if (mode == MODE_AI_START || mode == MODE_HUMAN_START) {
      if (!game_over && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        TTacCell move = mouse_to_cell(GetMouseX(), GetMouseY(), cell_w, cell_h);
        int idx = cell_to_index(move);
        if (idx < 0 || board[idx] != 0) continue;

        board[idx] = 1; // human = red
        TTacCell ai_move = ttac_play(game, move);
        int ai_idx = cell_to_index(ai_move);
        if (ai_idx >= 0) board[ai_idx] = 2;

        if (ttac_game_state(game) != TTAC_GAME_PENDING) game_over = true;
      }
    } else if (mode == MODE_BOX) {
      TTacCell cell = mouse_to_cell(GetMouseX(), GetMouseY(), cell_w, cell_h);
      int idx = cell_to_index(cell);
      if (idx < 0) continue;

      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) board[idx] = 1;
      if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) board[idx] = 2;
      if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) board[idx] = 0;
    }

    // ===== CONTROLS =====
    if (IsKeyPressed(KEY_R)) start_game(mode);             // restart same mode
    if (IsKeyPressed(KEY_A)) start_game(MODE_AI_START);    // AI starts
    if (IsKeyPressed(KEY_H)) start_game(MODE_HUMAN_START); // Human starts
    if (IsKeyPressed(KEY_B)) start_game(MODE_BOX);         // box mode
    if (IsKeyPressed(KEY_D)) mode = MODE_BOX;              // box mode
  }

  CloseWindow();
  return 0;
}
