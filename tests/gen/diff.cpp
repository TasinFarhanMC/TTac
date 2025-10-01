#include <test.hpp>

inline void REQUIRE_GEN_OPP_DIFF(TTacCell x, TTacCell expect1, TTacCell expect2) {
  TTacCell r1 = OPP1_DIFF(x);
  TTacCell r2 = OPP2_DIFF(x);

  if (r1 != expect1) { std::swap(expect1, expect2); }

  INFO("x: " << cell_name(x) << " [" << std::bitset<4>(x) << "]");
  INFO("r1: " << cell_name(r1) << " [" << std::bitset<4>(r1) << "]");
  INFO("r2: " << cell_name(r2) << " [" << std::bitset<4>(r2) << "]");
  INFO(
      "expected: [" << cell_name(expect1) << ", " << cell_name(expect2) << "], [" << std::bitset<4>(expect1) << ", " << std::bitset<4>(expect2) << "]"
  );

  REQUIRE(OPP1_DIFF(x) != OPP2_DIFF(x));
  REQUIRE(OPP1_DIFF(x) == expect1);
  REQUIRE(OPP2_DIFF(x) == expect2);
}

TEST_CASE("GENERATE OPPOSITE DIFFERENT", "[GENERATE][OPPOSITE][DIFFERENT]") {
  // Corners → opposite edges
  REQUIRE_GEN_OPP_DIFF(TTAC_TOP_LEFT, TTAC_BOTTOM, TTAC_RIGHT);
  REQUIRE_GEN_OPP_DIFF(TTAC_TOP_RIGHT, TTAC_BOTTOM, TTAC_LEFT);
  REQUIRE_GEN_OPP_DIFF(TTAC_BOTTOM_RIGHT, TTAC_TOP, TTAC_LEFT);
  REQUIRE_GEN_OPP_DIFF(TTAC_BOTTOM_LEFT, TTAC_TOP, TTAC_RIGHT);

  // Edges → opposite corners
  REQUIRE_GEN_OPP_DIFF(TTAC_TOP, TTAC_BOTTOM_LEFT, TTAC_BOTTOM_RIGHT);
  REQUIRE_GEN_OPP_DIFF(TTAC_BOTTOM, TTAC_TOP_LEFT, TTAC_TOP_RIGHT);
  REQUIRE_GEN_OPP_DIFF(TTAC_LEFT, TTAC_TOP_RIGHT, TTAC_BOTTOM_RIGHT);
  REQUIRE_GEN_OPP_DIFF(TTAC_RIGHT, TTAC_TOP_LEFT, TTAC_BOTTOM_LEFT);
}

inline void REQUIRE_GEN_ADJ_DIFF(TTacCell x, TTacCell expect1, TTacCell expect2) {
  TTacCell r1 = ADJ1_DIFF(x);
  TTacCell r2 = ADJ2_DIFF(x);
  if (r1 != expect1) { std::swap(expect1, expect2); }

  INFO("x: " << cell_name(x) << " [" << std::bitset<4>(x) << "]");
  INFO("r1: " << cell_name(r1) << " [" << std::bitset<4>(r1) << "]");
  INFO("r2: " << cell_name(r2) << " [" << std::bitset<4>(r2) << "]");
  INFO(
      "expected: [" << cell_name(expect1) << ", " << cell_name(expect2) << "], [" << std::bitset<4>(expect1) << ", " << std::bitset<4>(expect2) << "]"
  );

  REQUIRE(ADJ1_DIFF(x) != ADJ2_DIFF(x));
  REQUIRE(ADJ1_DIFF(x) == expect1);
  REQUIRE(ADJ2_DIFF(x) == expect2);
}

TEST_CASE("GENERATE ADJACENT DIFFERENT", "[GENERATE][ADJACENT][DIFFERENT]") {
  // Corners → expect adjacent edges
  REQUIRE_GEN_ADJ_DIFF(TTAC_TOP_LEFT, TTAC_TOP, TTAC_LEFT);
  REQUIRE_GEN_ADJ_DIFF(TTAC_TOP_RIGHT, TTAC_TOP, TTAC_RIGHT);
  REQUIRE_GEN_ADJ_DIFF(TTAC_BOTTOM_RIGHT, TTAC_BOTTOM, TTAC_RIGHT);
  REQUIRE_GEN_ADJ_DIFF(TTAC_BOTTOM_LEFT, TTAC_BOTTOM, TTAC_LEFT);

  // Edges → expect adjacent corners
  REQUIRE_GEN_ADJ_DIFF(TTAC_TOP, TTAC_TOP_LEFT, TTAC_TOP_RIGHT);
  REQUIRE_GEN_ADJ_DIFF(TTAC_BOTTOM, TTAC_BOTTOM_LEFT, TTAC_BOTTOM_RIGHT);
  REQUIRE_GEN_ADJ_DIFF(TTAC_LEFT, TTAC_TOP_LEFT, TTAC_BOTTOM_LEFT);
  REQUIRE_GEN_ADJ_DIFF(TTAC_RIGHT, TTAC_TOP_RIGHT, TTAC_BOTTOM_RIGHT);
}
