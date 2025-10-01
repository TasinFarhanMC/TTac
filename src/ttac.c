#include "ttac.h"

static TTacCell ai_draw_end(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  TTacCell c1 = game_ptr->c1;
  TTacCell c2 = game_ptr->c2;

  if (move == c1) {
    game_ptr->state = TTAC_GAME_DRAW;
    return c2;
  }

  game_ptr->state = TTAC_GAME_AI_WIN;
  return c1;
}

static TTacCell ai_draw(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  TTacCell c1 = game_ptr->c1;
  TTacCell c2 = game_ptr->c2;
  game_ptr->branch = ai_draw_end;

  if (TTAC_IS_EDGE(move)) {
    TTacCell result = TTAC_OPP_SAME(move);
    game_ptr->c1 = TTAC_EDGE_CORNER(result, c2);
    game_ptr->c2 = TTAC_EDGE_CORNER(move, c2);
    return result;
  }

  TTacCell result = TTAC_IS_OPP_DIFF(TTAC_ADJ1_SAME(c2), move) ? TTAC_ADJ1_SAME(c2) : TTAC_ADJ1_SAME(c1);
  game_ptr->c1 = TTAC_EDGE_CORNER(result, c2);
  game_ptr->c2 = TTAC_OPP_SAME(result);
  return result;
}

static TTacCell ai_center(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  TTacCell c1 = game_ptr->c1;
  TTacCell c2 = game_ptr->c2;

  TTacCell middle = TTAC_MIDDLE(c1, c2);
  if (move != middle) {
    game_ptr->state = TTAC_GAME_AI_WIN;
    return middle;
  }

  game_ptr->branch = ai_draw;
  game_ptr->c1 = move;
  game_ptr->c2 = TTAC_OPP_SAME(move);
  return game_ptr->c2;
}

static TTacCell triangle_end(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  game_ptr->state = TTAC_GAME_AI_WIN;

  return move == TTAC_CENTER ? TTAC_MIDDLE(game_ptr->c1, game_ptr->c2) : TTAC_CENTER;
}

static TTacCell triangle(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  TTacCell c1 = game_ptr->c1;
  TTacCell c2 = game_ptr->c2;

  TTacCell middle = TTAC_MIDDLE(c1, c2);

  if (move != middle) {
    game_ptr->state = TTAC_GAME_AI_WIN;
    return middle;
  }

  game_ptr->branch = triangle_end;
  game_ptr->c2 = TTAC_OPP_SAME(c2);
  return game_ptr->c2;
}

static TTacCell ai_corner(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  TTacCell control = game_ptr->c1;

  // Center move
  if (move == TTAC_CENTER) {
    game_ptr->branch = ai_center;
    game_ptr->c1 = TTAC_ADJ1_SAME(control);
    game_ptr->c2 = control;
    return game_ptr->c1;
  }

  game_ptr->branch = triangle;

  const TTacBool is_corner = TTAC_IS_CORNER(move);

  // clang-format off
  // Determine adjacency based on type
  const TTacBool adjacent = is_corner
                            ? TTAC_IS_ADJ_SAME(control, move)
                            : TTAC_IS_ADJ_DIFF(control, move);

  // Compute output based on type and adjacency
  TTacCell output = is_corner
               ? (adjacent ? TTAC_OPP_SAME(move)
                           : TTAC_ADJ1_SAME(move))
               : (adjacent ? TTAC_LINE_OPP(control, move)
                           : TTAC_ADJ_LINE(control, move));
  // clang-format on

  // Set control cells
  game_ptr->c1 = adjacent ? output : control;
  game_ptr->c2 = adjacent ? control : output;

  return output;
}

static TTacCell ai(void *game, TTacCell move) {
  TTacGame *game_ptr = game;

  game_ptr->branch = ai_corner;
  game_ptr->c1 = TTAC_TOP_LEFT;

  return TTAC_TOP_LEFT;
}

static TTacCell player(void *game, TTacCell move) {
  TTacGame *game_ptr = game;

  // CORNER
  if (TTAC_IS_CORNER(move)) { return TTAC_CENTER; }
  // TODO:
}

void ttac_create_game(TTacGame *game, TTacBool ai_start) {
  game->state = TTAC_GAME_PENDING;
  game->branch = ai_start ? ai : player;
}
