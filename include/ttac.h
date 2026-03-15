#ifndef TTAC_H
#define TTAC_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef unsigned char TTacByte;
typedef TTacByte TTacCell;
typedef TTacByte TTacBool;
typedef TTacCell (*TTacBranch)(void *game, TTacCell move);

typedef struct {
  TTacCell c1;
  TTacCell c2;
  TTacBool state;
  TTacBranch branch;
} TTacGame;

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
#define TTAC_IS_EDGE(x) !(x & 0b1000)
#define TTAC_IS_CENTER(x) (x == TTAC_CENTER)

#define TTAC_IS_OPP_SAME(a, b) !((a ^ b) & 0b0001)
#define TTAC_IS_ADJ_SAME(a, b) ((a ^ b) & 0b0001)

#define TTAC_IS_OPP_DIFF(a, b) !((a ^ b) & (0b0010 << (a & 0b0001)))
#define TTAC_IS_ADJ_DIFF(a, b) ((a ^ b) & (0b0010 << (a & 0b0001)))

#define TTAC_IS_SAME(a, b) !((a ^ b) & 0b1000)
#define TTAC_IS_DIFF(a, b) ((a ^ b) & 0b1000)

#define TTAC_LINE(corner, edge) (TTAC_IS_ADJ_DIFF(TTAC_ADJ1_SAME(corner), edge) ? TTAC_ADJ1_SAME(corner) : TTAC_ADJ2_SAME(corner))
#define TTAC_LINE_ADJ(corner, edge) (TTAC_IS_ADJ_DIFF(TTAC_ADJ1_SAME(corner), edge) ? TTAC_ADJ1_SAME(corner) : TTAC_ADJ2_SAME(corner))
#define TTAC_LINE_OPP(corner, edge) (TTAC_IS_ADJ_DIFF(TTAC_ADJ1_SAME(corner), edge) ? TTAC_ADJ2_SAME(corner) : TTAC_ADJ1_SAME(corner))

#define TTAC_MIDDLE(a, b) (TTAC_IS_ADJ_DIFF(TTAC_ADJ1_DIFF(a), b) ? TTAC_ADJ1_DIFF(a) : TTAC_ADJ1_DIFF(b))

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

#define TTAC_BRANCH_DEF(name) static TTacCell ttac_branch_##name(void *game, TTacCell move)

static TTacCell ttac_branch_fork(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  const TTacCell c1 = game_ptr->c1;
  const TTacCell c2 = game_ptr->c2;
  game_ptr->state = TTAC_GAME_AI_WIN;

  return move == c1 ? c2 : c1;
}
static TTacCell ttac_branch_draw(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  game_ptr->state = TTAC_GAME_DRAW;
  return TTAC_NULL;
}

TTAC_BRANCH_DEF(ai);
TTAC_BRANCH_DEF(player);

void ttac_create_game(TTacGame *game, TTacBool ai_start) {
  game->state = TTAC_GAME_PENDING;
  game->branch = ai_start ? ttac_branch_ai : ttac_branch_player;
}

TTAC_BRANCH_DEF(ai_next);
TTAC_BRANCH_DEF(ai_end);

TTAC_BRANCH_DEF(ai_center);
TTAC_BRANCH_DEF(ai_center_next);
TTAC_BRANCH_DEF(ai_center_end);

static TTacCell ttac_branch_ai(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;

  game_ptr->branch = ttac_branch_ai_next;

  const TTacCell corners[4] = {TTAC_TOP_LEFT, TTAC_TOP_RIGHT, TTAC_BOTTOM_LEFT, TTAC_BOTTOM_RIGHT};
  game_ptr->c1 = corners[ttac_random() & 0b11];

  return game_ptr->c1;
}
static TTacCell ttac_branch_ai_next(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  const TTacCell control = game_ptr->c1;

  // Center move
  if (move == TTAC_CENTER) {
    game_ptr->branch = ttac_branch_ai_center;
    game_ptr->c2 = TTAC_OPP_SAME(control);
    return game_ptr->c2;
  }

  game_ptr->branch = ttac_branch_ai_end;

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
               : (adjacent ? TTAC_LINE_OPP(control, move)
                           : TTAC_LINE_ADJ(control, move));
  // clang-format on

  game_ptr->c1 = adjacent ? output : control;
  game_ptr->c2 = adjacent ? control : output;

  return output;
}
static TTacCell ttac_branch_ai_end(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  const TTacCell c1 = game_ptr->c1;
  const TTacCell c2 = game_ptr->c2;

  const TTacCell middle = TTAC_MIDDLE(c1, c2);

  if (move != middle) {
    game_ptr->state = TTAC_GAME_AI_WIN;
    return middle;
  }

  const TTacCell result = TTAC_OPP_SAME(c2);
  game_ptr->branch = ttac_branch_fork;
  game_ptr->c1 = TTAC_MIDDLE(game_ptr->c1, result);
  game_ptr->c2 = TTAC_CENTER;
  return result;
}

