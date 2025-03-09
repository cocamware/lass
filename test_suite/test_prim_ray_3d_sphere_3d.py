# /// script
# dependencies = [
#     "mpmath==1.3.0",
# ]
# ///

""""
This script is used to calculate t1Expected and t2Expected for a ray-sphere intersection test case

It uses the mpmath library to perform high-precision floating-point arithmetic

The alternative calculation of the discriminant is based on the following paper:

  Haines, E., Günther, J., Akenine-Möller, T. (2019).
  Precision Improvements for Ray/Sphere Intersection.
  In: Haines, E., Akenine-Möller, T. (eds) Ray Tracing Gems. Apress, Berkeley, CA. 
  https://doi.org/10.1007/978-1-4842-4427-2_7

  
*** BEGIN LICENSE INFORMATION ***

The contents of this file are subject to the Common Public Attribution License
Version 1.0 (the "License"); you may not use this file except in compliance with
the License. You may obtain a copy of the License at
https://lass.cocamware.com/cpal-license. The License is based on the
Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover
use of software over a computer network and provide for limited attribution for
the Original Developer. In addition, Exhibit A has been modified to be consistent
with Exhibit B.

Software distributed under the License is distributed on an "AS IS" basis, WITHOUT
WARRANTY OF ANY KIND, either express or implied. See the License for the specific
language governing rights and limitations under the License.

The Original Code is LASS - Library of Assembled Shared Sources.

The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
The Original Developer is the Initial Developer.

All portions of the code written by the Initial Developer are:
Copyright (C) 2025 the Initial Developer.
All Rights Reserved.

Contributor(s):

Alternatively, the contents of this file may be used under the terms of the
GNU General Public License Version 2 or later (the GPL), in which case the
provisions of GPL are applicable instead of those above.  If you wish to allow use
of your version of this file only under the terms of the GPL and not to allow
others to use your version of this file under the CPAL, indicate your decision by
deleting the provisions above and replace them with the notice and other
provisions required by the GPL License. If you do not delete the provisions above,
a recipient may use your version of this file under either the CPAL or the GPL.

*** END LICENSE INFORMATION ***
"""

from mpmath import mp, mpf

mp.prec = 512

def fromhex(s: str) -> mpf:
    return mpf(float.fromhex(s))

def subtract(A, B):
    return tuple(a - b for a, b in zip(A, B))

def dot(A, B):
    return sum(a * b for a, b in zip(A, B))

def scale(A, s):
    return tuple(a * s for a in A)

def intersect_ray_sphere(center, radius, origin, direction):
    print (f"center = {center}")
    print (f"radius = {radius}")
    print (f"support = {support}")
    print (f"direction = {direction}")
    print ("")

    # As for solving the quadratic equation, we can use the following formula:
    # 
    # ray: p(t) = center + t * direction
    # sphere: ||p - support||² = radius
    # cs = support - center
    #
    # ||center + t * direction - support||² = radius²
    # ||cs + t * direction||² = radius²
    # (cs + t * direction) · (cs + t * direction) - radius² == 0
    # t² * direction² + t * cs · direction + (cs² - radius²) == 0
    #
    # t² * a + t * b + c == 0
    # with:
    #  a = direction · direction
    #  b = 2 * cs · direction
    #  c = cs² - radius²
    #
    # D = b² - 4 * a * c


    cs = subtract(support, center)
    a = dot(direction, direction)
    print(f"a = {a}")
    b = 2 * dot(cs, direction)
    c = dot(cs, cs) - radius**2
    discriminant = b**2 - 4 * a * c
    print (f"discriminant  = {discriminant}")

    # "Precision Improvements for Ray/Sphere Intersection." shows an alternative
    # way to calculate the discriminant. Indeed, the result is the same:
    #
    # l = cs - direction * (cs · direction) / a
    # discriminant2 = 4 * a * (radius² - l²)

    l = subtract(cs, scale(direction, dot(cs, direction) / a))
    discriminant2 = 4 * a * (radius **2 - dot(l, l))
    print (f"discriminant2 = {discriminant2}")

    assert discriminant > 0, "We're only handling the case of 2 intersection points"
    #assert mp.almosteq(discriminant, discriminant2)

    # Finally:
    # t1 = (-b - sqrt(D)) / (2 * a)
    # t2 = (-b + sqrt(D)) / (2 * a)

    t1 = (-b - mp.sqrt(discriminant)) / (2 * a)
    t2 = (-b + mp.sqrt(discriminant)) / (2 * a)

    if b > 0:
        q = -0.5 * (b + mp.sqrt(discriminant))
    else:
        q = -0.5 * (b - mp.sqrt(discriminant))
    t1_ = c / q
    t2_ = q / a

    print ("")
    print (f"t1 = {t1}")
    print (f"t1_ = {t1_}")
    print (f"t2 = {t2}")
    print (f"t2_ = {t2_}")

    return t1, t2


# The inputs are taken from a failing test case in test_spat_object_trees.cpp
# These are hexadecimally encoded floating point numbers for perfect reproduction

center = fromhex("-0x1.de5af80000000p+9"), fromhex("-0x1.ca3b200000000p+8"), fromhex("0x1.5389140000000p+9")
radius = fromhex("0x1.e8b8280000000p+1")
support = fromhex("0x1.fbc3500000000p+9"), fromhex("0x1.edfdc00000000p+9"), fromhex("0x1.828c600000000p+6")
direction = fromhex("-0x1.91530c0000000p-1"), fromhex("-0x1.26e4840000000p-1"), fromhex("0x1.db54220000000p-3")

t1, t2 = intersect_ray_sphere(center, radius, support, direction)

print ("")
print (f"float(t1) = {float(t1)} = {float(t1).hex()}")
print (f"float(t2) = {float(t2)} = {float(t2).hex()}")


print ("------------------------------------")

# The paper also describes an improvement when the ray origin is very close to the
# sphere's surface, with a large radius.

center = fromhex("-0x1.de5af80000000p+9"), fromhex("-0x1.ca3b200000000p+8"), fromhex("0x1.5389140000000p+9")
radius = fromhex("0x1.38b8280000000p+11")
support = fromhex("0x1.fbc3500000000p+9"), fromhex("0x1.edfdc00000000p+9"), fromhex("0x1.828c600000000p+6")
direction = fromhex("-0x1.91530c0000000p-1"), fromhex("-0x1.26e4840000000p-1"), fromhex("0x1.db54220000000p-3")

t1, t2 = intersect_ray_sphere(center, radius, support, direction)

print ("")
print (f"float(t1) = {float(t1)} = {float(t1).hex()}")
print (f"float(t2) = {float(t2)} = {float(t2).hex()}")
