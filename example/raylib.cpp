// Developers note:
// The core library is built and tested by me but
// this example is almost entirely made by chatgpt & revised by gemini with some minor tweaks by me to make it work
// as I am not good with guis we have to compensate but i will redo this eventually

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

enum Mode {
  MODE_AI_START,
  MODE_HUMAN_START,
  MODE_AI_VS_AI
};

struct GameState {
  TTacGame game;
  TTacGame game2;
  TTacCell last_move;
  int current_ai;
  int sprint_timer = 0;
  int hold_delay = 0;
  std::array<int, 9> board {};
  bool game_over = false;
  bool show_help = false;
  bool dark_theme = false;
  Mode mode = MODE_AI_START;
  int win_w = 600;
  int win_h = 600;
  int header_h = 60;
  bool step_btn_down = false; // Added to track mouse state across functions
};

GameState g_state;

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

TTacCell mouse_to_cell(int x, int y, int cell_w, int cell_h) {
  int col = x / cell_w;
  int row = (y - g_state.header_h) / cell_h;
  if (row < 0 || row >= 3 || col < 0 || col >= 3) return 255;
  TTacCell cells[3][3] = {
      {   TTAC_TOP_LEFT,    TTAC_TOP,    TTAC_TOP_RIGHT},
      {       TTAC_LEFT, TTAC_CENTER,        TTAC_RIGHT},
      {TTAC_BOTTOM_LEFT, TTAC_BOTTOM, TTAC_BOTTOM_RIGHT}
  };
  return cells[row][col];
}

bool mouse_in_rect(int x, int y, int w, int h) {
  Vector2 m = GetMousePosition();
  return (m.x >= x && m.x <= x + w && m.y >= y && m.y <= y + h);
}

void draw_symbol(int index, int player, int cell_w, int cell_h) {
  int row = index / GRID_SIZE;
  int col = index % GRID_SIZE;
  int px = col * cell_w + cell_w / 2;
  int py = row * cell_h + cell_h / 2 + g_state.header_h;
  int radius = std::min(cell_w, cell_h) / 3;
  if (player == 1) DrawCircle(px, py, radius, g_state.dark_theme ? (Color) {255, 0, 0, 255} : RED);
  if (player == 2) DrawCircle(px, py, radius, g_state.dark_theme ? (Color) {0, 200, 255, 255} : BLUE);
}

void start_game(Mode start_mode) {
  g_state.board.fill(0);
  g_state.game_over = false;
  g_state.mode = start_mode;
  g_state.sprint_timer = 0;
  g_state.hold_delay = 0;
  if (start_mode == MODE_AI_START) {
    ttac_start_game(&g_state.game, true);
    TTacCell ai_move = ttac_play(g_state.game, 255);
    int idx = cell_to_index(ai_move);
    if (idx >= 0) g_state.board[idx] = 2;
  } else if (start_mode == MODE_HUMAN_START) {
    ttac_start_game(&g_state.game, false);
  } else if (start_mode == MODE_AI_VS_AI) {
    ttac_start_game(&g_state.game, true);
    ttac_start_game(&g_state.game2, false);
    g_state.current_ai = 1;
    g_state.last_move = 255;
  }
}

void step_ai_vs_ai() {
  if (g_state.game_over || g_state.mode != MODE_AI_VS_AI) return;
  if (g_state.current_ai == 1) {
    g_state.last_move = ttac_play(g_state.game, g_state.last_move);
    int idx = cell_to_index(g_state.last_move);
    if (idx >= 0) g_state.board[idx] = 1;
    if (g_state.game.state != TTAC_GAME_PENDING) g_state.game_over = true;
    g_state.current_ai = 2;
  } else {
    g_state.last_move = ttac_play(g_state.game2, g_state.last_move);
    int idx = cell_to_index(g_state.last_move);
    if (idx >= 0) g_state.board[idx] = 2;
    if (g_state.game2.state != TTAC_GAME_PENDING) g_state.game_over = true;
    g_state.current_ai = 1;
  }
}

