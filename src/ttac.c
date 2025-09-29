#include "ttac.h"

static TTacCell ai_corner_fork(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  TTacCell control1 = game_ptr->control1;
  TTacCell control2 = game_ptr->control2;

  TTacCell output = TTAC_MIDDLE(control1, control2);
  return output;
}

static TTacCell ai_corner(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  TTacCell control = game_ptr->control1;
  game_ptr->control2 = control;

  // CORNER

  if (TTAC_IS_CORNER(move)) {
    // ADJACENT
    if (TTAC_IS_ADJ_SAME(control, move)) {
      TTacCell output = TTAC_OPP_SAME(move);
      game_ptr->branch = ai_corner_fork;
      game_ptr->control1 = output;
      return output;
    }

    // OPPOSITE
    TTacCell output = TTAC_ADJ1_SAME(move);

    game_ptr->branch = ai_corner_fork;
    game_ptr->control1 = output;
    return output;
  }

  // EDGE

  return TTAC_CENTER;
}

static TTacCell ai(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  game_ptr->branch = ai_corner;
  game_ptr->control1 = TTAC_TOP_LEFT;
  return TTAC_TOP_LEFT;
}

void ttac_create_game(TTacGame *game, TTacBool starting_player) {
  if (starting_player) { game->branch = ai; }
}

TTacCell ttac_play(TTacGame *game, TTacCell move) { return game->branch(game, move); }
