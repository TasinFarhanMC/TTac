#include "ttac.h"

static TTacCell ai_triangle_end(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  game_ptr->state = TTAC_GAME_AI_WIN;

  return move == TTAC_CENTER ? TTAC_MIDDLE(game_ptr->control1, game_ptr->control2) : TTAC_CENTER;
}

static TTacCell ai_triangle(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  TTacCell control1 = game_ptr->control1;
  TTacCell control2 = game_ptr->control2;

  TTacCell middle = TTAC_MIDDLE(control1, control2);

  if (move != middle) {
    game_ptr->state = TTAC_GAME_AI_WIN;
    return middle;
  }

  game_ptr->branch = ai_triangle_end;
  game_ptr->control2 = TTAC_OPP_SAME(control2);
  return game_ptr->control2;
}
static TTacCell ai_draw_edge(void *game, TTacCell move) {}

static TTacCell ai_draw_corner(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  TTacCell win = TTAC_LINE(game_ptr->control1, game_ptr->control2);

  if (move != win) {
    game_ptr->state = TTAC_GAME_AI_WIN;
    return win;
  }

  game_ptr->state = TTAC_GAME_DRAW;
  return TTAC_OPP_SAME(game_ptr->control2);
}

static TTacCell ai_draw(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  TTacCell control2 = game_ptr->control2;
  if (TTAC_IS_EDGE(move)) { return TTAC_OPP_SAME(move); }

  game_ptr->control2 = TTAC_IS_OPP_DIFF(TTAC_ADJ1_SAME(control2), move) ? TTAC_ADJ1_SAME(control2) : TTAC_ADJ2_SAME(control2);
  game_ptr->branch = ai_draw_corner;
  return game_ptr->control2;
}

static TTacCell ai_center(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  TTacCell control1 = game_ptr->control1;
  TTacCell control2 = game_ptr->control2;

  TTacCell middle = TTAC_MIDDLE(control1, control2);
  if (move != middle) {
    game_ptr->state = TTAC_GAME_AI_WIN;
    return middle;
  }

  game_ptr->branch = ai_draw;
  game_ptr->control2 = TTAC_OPP_SAME(move);
  return game_ptr->control2;
}

// static TTacCell ai_corner(void *game, TTacCell move) {{{{
//   TTacGame *game_ptr = game;
//   TTacCell control = game_ptr->control1;
//
// if (move == TTAC_CENTER) {
// .............
// }
//
//   // CORNER
//   if (TTAC_IS_CORNER(move)) {
//     // ADJACENT
//     if (TTAC_IS_ADJ_SAME(control, move)) {
//       TTacCell output = TTAC_OPP_SAME(move);
//       game_ptr->branch = ai_triangle;
//       game_ptr->control1 = output;
//       game_ptr->control2 = control;
//       return output;
//     }
//
//     // OPPOSITE
//     game_ptr->branch = ai_triangle;
//     game_ptr->control2 = TTAC_ADJ1_SAME(move);
//     return game_ptr->control2;
//   }
//
//   // EDGE
//   // ADJACENT
//   if (TTAC_IS_ADJ_DIFF(control, move)) {
//     game_ptr->control1 = TTAC_LINE_OPP(control, move);
//     game_ptr->control2 = control;
//     game_ptr->branch = ai_triangle;
//     return game_ptr->control1;
//   }
//   // OPPOSITE
//   game_ptr->control2 = (TTAC_IS_ADJ_SAME(TTAC_ADJ1_SAME(control), move) ?: TTAC_ADJ1_SAME(control), TTAC_ADJ2_SAME(control));
//   game_ptr->branch = ai_triangle;
//   return game_ptr->control2;
// }}}}

static TTacCell ai_corner(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  TTacCell control = game_ptr->control1;
  TTacCell output;
  game_ptr->branch = ai_triangle;

  if (move == TTAC_CENTER) {
    game_ptr->branch = ai_center;
    game_ptr->control1 = TTAC_ADJ1_SAME(control);
    game_ptr->control2 = control;
    return game_ptr->control1;
  }

  if (TTAC_IS_CORNER(move)) {
    // corner logic: adjacent vs opposite
    output = TTAC_IS_ADJ_SAME(control, move) ? TTAC_OPP_SAME(move) : TTAC_ADJ1_SAME(move);
    TTacBool swap = TTAC_IS_ADJ_SAME(control, move);

    game_ptr->control1 = swap ? output : control;
    game_ptr->control2 = swap ? control : output;
    return output;
  }

  // edge logic: adjacent vs opposite
  if (TTAC_IS_ADJ_DIFF(control, move)) {
    output = TTAC_LINE_OPP(control, move);
    game_ptr->control1 = output;
    game_ptr->control2 = control;
    return output;
  }

  // edge opposite
  output = TTAC_IS_ADJ_SAME(TTAC_ADJ1_SAME(control), move) ? TTAC_ADJ1_SAME(control) : TTAC_ADJ2_SAME(control);
  game_ptr->control2 = output;
  return output;
}

static TTacCell ai(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  game_ptr->branch = ai_corner;
  game_ptr->control1 = TTAC_TOP_LEFT;
  return TTAC_TOP_LEFT;
}

static TTacCell player(void *game, TTacCell move) {
  TTacGame *game_ptr = game;

  // CORNER
  if (TTAC_IS_CORNER(move)) { return TTAC_CENTER; }
}

void ttac_create_game(TTacGame *game, TTacBool ai_start) {
  game->state = TTAC_GAME_PENDING;
  game->branch = ai_start ? ai : player;
}
