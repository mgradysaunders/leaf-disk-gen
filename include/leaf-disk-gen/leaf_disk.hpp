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
#ifndef LEAF_DISK_GEN_LEAF_DISK_HPP
#define LEAF_DISK_GEN_LEAF_DISK_HPP

#include <leaf-disk-gen/common.hpp>

namespace ld {

/**
 * @defgroup leaf_disk Leaf disk
 *
 * `<leaf-disk-gen/leaf_disk.hpp>`
 */
/**@{*/

/**
 * @brief Leaf disk.
 */
class LeafDisk
{
public:

    /**
     * @brief Position.
     */
    Vec3<Float> pos = {0, 0, 0};

    /**
     * @brief Normal direction.
     */
    Vec3<Float> normal = {0, 0, 1};

    /**
     * @brief Radius.
     */
    Float radius = 1;

public:

    /**
     * @name Area helpers
     */
    /**@{*/

    /**
     * @brief Compute area.
     */
    Float computeArea() const
    {
        return pr::numeric_constants<Float>::M_pi() * (radius * radius);
    }

    /**
     * @brief Compute projected area.
     */
    Float computeProjectedArea(const Vec3<Float>& dir) const
    {
        return computeArea() * pr::abs(pr::dot(normal, dir));
    }

    /**@}*/

public:

    /**
     * @name Write helpers
     */
    /**@{*/

    /**
     * @brief Write GList instance. 
     *
     * @param[inout] ostr
     * Output stream.
     */
    void writeGListInstance(std::ostream& ostr) const;

    /**
     * @brief Write OBJ.
     *
     * @param[inout] ostr
     * Output stream.
     *
     * @param[inout] ver_offset
     * Vertex offset.
     *
     * @param[in] ver_res
     * Vertex resolution.
     *
     * @note
     * The `ver_offset` argument allows multiple disks to be written to 
     * the same file by keeping track of the number of vertices written so 
     * far. This should be zero initially, and should not be modified by the
     * user between calls. 
     *
     * @note
     * The `ver_res` argument is the number of vertices to generate on
     * the perimeter of the disk, which should be greater than or equal to 
     * three. Also note that the disk is tessellated as a triangle fan about
     * a vertex in the center, such that the implementation actually writes 
     * `ver_res + 1` vertices. Furthermore, each triangle is locally scaled 
     * according to `ver_res` so that its area is equivalent to the area of 
     * the sector it represents.
     */
    void writeObj(std::ostream& ostr, 
                  unsigned int& ver_offset, 
                  unsigned int ver_res = 12) const;

    /**@}*/
};

/**@}*/

} // namespace ld

#endif // #ifndef LEAF_DISK_GEN_LEAF_DISK_HPP
