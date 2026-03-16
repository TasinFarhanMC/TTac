#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
EM_JS(bool, prefers_dark_mode, (), { return window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches; });
#endif

#define TTAC_IMPLEMENTATION
#define TTAC_C_RANDOM

#include "raylib.h"
#include "ttac.h"
#include <algorithm>
#include <array>
#include <ctime>

constexpr int GRID_SIZE = 3;
constexpr int HEADER_HEIGHT = 60;

enum Mode {
  MODE_AI_START,
  MODE_HUMAN_START
};

// Map TTacCell to index
int cell_to_index(TTacCell cell) {
  switch (cell) {
  case TTAC_TOP_LEFT: return 0;
  case TTAC_TOP: return 1;
  case TTAC_TOP_RIGHT: return 2;
  case TTAC_LEFT: return 3;
  case TTAC_CENTER: return 4;
  case TTAC_RIGHT: return 5;
  case TTAC_BOTTOM_LEFT: return 6;
  case TTAC_BOTTOM: return 7;
  case TTAC_BOTTOM_RIGHT: return 8;
  default: return -1;
  }
}

// Map mouse coords to TTacCell
TTacCell mouse_to_cell(int x, int y, int cell_w, int cell_h) {
  int col = x / cell_w;
  int row = (y - HEADER_HEIGHT) / cell_h;
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

// Shared state
struct GameState {
  TTacGame game;
  std::array<int, 9> board {};
  bool game_over = false;
  bool show_help = false;
  bool dark_theme = false;
  bool ignore_clicks_this_frame = false;
  Mode mode = MODE_AI_START;
  int win_w = 600;
  int win_h = 600;

  // Sandbox
  bool sandbox_mode = false;
  std::array<int, 9> sandbox_board {}; // 0=empty, 1=blue, 2=red
};

GameState g_state;

// Draw a symbol
void draw_symbol(int index, int player, int cell_w, int cell_h) {
  int row = index / GRID_SIZE;
  int col = index % GRID_SIZE;
  int px = col * cell_w + cell_w / 2;
  int py = row * cell_h + cell_h / 2 + HEADER_HEIGHT;
  int radius = std::min(cell_w, cell_h) / 3;

  if (player == 1) DrawCircle(px, py, radius, g_state.dark_theme ? (Color) {0, 200, 255, 255} : BLUE);
  if (player == 2) DrawCircle(px, py, radius, g_state.dark_theme ? (Color) {255, 0, 0, 255} : RED);
}

// Start/reset game
void start_game(Mode start_mode) {
  g_state.board.fill(0);
  g_state.game_over = false;
  g_state.mode = start_mode;
  g_state.sandbox_board.fill(0);

  if (start_mode == MODE_AI_START) {
    ttac_create_game(&g_state.game, true);
    TTacCell ai_move = ttac_play(g_state.game, 255);
    int idx = cell_to_index(ai_move);
    if (idx >= 0) g_state.board[idx] = 2;
  } else {
    ttac_create_game(&g_state.game, false);
  }
}

// ===== BUTTON HELPERS =====
bool mouse_in_rect(int x, int y, int w, int h) {
  Vector2 m = GetMousePosition();
  return (m.x >= x && m.x <= x + w && m.y >= y && m.y <= y + h);
}

// Draw header with buttons
void draw_header() {
  int btn_w = 138, btn_h = 40, padding = 10;
  int x = padding, y = 10;
  Color text_color = g_state.dark_theme ? WHITE : BLACK;

  // Reset button
  bool hovered_reset = mouse_in_rect(x, y, btn_w, btn_h);
  DrawRectangle(x, y, btn_w, btn_h, hovered_reset ? LIGHTGRAY : GRAY);
  DrawRectangleLines(x, y, btn_w, btn_h, BLACK);
  DrawText("Reset", x + 30, y + 10, 20, text_color);
  if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsGestureDetected(GESTURE_TAP)) && hovered_reset) start_game(g_state.mode);

  // Mode button
  x += btn_w + padding;
  bool hovered_mode = mouse_in_rect(x, y, btn_w, btn_h);
  DrawRectangle(x, y, btn_w, btn_h, hovered_mode ? LIGHTGRAY : GRAY);
  DrawRectangleLines(x, y, btn_w, btn_h, BLACK);
  DrawText((g_state.mode == MODE_AI_START ? "AI First" : "Human First"), x + 10, y + 10, 20, text_color);
  if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsGestureDetected(GESTURE_TAP)) && hovered_mode)
    start_game(g_state.mode == MODE_AI_START ? MODE_HUMAN_START : MODE_AI_START);

  // Help button
  x += btn_w + padding;
  bool hovered_help = mouse_in_rect(x, y, btn_w, btn_h);
  DrawRectangle(x, y, btn_w, btn_h, hovered_help ? LIGHTGRAY : GRAY);
  DrawRectangleLines(x, y, btn_w, btn_h, BLACK);
  DrawText("Help", x + 30, y + 10, 20, text_color);
  if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsGestureDetected(GESTURE_TAP)) && hovered_help) g_state.show_help = !g_state.show_help;

  // Theme button
  x += btn_w + padding;
  bool hovered_theme = mouse_in_rect(x, y, btn_w, btn_h);
  DrawRectangle(x, y, btn_w, btn_h, hovered_theme ? LIGHTGRAY : GRAY);
  DrawRectangleLines(x, y, btn_w, btn_h, BLACK);
  DrawText(g_state.dark_theme ? "Dark" : "Light", x + 30, y + 10, 20, text_color);
  if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsGestureDetected(GESTURE_TAP)) && hovered_theme) g_state.dark_theme = !g_state.dark_theme;

  // Sandbox button
  x += btn_w + padding;
  bool hovered_sandbox = mouse_in_rect(x, y, btn_w, btn_h);
  DrawRectangle(x, y, btn_w, btn_h, hovered_sandbox ? LIGHTGRAY : GRAY);
  DrawRectangleLines(x, y, btn_w, btn_h, BLACK);
  DrawText(g_state.sandbox_mode ? "Sandbox ON" : "Sandbox OFF", x + 10, y + 10, 20, text_color);
  if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsGestureDetected(GESTURE_TAP)) && hovered_sandbox) g_state.sandbox_mode = !g_state.sandbox_mode;
}

