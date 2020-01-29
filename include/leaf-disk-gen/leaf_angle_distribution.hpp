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
#ifndef LEAF_DISK_GEN_LEAF_ANGLE_DISTRIBUTION_HPP
#define LEAF_DISK_GEN_LEAF_ANGLE_DISTRIBUTION_HPP

#include <leaf-disk-gen/common.hpp>

namespace ld {

/**
 * @defgroup leaf_angle_distribution Leaf angle distribution
 *
 * `<leaf-disk-gen/leaf_angle_distribution.hpp>`
 */
/**@{*/

/**
 * @brief Leaf angle distribution.
 */
class LeafAngleDistribution
{
public:

    /**
     * @brief Destructor.
     */
    virtual ~LeafAngleDistribution() {}

    /**
     * @brief Sample normal direction.
     */
    virtual Vec3<Float> sampleNormal(Pcg32& pcg) const = 0;
};

/**
 * @brief Isotropic leaf angle distribution.
 */
class IsotropicLeafAngleDistribution final : public LeafAngleDistribution
{
public:

    /**
     * @copydoc LeafAngleDistribution::sampleNormal()
     */
    Vec3<Float> sampleNormal(Pcg32& pcg) const;
};

/**@}*/

} // namespace ld

#endif // #ifndef LEAF_DISK_GEN_LEAF_ANGLE_DISTRIBUTION_HPP
