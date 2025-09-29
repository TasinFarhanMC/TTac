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

#define TTAC_CENTER 255
#define TTAC_GAME_DRAW 254
#define TTAC_GAME_WIN 253
#define TTAC_GAME_LOSE 252

#define TTAC_OPP_SAME(x) (x ^ 0b0110)
#define TTAC_ADJ1_SAME(x) (x ^ 0b0011)
#define TTAC_ADJ2_SAME(x) (x ^ 0b0101)

#define TTAC_OPP1_DIFF(x) (x ^ 0b1001)
#define TTAC_OPP2_DIFF(x) (x ^ (0b1000 | 0b0010 << (x ^ 0b0001)))

#define TTAC_ADJ1_DIFF(x) (x ^ 0b1111)
#define TTAC_ADJ2_DIFF(x) (x ^ (0b1000 | 0b0010 << (x & 0b0001)))

#define TTAC_MIDDLE(a, b) (TTAC_IS_ADJ_DIFF(TTAC_ADJ1_DIFF(a), b) ? TTAC_ADJ1_DIFF(a) : TTAC_ADJ1_DIFF(b))

#define TTAC_IS_CORNER(x) (x & 0b1000)
#define TTAC_IS_EDGE(x) !(x & 0b1000)

#define TTAC_IS_OPP_SAME(a, b) !((a ^ b) & 0b0001)
#define TTAC_IS_ADJ_SAME(a, b) ((a ^ b) & 0b0001)

#define TTAC_IS_OPP_DIFF(a, b) ((a ^ b) ^ (0b1000 | 0b0010 << (a ^ 0b0001)))
#define TTAC_IS_ADJ_DIFF(a, b) ((a ^ b) ^ (0b1000 | 0b0010 << (a & 0b0001)))

extern void ttac_create_game(TTacGame *game, TTacBool starting_player);
extern TTacCell ttac_play(TTacGame *game, TTacCell move);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