// Draw help overlay
void draw_help() {
  Color text_color = g_state.dark_theme ? WHITE : BLACK;
  Color bg_color = g_state.dark_theme ? Fade((Color) {30, 30, 30, 255}, 0.95f) : Fade(LIGHTGRAY, 0.9f);

  DrawRectangle(50, 50, g_state.win_w - 100, g_state.win_h - 100, bg_color);
  DrawText("Key Bindings:", 70, 70, 20, text_color);
  DrawText("R - Reset game", 70, 100, 20, text_color);
  DrawText("A - AI starts first", 70, 130, 20, text_color);
  DrawText("H - Human starts first", 70, 160, 20, text_color);
  DrawText("F1 - Toggle help menu", 70, 190, 20, text_color);
  DrawText("T - Toggle dark/light theme", 70, 220, 20, text_color);
  DrawText("S - Toggles sandbox mode", 70, 250, 20, text_color);
}

// Draw game over popup
void draw_game_over_popup() {
  int popup_w = 300, popup_h = 150;
  int x = (g_state.win_w - popup_w) / 2;
  int y = (g_state.win_h - popup_h) / 2;

  DrawRectangle(0, 0, g_state.win_w, g_state.win_h, Fade(BLACK, 0.5f));
  DrawRectangle(x, y, popup_w, popup_h, g_state.dark_theme ? DARKGRAY : LIGHTGRAY);
  DrawRectangleLines(x, y, popup_w, popup_h, BLACK);

  const char *msg;
  TTacByte state = ttac_game_state(g_state.game);
  if (state == TTAC_GAME_PLAYER_WIN)
    msg = "You Win!";
  else if (state == TTAC_GAME_PLAYER_LOSS)
    msg = "You Lose!";
  else
    msg = "Draw!";

  int text_w = MeasureText(msg, 30);
  DrawText(msg, x + (popup_w - text_w) / 2, y + 30, 30, g_state.dark_theme ? WHITE : BLACK);

  int btn_w = 120, btn_h = 40;
  int btn_x = x + (popup_w - btn_w) / 2;
  int btn_y = y + 90;
  Color text_color = g_state.dark_theme ? WHITE : BLACK;

  Vector2 mouse = GetMousePosition();
  bool hovered = mouse.x >= btn_x && mouse.x <= btn_x + btn_w && mouse.y >= btn_y && mouse.y <= btn_y + btn_h;

  DrawRectangle(btn_x, btn_y, btn_w, btn_h, hovered ? LIGHTGRAY : GRAY);
  DrawRectangleLines(btn_x, btn_y, btn_w, btn_h, BLACK);
  DrawText("Play Again", btn_x + 10, btn_y + 10, 20, text_color);

  if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsGestureDetected(GESTURE_TAP)) && hovered && !g_state.ignore_clicks_this_frame)
    start_game(g_state.mode);
}