// Logic for sprinting through moves when holding Step or Space
void update_sprint() {
  if (g_state.game_over || g_state.mode != MODE_AI_VS_AI) return;
  if (++g_state.hold_delay > 20) {
    if (++g_state.sprint_timer >= 4) {
      step_ai_vs_ai();
      g_state.sprint_timer = 0;
    }
  }
}

void draw_header() {
  float v_scale = std::min((float)g_state.win_h / 600.0f, 1.2f);
  if (v_scale < 0.7f) v_scale = 0.7f;

  int padding = 8;
  int btn_h = (int)(40 * v_scale);
  g_state.header_h = btn_h + 20;

  int total_btns = 5;
  int btn_w = (g_state.win_w - (padding * (total_btns + 1))) / total_btns;

  float h_scale = (float)btn_w / 110.0f;
  float text_scale = std::min(v_scale, h_scale);
  int font_size = (int)(18 * text_scale);
  if (font_size < 10) font_size = 10;

  int x = padding, y = 10;
  Color text_color = g_state.dark_theme ? WHITE : BLACK;

  auto draw_btn = [&](const char *label, int w, bool active) {
    bool hovered = mouse_in_rect(x, y, w, btn_h) && active;
    DrawRectangle(x, y, w, btn_h, active ? (hovered ? LIGHTGRAY : GRAY) : DARKGRAY);
    DrawRectangleLines(x, y, w, btn_h, BLACK);

    int tw = MeasureText(label, font_size);
    DrawText(label, x + (w - tw) / 2, y + (btn_h - font_size) / 2, font_size, active ? text_color : GRAY);

    bool clicked = (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hovered);
    bool down = (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && hovered);

    x += w + padding;
    return std::make_pair(clicked, down);
  };

  if (draw_btn("Reset (R)", btn_w, true).first) start_game(g_state.mode);

  const char *mode_txt = (g_state.mode == MODE_AI_START) ? "AI First" : (g_state.mode == MODE_HUMAN_START) ? "You First" : "AI v AI";
  if (draw_btn(mode_txt, btn_w, true).first) start_game((Mode)((g_state.mode + 1) % 3));

  auto step_state = draw_btn("Step", btn_w, g_state.mode == MODE_AI_VS_AI);
  if (step_state.first) {
    step_ai_vs_ai();
    g_state.hold_delay = 0;
  }
  g_state.step_btn_down = step_state.second;

  if (draw_btn(g_state.dark_theme ? "Dark" : "Light", btn_w, true).first) g_state.dark_theme = !g_state.dark_theme;
  if (draw_btn("?", btn_w, true).first) g_state.show_help = !g_state.show_help;
}

void draw_help() {
  float scale = std::min((float)g_state.win_w / 600.0f, (float)g_state.win_h / 600.0f);
  int fs = (int)(20 * scale);
  Color text_color = g_state.dark_theme ? WHITE : BLACK;
  Color bg_color = g_state.dark_theme ? Fade((Color) {30, 30, 30, 255}, 0.95f) : Fade(LIGHTGRAY, 0.9f);
  DrawRectangle(50, 50, g_state.win_w - 100, g_state.win_h - 100, bg_color);
  DrawText("Key Bindings:", 70, 70, fs, text_color);
  DrawText("Space - Step through moves", 70, 70 + (int)(30 * scale), fs, text_color);
  DrawText("Space/Step (Hold) - Sprint auto-play", 70, 70 + (int)(60 * scale), fs, text_color);
  DrawText("R - Reset / M - Cycle Modes", 70, 70 + (int)(90 * scale), fs, text_color);
  DrawText("T - Dark Theme / F1 - Help", 70, 70 + (int)(120 * scale), fs, text_color);
}

