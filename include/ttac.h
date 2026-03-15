#ifndef TTAC_H
#define TTAC_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef unsigned char TTacByte;
typedef TTacByte TTacCell;
typedef TTacByte TTacBool;

typedef struct TTacGame TTacGame;
typedef TTacCell (*TTacBranch)(TTacGame *game, TTacCell move);

struct TTacGame {
  TTacCell c1;
  TTacCell c2;
  TTacBool state;
  TTacBranch branch;
};

#define TTAC_TOP_LEFT 0b1000
#define TTAC_TOP_RIGHT 0b1101
#define TTAC_BOTTOM_LEFT 0b1011
#define TTAC_BOTTOM_RIGHT 0b1110

#define TTAC_TOP 0b0010
#define TTAC_BOTTOM 0b0100
#define TTAC_LEFT 0b0111
#define TTAC_RIGHT 0b0001

#define TTAC_CENTER 0b1111
#define TTAC_NULL 0b0000

#define TTAC_GAME_PENDING 0
#define TTAC_GAME_DRAW 1
#define TTAC_GAME_AI_WIN 2
#define TTAC_GAME_PLAYER_LOSS 2
#define TTAC_GAME_AI_LOSS 3
#define TTAC_GAME_PLAYER_WIN 3

#define TTAC_OPP_SAME(x) (x ^ 0b0110)
#define TTAC_ADJ1_SAME(x) (x ^ 0b0011)
#define TTAC_ADJ2_SAME(x) (x ^ 0b0101)

#define TTAC_OPP1_DIFF(x) (x ^ 0b1001)
#define TTAC_OPP2_DIFF(x) (x ^ (0b1000 | 0b0100 >> (x & 0b0001)))

#define TTAC_ADJ1_DIFF(x) (x ^ 0b1111)
#define TTAC_ADJ2_DIFF(x) (x ^ (0b1000 | 0b0010 << (x & 0b0001)))

#define TTAC_IS_CORNER(x) (x & 0b1000)
#define TTAC_IS_EDGE(x) (!(x & 0b1000))
#define TTAC_IS_CENTER(x) (x == TTAC_CENTER)

#define TTAC_IS_OPP_SAME(a, b) !((a ^ b) & 0b0001)
#define TTAC_IS_ADJ_SAME(a, b) ((a ^ b) & 0b0001)

#define TTAC_IS_OPP_DIFF(a, b) !((a ^ b) & (0b0010 << (a & 0b0001)))
#define TTAC_IS_ADJ_DIFF(a, b) ((a ^ b) & (0b0010 << (a & 0b0001)))

#define TTAC_IS_SAME(a, b) !((a ^ b) & 0b1000)
#define TTAC_IS_DIFF(a, b) ((a ^ b) & 0b1000)

#define TTAC_MIDDLE(a, b) (TTAC_IS_ADJ_DIFF(TTAC_ADJ1_DIFF(a), b) ? TTAC_ADJ1_DIFF(a) : TTAC_ADJ1_DIFF(b))
#define TTAC_ADJ_GEN_ADJ(a, b) (TTAC_IS_ADJ_DIFF(a, TTAC_ADJ1_SAME(b)) ? TTAC_ADJ1_SAME(b) : TTAC_ADJ2_SAME(b))
#define TTAC_OPP_GEN_ADJ(a, b) (TTAC_IS_OPP_DIFF(a, TTAC_ADJ1_SAME(b)) ? TTAC_ADJ1_SAME(b) : TTAC_ADJ2_SAME(b))

extern void ttac_create_game(TTacGame *game, TTacBool ai_start);
#define ttac_play(game, move) game.branch(&game, move)
#define ttac_game_state(game) game.state

#ifdef __cplusplus
}
#endif // __cplusplus
#endif

#ifdef TTAC_IMPLEMENTATION
#ifdef TTAC_C_RANDOM
#include <stdlib.h>
#define ttac_random random
#elif defined(TTAC_USER_RANDOM)
#define ttac_random TTAC_USER_RANDOM
#else
#define ttac_random() 0
#endif // TTAC_C_RANDOM

