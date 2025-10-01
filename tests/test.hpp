#define CATCH_CONFIG_MAIN

#include "ttac.h"
#include <bitset>
#include <catch2/catch_all.hpp>
#include <string>

#define GEN_FUNC(name)                                                                                                                                 \
  inline TTacCell name(TTacCell x) { return TTAC_##name(x); }

#define GEN_DUAL_FUNC(name)                                                                                                                            \
  inline TTacCell name(TTacCell a, TTacCell b) { return TTAC_##name(a, b); }

#define GEN_CHECK(name)                                                                                                                                \
  inline bool IS_##name(TTacCell x) { return TTAC_IS_##name(x); }                                                                                      \
  inline void REQUIRE_##name(TTacCell x) {                                                                                                             \
    INFO("x: " << cell_name(x) << " [" << std::bitset<4>(x) << "]");                                                                                   \
    REQUIRE(IS_##name(x));                                                                                                                             \
  }                                                                                                                                                    \
  inline void REQUIRE_##name##_FALSE(TTacCell x) {                                                                                                     \
    INFO("x: " << cell_name(x) << " [" << std::bitset<4>(x) << "]");                                                                                   \
    REQUIRE_FALSE(IS_##name(x));                                                                                                                       \
  }

#define GEN_REQUIRE(name)                                                                                                                              \
  inline bool IS_##name(TTacCell a, TTacCell b) { return TTAC_IS_##name(a, b); }                                                                       \
  inline void REQUIRE_##name(TTacCell a, TTacCell b) {                                                                                                 \
    INFO("a: " << cell_name(a) << " [" << std::bitset<4>(a) << "]");                                                                                   \
    INFO("b: " << cell_name(b) << " [" << std::bitset<4>(b) << "]");                                                                                   \
    REQUIRE(IS_##name(a, b));                                                                                                                          \
  }                                                                                                                                                    \
  inline void REQUIRE_##name##_FALSE(TTacCell a, TTacCell b) {                                                                                         \
    INFO("a: " << cell_name(a) << " [" << std::bitset<4>(a) << "]");                                                                                   \
    INFO("b: " << cell_name(b) << " [" << std::bitset<4>(b) << "]");                                                                                   \
    REQUIRE_FALSE(IS_##name(a, b));                                                                                                                    \
  }

static std::string cell_name(TTacCell const &cell) {
  switch (cell) {
  case TTAC_TOP:
    return "TOP";
  case TTAC_LEFT:
    return "LEFT";
  case TTAC_RIGHT:
    return "RIGHT";
  case TTAC_BOTTOM:
    return "BOTTOM";
  case TTAC_TOP_LEFT:
    return "TOP LEFT";
  case TTAC_TOP_RIGHT:
    return "TOP RIGHT";
  case TTAC_BOTTOM_RIGHT:
    return "BOTTOM RIGHT";
  case TTAC_BOTTOM_LEFT:
    return "BOTTOM LEFT";
  case TTAC_CENTER:
    return "CENTER";
  default:
    return "UNKNOWN";
  }
}

GEN_REQUIRE(ADJ_SAME);
GEN_REQUIRE(OPP_SAME);

GEN_REQUIRE(ADJ_DIFF);
GEN_REQUIRE(OPP_DIFF);

GEN_REQUIRE(DIFF);
GEN_REQUIRE(SAME);

GEN_CHECK(CENTER);
GEN_CHECK(CORNER);
GEN_CHECK(EDGE);

GEN_FUNC(OPP_SAME);

GEN_FUNC(OPP1_DIFF);
GEN_FUNC(OPP2_DIFF);

GEN_FUNC(ADJ1_SAME);
GEN_FUNC(ADJ2_SAME);

GEN_FUNC(ADJ1_DIFF);
GEN_FUNC(ADJ2_DIFF);

GEN_DUAL_FUNC(LINE);
GEN_DUAL_FUNC(LINE_OPP);
GEN_DUAL_FUNC(MIDDLE);
