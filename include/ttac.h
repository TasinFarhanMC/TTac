#ifndef TTAC_H
#define TTAC_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef unsigned char TTacCell;
typedef char TTacBool;
typedef TTacCell (*TTacBranch)(void *game, TTacCell move);

typedef struct {
  TTacCell control1;
  TTacCell control2;
  TTacBool state;
  TTacBranch branch;
} TTacGame;

#define TTAC_TOP_LEFT 0b1010
#define TTAC_TOP_RIGHT 0b1001
#define TTAC_BOTTOM_RIGHT 0b1100
#define TTAC_BOTTOM_LEFT 0b1111

#define TTAC_TOP 0b0101
#define TTAC_BOTTOM 0b0011
#define TTAC_LEFT 0b0000
#define TTAC_RIGHT 0b0110

#define TTAC_CENTER 0b0001

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
#define TTAC_OPP2_DIFF(x) (x ^ (0b1000 | 0b0010 << (x ^ 0b0001)))

#define TTAC_ADJ1_DIFF(x) (x ^ 0b1111)
#define TTAC_ADJ2_DIFF(x) (x ^ (0b1000 | 0b0010 << (x & 0b0001)))

#define TTAC_IS_CORNER(x) (x & 0b1000)
#define TTAC_IS_EDGE(x) !(x & 0b1000)

#define TTAC_IS_OPP_SAME(a, b) !((a ^ b) & 0b0001)
#define TTAC_IS_ADJ_SAME(a, b) ((a ^ b) & 0b0001)

#define TTAC_IS_OPP_DIFF(a, b) !((a ^ b) & (0b0010 << (a & 0b0001)))
#define TTAC_IS_ADJ_DIFF(a, b) ((a ^ b) & (0b0010 << (a & 0b0001)))

#define TTAC_MIDDLE(a, b) (TTAC_IS_ADJ_DIFF(TTAC_ADJ1_DIFF(a), b) ? TTAC_ADJ1_DIFF(a) : TTAC_ADJ1_DIFF(b))
#define TTAC_LINE(corner, edge) (TTAC_IS_ADJ_DIFF(TTAC_ADJ1_SAME(corner), edge) ? TTAC_ADJ1_SAME(corner) : TTAC_ADJ2_SAME(corner))
#define TTAC_LINE_ORTHO(corner, edge) (TTAC_IS_ADJ_DIFF(TTAC_ADJ1_SAME(corner), edge) ? TTAC_ADJ2_SAME(corner) : TTAC_ADJ1_SAME(corner))

extern void ttac_create_game(TTacGame *game, TTacBool ai_start);
#define ttac_play(game, move) game.branch(&game, move)
#define ttac_game_state(game) game.state

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
