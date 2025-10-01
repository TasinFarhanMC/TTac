#include <test.hpp>

inline void REQUIRE_GEN_MIDDLE(TTacCell corner1, TTacCell corner2, TTacCell expect) {
  TTacCell result = MIDDLE(corner1, corner2);

  INFO("corner 1: " << cell_name(corner1) << " [" << std::bitset<4>(corner1) << "]");
  INFO("corner 2: " << cell_name(corner2) << " [" << std::bitset<4>(corner2) << "]");
  INFO("result: " << cell_name(result) << " [" << std::bitset<4>(result) << "]");
  INFO("expected: " << cell_name(expect) << " [" << std::bitset<4>(result) << "]");

  REQUIRE(MIDDLE(corner1, corner2) == expect);
}

TEST_CASE("MIDDLE", "[GENERATE][MIDDLE]") {
  REQUIRE_GEN_MIDDLE(TTAC_TOP_LEFT, TTAC_TOP_RIGHT, TTAC_TOP);
  REQUIRE_GEN_MIDDLE(TTAC_TOP_RIGHT, TTAC_TOP_LEFT, TTAC_TOP);

  // Bottom edge
  REQUIRE_GEN_MIDDLE(TTAC_BOTTOM_LEFT, TTAC_BOTTOM_RIGHT, TTAC_BOTTOM);
  REQUIRE_GEN_MIDDLE(TTAC_BOTTOM_RIGHT, TTAC_BOTTOM_LEFT, TTAC_BOTTOM);

  // Left edge
  REQUIRE_GEN_MIDDLE(TTAC_TOP_LEFT, TTAC_BOTTOM_LEFT, TTAC_LEFT);
  REQUIRE_GEN_MIDDLE(TTAC_BOTTOM_LEFT, TTAC_TOP_LEFT, TTAC_LEFT);

  // Right edge
  REQUIRE_GEN_MIDDLE(TTAC_TOP_RIGHT, TTAC_BOTTOM_RIGHT, TTAC_RIGHT);
  REQUIRE_GEN_MIDDLE(TTAC_BOTTOM_RIGHT, TTAC_TOP_RIGHT, TTAC_RIGHT);
}
