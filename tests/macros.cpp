#define CATCH_CONFIG_MAIN

#include "ttac.h"
#include <catch2/catch_all.hpp>
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

#define GEN_CHECK(name)                                                                                                                                \
  inline bool name(TTacCell a, TTacCell b) { return TTAC_##name(a, b); }

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

inline bool IS_CORNER(TTacCell x) { return TTAC_IS_CORNER(x); }
inline bool IS_EDGE(TTacCell x) { return TTAC_IS_EDGE(x); }

GEN_CHECK(IS_ADJ_SAME);
GEN_CHECK(IS_OPP_SAME);

// ------------------------------
// Helper macros for testing
// ------------------------------
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

// ------------------------------
// Test Cases
// ------------------------------

TEST_CASE("TTac adjacency and opposite macros for same type", "[ttac]") {
  SECTION("Adjacent edges") {
    REQUIRE_ADJ_SAME(TTAC_TOP, TTAC_LEFT);
    REQUIRE_ADJ_SAME(TTAC_TOP, TTAC_RIGHT);

    REQUIRE_ADJ_SAME(TTAC_BOTTOM, TTAC_LEFT);
    REQUIRE_ADJ_SAME(TTAC_BOTTOM, TTAC_RIGHT);

    REQUIRE_ADJ_SAME(TTAC_LEFT, TTAC_TOP);
    REQUIRE_ADJ_SAME(TTAC_RIGHT, TTAC_TOP);

    REQUIRE_ADJ_SAME(TTAC_LEFT, TTAC_BOTTOM);
    REQUIRE_ADJ_SAME(TTAC_RIGHT, TTAC_BOTTOM);

    REQUIRE_ADJ_SAME_FALSE(TTAC_TOP, TTAC_BOTTOM);
    REQUIRE_ADJ_SAME_FALSE(TTAC_BOTTOM, TTAC_TOP);

    REQUIRE_ADJ_SAME_FALSE(TTAC_LEFT, TTAC_RIGHT);
    REQUIRE_ADJ_SAME_FALSE(TTAC_RIGHT, TTAC_LEFT);
  }

  SECTION("Opposite edges") {
    REQUIRE_OPP_SAME(TTAC_TOP, TTAC_BOTTOM);
    REQUIRE_OPP_SAME(TTAC_LEFT, TTAC_RIGHT);

    REQUIRE_OPP_SAME(TTAC_BOTTOM, TTAC_TOP);
    REQUIRE_OPP_SAME(TTAC_RIGHT, TTAC_LEFT);

    REQUIRE_OPP_SAME_FALSE(TTAC_TOP, TTAC_LEFT);
    REQUIRE_OPP_SAME_FALSE(TTAC_LEFT, TTAC_TOP);

    REQUIRE_OPP_SAME_FALSE(TTAC_TOP, TTAC_RIGHT);
    REQUIRE_OPP_SAME_FALSE(TTAC_RIGHT, TTAC_TOP);

    REQUIRE_OPP_SAME_FALSE(TTAC_BOTTOM, TTAC_LEFT);
    REQUIRE_OPP_SAME_FALSE(TTAC_LEFT, TTAC_BOTTOM);

    REQUIRE_OPP_SAME_FALSE(TTAC_BOTTOM, TTAC_RIGHT);
    REQUIRE_OPP_SAME_FALSE(TTAC_RIGHT, TTAC_BOTTOM);
  }

  SECTION("Adjacent corners") {
    REQUIRE_ADJ_SAME(TTAC_TOP_LEFT, TTAC_TOP_RIGHT);
    REQUIRE_ADJ_SAME(TTAC_BOTTOM_LEFT, TTAC_BOTTOM_RIGHT);

    REQUIRE_ADJ_SAME(TTAC_TOP_RIGHT, TTAC_TOP_LEFT);
    REQUIRE_ADJ_SAME(TTAC_BOTTOM_RIGHT, TTAC_BOTTOM_LEFT);

    REQUIRE_ADJ_SAME(TTAC_TOP_LEFT, TTAC_BOTTOM_LEFT);
    REQUIRE_ADJ_SAME(TTAC_TOP_RIGHT, TTAC_BOTTOM_RIGHT);

    REQUIRE_ADJ_SAME(TTAC_BOTTOM_LEFT, TTAC_TOP_LEFT);
    REQUIRE_ADJ_SAME(TTAC_BOTTOM_RIGHT, TTAC_TOP_RIGHT);

    REQUIRE_ADJ_SAME_FALSE(TTAC_TOP_LEFT, TTAC_BOTTOM_RIGHT);
    REQUIRE_ADJ_SAME_FALSE(TTAC_BOTTOM_RIGHT, TTAC_TOP_LEFT);

    REQUIRE_ADJ_SAME_FALSE(TTAC_TOP_RIGHT, TTAC_BOTTOM_LEFT);
    REQUIRE_ADJ_SAME_FALSE(TTAC_BOTTOM_LEFT, TTAC_TOP_RIGHT);
  }

  SECTION("Opposite corners") {
    REQUIRE_OPP_SAME(TTAC_TOP_LEFT, TTAC_BOTTOM_RIGHT);
    REQUIRE_OPP_SAME(TTAC_TOP_RIGHT, TTAC_BOTTOM_LEFT);

    REQUIRE_OPP_SAME(TTAC_BOTTOM_RIGHT, TTAC_TOP_LEFT);
    REQUIRE_OPP_SAME(TTAC_BOTTOM_LEFT, TTAC_TOP_RIGHT);

    REQUIRE_OPP_SAME_FALSE(TTAC_TOP_LEFT, TTAC_TOP_RIGHT);
    REQUIRE_OPP_SAME_FALSE(TTAC_TOP_RIGHT, TTAC_TOP_LEFT);

    REQUIRE_OPP_SAME_FALSE(TTAC_TOP_LEFT, TTAC_BOTTOM_LEFT);
    REQUIRE_OPP_SAME_FALSE(TTAC_BOTTOM_LEFT, TTAC_TOP_LEFT);

    REQUIRE_OPP_SAME_FALSE(TTAC_TOP_RIGHT, TTAC_BOTTOM_RIGHT);
    REQUIRE_OPP_SAME_FALSE(TTAC_BOTTOM_RIGHT, TTAC_TOP_RIGHT);

    REQUIRE_OPP_SAME_FALSE(TTAC_BOTTOM_LEFT, TTAC_BOTTOM_RIGHT);
    REQUIRE_OPP_SAME_FALSE(TTAC_BOTTOM_RIGHT, TTAC_BOTTOM_LEFT);
  }
}

TEST_CASE("TTac edge and corner check", "[ttac]") {
  SECTION("Edges") {
    REQUIRE_EDGE(TTAC_TOP);
    REQUIRE_EDGE(TTAC_BOTTOM);
    REQUIRE_EDGE(TTAC_LEFT);
    REQUIRE_EDGE(TTAC_RIGHT);

    REQUIRE_EDGE_FALSE(TTAC_TOP_LEFT);
    REQUIRE_EDGE_FALSE(TTAC_TOP_RIGHT);
    REQUIRE_EDGE_FALSE(TTAC_BOTTOM_LEFT);
    REQUIRE_EDGE_FALSE(TTAC_BOTTOM_RIGHT);
  }

  SECTION("Corners") {
    REQUIRE_CORNER(TTAC_TOP_LEFT);
    REQUIRE_CORNER(TTAC_TOP_RIGHT);
    REQUIRE_CORNER(TTAC_BOTTOM_LEFT);
    REQUIRE_CORNER(TTAC_BOTTOM_RIGHT);

    REQUIRE_CORNER_FALSE(TTAC_TOP);
    REQUIRE_CORNER_FALSE(TTAC_BOTTOM);
    REQUIRE_CORNER_FALSE(TTAC_LEFT);
    REQUIRE_CORNER_FALSE(TTAC_RIGHT);
  }
}
