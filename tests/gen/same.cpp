#include <test.hpp>

inline void REQUIRE_GEN_OPP_SAME(TTacCell x, TTacCell expected) {
  TTacCell result = OPP_SAME(x);

  INFO("x: " << cell_name(x) << " [" << std::bitset<4>(x) << "]");
  INFO("expected: " << cell_name(expected) << " [" << std::bitset<4>(expected) << "]");
  INFO("result: " << cell_name(result) << " [" << std::bitset<4>(result) << "]");

  REQUIRE(OPP_SAME(x) == expected);
}

TEST_CASE("GENERATE OPPOSITE SAME", "[GENERATE][OPPOSITE][SAME]") {
  // Corners
  REQUIRE_GEN_OPP_SAME(TTAC_TOP_LEFT, TTAC_BOTTOM_RIGHT);
  REQUIRE_GEN_OPP_SAME(TTAC_TOP_RIGHT, TTAC_BOTTOM_LEFT);
  REQUIRE_GEN_OPP_SAME(TTAC_BOTTOM_RIGHT, TTAC_TOP_LEFT);
  REQUIRE_GEN_OPP_SAME(TTAC_BOTTOM_LEFT, TTAC_TOP_RIGHT);

  // Edges
  REQUIRE_GEN_OPP_SAME(TTAC_TOP, TTAC_BOTTOM);
  REQUIRE_GEN_OPP_SAME(TTAC_BOTTOM, TTAC_TOP);
  REQUIRE_GEN_OPP_SAME(TTAC_LEFT, TTAC_RIGHT);
  REQUIRE_GEN_OPP_SAME(TTAC_RIGHT, TTAC_LEFT);
}

inline void REQUIRE_GEN_ADJ_SAME(TTacCell x, TTacCell expect1, TTacCell expect2) {
  TTacCell r1 = ADJ1_SAME(x);
  TTacCell r2 = ADJ2_SAME(x);
  if (r1 != expect1) { std::swap(expect1, expect2); }

  INFO("x: " << cell_name(x) << " [" << std::bitset<4>(x) << "]");
  INFO("r1: " << cell_name(r1) << " [" << std::bitset<4>(r1) << "]");
  INFO("r2: " << cell_name(r2) << " [" << std::bitset<4>(r2) << "]");
  INFO(
      "expected: [" << cell_name(expect1) << ", " << cell_name(expect2) << "], [" << std::bitset<4>(expect1) << ", " << std::bitset<4>(expect2) << "]"
  );

  REQUIRE(ADJ1_SAME(x) != ADJ2_SAME(x));
  REQUIRE(ADJ1_SAME(x) == expect1);
  REQUIRE(ADJ2_SAME(x) == expect2);
}

TEST_CASE("GENERATE ADJACENT SAME", "[GENERATE][ADJACENT][SAME]") {
  // Corners (adjacent corners)
  REQUIRE_GEN_ADJ_SAME(TTAC_TOP_LEFT, TTAC_TOP_RIGHT, TTAC_BOTTOM_LEFT);
  REQUIRE_GEN_ADJ_SAME(TTAC_TOP_RIGHT, TTAC_TOP_LEFT, TTAC_BOTTOM_RIGHT);
  REQUIRE_GEN_ADJ_SAME(TTAC_BOTTOM_RIGHT, TTAC_BOTTOM_LEFT, TTAC_TOP_RIGHT);
  REQUIRE_GEN_ADJ_SAME(TTAC_BOTTOM_LEFT, TTAC_BOTTOM_RIGHT, TTAC_TOP_LEFT);

  // Edges (adjacent edges)
  REQUIRE_GEN_ADJ_SAME(TTAC_TOP, TTAC_LEFT, TTAC_RIGHT);
  REQUIRE_GEN_ADJ_SAME(TTAC_BOTTOM, TTAC_LEFT, TTAC_RIGHT);
  REQUIRE_GEN_ADJ_SAME(TTAC_LEFT, TTAC_TOP, TTAC_BOTTOM);
  REQUIRE_GEN_ADJ_SAME(TTAC_RIGHT, TTAC_TOP, TTAC_BOTTOM);
}
