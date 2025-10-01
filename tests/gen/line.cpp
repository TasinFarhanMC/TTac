#include <test.hpp>

inline void REQUIRE_GEN_LINE(TTacCell corner, TTacCell edge, TTacCell expect) {
  TTacCell result = LINE(corner, edge);

  INFO("corner: " << cell_name(corner) << " [" << std::bitset<4>(corner) << "]");
  INFO("edge: " << cell_name(edge) << " [" << std::bitset<4>(edge) << "]");
  INFO("result: " << cell_name(result) << " [" << std::bitset<4>(result) << "]");
  INFO("expected: " << cell_name(expect) << " [" << std::bitset<4>(result) << "]");

  REQUIRE(LINE(corner, edge) == expect);
}

TEST_CASE("LINE", "[GENERATE][LINE]") {
  // Top row
  REQUIRE_GEN_LINE(TTAC_TOP_LEFT, TTAC_TOP, TTAC_TOP_RIGHT);
  REQUIRE_GEN_LINE(TTAC_TOP_RIGHT, TTAC_TOP, TTAC_TOP_LEFT);

  // Bottom row
  REQUIRE_GEN_LINE(TTAC_BOTTOM_LEFT, TTAC_BOTTOM, TTAC_BOTTOM_RIGHT);
  REQUIRE_GEN_LINE(TTAC_BOTTOM_RIGHT, TTAC_BOTTOM, TTAC_BOTTOM_LEFT);

  // Left column
  REQUIRE_GEN_LINE(TTAC_TOP_LEFT, TTAC_LEFT, TTAC_BOTTOM_LEFT);
  REQUIRE_GEN_LINE(TTAC_BOTTOM_LEFT, TTAC_LEFT, TTAC_TOP_LEFT);

  // Right column
  REQUIRE_GEN_LINE(TTAC_TOP_RIGHT, TTAC_RIGHT, TTAC_BOTTOM_RIGHT);
  REQUIRE_GEN_LINE(TTAC_BOTTOM_RIGHT, TTAC_RIGHT, TTAC_TOP_RIGHT);
}

inline void REQUIRE_GEN_LINE_OPP(TTacCell corner, TTacCell edge, TTacCell expect) {
  TTacCell result = LINE_OPP(corner, edge);

  INFO("corner: " << cell_name(corner) << " [" << std::bitset<4>(corner) << "]");
  INFO("edge: " << cell_name(edge) << " [" << std::bitset<4>(edge) << "]");
  INFO("result: " << cell_name(result) << " [" << std::bitset<4>(result) << "]");
  INFO("expected: " << cell_name(expect) << " [" << std::bitset<4>(result) << "]");

  REQUIRE(LINE_OPP(corner, edge) == expect);
}

TEST_CASE("LINE OPP", "[GENERATE][LINE OPP]") {
  // TOP_LEFT
  REQUIRE_GEN_LINE_OPP(TTAC_TOP_LEFT, TTAC_TOP, TTAC_BOTTOM_LEFT); // vertical opposite
  REQUIRE_GEN_LINE_OPP(TTAC_TOP_LEFT, TTAC_LEFT, TTAC_TOP_RIGHT);  // horizontal opposite

  // TOP_RIGHT
  REQUIRE_GEN_LINE_OPP(TTAC_TOP_RIGHT, TTAC_TOP, TTAC_BOTTOM_RIGHT);
  REQUIRE_GEN_LINE_OPP(TTAC_TOP_RIGHT, TTAC_RIGHT, TTAC_TOP_LEFT);

  // BOTTOM_LEFT
  REQUIRE_GEN_LINE_OPP(TTAC_BOTTOM_LEFT, TTAC_BOTTOM, TTAC_TOP_LEFT);
  REQUIRE_GEN_LINE_OPP(TTAC_BOTTOM_LEFT, TTAC_LEFT, TTAC_BOTTOM_RIGHT);

  // BOTTOM_RIGHT
  REQUIRE_GEN_LINE_OPP(TTAC_BOTTOM_RIGHT, TTAC_BOTTOM, TTAC_TOP_RIGHT);
  REQUIRE_GEN_LINE_OPP(TTAC_BOTTOM_RIGHT, TTAC_RIGHT, TTAC_BOTTOM_LEFT);
}
