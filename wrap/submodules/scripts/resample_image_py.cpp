/* ********************************************************************
 * Copyright (C) 2020 Pablo Hernandez-Cerdan.
 *
 * This file is part of SGEXT: http://github.com/phcerdan/sgext.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * *******************************************************************/

#include "pybind11_common.h"

#include "resample_image_function.hpp"

namespace py = pybind11;
using namespace SG;

void init_resample_image(py::module &m) {
    m.def("resample_image", &resample_image_function,
          R"delimiter(
Resample binary image.

If shrink_factor is greater than 1, it downsamples the image.
If shrink_factor is between (0, 1), it upsamples the image.

Output image has the same origin, and the right spacing and size based
on shrink factor.

Parameters:
----------
input: BinaryImageType
  input binary image.

shrink_factor: Float
  value greater than 0 to downsample or upsample the image.

verbose: bool
  extra information displayed during the algorithm.
            )delimiter",
          py::arg("input"), py::arg("shrink_factor"),
          py::arg("verbose") = false);
}