void game_loop() {
  g_state.win_w = GetScreenWidth();
  g_state.win_h = GetScreenHeight();
  int cell_w = g_state.win_w / GRID_SIZE;
  int cell_h = (g_state.win_h - g_state.header_h) / GRID_SIZE;

#if defined(PLATFORM_WEB)
  SetMouseScale((float)g_state.win_w / GetRenderWidth(), (float)g_state.win_h / GetRenderHeight());
#endif

  // Input Handling
  if (!g_state.game_over) {
    if (g_state.mode != MODE_AI_VS_AI && GetMouseY() >= g_state.header_h && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      TTacCell move = mouse_to_cell(GetMouseX(), GetMouseY(), cell_w, cell_h);
      int idx = cell_to_index(move);
      if (idx >= 0 && g_state.board[idx] == 0) {
        g_state.board[idx] = 1;
        TTacCell ai_move = ttac_play(g_state.game, move);
        int ai_idx = cell_to_index(ai_move);
        if (ai_idx >= 0) g_state.board[ai_idx] = 2;
        if (g_state.game.state != TTAC_GAME_PENDING) g_state.game_over = true;
      }
    }
  }

  if (IsKeyPressed(KEY_R)) start_game(g_state.mode);
  if (IsKeyPressed(KEY_M)) start_game((Mode)((g_state.mode + 1) % 3));
  if (IsKeyPressed(KEY_F1)) g_state.show_help = !g_state.show_help;
  if (IsKeyPressed(KEY_T)) g_state.dark_theme = !g_state.dark_theme;

  if (g_state.mode == MODE_AI_VS_AI && !g_state.game_over) {
    if (IsKeyPressed(KEY_SPACE)) {
      step_ai_vs_ai();
      g_state.hold_delay = 0;
    }
    if (IsKeyDown(KEY_SPACE) || g_state.step_btn_down) {
      update_sprint();
    } else {
      g_state.hold_delay = 0;
      g_state.sprint_timer = 0;
    }
  }

  BeginDrawing();
  ClearBackground(g_state.dark_theme ? DARKGRAY : RAYWHITE);

  // 1. Draw the Grid and Symbols first
  Color grid_color = g_state.dark_theme ? (Color) {110, 110, 110, 255} : BLACK;
  for (int i = 0; i <= GRID_SIZE; ++i) {
    DrawLine(i * cell_w, g_state.header_h, i * cell_w, g_state.win_h, grid_color);
    DrawLine(0, i * cell_h + g_state.header_h, g_state.win_w, i * cell_h + g_state.header_h, grid_color);
  }
  for (int i = 0; i < 9; ++i)
    if (g_state.board[i] != 0) draw_symbol(i, g_state.board[i], cell_w, cell_h);

  // 2. If game is over, darken ONLY the board area
  if (g_state.game_over) {
    float scale = std::min((float)g_state.win_w / 600.0f, (float)g_state.win_h / 600.0f);
    // Darken starting from below the header
    DrawRectangle(0, g_state.header_h, g_state.win_w, g_state.win_h - g_state.header_h, Fade(BLACK, 0.6f));

    const char *res_txt =
        (g_state.mode == MODE_AI_VS_AI)
            ? "Perfect Draw (AI vs AI)"
            : (g_state.game.state == TTAC_GAME_PLAYER_WIN ? "You Win!" : (g_state.game.state == TTAC_GAME_PLAYER_LOSS ? "You Lose!" : "It's a Draw!"));

    Color res_col = (g_state.game.state == TTAC_GAME_PLAYER_WIN) ? GREEN : (g_state.game.state == TTAC_GAME_PLAYER_LOSS ? RED : RAYWHITE);
    int tw = MeasureText(res_txt, (int)(30 * scale));
    DrawText(res_txt, (g_state.win_w - tw) / 2, (g_state.win_h + g_state.header_h) / 2 - (int)(20 * scale), (int)(30 * scale), res_col);
  }

  // 3. Draw Header LAST so it stays bright and on top
  draw_header();

  if (g_state.show_help) draw_help();

  EndDrawing();
}

int main() {
  srand(time(nullptr));
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(600, 600, "TTac");
  SetTargetFPS(60);
  start_game(MODE_AI_START);
#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(game_loop, 0, 1);
#else
  while (!WindowShouldClose()) game_loop();
#endif
  CloseWindow();
  return 0;
}
