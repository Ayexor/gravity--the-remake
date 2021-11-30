#pragma once

//////////////////////////////////////////////////////////////
// 2D Matrix for linear algebra transformations of vectors
//////////////////////////////////////////////////////////////

struct mat2
{
  double a,b,c,d;

  mat2& operator +=(const mat2& rhs)
  {
    a += rhs.a;
    b += rhs.b;
    c += rhs.c;
    d += rhs.d;
    return *this;
  }

  mat2& operator -=(const mat2& rhs)
  {
    a -= rhs.a;
    b -= rhs.b;
    c -= rhs.c;
    d -= rhs.d;
    return *this;
  }

  mat2& operator *= (const double& fac)
  {
    a *= fac;
    b *= fac;
    c *= fac;
    d *= fac;
    return *this;
  }

  mat2& operator /= (const double& fac)
  {
    a /= fac;
    b /= fac;
    c /= fac;
    d /= fac;
    return *this;
  }
  
  friend mat2 operator+ (mat2 mata, const mat2& matb)
  {
    mata.a += matb.a;
    mata.b += matb.b;
    mata.c += matb.c;
    mata.d += matb.d;
    return mata;
  }

  friend mat2 operator- (mat2 mata, const mat2& matb)
  {
    mata.a -= matb.a;
    mata.b -= matb.b;
    mata.c -= matb.c;
    mata.d -= matb.d;
    return mata;
  }

  friend mat2 operator* (const mat2& mata, const mat2& matb)
  {
    mat2 matc;
    matc.a += mata.a * matb.a + mata.b * matb.c;
    matc.b += mata.a * matb.b + mata.b * matb.d;
    matc.c += mata.c * matb.a + mata.d * matb.c;
    matc.d += mata.c * matb.b + mata.d * matb.d;
    return matc;
  }
  
  friend mat2 operator* (double fac, mat2 mata)
  {
    mata *= fac;
    return mata;
  }

  friend mat2 operator/ (mat2 mata, double fac)
  {
    mata /= fac;
    return mata;
  }
  
  friend std::ostream& operator<<(std::ostream& os, const mat2& mata)
  {
    os << "(" << mata.a << ", " << mata.b << "; " << mata.c << ", " << mata.d << ")";
    return os;
  }
};
