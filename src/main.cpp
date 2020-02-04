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
#include <fstream>
#include <preform/aabb.hpp>
#include <preform/misc_string.hpp>
#include <preform/option_parser.hpp>
#include <preform/medium.hpp>
#include <leaf-disk-gen/common.hpp>
#include <leaf-disk-gen/leaf_angle_distribution.hpp>
#include <leaf-disk-gen/leaf_disk.hpp>

int main(int argc, char** argv)
{
    using namespace ld;

    pr::option_parser opt_parser(
        "desc [OPTIONS] [<box> [BOX-OPTIONS]|<sphere> [SPHERE-OPTIONS]]...");

    int seed = 0;
    int matid = 100;
    Float lai = 1;
    Float radius = 0.1;
    std::string ofs_filename = "leaf.glist";
    std::string angle_distribution_args = "Uniform";

    unsigned int obj_ver_offset = 0;
    unsigned int obj_ver_res = 6;
    bool is_glist = true;

    // -s/--seed
    opt_parser.on_option("-s", "--seed", 1,
    [&](char** argv) {
        try {
            seed = std::stoi(argv[0]);
        }
        catch (const std::exception&) {
            throw 
                std::runtime_error(
                std::string("-s/--seed expects 1 integer ")
                    .append("(can't parse ").append(argv[0])
                    .append(")"));
        }
    })
    << "Specify seed. By default, 0.\n";

    // -m/--matid
    opt_parser.on_option("-m", "--matid", 1,
    [&](char** argv) {
        try {
            matid = std::stoi(argv[0]);
        }
        catch (const std::exception&) {
            throw 
                std::runtime_error(
                std::string("-m/--matid expects 1 integer ")
                    .append("(can't parse ").append(argv[0])
                    .append(")"));
        }
    })
    << "Specify material ID of leaf primitives. By default, 100.\n";

    // -l/--lai
    opt_parser.on_option("-l", "--lai", 1,
    [&](char** argv) {
        try {
            lai = std::stod(argv[0]);
            if (!(lai > 0)) {
                throw std::exception();
            }
        }
        catch (const std::exception&) {
            throw 
                std::runtime_error(
                std::string("-l/--lai expects 1 positive float ")
                    .append("(can't parse ").append(argv[0])
                    .append(")")); 
        }
    })
    << "Specify Leaf Area Index (LAI) with respect to XY. By default, 1.\n";

    // -r/--radius
    opt_parser.on_option("-r", "--radius", 1,
    [&](char** argv) {
        try {
            radius = std::stod(argv[0]);
            if (!(radius > 0)) {
                throw std::exception();
            }
        }
        catch (const std::exception&) {
            throw 
                std::runtime_error(
                std::string("-r/--radius expects 1 positive float ")
                    .append("(can't parse ").append(argv[0]).append(")")); 
        }
    })
    << "Specify leaf radius in meters. By default, 0.1.\n";

    // -o/--output
    opt_parser.on_option("-o", "--output", 1,
    [&](char** argv) {
        ofs_filename = argv[0];
    })
    << "Specify output filename. By default, \"leaf.glist\".\n";

    // -ov/--output-ver-res
    opt_parser.on_option("-ov", "--output-ver-res", 1,
    [&](char** argv) {
        try {
            obj_ver_res = std::stoi(argv[0]);
            if (obj_ver_res < 4 || 
                obj_ver_res > 32) {
                throw std::exception();
            }
        }
        catch (const std::exception&) {
            throw
                std::runtime_error(
                std::string("-ov/--output-ver-res expects 1 integer in ")
                    .append("[4, 32] (can't parse ").append(argv[0])
                    .append(")"));
        }
    })
    << "Specify output vertex resolution, being the number of vertices\n"
       "generated on the perimeter of each disk. This only affects OBJ\n"
       "output, since GList output has a proper disk primitive.\n"
       "By default, 6.\n";

    // -h/--help
    opt_parser.on_option("-h", "--help", 0,
    [&](char**) {
        std::cout << opt_parser << std::endl;
        std::exit(EXIT_SUCCESS);
    })
    << "Display this help and exit.\n";

    // Positional angle distribution arguments.
    opt_parser.on_positional(
    [&](char* argv) {
        angle_distribution_args = argv;
    });

    std::ofstream ofs;
    Pcg32 pcg;
    LeafAngleDistribution* angle_distribution = nullptr;

    // End global
    opt_parser.on_end(
    [&]() {

        // Does not have GList extension?
        pr::ci_string ci_ofs_filename = ofs_filename.c_str();
        if (ci_ofs_filename.rfind(".glist") + 6 != 
            ci_ofs_filename.size()) {
            is_glist = false;

            // Does not have OBJ extension?
            if (ci_ofs_filename.rfind(".obj") + 4 !=
                ci_ofs_filename.size()) {
                throw std::runtime_error(
                      "-o/--output filename must end "
                      "with either \".glist\" or \".obj\"");
            }
            
        }

        // Try to open output file stream.
        ofs.open(ofs_filename);
        if (!ofs.is_open()) {
            throw std::runtime_error(
                  std::string("can't open ").append(ofs_filename));
        }

        if (is_glist) {
            ofs << 
                "<geometrylist enabled=\"true\">\n"
                "<object>\n"
                "<basegeometry>\n"
                "<disk><matid>";
            ofs << matid;
            ofs << 
                "</matid></disk>\n"
                "</basegeometry>\n";
        }
        else {
            ofs << "usemtl " << matid << "\n";
        }

        // Seed.
        pcg = Pcg32(seed);

        // Angle distribution.
        angle_distribution = 
            LeafAngleDistribution::fromString(angle_distribution_args);
    });

    // Box options.
    Vec3<Float> box_from = {0, 0, 0};
    Vec3<Float> box_to = {1, 1, 1};

    // <box>
    opt_parser.in_group("box") 
    << "Axis-aligned box volume.\n";

    // --from
    opt_parser.on_option(nullptr, "--from", 1, 
    [&](char** argv) {
        std::stringstream sstr(argv[0]);
        sstr >> box_from;
        if (!sstr.good()) {
            throw std::runtime_error(
                    "--from expects a 3-dimensional coordinate "
                    "as a string, e.g., \"[1, 2, 3]\"");
        }
    })
    << "Specify box corner position. By default, \"[0, 0, 0]\".\n";

    // --to
    opt_parser.on_option(nullptr, "--to", 1,
    [&](char** argv) {
        std::stringstream sstr(argv[0]);
        sstr >> box_to;
        if (!sstr.good()) {
            throw std::runtime_error(
                    "--to expects a 3-dimensional coordinate "
                    "as a string, e.g., \"[1, 2, 3]\"");
        }
    })
    << "Specify box corner position. By default, \"[1, 1, 1]\".\n";

    // End <box>
    opt_parser.on_end(
    [&]() {
        pr::aabb3<Float> box = {
            pr::min(box_from, box_to),
            pr::max(box_from, box_to)
        };
        int num_leaves = 
            static_cast<int>(
                lai * 
                (box[1][0] - box[0][0]) *
                (box[1][1] - box[0][1]) /
                (pr::numeric_constants<Float>::M_pi() * radius * radius));

        for (int k = 0; k < num_leaves; k++) {
            LeafDisk leaf_disk;
            leaf_disk.pos = box.lerp(generateCanonical3(pcg));
            leaf_disk.normal = angle_distribution->sampleNormal(pcg);
            leaf_disk.radius = radius;
            if (is_glist) {
                leaf_disk.writeGListInstance(ofs);
            }
            else {
                leaf_disk.writeObj(
                        ofs, 
                        obj_ver_offset, 
                        obj_ver_res);
            }
        }
    });

    Vec3<Float> sphere_center = {0, 0, 0};
    Float sphere_radius = 1;

    // <sphere>
    opt_parser.in_group("sphere") 
    << "Sphere volume.\n";

    // --center
    opt_parser.on_option(nullptr, "--center", 1, 
    [&](char** argv) {
        std::stringstream sstr(argv[0]);
        sstr >> sphere_center;
        if (!sstr.good()) {
            throw std::runtime_error(
                    "--center expects a 3-dimensional coordinate "
                    "as a string, e.g., \"[1, 2, 3]\"");
        }
    })
    << "Specify sphere center. By default, \"[0, 0, 0]\".\n";

    // --radius
    opt_parser.on_option(nullptr, "--radius", 1, 
    [&](char** argv) {
        try {
            sphere_radius = std::stod(argv[0]);
            if (!(sphere_radius > 0)) {
                throw std::exception();
            }
        }
        catch (const std::exception&) {
            throw std::runtime_error(
                    "--radius expects 1 positive float");
        }
    })
    << "Specify sphere radius. By default, 1.\n";

    // End <sphere>
    opt_parser.on_end(
    [&]() {
        int num_leaves = 
            static_cast<int>(
                lai * 
                sphere_radius * 
                sphere_radius / 
                (radius * radius));

        for (int k = 0; k < num_leaves; k++) {
            LeafDisk leaf_disk;
            Vec2<Float> pos = 
            Vec2<Float>::uniform_disk_pdf_sample(generateCanonical2(pcg));
            leaf_disk.pos = {
                pos[0],
                pos[1],
                pr::sqrt(1 - pr::dot(pos, pos)) *
                        (2 * generateCanonical(pcg) - 1)
            };
            leaf_disk.pos *= sphere_radius;
            leaf_disk.pos += sphere_center;
            leaf_disk.normal = angle_distribution->sampleNormal(pcg);
            leaf_disk.radius = radius;
            if (is_glist) {
                leaf_disk.writeGListInstance(ofs);
            }
            else {
                leaf_disk.writeObj(
                        ofs, 
                        obj_ver_offset, 
                        obj_ver_res);
            }
        }
    });

    try {
        // Parse args.
        opt_parser.parse(argc, argv);
    }
    catch (const std::exception& exception) {
        std::cerr << "Unhandled exception in command line arguments!\n";
        std::cerr << "exception.what(): " << exception.what() << "\n";
        std::exit(EXIT_FAILURE);
    }

    if (is_glist) {
        ofs << 
            "</object>\n"
            "</geometrylist>\n";
    }

    delete angle_distribution;

    return EXIT_SUCCESS;
}