static TTacCell ttac_branch_ai_center(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  TTacCell c1 = game_ptr->c1;
  const TTacCell c2 = game_ptr->c2;
  const TTacCell result = TTAC_OPP_SAME(move);

  if (TTAC_IS_CORNER(move)) {
    game_ptr->branch = ttac_branch_fork;

    game_ptr->c1 = TTAC_ADJ1_DIFF(result);
    game_ptr->c2 = TTAC_ADJ2_DIFF(result);
    return result;
  }

  game_ptr->branch = ttac_branch_ai_center_next;

  c1 = TTAC_IS_ADJ_DIFF(result, c2) ? c2 : c1;
  game_ptr->c2 = (TTAC_ADJ1_DIFF(c1) == result) ? TTAC_ADJ2_DIFF(c1) : TTAC_ADJ1_DIFF(c1);
  game_ptr->c1 = TTAC_LINE(c1, result);
  return result;
}
static TTacCell ttac_branch_ai_center_next(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  const TTacCell control = game_ptr->c1;

  if (move == control) {
    game_ptr->branch = ttac_branch_ai_center_end;
    return TTAC_OPP_SAME(move);
  }

  game_ptr->state = TTAC_GAME_AI_WIN;
  return control;
}
static TTacCell ttac_branch_ai_center_end(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  const TTacCell control = game_ptr->c2;

  game_ptr->state = (move == control) ? TTAC_GAME_DRAW : TTAC_GAME_AI_WIN;

  return TTAC_OPP_SAME(move);
}

TTAC_BRANCH_DEF(player_center);

TTAC_BRANCH_DEF(player_center_opp_corner);
TTAC_BRANCH_DEF(player_center_opp_corner_end);

TTAC_BRANCH_DEF(player_center_adj_corner);
TTAC_BRANCH_DEF(player_center_adj_corner_end);

TTAC_BRANCH_DEF(player_center_adj_edge);
TTAC_BRANCH_DEF(player_center_adj_edge_end);

TTAC_BRANCH_DEF(player_center_opp_edge);
TTAC_BRANCH_DEF(player_center_opp_edge_end);

static TTacCell ttac_branch_player(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;

  if (TTAC_IS_CORNER(move)) {
    game_ptr->c1 = move;
    game_ptr->branch = ttac_branch_player_center;
    return TTAC_CENTER;
  }

  return TTAC_CENTER;
}

static TTacCell ttac_branch_player_center(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  const TTacCell control = game_ptr->c1;

  if (TTAC_IS_CORNER(move)) {
    if (TTAC_IS_OPP_SAME(move, control)) {
      game_ptr->branch = ttac_branch_player_center_opp_corner;

      const TTacCell edges[4] = {TTAC_TOP, TTAC_BOTTOM, TTAC_LEFT, TTAC_RIGHT};
      game_ptr->c1 = move;
      game_ptr->c2 = edges[ttac_random() & 0b11];
      return game_ptr->c2;
    }

    game_ptr->branch = ttac_branch_player_center_adj_corner;
    const TTacCell result = TTAC_MIDDLE(control, move);
    game_ptr->c1 = TTAC_OPP_SAME(result);
    game_ptr->c2 = TTAC_ADJ1_SAME(result);

    return result;
  }

  if (TTAC_IS_ADJ_DIFF(move, control)) {
    game_ptr->branch = ttac_branch_player_center_adj_edge;

    const TTacCell result = TTAC_LINE(control, move);
    game_ptr->c2 = TTAC_OPP_SAME(result);

    return result;
  }

  game_ptr->branch = ttac_branch_player_center_opp_edge;
  const TTacCell result = TTAC_LINE_ADJ(control, move);
  game_ptr->c1 = TTAC_OPP_SAME(move);
  game_ptr->c2 = TTAC_OPP_SAME(result);
  return result;
}