static TTacCell ttac_branch_ai(TTacGame *game, TTacCell move);
static TTacCell ttac_branch_player(TTacGame *game, TTacCell move);
void ttac_create_game(TTacGame *game, TTacBool ai_start) {
  game->state = TTAC_GAME_PENDING;
  game->branch = (TTacBranch)(ai_start ? ttac_branch_ai : ttac_branch_player);
}

static TTacCell ttac_branch_fork(TTacGame *game, TTacCell move) {
  const TTacCell c1 = game->c1;
  const TTacCell c2 = game->c2;
  game->state = TTAC_GAME_AI_WIN;

  return move == c1 ? c2 : c1;
}

static TTacCell ttac_branch_ai_next(TTacGame *game, TTacCell move);
static TTacCell ttac_branch_ai_end(TTacGame *game, TTacCell move);

static TTacCell ttac_branch_ai_center(TTacGame *game, TTacCell move);
static TTacCell ttac_branch_ai_center_next(TTacGame *game, TTacCell move);
static TTacCell ttac_branch_ai_center_end(TTacGame *game, TTacCell move);

static TTacCell ttac_branch_ai(TTacGame *game, TTacCell move) {
  game->branch = ttac_branch_ai_next;

  const TTacCell corners[4] = {TTAC_TOP_LEFT, TTAC_TOP_RIGHT, TTAC_BOTTOM_LEFT, TTAC_BOTTOM_RIGHT};
  game->c1 = corners[ttac_random() & 0b11];

  return game->c1;
}
static TTacCell ttac_branch_ai_next(TTacGame *game, TTacCell move) {
  const TTacCell control = game->c1;

  // Center move
  if (move == TTAC_CENTER) {
    game->branch = ttac_branch_ai_center;

    game->c2 = TTAC_OPP_SAME(control);
    return game->c2;
  }

  game->branch = ttac_branch_ai_end;

  const TTacBool is_corner = TTAC_IS_CORNER(move);

  // clang-format off
  // Determine adjacency based on type
  const TTacBool adjacent = is_corner
                            ? TTAC_IS_ADJ_SAME(control, move)
                            : TTAC_IS_ADJ_DIFF(control, move);

  // Compute output based on type and adjacency
  const TTacCell output = is_corner
               ? (adjacent ? TTAC_OPP_SAME(move)
                           : TTAC_ADJ1_SAME(move))
               : (adjacent ? TTAC_OPP_GEN_ADJ(move, control)
                           : TTAC_ADJ_GEN_ADJ(move, control));
  // clang-format on

  game->c1 = adjacent ? output : control;
  game->c2 = adjacent ? control : output;

  return output;
}
static TTacCell ttac_branch_ai_end(TTacGame *game, TTacCell move) {
  const TTacCell c1 = game->c1;
  const TTacCell c2 = game->c2;

  const TTacCell middle = TTAC_MIDDLE(c1, c2);

  if (move != middle) {
    game->state = TTAC_GAME_AI_WIN;
    return middle;
  }

  game->branch = ttac_branch_fork;

  const TTacCell result = TTAC_OPP_SAME(c2);
  game->c1 = TTAC_MIDDLE(c1, result);
  game->c2 = TTAC_CENTER;
  return result;
}

