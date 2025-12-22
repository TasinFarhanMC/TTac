#include <iostream>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#include "raylib.h"
#include "ttac.h"
#include <algorithm>
#include <array>
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
TTacCell mouse_to_cell(Vector2 m, int cell_w, int cell_h) {
  int col = (int)(m.x / cell_w);
  int row = (int)(m.y / cell_h);

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

struct GameState {
  TTacGame game;
  std::array<int, 9> board {};
  bool game_over = false;
  bool result_printed = false;
  Mode mode = MODE_AI_START;
  int win_w = 600;
  int win_h = 600;
};

GameState g_state;

void start_game(Mode start_mode) {
  g_state.board.fill(0);
  g_state.game_over = false;
  g_state.result_printed = false;
  g_state.mode = start_mode;

  if (start_mode == MODE_AI_START) {
    ttac_create_game(&g_state.game, true);
    TTacCell ai_move = ttac_play(g_state.game, 255);
    int idx = cell_to_index(ai_move);
    if (idx >= 0) g_state.board[idx] = 2;
  } else {
    ttac_create_game(&g_state.game, false);
  }
}

bool mouse_in_rect(int x, int y, int w, int h) {
  Vector2 m = GetMousePosition();
  return (m.x >= x && m.x <= x + w && m.y >= y && m.y <= y + h);
}

void toggle_mode() {
  if (g_state.mode == MODE_AI_START)
    start_game(MODE_HUMAN_START);
  else if (g_state.mode == MODE_HUMAN_START)
    start_game(MODE_BOX);
  else
    start_game(MODE_AI_START);
}

void game_loop() {
  g_state.win_w = GetScreenWidth();
  g_state.win_h = GetScreenHeight();

  int cell_w = g_state.win_w / GRID_SIZE;
  int cell_h = g_state.win_h / GRID_SIZE;

  Vector2 mouse = GetMousePosition();

  if (!g_state.game_over && (g_state.mode == MODE_AI_START || g_state.mode == MODE_HUMAN_START)) {

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsGestureDetected(GESTURE_TAP)) {
      TTacCell move = mouse_to_cell(mouse, cell_w, cell_h);
      int idx = cell_to_index(move);
      if (idx >= 0 && g_state.board[idx] == 0) {
        g_state.board[idx] = 1;
        TTacCell ai_move = ttac_play(g_state.game, move);
        int ai_idx = cell_to_index(ai_move);
        if (ai_idx >= 0) g_state.board[ai_idx] = 2;
        if (ttac_game_state(g_state.game) != TTAC_GAME_PENDING) g_state.game_over = true;
      }
    }
  }

  int btn_w = 160;
  int btn_h = 50;
  int btn_x = g_state.win_w - btn_w - 10;
  int btn_y = 10;

  bool btn_hovered = mouse_in_rect(btn_x, btn_y, btn_w, btn_h);

  if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsGestureDetected(GESTURE_TAP)) && btn_hovered) { toggle_mode(); }

  BeginDrawing();
  ClearBackground(RAYWHITE);

  for (int i = 0; i <= GRID_SIZE; ++i) {
    DrawLine(i * cell_w, 0, i * cell_w, g_state.win_h, BLACK);
    DrawLine(0, i * cell_h, g_state.win_w, i * cell_h, BLACK);
  }

  for (int i = 0; i < 9; ++i)
    if (g_state.board[i] != 0) draw_symbol(i, g_state.board[i], cell_w, cell_h);

  DrawText(
      g_state.mode == MODE_AI_START      ? "Mode: AI Start"
      : g_state.mode == MODE_HUMAN_START ? "Mode: Human Start"
                                         : "Mode: Box",
      10, 10, 20, DARKGRAY
  );

  DrawRectangle(btn_x, btn_y, btn_w, btn_h, btn_hovered ? LIGHTGRAY : GRAY);
  DrawRectangleLines(btn_x, btn_y, btn_w, btn_h, BLACK);
  DrawText("Toggle Mode", btn_x + 10, btn_y + btn_h / 4, 20, BLACK);

  EndDrawing();
}

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(g_state.win_w, g_state.win_h, "TTac AI / Human / Box Mode");
  SetTargetFPS(60);

#ifdef PLATFORM_WEB
  float dpr = emscripten_get_device_pixel_ratio();
  SetMouseScale(1.0f / dpr, 1.0f / dpr);
#endif

  start_game(MODE_AI_START);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(game_loop, 0, 1);
#else
  while (!WindowShouldClose()) game_loop();
#endif

  CloseWindow();
  return 0;
}
