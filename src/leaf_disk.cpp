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
#include <leaf-disk-gen/leaf_disk.hpp>

namespace ld {

// Write GList instance.
void LeafDisk::writeGListInstance(std::ostream& ostr) const
{
    // TBN matrix.
    Mat3<Float> tbn = 
    Mat3<Float>::build_onb(normal);

    // Write static instance with affine transform.
    ostr << 
        "<staticinstance>"
        "<matrix>";
    for (int j = 0; j < 3; j++) {
        ostr << radius * tbn[j][0] << ", ";
        ostr << radius * tbn[j][1] << ", ";
        ostr << radius * tbn[j][2] << ", ";
        ostr << pos[j] << ", ";
    }
    ostr << 
        "0, 0, 0, 1"
        "</matrix>"
        "</staticinstance>\n";
}

// Write OBJ.
void LeafDisk::writeObj(
            std::ostream& ostr, 
            unsigned int& ver_offset, 
            unsigned int ver_res) const
{
    // TBN matrix.
    Mat3<Float> tbn = 
    Mat3<Float>::build_onb(normal);

    // Tangential directions.
    Vec3<Float> hatu = pr::transpose(tbn)[0];
    Vec3<Float> hatv = pr::transpose(tbn)[1];

    // Write center vertex.
    ostr << "v ";
    ostr << pos[0] << ' ';
    ostr << pos[1] << ' ';
    ostr << pos[2] << '\n';

    // Clamp.
    if (ver_res < 3) {
        ver_res = 3;
    }

    // Write vertices.
    for (unsigned int j = 0; j < ver_res; j++) {
        Float phi = 
            j / Float(ver_res) * 
            2 * pr::numeric_constants<Float>::M_pi();
        Vec3<Float> ver = 
            (radius * pr::cos(phi)) * hatu + 
            (radius * pr::sin(phi)) * hatv + pos;
        ostr << "v ";
        ostr << ver[0] << ' ';
        ostr << ver[1] << ' ';
        ostr << ver[2] << '\n';
    }

    // Write triangles.
    for (unsigned int j = 0; j < ver_res; j++) {
        unsigned int v0 = 0 + ver_offset;
        unsigned int v1 = 1 + (j + 0) % ver_res + ver_offset;
        unsigned int v2 = 1 + (j + 1) % ver_res + ver_offset;
        ostr << "f ";
        ostr << v0 + 1 << ' ';
        ostr << v1 + 1 << ' ';
        ostr << v2 + 1 << '\n';
    }

    // Bump vertex offset.
    ver_offset += ver_res + 1;
}

} // namespace ld
