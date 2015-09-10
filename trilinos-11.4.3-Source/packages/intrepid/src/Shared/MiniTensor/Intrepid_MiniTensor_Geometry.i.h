// @HEADER
// ************************************************************************
//
//                           Intrepid Package
//                 Copyright (2007) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions: Alejandro Mota (amota@sandia.gov)
//
// ************************************************************************
// @HEADER

#if !defined(Intrepid_MiniTensor_Geometry_i_h)
#define Intrepid_MiniTensor_Geometry_i_h


namespace Intrepid {

//
// Helper functions for determining the type of element
//
namespace {

inline
ELEMENT::Type
find_type_1D(Index const nodes)
{
  switch (nodes) {
    case 2:   return ELEMENT::SEGMENTAL;
    default:  return ELEMENT::UNKNOWN;
  }
}

inline
ELEMENT::Type
find_type_2D(Index const nodes)
{
  switch (nodes) {
    case 3:   return ELEMENT::TRIANGULAR;
    case 4:   return ELEMENT::QUADRILATERAL;
    default:  return ELEMENT::UNKNOWN;
  }
}

inline
ELEMENT::Type
find_type_3D(Index const nodes)
{
  switch (nodes) {
    case 4:   return ELEMENT::TETRAHEDRAL;
    case 8:   return ELEMENT::HEXAHEDRAL;
    default:  return ELEMENT::UNKNOWN;
  }
}

} // anonymous namespace


//
//
//
inline
ELEMENT::Type
find_type(Index const dimension, Index const number_nodes)
{

  ELEMENT::Type
  type = ELEMENT::UNKNOWN;

  switch (dimension) {

    case 1:
      type = find_type_1D(number_nodes);
      break;

    case 2:
      type = find_type_2D(number_nodes);
      break;

    case 3:
      type = find_type_3D(number_nodes);
      break;

    default:
      break;

  }

  if (type == ELEMENT::UNKNOWN) {
    std::cerr << "ERROR: " << __PRETTY_FUNCTION__;
    std::cerr << std::endl;
    std::cerr << "Unknown element type." << std::endl;
    std::cerr << std::endl;
    std::cerr << "Spatial dimension: ";
    std::cerr << dimension << std::endl;
    std::cerr << "Vertices per element: ";
    std::cerr << number_nodes << std::endl;
    exit(1);
  }

  return type;
}

//
// Constructor for SphericalParametrization
//
template<typename T>
inline
SphericalParametrization<T>::SphericalParametrization(
    Tensor4<T> const & A) : tangent_(A)
{
  minimum_ = std::numeric_limits<T>::max();
  maximum_ = std::numeric_limits<T>::min();
  return;
}

//
// Evaluation for SphericalParemetrization
//
template<typename T>
inline
void
SphericalParametrization<T>::operator()(Vector<T> const & parameters)
{
  assert(parameters.get_dimension() == 2);

  T const &
  phi = parameters(0);

  T const &
  theta = parameters(1);

  Vector<T> const
  normal(sin(phi) * sin(theta), cos(phi), sin(phi) * cos(theta));

  // Localization tensor
  Tensor<T> const
  Q = dot(normal, dot(tangent_, normal));

  T const
  determinant = det(Q);

  if (determinant < minimum_) {
    minimum_ = determinant;
    arg_minimum_ = parameters;
  }

  if (determinant > maximum_) {
    maximum_ = determinant;
    arg_maximum_ = parameters;
  }

  return;
}

//
// Constructor for StereographicParametrization
//
template<typename T>
inline
StereographicParametrization<T>::StereographicParametrization(
    Tensor4<T> const & A) : tangent_(A)
{
  minimum_ = std::numeric_limits<T>::max();
  maximum_ = std::numeric_limits<T>::min();
  return;
}

//
// Evaluation for StereographicParemetrization
//
template<typename T>
inline
void
StereographicParametrization<T>::operator()(Vector<T> const & parameters)
{
  assert(parameters.get_dimension() == 2);

  T const &
  x = parameters(0);

  T const &
  y = parameters(1);

  T const
  r2 = x * x + y * y;

  Vector<T>
  normal(2.0 * x, 2.0 * y, r2 - 1.0);

  normal /= (r2 + 1.0);

  // Localization tensor
  Tensor<T> const
  Q = dot(normal, dot(tangent_, normal));

  T const
  determinant = det(Q);

  if (determinant < minimum_) {
    minimum_ = determinant;
    arg_minimum_ = parameters;
  }

  if (determinant > maximum_) {
    maximum_ = determinant;
    arg_maximum_ = parameters;
  }

  return;
}

//
// Constructor for ProjectiveParametrization
//
template<typename T>
inline
ProjectiveParametrization<T>::ProjectiveParametrization(
    Tensor4<T> const & A) : tangent_(A)
{
  minimum_ = std::numeric_limits<T>::max();
  maximum_ = std::numeric_limits<T>::min();
  return;
}

//
// Evaluation for ProjectiveParemetrization
//
template<typename T>
inline
void
ProjectiveParametrization<T>::operator()(Vector<T> const & parameters)
{
  assert(parameters.get_dimension() == 4);

  T const &
  x = parameters(0);

  T const &
  y = parameters(1);

  T const &
  z = parameters(2);

  T const &
  lambda = parameters(3);

  const Vector<T>
  normal(x, y, z);

  // Localization tensor
  Tensor<T> const
  Q = dot(normal, dot(tangent_, normal));

  T const
  determinant = det(Q);

  T const
  function = determinant + lambda * (x * x + y * y + z * z - 1.0);

  if (function < minimum_) {
    minimum_ = function;
    arg_minimum_ = parameters;
  }

  if (function > maximum_) {
    maximum_ = function;
    arg_maximum_ = parameters;
  }

  return;
}

//
// Constructor for TangentParametrization
//
template<typename T>
inline
TangentParametrization<T>::TangentParametrization(
    Tensor4<T> const & A) : tangent_(A)
{
  minimum_ = std::numeric_limits<T>::max();
  maximum_ = std::numeric_limits<T>::min();
  return;
}

//
// Evaluation for TangentParemetrization
//
template<typename T>
inline
void
TangentParametrization<T>::operator()(Vector<T> const & parameters)
{
  assert(parameters.get_dimension() == 2);

  T const &
  x = parameters(0);

  T const &
  y = parameters(1);

  T const
  r = std::sqrt(x * x + y * y);

  Vector<T>
  normal(3, ZEROS);

  if (r > 0.0) {
    normal(0) = x * sin(r) / r;
    normal(1) = y * sin(r) / r;
    normal(2) = cos(r);
  }

  // Localization tensor
  Tensor<T> const
  Q = dot(normal, dot(tangent_, normal));

  T const
  determinant = det(Q);

  if (determinant < minimum_) {
    minimum_ = determinant;
    arg_minimum_ = parameters;
  }

  if (determinant > maximum_) {
    maximum_ = determinant;
    arg_maximum_ = parameters;
  }

  return;
}

//
// Constructor for CartesianParametrization
//
template<typename T>
inline
CartesianParametrization<T>::CartesianParametrization(
    Tensor4<T> const & A) : tangent_(A)
{
  minimum_ = std::numeric_limits<T>::max();
  maximum_ = std::numeric_limits<T>::min();
  return;
}

//
// Evaluation for CartesianParemetrization
//
template<typename T>
inline
void
CartesianParametrization<T>::operator()(Vector<T> const & parameters)
{
  assert(parameters.get_dimension() == 3);

  T const &
  x = parameters(0);

  T const &
  y = parameters(1);

  T const
  z = parameters(2);

  const Vector<T>
  normal(x, y, z);

  // Localization tensor
  Tensor<T> const
  Q = dot(normal, dot(tangent_, normal));

  T const
  determinant = det(Q);

  if (determinant < minimum_) {
    minimum_ = determinant;
    arg_minimum_ = parameters;
  }

  if (determinant > maximum_) {
    maximum_ = determinant;
    arg_maximum_ = parameters;
  }

  return;
}

//
// Constructor for ParametricGrid
//
template<typename T>
inline
ParametricGrid<T>::ParametricGrid(
    Vector<T> const & lower,
    Vector<T> const & upper,
    Vector<Index> const & points_per_dimension)
{
  assert(lower.get_dimension() == upper.get_dimension());
  assert(lower.get_dimension() == points_per_dimension.get_dimension());

  lower_ = lower;
  upper_ = upper;
  points_per_dimension_ = points_per_dimension;

  return;
}

//
// Traverse the grid and apply the visitor to each point.
//
template<typename T>
template<typename Visitor>
inline
void
ParametricGrid<T>::traverse(Visitor & visitor) const
{
  // Loop over the grid
  Index const
  number_parameters = lower_.get_dimension();

  LongCount
  total_number_points = 1;

  for (Index dimension = 0; dimension < number_parameters; ++dimension) {
    total_number_points *= points_per_dimension_(dimension);
  }

  //std::cout << "Total number of points: " << total_number_points << std::endl;

  Vector<LongCount>
  steps(number_parameters, ONES);

  for (Index dimension = 1; dimension < number_parameters; ++dimension) {
    steps(dimension) =
        steps(dimension - 1) * points_per_dimension_(dimension - 1);
  }

  //std::cout << "Points per dimension  : " << points_per_dimension_ << std::endl;
  //std::cout << "Steps                 : " << steps << std::endl;

  Vector<Index>
  indices(number_parameters, ZEROS);

  Vector<T>
  position_in_grid(number_parameters, ZEROS);

  Vector<T>
  span = upper_ - lower_;

  for (LongCount point = 1;  point <= total_number_points; ++point) {

    //std::cout << "Indices : ";

    for (Index dimension = 0; dimension < number_parameters; ++dimension) {

      position_in_grid(dimension) = indices(dimension) * span(dimension) /
          (points_per_dimension_(dimension) - 1) + lower_(dimension);

      visitor(position_in_grid);

      //std::cout << indices(dimension) << " ";

      // Check if index needs to be increased or rolled back
      if (point % steps(dimension) == 0) {
        ++indices(dimension);
      }
      if (indices(dimension) == points_per_dimension_(dimension)) {
        indices(dimension) = 0;
      }

    }

    //std::cout << std::endl;
    //std::cout << "Position : " << position_in_grid << std::endl;

  }

  return;
}

} // namespace Intrepid

#endif // Intrepid_MiniTensor_Geometry_i_h

