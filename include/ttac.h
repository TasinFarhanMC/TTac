#ifndef TTAC_H
#define TTAC_H

typedef unsigned char TTacCell;
typedef TTacCell (*TTacBranch)(TTacCell control, TTacCell move);

typedef struct {
  TTacCell control;
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

#define TTAC_OPP_SAME(x) (x ^ 0b0110)
#define TTAC_ADJ1_SAME(x) (x ^ 0b0011)
#define TTAC_ADJ2_SAME(x) (x ^ 0b0101)

#define TTAC_OPP1_DIFF(x) (x ^ 0b1001)
#define TTAC_OPP2_DIFF(x) ((x ^ 0b1000) ^ (0b0010 >> (x & 0b0001)))

#define TTAC_ADJ1_DIFF(x) (x ^ 0b1111)
#define TTAC_ADJ2_DIFF(x) ((x ^ 0b1000) ^ (0b0010 << (x & 0b0001)))

extern TTacCell ttac_start_branch(TTacCell control, TTacCell move);
#define ttac_starting_game() {TTAC_TOP_LEFT, ttac_start_branch}

#endif