static TTacCell ttac_branch_player_center_opp_corner(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  const TTacCell c1 = game_ptr->c1;
  const TTacCell c2 = game_ptr->c2;
  const TTacCell block = TTAC_OPP_SAME(c2);

  if (move != block) {
    game_ptr->state = TTAC_GAME_AI_WIN;
    return block;
  }

  const TTacCell result = TTAC_LINE(c1, block);

  game_ptr->c1 = TTAC_OPP_SAME(result);
  game_ptr->c2 = TTAC_MIDDLE(c1, game_ptr->c1);
  game_ptr->branch = ttac_branch_player_center_opp_corner_end;
  return result;
}
static TTacCell ttac_branch_player_center_opp_corner_end(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  const TTacCell c1 = game_ptr->c1;
  const TTacCell c2 = game_ptr->c2;

  if (move != c1) {
    game_ptr->state = TTAC_GAME_AI_WIN;
    return c1;
  }

  game_ptr->branch = ttac_branch_draw;
  return c2;
}

static TTacCell ttac_branch_player_center_adj_corner(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  const TTacCell c1 = game_ptr->c1;
  const TTacCell c2 = game_ptr->c2;

  if (move != c1) {
    game_ptr->state = TTAC_GAME_AI_WIN;
    return c1;
  }

  game_ptr->branch = ttac_branch_player_center_adj_corner_end;
  game_ptr->c2 = TTAC_OPP_SAME(c2);
  return c2;
}
static TTacCell ttac_branch_player_center_adj_corner_end(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  const TTacCell c1 = game_ptr->c1;
  const TTacCell c2 = game_ptr->c2;

  if (move != c2) {
    game_ptr->state = TTAC_GAME_AI_WIN;
    return c2;
  }

  game_ptr->branch = ttac_branch_draw;
  return TTAC_MIDDLE(c1, c2);
}

static TTacCell ttac_branch_player_center_adj_edge(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  const TTacCell c1 = game_ptr->c1;
  const TTacCell c2 = game_ptr->c2;

  if (move != c2) {
    game_ptr->state = TTAC_GAME_AI_WIN;
    return c2;
  }

  const TTacCell result = TTAC_MIDDLE(c1, c2);
  game_ptr->c1 = TTAC_OPP_SAME(c1);
  game_ptr->c2 = TTAC_OPP_SAME(result);
  game_ptr->branch = ttac_branch_player_center_adj_edge_end;
  return result;
}
static TTacCell ttac_branch_player_center_adj_edge_end(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  const TTacCell c1 = game_ptr->c1;
  const TTacCell c2 = game_ptr->c2;

  if (move != c2) {
    game_ptr->state = TTAC_GAME_AI_WIN;
    return c2;
  }

  game_ptr->branch = ttac_branch_draw;
  return c1;
}

static TTacCell ttac_branch_player_center_opp_edge(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  const TTacCell c1 = game_ptr->c1;
  const TTacCell c2 = game_ptr->c2;

  if (move != c2) {
    game_ptr->state = TTAC_GAME_AI_WIN;
    return c2;
  }

  game_ptr->branch = ttac_branch_player_center_opp_edge_end;
  game_ptr->c1 = TTAC_LINE_ADJ(c1, c2); /////////////////////////////: TODO RENAME THIS
  game_ptr->c2 = TTAC_LINE_OPP(c2, c1);
  return c1;
}
static TTacCell ttac_branch_player_center_opp_edge_end(void *game, TTacCell move) {
  TTacGame *game_ptr = (TTacGame *)game;
  const TTacCell c1 = game_ptr->c1;
  const TTacCell c2 = game_ptr->c2;

  game_ptr->branch = ttac_branch_draw;
  return (move == c1) ? c2 : c1;
}

#endif // TTAC_IMPLEMNTATION
