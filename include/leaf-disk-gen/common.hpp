/* Copyright (c) 2020 M. Grady Saunders
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *   1. Redistributions of source code must retain the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer.
 * 
 *   2. Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials
 *      provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*+-+*/
#pragma once
#ifndef LEAF_DISK_GEN_COMMON_HPP
#define LEAF_DISK_GEN_COMMON_HPP

#include <cassert>
#include <iostream>
#include <preform/multi.hpp>
#include <preform/multi_math.hpp>
#include <preform/multi_random.hpp>

namespace ld {

/**
 * @defgroup common Common
 *
 * `<leaf-disk-gen/common.hpp>`
 */
/**@{*/

/**
 * @brief Floating point type.
 */
typedef double Float;

/**
 * @brief 2-dimensional vector.
 */
template <typename T>
using Vec2 = pre::vec2<T>;

/**
 * @brief 3-dimensional vector.
 */
template <typename T>
using Vec3 = pre::vec3<T>;

/**
 * @brief 3-dimensional matrix.
 */
template <typename T>
using Mat3 = pre::mat3<T>;

/**
 * @brief Permuted congruential generator.
 */
typedef pre::pcg32 Pcg32;

/**
 * @brief Generate canonical random sample.
 */
inline
Float generateCanonical(Pcg32& pcg)
{
    return pre::generate_canonical<Float>(pcg);
}

/**
 * @brief Generate canonical 2-dimensional random sample.
 */
inline 
Vec2<Float> generateCanonical2(Pcg32& pcg)
{
    return pre::generate_canonical<Float, 2>(pcg);
}

/**
 * @brief Generate canonical 3-dimensional random sample.
 */
inline 
Vec3<Float> generateCanonical3(Pcg32& pcg)
{
    return pre::generate_canonical<Float, 3>(pcg);
}

/**@}*/

} // namespace ld

#endif // #ifndef LEAF_DISK_GEN_COMMON_HPP
