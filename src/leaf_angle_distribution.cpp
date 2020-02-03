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
#include <sstream>
#include <preform/misc_string.hpp>
#include <leaf-disk-gen/leaf_angle_distribution.hpp>

namespace ld {

// Sample normal direction.
Vec3<Float> UniformLeafAngleDistribution::sampleNormal(Pcg32& pcg) const
{
    return Vec3<Float>::uniform_hemisphere_pdf_sample(generateCanonical2(pcg));
}

// CDF initializer.
void IsotropicLidfLeafAngleDistribution::lidfInit(int n)
{
    assert(n > 2);
    lidf_.resize(n);
    for (int k = 1; k < n - 1; k++) {
        lidf_[k] = lidf(k / Float(n - 1) * 
                        pr::numeric_constants<Float>::M_pi_2());
    }
    // Always 0 and 1.
    lidf_[0] = 0;
    lidf_[n - 1] = 1;
}

// Sample normal.
Vec3<Float> IsotropicLidfLeafAngleDistribution::sampleNormal(Pcg32& pcg) const
{
    // Generate random numbers.
    Float u0 = generateCanonical(pcg);
    Float u1 = generateCanonical(pcg);

    // Sample zenith.
    Float theta = 0;
    Float sin_theta = 0;
    Float cos_theta = 1;
    {
        auto itr = 
        std::lower_bound(
                lidf_.begin(),
                lidf_.end(),
                u0);
        if (itr == lidf_.begin() || 
            itr == lidf_.end()) {
            theta = itr == lidf_.begin() ? 0 : 
                    pr::numeric_constants<Float>::M_pi_2();
        }
        else {
            --itr;
            std::ptrdiff_t k0 = std::distance(lidf_.begin(), itr);
            std::ptrdiff_t k1 = k0 + 1;
            assert(k0 >= 0 && k0 < std::ptrdiff_t(lidf_.size()));
            assert(k1 >= 0 && k1 < std::ptrdiff_t(lidf_.size()));
            Float lidf0 = lidf_[k0];
            Float lidf1 = lidf_[k1];
            Float fac = (u0 - lidf0) / (lidf1 - lidf0);
            theta = ((1 - fac) * k0 + fac * k1) /
                    (lidf_.size() - 1) * 
                    pr::numeric_constants<Float>::M_pi_2();
        }
        sin_theta = pr::sin(theta);
        cos_theta = pr::cos(theta);
    }

    // Sample azimumth.
    Float phi = 2 * pr::numeric_constants<Float>::M_pi() * u1;
    Float cos_phi = pr::cos(phi);
    Float sin_phi = pr::sin(phi);

    // Construct direction.
    return {
        sin_theta * cos_phi,
        sin_theta * sin_phi,
        cos_theta
    };
}

// LIDF.
Float TrigonometricLeafAngleDistribution::lidf(Float theta) const
{
    switch (type_) {
        default:
        case eTypePlanophile:
            return (theta + pr::sin(2 * theta) / 2) / 
                            pr::numeric_constants<Float>::M_pi_2();

        case eTypeErectophile:
            return (theta - pr::sin(2 * theta) / 2) /
                            pr::numeric_constants<Float>::M_pi_2();

        case eTypePlagiophile:
            return (theta - pr::sin(4 * theta) / 4) /
                            pr::numeric_constants<Float>::M_pi_2();

        case eTypeExtremophile:
            return (theta + pr::sin(4 * theta) / 4) / 
                            pr::numeric_constants<Float>::M_pi_2();

        case eTypeSpherical:
            return 1 - pr::cos(theta);
    }

    // Unreachable.
    return 0;
}

// LIDF.
Float VerhoefBimodalLeafAngleDistribution::lidf(Float theta) const
{
    double a = double(a_);
    double b = double(b_);
    double x = double(2 * theta);
    double y = 0;
    while (1) {
        y = a * pr::sin(x) + b * (pr::sin(2 * x) / 2);
        double dx = (y - x + 2 * theta) / 2;
        if (pr::fabs(dx) < 1e-6) {
            break;
        }
        x += dx;
    }
    return Float(y + theta) / 
                pr::numeric_constants<Float>::M_pi_2();
}

// Sample normal.
Vec3<Float> TrowbridgeReitzLeafAngleDistribution::sampleNormal(
            Pcg32& pcg) const
{
    Vec2<Float> u = generateCanonical2(pcg);
    Vec2<Float> m = 
        pr::sqrt(-1 - 1 / (4 * u * (u - 1))) * 
        pr::copysign(Vec2<Float>{1, 1}, u - Float(0.5));
    return 
        pr::normalize_safe(
        Vec3<Float>{
            -alphax_ * m[0], 
            -alphay_ * m[1], 
            1
        });
}

// Sample normal.
Vec3<Float> BeckmannLeafAngleDistribution::sampleNormal(
            Pcg32& pcg) const
{
    Vec2<Float> m = {
        pr::normal_distribution<Float>(0, 1)(pcg),
        pr::normal_distribution<Float>(0, 1)(pcg)
    };
    return 
        pr::normalize_safe(
        Vec3<Float>{
            -alphax_ * m[0], 
            -alphay_ * m[1], 
            1
        });
}

// From string.
LeafAngleDistribution* 
LeafAngleDistribution::fromString(const std::string& args)
{
    std::stringstream ss(args);
    std::string name;
    ss >> name;

    pr::ci_string ci_name = name.c_str();
    if (ci_name == "Uniform") {
        return new UniformLeafAngleDistribution();
    }
    else
    if (ci_name == "Trigonometric") {
        std::string type;
        ss >> type;

        pr::ci_string ci_type = type.c_str();
        if (ci_type == "Planophile") {
            return new TrigonometricLeafAngleDistribution(
                       TrigonometricLeafAngleDistribution::eTypePlanophile);
        }
        else
        if (ci_type == "Erectophile") {
            return new TrigonometricLeafAngleDistribution(
                       TrigonometricLeafAngleDistribution::eTypeErectophile);
        }
        else
        if (ci_type == "Plagiophile") {
            return new TrigonometricLeafAngleDistribution(
                       TrigonometricLeafAngleDistribution::eTypePlagiophile);
        }
        else
        if (ci_type == "Extremophile") {
            return new TrigonometricLeafAngleDistribution(
                       TrigonometricLeafAngleDistribution::eTypeExtremophile);
        }
        else
        if (ci_type == "Spherical") {
            return new TrigonometricLeafAngleDistribution(
                       TrigonometricLeafAngleDistribution::eTypeSpherical);
        }
        else {
            // Error.
            throw 
                std::runtime_error(
                std::string(__PRETTY_FUNCTION__)
                    .append(
                    ": format is 'Trigonometric TYPE' where "
                    "TYPE is 'Planophile', 'Erectophile', 'Plagiophile', "
                    "'Extremophile', or 'Spherical'"));
        }
    }
    else
    if (ci_name == "VerhoefBimodal") {
        Float a;
        Float b;
        try {
            std::string sa;
            std::string sb;
            ss >> sa;
            ss >> sb;
            a = std::stod(sa);
            b = std::stod(sb);
            if (!(pr::fabs(a) + pr::fabs(b) <= 1)) {
                throw std::exception();
            }
        }
        catch (const std::exception&) {
            // Error.
            throw
                std::runtime_error(
                std::string(__PRETTY_FUNCTION__)
                    .append(
                    ": format is 'VerhoefBimodal A B' where A and B "
                    "are floating point numbers satsifying |A| + |B| <= 1"));
        }
        return new VerhoefBimodalLeafAngleDistribution(a, b);
    }
    else
    if (ci_name == "TrowbridgeReitz" ||
        ci_name == "Beckmann") {
        Float alphax;
        Float alphay;
        try {
            std::string salphax;
            std::string salphay;
            ss >> salphax;
            ss >> salphay;
            alphax = std::stod(salphax);
            alphay = std::stod(salphay);
            if (!(alphax > 0 && alphay > 0)) {
                throw std::exception();
            }
        }
        catch (const std::exception&) {
            // Error.
            throw
                std::runtime_error(
                std::string(__PRETTY_FUNCTION__)
                    .append(": format is '").append(ci_name.c_str())
                    .append(" ALPHAX ALPHAY' "
                            "where ALPHAX and ALPHAY "
                            "are positive floating point numbers"));
        }
        if (ci_name == "TrowbridgeReitz") {
            return new TrowbridgeReitzLeafAngleDistribution(alphax, alphay);
        }
        else {
            return new BeckmannLeafAngleDistribution(alphax, alphay);
        }
    }
    else {
        // Error.
        throw 
            std::runtime_error(
            std::string(__PRETTY_FUNCTION__)
                .append(": unknown name \"").append(name).append("\"")); 
    }
    return nullptr;
}

} // namespace ld
