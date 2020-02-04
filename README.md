# leaf-disk-gen

Leaf disk generation for DIRSIG scenes.

### Building from source

```
$ git clone --recursive https://github.com/mgradysaunders/leaf-disk-gen
$ cd leaf-disk-gen
$ mkdir bin && cd bin
$ cmake ..
$ cmake --build .
```

<a href="https://cmake.org"><img alt="CMake" src="https://upload.wikimedia.org/wikipedia/commons/1/13/Cmake.svg" width="128px"></a>
<a href="https://github.com/ruby/rake"><img alt="Ruby/rake" src="https://upload.wikimedia.org/wikipedia/commons/7/73/Ruby_logo.svg" width="128px"></a>

### Usage

This program generates a cloud of disjoint, disk-shaped &ldquo;leaves&rdquo;
matching an underlying distribution of leaf angles, and outputs the resulting
geometry either as a DIRSIG GList of true disk primitives or a Wavefront OBJ 
of triangulated disks. This is useful for abstract canopy simulations, which
focus on overall light transport phenomena (not photorealism).
The general program usage is 
```
$ ./bin/leaf-disk-gen desc [OPTIONS] [<box> [BOX-OPTIONS]|<sphere> [SPHERE-OPTIONS]]...
```
where `desc` is a string describing the leaf angle distribution,
`[OPTIONS]` specifies global program options, and the sequence of `box` or
`sphere` subcommands with `[BOX-OPTIONS]` or `[SPHERE-OPTIONS]` options 
specifies axis-aligned bounding boxes or spheres in which to generate 
the leaf primitives.

As mentioned, the `desc` string describes the leaf angle 
distribution. There are currently four types of leaf angle distributions
implemented.
1. _Uniform_, such that all leaf angles are equally likely. 
This is specified by the description string `"Uniform"`.
2. _Trigonometric_, such that the Leaf Inclination Distribution Function
(LIDF) is described by 1 of 5 different closed-form expressions. This
is specified by the description string `"Trigonometric TYPE"` where `TYPE`
is either `Planophile`, `Erectophile`, `Plagiophile`, `Extremophile`,
or `Spherical`. See the doxygen doc for mathematical expressions
for each type.
3. _Verhoef Bimodal_, which continuously generalizes the trigonometric
expressions via 2 real parameters. This is specified by the description 
string `"VerhoefBimodal A B"` where `A` and `B` are real numbers such
that `|A| + |B| <= 1`.
4. _Microsurface_, such that the leaf angles follow one of the two
standard microsurface normal distributions, _Trowbridge Reitz (GGX)_
or _Beckmann_. This is specifed by the description string 
`"NAME ALPHAX ALPHAY"` where `NAME` is either `TrowbridgeReitz` or 
`Beckmann`, and `ALPHAX` and `ALPHAY` are positive floating point numbers
corresponding to surface roughness in the X and Y directions. If these
are equal, then the angle distribution is isotropic. Otherwise, the
angle distribution is anisotropic.

The global options `[OPTIONS]` are as follows.
- `-s/--seed` to specify the seed for the random number generator. 
By default, this is `0`.
- `-m/--matid` to specify the material ID number to assign to the leaf 
primitives. By default, this is `100`.
- `-l/--lai` to specify Leaf Area Index (LAI) with respect to 
&ldquo;ground&rdquo; in the XY plane. LAI is defined as _leaf area
per unit of ground area_. So, this indirectly controls the number of 
leaves generated. By default, this is `1`.
- `-r/--radius` to specify the radius of leaf disks in meters. By default,
this is `0.05`.
- `-o/--output` to specify the output filename. This must end in
either `.glist` or `.obj`, to designate the file as a DIRSIG GList or 
Wavefront OBJ respectively. By default, this is `leaf.glist`.
- `-ov/--output-ver-res` to specify the output vertex resolution. This is 
the number of vertices generated on the perimeter of each triangulated disk. 
_This only affects Wavefront OBJ output_, since DIRSIG GList output uses a
true disk primitive. By default, this is `6`.
- `-h/--help` to display program help, which includes brief 
descriptions of all program options.

The box options `[BOX-OPTIONS]` include only 2 options, `--from` 
and `--to`, which specify the coordinates of the corners of the box.
Importantly, these options accept a string (surrounded by quotes) of an
array (surrounded by square brackets, separated by commas) of three real 
numbers. For example, `--from "[1, 2, 3]"` specifies the point X=1, Y=2, 
Z=3, for the &ldquo;from&rdquo; corner.

The sphere options `[SPHERE-OPTIONS]` also include only 2 options,
`--center` and `--radius`, which specify the center coordinate and radius of
the sphere respectively. 

As a more complete example,
```
$ ./bin/leaf-disk-gen "VerhoefBimodal -0.3 0.2" -l 1.2 -r 0.05 -o "verhoef-canopy.glist" box --from "[-5, -5, 0]" --to "[5, 5, 1]"
```
generates a DIRSIG GList file `verhoef-canopy.glist` containing
disk primitive instances over `-5<X<5`, `-5<Y<5`, `0<Z<1`, with 
average LAI of 1.2, radius of 5 centimeters, and angles matching a 
Verhoef bimodal LIDF with `A=-0.3` and `B=0.2`.
