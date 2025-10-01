
#include <test.hpp>

inline void REQUIRE_GEN_EDGE_CORNER(TTacCell edge1, TTacCell edge2, TTacCell expect) {
  TTacCell result = EDGE_CORNER(edge1, edge2);

  INFO("edge 1: " << cell_name(edge1) << " [" << std::bitset<4>(edge1) << "]");
  INFO("edge 2: " << cell_name(edge2) << " [" << std::bitset<4>(edge2) << "]");
  INFO("result: " << cell_name(result) << " [" << std::bitset<4>(result) << "]");
  INFO("expected: " << cell_name(expect) << " [" << std::bitset<4>(result) << "]");

  REQUIRE(EDGE_CORNER(edge1, edge2) == expect);
}

TEST_CASE("EDGE CORNER", "[EDGE CORNER]") {
  // Top + Left = Top Left
  REQUIRE_GEN_EDGE_CORNER(TTAC_TOP, TTAC_LEFT, TTAC_TOP_LEFT);
  REQUIRE_GEN_EDGE_CORNER(TTAC_LEFT, TTAC_TOP, TTAC_TOP_LEFT);

  // Top + Right = Top Right
  REQUIRE_GEN_EDGE_CORNER(TTAC_TOP, TTAC_RIGHT, TTAC_TOP_RIGHT);
  REQUIRE_GEN_EDGE_CORNER(TTAC_RIGHT, TTAC_TOP, TTAC_TOP_RIGHT);

  // Bottom + Left = Bottom Left
  REQUIRE_GEN_EDGE_CORNER(TTAC_BOTTOM, TTAC_LEFT, TTAC_BOTTOM_LEFT);
  REQUIRE_GEN_EDGE_CORNER(TTAC_LEFT, TTAC_BOTTOM, TTAC_BOTTOM_LEFT);

  // Bottom + Right = Bottom Right
  REQUIRE_GEN_EDGE_CORNER(TTAC_BOTTOM, TTAC_RIGHT, TTAC_BOTTOM_RIGHT);
  REQUIRE_GEN_EDGE_CORNER(TTAC_RIGHT, TTAC_BOTTOM, TTAC_BOTTOM_RIGHT);
}