static TTacCell ttac_branch_ai_center(TTacGame *game, TTacCell move) {
  const TTacCell c1 = game->c1;
  const TTacCell c2 = game->c2;
  const TTacCell result = TTAC_OPP_SAME(move);

  if (TTAC_IS_CORNER(move)) {
    game->branch = ttac_branch_fork;

    game->c1 = TTAC_ADJ1_DIFF(result);
    game->c2 = TTAC_ADJ2_DIFF(result);
    return result;
  }

  game->branch = ttac_branch_ai_center_next;

  game->c1 = TTAC_ADJ_GEN_ADJ(result, c1);
  game->c2 = TTAC_OPP_GEN_ADJ(game->c1, result);
  return result;
}
static TTacCell ttac_branch_ai_center_next(TTacGame *game, TTacCell move) {
  const TTacCell c1 = game->c1;
  const TTacCell c2 = game->c2;

  if (move != c1) {
    game->state = TTAC_GAME_AI_WIN;
    return c1;
  }

  game->branch = ttac_branch_ai_center_end;

  game->c1 = TTAC_OPP_SAME(c2);
  return TTAC_OPP_SAME(move);
}
static TTacCell ttac_branch_ai_center_end(TTacGame *game, TTacCell move) {
  const TTacCell c1 = game->c1;
  const TTacCell c2 = game->c2;

  if (move != c2) {
    game->state = TTAC_GAME_AI_WIN;
    return c2;
  }

  game->state = TTAC_GAME_DRAW;
  return c1;
}

// Center
static TTacCell ttac_branch_player_center(TTacGame *game, TTacCell move);

static TTacCell ttac_branch_player_center_corner(TTacGame *game, TTacCell move);
static TTacCell ttac_branch_player_center_corner_end(TTacGame *game, TTacCell move);

// Corner
static TTacCell ttac_branch_player_corner(TTacGame *game, TTacCell move);

static TTacCell ttac_branch_player_corner_opp_corner(TTacGame *game, TTacCell move);
static TTacCell ttac_branch_player_corner_adj_corner(TTacGame *game, TTacCell move);

static TTacCell ttac_branch_player_corner_adj_edge(TTacGame *game, TTacCell move);
static TTacCell ttac_branch_player_corner_opp_edge(TTacGame *game, TTacCell move);

static TTacCell ttac_branch_player(TTacGame *game, TTacCell move) {
  if (TTAC_IS_CENTER(move)) {
    game->branch = ttac_branch_player_center;

    const TTacCell corners[4] = {TTAC_TOP_LEFT, TTAC_TOP_RIGHT, TTAC_BOTTOM_LEFT, TTAC_BOTTOM_RIGHT};
    game->c1 = corners[ttac_random() & 0b11];

    return game->c1;
  }

  if (TTAC_IS_CORNER(move)) {
    game->branch = ttac_branch_player_corner;
    game->c1 = move;
    return TTAC_CENTER;
  }

  return TTAC_CENTER;
}

// Draw
static TTacCell ttac_branch_player_draw(TTacGame *game, TTacCell move) {
  game->state = TTAC_GAME_DRAW;
  return TTAC_NULL;
}
static TTacCell ttac_branch_player_checked_draw(TTacGame *game, TTacCell move) {
  const TTacCell c1 = game->c1;
  const TTacCell c2 = game->c2;

  if (move != c1) {
    game->state = TTAC_GAME_AI_WIN;
    return c1;
  }

  game->branch = ttac_branch_player_draw;
  return c2;
}
static TTacCell ttac_branch_player_move_draw(TTacGame *game, TTacCell move) {
  game->branch = ttac_branch_player_draw;
  return (move == game->c1) ? game->c2 : game->c1;
}

// Center
static TTacCell ttac_branch_player_center(TTacGame *game, TTacCell move) {
  const TTacCell control = game->c1;

  if (TTAC_IS_CORNER(move)) {
    game->branch = ttac_branch_player_center_corner;

    const TTacCell result = TTAC_IS_ADJ_SAME(move, control) ? TTAC_OPP_SAME(move) : TTAC_ADJ1_SAME(move);
    game->c1 = TTAC_MIDDLE(result, control);
    game->c2 = TTAC_ADJ1_SAME(game->c1);
    return result;
  }

  return TTAC_CENTER;
}

