#include <math.h>

namespace rif {
namespace numeric {

template <class V>
void cube_to_sphere(V &v) {
  typedef typename V::Scalar Float;
  double const xx = v[0] * v[0];
  double const yy = v[1] * v[1];
  double const zz = v[2] * v[2];
  v[0] *= sqrt(fmax(0, 1.0 - yy * 0.5 - zz * 0.5 + yy * zz / 3.0));
  v[1] *= sqrt(fmax(0, 1.0 - zz * 0.5 - xx * 0.5 + zz * xx / 3.0));
  v[2] *= sqrt(fmax(0, 1.0 - xx * 0.5 - yy * 0.5 + xx * yy / 3.0));
  v.normalize();
}

// assumes we are on the +Z face!!!!!
template <class V>
void sphere_to_cube_facenum0(V &v) {
  typedef typename V::Scalar Float;
  Float const x = v[0];
  Float const y = v[1];
  // Float const z = v[2];
  Float &X(v[0]);
  Float &Y(v[1]);
  Float &Z(v[2]);
  Float eps = std::sqrt(std::numeric_limits<Float>::epsilon());
  Float const INV_SQRT_2 = 0.70710676908493042;
  Float const a2 = x * x * 2.0;
  Float const b2 = y * y * 2.0;
  Float const inner = -a2 + b2 - 3.0;
  Float const innersqrt = -sqrt((inner * inner) - 12.0 * a2);
  X = (fabs(x) < eps) ? 0.0 : sqrt(innersqrt + a2 - b2 + 3.0) * INV_SQRT_2;
  Y = (fabs(y) < eps) ? 0.0 : sqrt(innersqrt - a2 + b2 + 3.0) * INV_SQRT_2;
  Z = 1.0;  // not really necessary for lookups...
  X = x < 0.0 ? -X : X;
  Y = y < 0.0 ? -Y : Y;
  // if(X > 1.0) X = 1.0; // shouldn't be necessary
  // if(Y > 1.0) Y = 1.0; // shouldn't be necessary
}

/**
 * @brief      move face0 to face (may be improper rotation)
 *
 * @param[in]  face   The face
 * @param      x      { parameter_description }
 * @param      y      { parameter_description }
 * @param      z      { parameter_description }
 *
 * @tparam     Float  { description }
 */
template <class Float>
void permute_cube_face(int face, Float &x, Float &y, Float &z) {
  Float X = x, Y = y, Z = z;
  switch (face) {
    case 0:
      x = X;
      y = Y;
      z = Z;
      break;
    case 1:
      x = Z;
      y = X;
      z = Y;
      break;
    case 2:
      x = -Y;
      y = -Z;
      z = +X;
      break;
    case 3:
      x = -Z;
      y = +X;
      z = -Y;
      break;
    case 4:
      x = Y;
      y = Z;
      z = X;
      break;
    case 5:
      x = -Y;
      y = +X;
      z = -Z;
      break;
    default:
      break;
  }
}

/**
 * @brief      move face to face0 (may be improper rotation)
 *
 * @param[in]  face   The face
 * @param      x      { parameter_description }
 * @param      y      { parameter_description }
 * @param      z      { parameter_description }
 *
 * @tparam     Float  { description }
 */
template <class Float>
void permute_cube_face_inverse(int face, Float &x, Float &y, Float &z) {
  Float X = x, Y = y, Z = z;
  switch (face) {
    case 0:
      x = X;
      y = Y;
      z = Z;
      break;
    case 1:
      x = Y;
      y = Z;
      z = X;
      break;
    case 2:
      x = +Z;
      y = -X;
      z = -Y;
      break;
    case 3:
      x = +Y;
      y = -Z;
      z = -X;
      break;
    case 4:
      x = Z;
      y = X;
      z = Y;
      break;
    case 5:
      x = +Y;
      y = -X;
      z = -Z;
      break;
    default:
      break;
  }
}

template <class V>
void permute_cube_face(int face, V &v) {
  permute_cube_face(face, v[0], v[1], v[2]);
}
template <class V>
void permute_cube_face_inverse(int face, V &v) {
  permute_cube_face_inverse(face, v[0], v[1], v[2]);
}

/**
 * @brief      Gets the cube facenum, 0-5; order: Z,X,-Y,-X,Y,-Z
 *
 * @param      p     { parameter_description }
 *
 * @tparam     V  { description }
 *
 * @return     The cube facenum.
 */
template <class V>
uint64_t get_cube_facenum(V p) {
  typedef typename V::Scalar Float;
  Float const ax = fabs(p[0]), ay = fabs(p[1]), az = fabs(p[2]);
  uint64_t facenum = 0;
  if (ax >= ay && ax >= az)
    facenum = (p[0] > 0) ? 1 : 3;
  else if (ay >= az && ay >= ax)
    facenum = (p[1] > 0) ? 4 : 2;
  else if (az >= ax && az >= ay)
    facenum = (p[2] > 0) ? 0 : 5;
  return facenum;
}

/**
 * @brief      compute quadsphere coords from *unit* vector
 *
 * @param[in]  p     { parameter_description }
 * @param      x     { parameter_description }
 * @param      y     { parameter_description }
 *
 * @tparam     V     Vector of 3 floats type
 * @tparam     S     Scalar type
 *
 * @return     The coordinates.
 */
template <class V, class S>
uint64_t get_quadsphere_coords(V p, S &x, S &y) {
  uint64_t face = get_cube_facenum(p);
  permute_cube_face_inverse(face, p);
  sphere_to_cube_facenum0(p);
  x = p[0];
  y = p[1];
  assert(fabs(p[2] - 1) < 0.0001);
  return face;
}

/**
 * @brief      Gets the point from quadsphere coordinates.
 *
 * @param[in]  face  The face
 * @param[in]  x     { parameter_description }
 * @param[in]  y     { parameter_description }
 * @param      out   The out
 *
 * @tparam     V     { description }
 * @tparam     S     { description }
 * @tparam     Int   { description }
 */
template <class V, class S, class Int>
V get_point_from_quadsphere_coords(Int face, S x, S y) {
  V out;
  out[0] = x;
  out[1] = y;
  out[2] = 1;
  permute_cube_face(face, out);
  cube_to_sphere(out);
  return out;
}

// old usage from rosetta branch sheffler/scheme/master
// void NestQSph::set_position( uint64_t index, uint64_t resl ) {
//  debug_assert_msg(index < size(resl),"index out of bounds");
//  debug_assert_msg(resl  < 64,"resl >= 0 and < 64");
//  uint64_t const nhier = ONE << (TWO*resl);
//  uint64_t const baseindex = ((index >>TWO*resl)<<resl);
//  uint64_t const hierindex = (index & (nhier-1));
//  uint64_t const hi0 = utility::zorder::undilate<2>(hierindex     );
//  uint64_t const hi1 = utility::zorder::undilate<2>(hierindex>>ONE);
//  set_position(baseindex+hi0,hi1,resl);
//  }
// void NestQSph::set_position( uint64_t index1, uint64_t index2, uint64_t resl
// ) {
//  uint64_t facenum = index1>>resl;
//  // cout << "get_position face: " << facenum << endl;
//  index1 = index1 & ((ONE<<resl)-ONE);
//  debug_assert_msg( facenum < 6,"NestQSph must have 0 <= base_index < 6");
//  uint64_t const nside = (ONE<<resl);
//  double const x = 2.0*((double)index1+0.5)/(double)nside-1.0;
//  double const y = 2.0*((double)index2+0.5)/(double)nside-1.0;
//  V psphere(x,y,1.0);
//  cube_to_sphere(psphere);
//  debug_assert_msg( -1.0 <= psphere[0] && psphere[0] <= 1.0, "chi out of
// bounds" );
//  debug_assert_msg( -1.0 <= psphere[1] && psphere[1] <= 1.0, "psi out of
// bounds" );
//  debug_assert_msg( -1.0 <= psphere[2] && psphere[2] <= 1.0, "psi out of
// bounds" );
//     permute_cube_face(facenum,psphere[0],psphere[1],psphere[2]);
//  managed_xform_ptr_->t = psphere;
//  }
// uint64_t NestQSph::get_index( Xform position, uint64_t resl ) const {
//  V p = position.t.normalized();
//  uint64_t const facenum = get_cube_facenum(p);
//  // cout << "get_index face " << facenum << endl;

//  // cout << facenum << endl;
//  inverse_permute_cube_face(facenum,p[0],p[1],p[2]); // move to
// face 0
//  sphere_to_cube_facenum0(p);

//  uint64_t const nside = (ONE<<resl);
//  double const delta0 = p[0]/2.0 + 0.5;
//  double const delta1 = p[1]/2.0 + 0.5;
//  uint64_t const index0unbound = delta0 * (double)nside;
//  uint64_t const index1unbound = delta1 * (double)nside;
//  uint64_t const index0 = std::min(index0unbound,nside-ONE);
//  uint64_t const index1 = std::min(index1unbound,nside-ONE);
//  uint64_t const hi0 = utility::zorder::dilate<2>( index0 & (nside-ONE) );
//  uint64_t const hi1 = utility::zorder::dilate<2>( index1 & (nside-ONE) );
//  uint64_t const index = facenum<<(TWO*resl) | hi0 | (hi1<<ONE);
//  return index;
//  }

}  // numeric
}  // scheme
