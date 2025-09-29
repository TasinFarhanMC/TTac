#include "ttac.h"

static TTacCell opp_corner(void *game, TTacCell move) {}

static TTacCell corner(void *game, TTacCell move) {
  TTacGame *game_ptr = game;

  if (TTAC_IS_EDGE(move)) {
    if (TTAC_IS_OPP_SAME(game_ptr->control1, move)) {
      TTacCell ret = TTAC_ADJ1_SAME(move);
      game_ptr->branch = opp_corner;
      game_ptr->control2 = ret;
      return ret;
    }
  }

  return TTAC_CENTER;
}

static TTacCell start(void *game, TTacCell move) {
  TTacGame *game_ptr = game;
  game_ptr->branch = corner;
  game_ptr->control1 = TTAC_TOP_LEFT;
  return TTAC_TOP_LEFT;
}

void ttac_create_game(TTacGame *game, TTacBool starting_player) {
  if (starting_player) { game->branch = start; }
}

TTacCell ttac_play(TTacGame *game, TTacCell move) { return game->branch(game, move); }