// Main loop
void game_loop() {
  g_state.win_w = GetScreenWidth();
  g_state.win_h = GetScreenHeight();
  int cell_w = g_state.win_w / GRID_SIZE;
  int cell_h = (g_state.win_h - HEADER_HEIGHT) / GRID_SIZE;

#if defined(PLATFORM_WEB)
  SetMouseScale(static_cast<float>(g_state.win_w) / GetRenderWidth(), static_cast<float>(g_state.win_h) / GetRenderHeight());
#endif

  // ===== INPUT =====
  Vector2 mouse = GetMousePosition();
  if (mouse.y >= HEADER_HEIGHT && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsGestureDetected(GESTURE_TAP))) {
    if (g_state.sandbox_mode) {
      TTacCell cell = mouse_to_cell(mouse.x, mouse.y, cell_w, cell_h);
      int idx = cell_to_index(cell);
      if (idx >= 0) g_state.sandbox_board[idx] = (g_state.sandbox_board[idx] + 1) % 3;
    } else if (!g_state.game_over) {
      TTacCell move = mouse_to_cell(mouse.x, mouse.y, cell_w, cell_h);
      int idx = cell_to_index(move);
      if (idx >= 0 && g_state.board[idx] == 0) {
        g_state.board[idx] = 1; // human
        TTacCell ai_move = ttac_play(g_state.game, move);
        int ai_idx = cell_to_index(ai_move);
        if (ai_idx >= 0) g_state.board[ai_idx] = 2;

        if (ttac_game_state(g_state.game) != TTAC_GAME_PENDING) {
          g_state.game_over = true;
          g_state.ignore_clicks_this_frame = true;
        }
      }
    }
  }

  // ===== KEYS =====
  if (IsKeyPressed(KEY_R)) start_game(g_state.mode);
  if (IsKeyPressed(KEY_A)) start_game(MODE_AI_START);
  if (IsKeyPressed(KEY_H)) start_game(MODE_HUMAN_START);
  if (IsKeyPressed(KEY_F1)) g_state.show_help = !g_state.show_help;
  if (IsKeyPressed(KEY_T)) g_state.dark_theme = !g_state.dark_theme;
  if (IsKeyPressed(KEY_S)) g_state.sandbox_mode = !g_state.sandbox_mode;

  // ===== DRAW =====
  BeginDrawing();
  ClearBackground(g_state.dark_theme ? DARKGRAY : RAYWHITE);

  draw_header();

  // Grid
  Color grid_color = g_state.dark_theme ? (Color) {110, 110, 110, 255} : BLACK;
  for (int i = 0; i <= GRID_SIZE; ++i) {
    DrawLine(i * cell_w, HEADER_HEIGHT, i * cell_w, g_state.win_h, grid_color);
    DrawLine(0, i * cell_h + HEADER_HEIGHT, g_state.win_w, i * cell_h + HEADER_HEIGHT, grid_color);
  }

  // Symbols
  for (int i = 0; i < 9; ++i) {
    if (g_state.sandbox_mode) {
      if (g_state.sandbox_board[i] != 0) draw_symbol(i, g_state.sandbox_board[i], cell_w, cell_h);
    } else {
      if (g_state.board[i] != 0) draw_symbol(i, g_state.board[i], cell_w, cell_h);
    }
  }

  if (g_state.show_help) draw_help();
  if (g_state.game_over) draw_game_over_popup();
  g_state.ignore_clicks_this_frame = false;

  EndDrawing();
}

int main() {
  srandom(time(nullptr));
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(g_state.win_w, g_state.win_h, "TTac AI / Human");

  SetTargetFPS(60);
  start_game(MODE_AI_START);

#if defined(PLATFORM_WEB)
  SetWindowSize(GetScreenWidth(), GetScreenHeight());
  g_state.dark_theme = prefers_dark_mode();
  emscripten_set_main_loop(game_loop, 0, 1);
#else
  while (!WindowShouldClose()) game_loop();
#endif

  CloseWindow();
  return 0;
}
