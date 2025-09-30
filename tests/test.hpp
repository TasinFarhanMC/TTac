#define CATCH_CONFIG_MAIN

#include "ttac.h"
#include <catch2/catch_all.hpp>
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#define GEN_CHECK(name)                                                                                                                                \
  inline bool name(TTacCell a, TTacCell b) { return TTAC_##name(a, b); }

#define GEN_MONO(name)                                                                                                                                 \
  inline bool name(TTacCell x) { return TTAC_##name(x); }

#define GEN_REQUIRE(name)                                                                                                                              \
  inline void REQUIRE_##name(TTacCell a, TTacCell b) {                                                                                                 \
    INFO("a: " << cell_name(a));                                                                                                                       \
    INFO("b: " << cell_name(b));                                                                                                                       \
    REQUIRE(IS_##name(a, b));                                                                                                                          \
  }                                                                                                                                                    \
  inline void REQUIRE_##name##_FALSE(TTacCell a, TTacCell b) {                                                                                         \
    INFO("a: " << cell_name(a));                                                                                                                       \
    INFO("b: " << cell_name(b));                                                                                                                       \
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

GEN_CHECK(IS_ADJ_SAME);
GEN_CHECK(IS_OPP_SAME);
GEN_CHECK(IS_ADJ_DIFF);
GEN_CHECK(IS_OPP_DIFF);

GEN_MONO(IS_CORNER);
GEN_MONO(IS_EDGE);

GEN_MONO(OPP_SAME);
GEN_MONO(OPP1_DIFF);
GEN_MONO(OPP2_DIFF);

GEN_MONO(ADJ1_SAME);
GEN_MONO(ADJ2_SAME);
GEN_MONO(ADJ1_DIFF);
GEN_MONO(ADJ2_DIFF);

inline void REQUIRE_CORNER(TTacCell x) {
  INFO("x: " << cell_name(x));
  REQUIRE(IS_CORNER(x));
}

inline void REQUIRE_CORNER_FALSE(TTacCell x) {
  INFO("x: " << cell_name(x));
  REQUIRE_FALSE(IS_CORNER(x));
}

inline void REQUIRE_EDGE(TTacCell x) {
  INFO("x: " << cell_name(x));
  REQUIRE(IS_EDGE(x));
}

inline void REQUIRE_EDGE_FALSE(TTacCell x) {
  INFO("x: " << cell_name(x));
  REQUIRE_FALSE(IS_EDGE(x));
}

GEN_REQUIRE(ADJ_SAME);
GEN_REQUIRE(OPP_SAME);
GEN_REQUIRE(ADJ_DIFF);
GEN_REQUIRE(OPP_DIFF);