static TTacCell ttac_branch_player_center_corner(TTacGame *game, TTacCell move) {
  const TTacCell control = game->c1;

  if (move != control) {
    game->state = TTAC_GAME_AI_WIN;
    return control;
  }

  game->branch = ttac_branch_player_center_corner_end;
  return TTAC_OPP_SAME(control);
}
static TTacCell ttac_branch_player_center_corner_end(TTacGame *game, TTacCell move) {
  game->branch = ttac_branch_player_draw;
  return TTAC_IS_EDGE(move) ? TTAC_OPP_SAME(move) : game->c2;
}

// Corner
static TTacCell ttac_branch_player_corner(TTacGame *game, TTacCell move) {
  const TTacCell control = game->c1;

  if (TTAC_IS_CORNER(move)) {
    if (TTAC_IS_OPP_SAME(move, control)) {
      game->branch = ttac_branch_player_corner_opp_corner;

      const TTacCell edges[4] = {TTAC_TOP, TTAC_BOTTOM, TTAC_LEFT, TTAC_RIGHT};
      const TTacCell result = edges[ttac_random() & 0b11];

      game->c1 = TTAC_OPP_SAME(result);
      game->c2 = TTAC_OPP_GEN_ADJ(result, control);
      return result;
    }

    game->branch = ttac_branch_player_corner_adj_corner;

    const TTacCell result = TTAC_MIDDLE(control, move);
    game->c1 = TTAC_OPP_SAME(result);
    game->c2 = TTAC_ADJ1_SAME(result);

    return result;
  }

  if (TTAC_IS_ADJ_DIFF(move, control)) {
    game->branch = ttac_branch_player_corner_adj_edge;

    const TTacCell result = TTAC_ADJ_GEN_ADJ(move, control);
    game->c2 = TTAC_OPP_SAME(result);

    return result;
  }

  game->branch = ttac_branch_player_corner_opp_edge;

  const TTacCell result = TTAC_ADJ_GEN_ADJ(move, control);
  game->c1 = TTAC_OPP_SAME(move);
  game->c2 = TTAC_OPP_SAME(result);
  return result;
}

static TTacCell ttac_branch_player_corner_opp_corner(TTacGame *game, TTacCell move) {
  const TTacCell c1 = game->c1;
  const TTacCell c2 = game->c2;

  if (move != c1) {
    game->state = TTAC_GAME_AI_WIN;
    return c1;
  }

  game->branch = ttac_branch_player_checked_draw;

  game->c1 = TTAC_OPP_SAME(c2);
  game->c2 = TTAC_ADJ1_SAME(c1);
  return c2;
}
static TTacCell ttac_branch_player_corner_adj_corner(TTacGame *game, TTacCell move) {
  const TTacCell c1 = game->c1;
  const TTacCell c2 = game->c2;

  if (move != c1) {
    game->state = TTAC_GAME_AI_WIN;
    return c1;
  }

  game->branch = ttac_branch_player_checked_draw;

  game->c1 = TTAC_OPP_SAME(c2);
  game->c2 = TTAC_MIDDLE(c1, c2);
  return c2;
}

static TTacCell ttac_branch_player_corner_adj_edge(TTacGame *game, TTacCell move) {
  const TTacCell c1 = game->c1;
  const TTacCell c2 = game->c2;

  if (move != c2) {
    game->state = TTAC_GAME_AI_WIN;
    return c2;
  }

  game->branch = ttac_branch_player_checked_draw;

  const TTacCell result = TTAC_MIDDLE(c1, c2);
  game->c1 = TTAC_OPP_SAME(result);
  game->c2 = TTAC_OPP_SAME(c1);
  return result;
}
static TTacCell ttac_branch_player_corner_opp_edge(TTacGame *game, TTacCell move) {
  const TTacCell c1 = game->c1;
  const TTacCell c2 = game->c2;

  if (move != c2) {
    game->state = TTAC_GAME_AI_WIN;
    return c2;
  }

  game->branch = ttac_branch_player_move_draw;
  game->c1 = TTAC_ADJ_GEN_ADJ(c2, c1);
  game->c2 = TTAC_OPP_GEN_ADJ(c1, c2);
  return c1;
}

#endif // TTAC_IMPLEMENTATION
