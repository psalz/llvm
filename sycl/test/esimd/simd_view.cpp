// RUN: %clangxx -fsycl -fsycl-device-only -fsyntax-only -Xclang -verify %s
// expected-no-diagnostics

#include <CL/sycl/INTEL/esimd.hpp>
#include <limits>
#include <utility>

using namespace sycl::INTEL::gpu;

bool test_simd_view_ctors() __attribute__((sycl_device)) {
  simd<int, 16> v0(0, 1);

  region1d_t<int, 4, 1> r0(4);
  simd_view<simd<int, 16>, region1d_t<int, 4, 1>> ref0(v0, r0);
  simd_view<simd<int, 16>, region1d_t<int, 4, 1>> ref1(std::move(ref0));
  simd_view<simd<int, 16>, region1d_t<int, 4, 1>> ref2(
      std::move(v0.select<4, 1>(8)));

  return (ref0[0] == 4) && (ref1[1] == 5) && (ref2[0] == 8);
}

bool test_simd_view_bin_ops() __attribute__((sycl_device)) {
  simd<int, 16> v0 = 1;
  simd<int, 16> v1 = 2;
  auto ref0 = v0.select<8, 2>(0);
  auto ref1 = v1.select<8, 2>(0);
  ref0 += ref1;
  ref0 += 2;
  ref0 %= ref1;
  ref0 -= ref1;
  ref0 -= 2;
  ref0 *= ref1;
  ref0 *= 2;
  ref0 /= ref1;
  ref0 /= 2;
  return v0[0] == 1;
}

bool test_simd_view_unary_ops() __attribute__((sycl_device)) {
  simd<int, 16> v0 = 1;
  simd<int, 16> v1 = 2;
  auto ref0 = v0.select<8, 2>(0);
  auto ref1 = v1.select<8, 2>(0);
  ref0 <<= ref1;
  ref1 = -ref0;
  ref0 = ~ref1;
  ref1 = !ref0;
  return v1[0] == 1;
}

bool test_simd_view_assign1() __attribute__((sycl_device)) {
  simd<int, 32> v0(0, 1);
  simd<int, 16> v1(0, 1);
  v0.select<8, 1>(0) = v1.select<8, 1>(0) + v1.select<8, 1>(1);
  return v0[8] == 8 + 9;
}

bool test_simd_view_assign2() __attribute__((sycl_device)) {
  simd<int, 32> v0 = 0;
  simd<int, 16> v1 = 1;
  v0.select<8, 1>(0) = v1.select<8, 1>(0);
  return v0[0] == 1;
}

bool test_simd_view_assign3() __attribute__((sycl_device)) {
  simd<int, 64> v0 = 0;
  simd<int, 64> v1 = 1;
  auto mask = (v0.select<16, 1>(0) > v1.select<16, 1>(0));
  auto mask2 = (v0 > v1);
  simd<ushort, 64> s = 0;
  auto g4 = s.format<ushort, 4, 16>();
  simd<ushort, 16> val = (g4.row(2) & mask);
  simd<ushort, 16> val1 = (g4.row(2) & mask2.format<ushort, 4, 16>().row(0));
  return val[0] == 0 && val1[0] == 0;
}
