/*------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Mathias Soeken
*-----------------------------------------------------------------------------*/
#include <catch.hpp>
#include <cstdint>
#include <tweedledum/algorithms/synthesis/transformation_based.hpp>
#include <tweedledum/gates/mcmt_gate.hpp>
#include <tweedledum/networks/netlist.hpp>
#include <vector>

using namespace tweedledum;

TEST_CASE("Synthesize PRIME(3) with unidirectional transformation based synthesis",
          "[transformation_based]")
{
	std::vector<uint16_t> permutation{{0, 2, 3, 5, 7, 1, 4, 6}};
	const auto circ = transformation_based_synthesis<netlist<mcmt_gate>>(permutation);

	for (auto i = 0; i < 8; ++i) {
		CHECK(i == permutation[i]);
	}

	CHECK(circ.num_gates() == 4u);
	CHECK(circ.num_qubits() == 3u);
}

TEST_CASE("Synthesize PRIME(3) with bidirectional transformation based synthesis",
          "[transformation_based]")
{
	std::vector<uint16_t> permutation{{0, 2, 3, 5, 7, 1, 4, 6}};
	const auto circ = transformation_based_synthesis_bidirectional<netlist<mcmt_gate>>(permutation);

	for (auto i = 0; i < 8; ++i) {
		CHECK(i == permutation[i]);
	}

	CHECK(circ.num_gates() == 4u);
	CHECK(circ.num_qubits() == 3u);
}

TEST_CASE("Synthesize PRIME(3) with multi-directional transformation based synthesis",
          "[transformation_based]")
{
	std::vector<uint16_t> permutation{{0, 2, 3, 5, 7, 1, 4, 6}};
	const auto circ = transformation_based_synthesis_multidirectional<netlist<mcmt_gate>>(permutation);

	for (auto i = 0; i < 8; ++i) {
		CHECK(i == permutation[i]);
	}

	CHECK(circ.num_gates() == 4u);
	CHECK(circ.num_qubits() == 3u);
}
