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

public:

    /**
     * @brief Initialize from string.
     */
    static LeafAngleDistribution* fromString(const std::string& args);
};

/**
 * @brief Uniform leaf angle distribution.
 */
class UniformLeafAngleDistribution final : public LeafAngleDistribution
{
public:

    /**
     * @copydoc LeafAngleDistribution::sampleNormal()
     */
    Vec3<Float> sampleNormal(Pcg32& pcg) const;
};

/**
 * @brief Isotropic LIDF leaf angle distribution.
 */
class IsotropicLidfLeafAngleDistribution : public LeafAngleDistribution
{
public:

    /**
     * @copydoc LeafAngleDistribution::sampleNormal()
     */
    Vec3<Float> sampleNormal(Pcg32& pcg) const final;

protected:

    /**
     * @brief Leaf inclination distribution function initializer.
     */
    void lidfInit(int n = 256);

    /**
     * @brief Leaf inclination distribution function. 
     */
    virtual Float lidf(Float theta) const = 0;

private:

    /**
     * @brief Leaf inclination distribution function values.
     */
    std::vector<Float> lidf_;
};

/**
 * @brief Trigonometric leaf angle distribution.
 */
class TrigonometricLeafAngleDistribution final : 
                        public IsotropicLidfLeafAngleDistribution
{
public:

    /**
     * @brief Type.
     */
    enum Type {

        /**
         * @brief Planophile.
         */
        eTypePlanophile,

        /**
         * @brief Erectophile.
         */
        eTypeErectophile,

        /**
         * @brief Plagiophile.
         */
        eTypePlagiophile,

        /**
         * @brief Extremophile.
         */
        eTypeExtremophile,

        /**
         * @brief Spherical.
         */
        eTypeSpherical
    };

    /**
     * @brief Constructor.
     */
    explicit
    TrigonometricLeafAngleDistribution(Type type) : type_(type)
    {
        // Delegate.
        this->lidfInit(256);
    }

protected:

    /**
     * @copydoc IsotropicLidfLeafAngleDistribution::lidf()
     *
     * @par Expression
     * - Planophile:
     * @f[
     *      F(\theta) = 
     *      \frac{2}{\pi}\left[\theta + \frac{1}{2}\sin{2\theta}\right] 
     * @f]
     * - Erectophile:
     * @f[
     *      F(\theta) = 
     *      \frac{2}{\pi}\left[\theta - \frac{1}{2}\sin{2\theta}\right] 
     * @f]
     * - Plagiophile:
     * @f[
     *      F(\theta) = 
     *      \frac{2}{\pi}\left[\theta - \frac{1}{4}\sin{4\theta}\right] 
     * @f]
     * - Extremophile:
     * @f[
     *      F(\theta) = 
     *      \frac{2}{\pi}\left[\theta + \frac{1}{4}\sin{4\theta}\right] 
     * @f]
     * - Spherical:
     * @f[
     *      F(\theta) = 1 - \cos{\theta}
     * @f]
     */
    Float lidf(Float theta) const;

private:

    /**
     * @brief Type.
     */
    Type type_ = eTypePlanophile;
};

/**
 * @brief Verhoef bimodal leaf angle distribution.
 */
class VerhoefBimodalLeafAngleDistribution final :
                        public IsotropicLidfLeafAngleDistribution
{
public:

    /**
     * @brief Constructor.
     */
    explicit
    VerhoefBimodalLeafAngleDistribution(Float a, Float b) : a_(a), b_(b)
    {
        this->lidfInit(256);
    }

    /**
     * @copydoc IsotropicLidfLeafAngleDistribution::lidf()
     */
    Float lidf(Float theta) const;

private:

    /**
     * @brief Coefficient @f$ a @f$, controlling average slope.
     */
    Float a_ = 0;

    /**
     * @brief Coefficient @f$ b @f$, controlling bimodality.
     */
    Float b_ = 0;
};

/**
 * @brief Trowbridge-Reitz (GGX) leaf angle distribution.
 */
class TrowbridgeReitzLeafAngleDistribution : public LeafAngleDistribution
{
public:

    /**
     * @brief Default constructor.
     */
    TrowbridgeReitzLeafAngleDistribution() = default;
    
    /**
     * @brief Constructor.
     */
    TrowbridgeReitzLeafAngleDistribution(Float alphax, Float alphay) :
            alphax_(alphax),
            alphay_(alphay)
    {
    }

    /**
     * @copydoc LeafAngleDistribution::sampleNormal()
     */
    Vec3<Float> sampleNormal(Pcg32& pcg) const;

private:

    /**
     * @brief Roughness in X @f$ \alpha_x @f$.
     */
    Float alphax_ = 1;

    /**
     * @brief Roughness in Y @f$ \alpha_y @f$.
     */
    Float alphay_ = 1;
};

/**
 * @brief Beckmann leaf angle distribution.
 */
class BeckmannLeafAngleDistribution : public LeafAngleDistribution
{
public:

    /**
     * @brief Default constructor.
     */
    BeckmannLeafAngleDistribution() = default;

    /**
     * @brief Constructor.
     */
    BeckmannLeafAngleDistribution(Float alphax, Float alphay) :
            alphax_(alphax),
            alphay_(alphay)
    {
    }

    /**
     * @copydoc LeafAngleDistribution::sampleNormal()
     */
    Vec3<Float> sampleNormal(Pcg32& pcg) const;

private:

    /**
     * @brief Roughness in X @f$ \alpha_x @f$.
     */
    Float alphax_ = 1;

    /**
     * @brief Roughness in Y @f$ \alpha_y @f$.
     */
    Float alphay_ = 1;
};


/**@}*/

} // namespace ld

#endif // #ifndef LEAF_DISK_GEN_LEAF_ANGLE_DISTRIBUTION_HPP
