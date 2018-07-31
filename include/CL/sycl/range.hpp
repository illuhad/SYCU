/*
 * This file is part of SYCU, a SYCL implementation based CUDA/HIP
 *
 * Copyright (c) 2018 Aksel Alpay
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef SYCU_RANGE_HPP
#define SYCU_RANGE_HPP

#include <type_traits>
#include <array>

namespace cl {
namespace sycl {

template <std::size_t dimensions = 1>
class range {
public:

  /* The following constructor is only available in the range class specialization where:
dimensions==1 */
  template<int D = dimensions,
           typename = std::enable_if_t<D == 1>>
  __host__ __device__
  range(size_t dim0)
    : _data{dim0}
  {}

  /* The following constructor is only available in the range class specialization where:
dimensions==2 */
  template<int D = dimensions,
           typename = std::enable_if_t<D == 2>>
  __host__ __device__
  range(size_t dim0, size_t dim1)
    : _data{dim0, dim1}
  {}

  /* The following constructor is only available in the range class specialization where:
dimensions==3 */
  template<int D = dimensions,
           typename = std::enable_if_t<D == 3>>
  __host__ __device__
  range(size_t dim0, size_t dim1, size_t dim2)
    : _data{dim0, dim1, dim2}
  {}

  /* -- common interface members -- */

  __host__ __device__
  std::size_t get(int dimension) const {
    return _data[dimension];
  }

  __host__ __device__
  std::size_t &operator[](int dimension) {
    return _data[dimension];
  }

  __host__ __device__
  std::size_t size() const {
    std::size_t result = 1;
    for(const auto x : _data)
      result *= x;
    return result;
  }

  // Implementation of id<dimensions> operatorOP(const size_t &rhs) const;
  // OP is: +, -, *, /, %, <<, >>, &, |, ˆ, &&, ||, <, >, <=, >=
#define SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(op) \
  __host__ __device__ \
  range<dimensions> operator op(const range<dimensions> &rhs) const { \
    range<dimensions> result; \
    for(std::size_t i = 0; i < dimensions; ++i) \
      result._data[i] = static_cast<std::size_t>(_data[i] op rhs._data[i]); \
    return result; \
  }

  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(+)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(-)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(*)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(/)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(%)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(<<)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(>>)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(&)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(|)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(^)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(&&)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(||)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(<)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(>)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(<=)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE(>=)

#define SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(op) \
  __host__ __device__ \
  range<dimensions> operator op(const std::size_t &rhs) const { \
    range<dimensions> result; \
    for(std::size_t i = 0; i < dimensions; ++i) \
      result._data[i] = static_cast<std::size_t>(_data[i] op rhs); \
    return result; \
  }

  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(+)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(-)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(*)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(/)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(%)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(<<)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(>>)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(&)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(|)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(^)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(&&)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(||)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(<)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(>)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(<=)
  SYCU_RANGE_BINARY_OP_OUT_OF_PLACE_SIZE_T(>=)


  // Implementation of id<dimensions> &operatorOP(const id<dimensions> &rhs);
  // OP is: +=, -=, *=, /=, %=, <<=, >>=, &=, |=, ˆ=
#define SYCU_RANGE_BINARY_OP_IN_PLACE(op) \
  __host__ __device__ \
  range<dimensions>& operator op(const range<dimensions> &rhs) const { \
    for(std::size_t i = 0; i < dimensions; ++i) \
      _data[i] op rhs._data[i]; \
    return *this; \
  }

  SYCU_RANGE_BINARY_OP_IN_PLACE(+=)
  SYCU_RANGE_BINARY_OP_IN_PLACE(-=)
  SYCU_RANGE_BINARY_OP_IN_PLACE(*=)
  SYCU_RANGE_BINARY_OP_IN_PLACE(/=)
  SYCU_RANGE_BINARY_OP_IN_PLACE(%=)
  SYCU_RANGE_BINARY_OP_IN_PLACE(<<=)
  SYCU_RANGE_BINARY_OP_IN_PLACE(>>=)
  SYCU_RANGE_BINARY_OP_IN_PLACE(&=)
  SYCU_RANGE_BINARY_OP_IN_PLACE(|=)
  SYCU_RANGE_BINARY_OP_IN_PLACE(^=)

#define SYCU_RANGE_BINARY_OP_IN_PLACE_SIZE_T(op) \
  __host__ __device__ \
  range<dimensions>& operator op(const std::size_t &rhs) const { \
    for(std::size_t i = 0; i < dimensions; ++i) \
      _data[i] op rhs; \
    return *this; \
  }

  SYCU_RANGE_BINARY_OP_IN_PLACE_SIZE_T(+=)
  SYCU_RANGE_BINARY_OP_IN_PLACE_SIZE_T(-=)
  SYCU_RANGE_BINARY_OP_IN_PLACE_SIZE_T(*=)
  SYCU_RANGE_BINARY_OP_IN_PLACE_SIZE_T(/=)
  SYCU_RANGE_BINARY_OP_IN_PLACE_SIZE_T(%=)
  SYCU_RANGE_BINARY_OP_IN_PLACE_SIZE_T(<<=)
  SYCU_RANGE_BINARY_OP_IN_PLACE_SIZE_T(>>=)
  SYCU_RANGE_BINARY_OP_IN_PLACE_SIZE_T(&=)
  SYCU_RANGE_BINARY_OP_IN_PLACE_SIZE_T(|=)
  SYCU_RANGE_BINARY_OP_IN_PLACE_SIZE_T(^=)


private:
  std::array<std::size_t, dimensions> _data;

};


#define SYCU_RANGE_BINARY_OP_SIZE_T(op) \
  template<int dimensions> \
  __host__ __device__ \
  range<dimensions> operator op(const std::size_t &lhs, const id<dimensions> &rhs) { \
    range<dimensions> result; \
    for(std::size_t i = 0; i < dimensions; ++i) \
      result[i] = lhs op rhs[i]; \
    return result; \
  }

// OP is: +, -, *, /, %, <<, >>, &, |, ˆ
SYCU_RANGE_BINARY_OP_SIZE_T(+)
SYCU_RANGE_BINARY_OP_SIZE_T(-)
SYCU_RANGE_BINARY_OP_SIZE_T(*)
SYCU_RANGE_BINARY_OP_SIZE_T(/)
SYCU_RANGE_BINARY_OP_SIZE_T(%)
SYCU_RANGE_BINARY_OP_SIZE_T(<<)
SYCU_RANGE_BINARY_OP_SIZE_T(>>)
SYCU_RANGE_BINARY_OP_SIZE_T(&)
SYCU_RANGE_BINARY_OP_SIZE_T(|)
SYCU_RANGE_BINARY_OP_SIZE_T(^)

} // sycl
} // cl

#endif
