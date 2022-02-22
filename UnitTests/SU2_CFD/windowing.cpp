/*!
 * \file windowing.cpp
 * \brief Unit tests for windowed time-averaging.
 * \author C. Bauer
 * \version 7.3.0 "Blackbird"
 *
 * SU2 Project Website: https://su2code.github.io
 *
 * The SU2 Project is maintained by the SU2 Foundation
 * (http://su2foundation.org)
 *
 * Copyright 2012-2022, SU2 Contributors (cf. AUTHORS.md)
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SU2. If not, see <http://www.gnu.org/licenses/>.
 */

#include "catch.hpp"
#include "../../SU2_CFD/include/output/COutput.hpp"
#include "../../SU2_CFD/include/output/tools/CWindowingTools.hpp"

/*!
 * \brief Wrapper class that provides reference values and methods for
 * averaging these over different windows.
 */
struct CWindowingTest {
  static constexpr size_t MAX_INNER_ITERATIONS = 5;

  /*!
  * \brief Calculates sample value at the specified time-step. Simple SIN function.
  */
  static su2double GetSampleAtIteration(unsigned long currentTimeStep) {
    return 0.5 * sin(0.1 * currentTimeStep + 2) + 1.;
  }

  /*!
   * \brief Perform averaging over simulated inner and outer solver iterations.
   */
  static su2double calcAverage(WINDOW_FUNCTION win, unsigned long nIterations, unsigned long startIteration) {
    CWindowedAverage avg{win}; // Create averaging object with the specified window.
    su2double previousSample = 0.;
    su2double currentSample = 0.;
    su2double input = 0.;   // Value that is specified as actual input to the addValue function.
    su2double weightPrevious = 0.;
    su2double weightCurrent = 0.;
    // Simulate solver time-steps
    for (size_t outerIteration = 0; outerIteration < nIterations; outerIteration++) {
      previousSample = currentSample;
      currentSample = GetSampleAtIteration(outerIteration);
      // Simulate inner time-steps
      for (size_t innerIteration = 0; innerIteration < MAX_INNER_ITERATIONS; innerIteration++) {
        weightPrevious = static_cast<su2double>(MAX_INNER_ITERATIONS - innerIteration - 1);
        weightCurrent = innerIteration;
        // Simulate gradual change of sample during inner iterations.
        input = (weightPrevious * previousSample + weightCurrent * currentSample) / (weightCurrent + weightPrevious);
        avg.addValue(input, outerIteration, startIteration);
      }
    }
    return avg.GetVal();
  }
};

TEST_CASE("BUMP", "[Windowing]") { 
    su2double avg = 0;
    avg = CWindowingTest::calcAverage(WINDOW_FUNCTION::BUMP, 10, 0);
    REQUIRE(avg > 1.1850);
    REQUIRE(avg < 1.1852);
    avg = CWindowingTest::calcAverage(WINDOW_FUNCTION::BUMP, 100, 10);
    REQUIRE(avg > 1.1882);
    REQUIRE(avg < 1.1884);
}

TEST_CASE("HANN", "[Windowing]") {
  su2double avg = 0;
  avg = CWindowingTest::calcAverage(WINDOW_FUNCTION::HANN, 10, 0);
  REQUIRE(avg > 1.1831);
  REQUIRE(avg < 1.1833);
  avg = CWindowingTest::calcAverage(WINDOW_FUNCTION::HANN, 100, 10);
  REQUIRE(avg > 1.0868);
  REQUIRE(avg < 1.0870);

}

TEST_CASE("HANN_SQUARE", "[Windowing]") {
  su2double avg = 0;
  avg = CWindowingTest::calcAverage(WINDOW_FUNCTION::HANN_SQUARE, 10, 0);
  REQUIRE(avg > 1.1846);
  REQUIRE(avg < 1.1848);
  avg = CWindowingTest::calcAverage(WINDOW_FUNCTION::HANN_SQUARE, 100, 10);
  REQUIRE(avg > 1.1855);
  REQUIRE(avg < 1.1857);

}

TEST_CASE("SQUARE", "[Windowing]") {
  su2double avg = 0;
  avg = CWindowingTest::calcAverage(WINDOW_FUNCTION::SQUARE, 10, 0);
  REQUIRE(avg > 1.3058);
  REQUIRE(avg < 1.3060);
  avg = CWindowingTest::calcAverage(WINDOW_FUNCTION::SQUARE, 100, 10);
  REQUIRE(avg > 0.9000);
  REQUIRE(avg < 0.9002);

}
