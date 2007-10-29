/*********************************************************************
  libvideogfx/graphics/geometry/matrix.hh

  purpose:
    Matrix computations for computer graphics.

  notes:

  to do:

  author(s):
   - Dirk Farin, farin@informatik.uni-mannheim.de

  modifications:
   26/Oct/2002 - Dirk Farin
    - initial implementation
 ********************************************************************************
    LibVideoGfx - video processing library
    Copyright (C) 2002  Dirk Farin

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************************************/

#ifndef LIBVIDEOGFX_GRAPHICS_GEOMETRY_MATRIX_HH
#define LIBVIDEOGFX_GRAPHICS_GEOMETRY_MATRIX_HH

#include <libvideogfx/graphics/datatypes/primitives.hh>
#include <iostream>

namespace videogfx {

  class Matrix4G
  {
  public:
    Matrix4G();
    Matrix4G(const Matrix4G&);
    Matrix4G(int rows,int columns);  // will be initialized with an identity matrix

    int AskRows() const    { return d_rows;    }
    int AskColumns() const { return d_columns; }

    double*       operator[](int i)       { return d_mat[i]; }
    const double* operator[](int i) const { return d_mat[i]; }

    Matrix4G operator+(const Matrix4G&) const;
    Matrix4G operator-(const Matrix4G&) const;
    Matrix4G operator*(const Matrix4G&) const;
    Matrix4G operator*(double factor) const;
    Matrix4G operator/(double factor) const;
    const Matrix4G& operator=(const Matrix4G&);

    Matrix4G Transpose() const;
    Matrix4G Inverse() const;
    Matrix4G PseudoInverse() const;

    double Det() const;
    double Trace() const;
    double Norm() const; // L_2 norm / Frobenius norm

    void Threshold(double t=0.000001); /* set all entries to zero for which |value|<t */

    void DeleteColumn(int column);
    void DeleteRow(int row);
    void AddRow();
    void AddColumn();

    Matrix4G SubMatrix(int row0,int col0, int h,int w) const;

  private:
    int d_rows,d_columns;
    double d_mat[4][4];
  };

  Matrix4G ColumnVector(double a,double b);
  Matrix4G ColumnVector(double a,double b,double c);
  Matrix4G ColumnVector(double a,double b,double c,double d);
  Matrix4G RowVector(double a,double b);
  Matrix4G RowVector(double a,double b,double c);
  Matrix4G RowVector(double a,double b,double c,double d);

  Matrix4G CWRotationX(double angle,int dim=3); // clockwise when looking in positive axis direction
  Matrix4G CWRotationY(double angle,int dim=3); // clockwise when looking in positive axis direction
  Matrix4G CWRotationZ(double angle,int dim=3); // clockwise when looking in positive axis direction

  Matrix4G CCWRotationX(double angle,int dim=3); // counter-clockwise when looking in positive axis direction
  Matrix4G CCWRotationY(double angle,int dim=3); // counter-clockwise when looking in positive axis direction
  Matrix4G CCWRotationZ(double angle,int dim=3); // counter-clockwise when looking in positive axis direction

  Matrix4G Translation3D(double tx,double ty,double tz);
  Matrix4G Translation2D(double tx,double ty);
  Matrix4G Scale3D(double x,double y,double z);

  // cross product of two 3x1 vectors
  Matrix4G CrossProduct(const Matrix4G& vec1, const Matrix4G& vec2);
  Matrix4G CrossProductMatrix(const Matrix4G& v);  // generates [v]_x

  Point2D<double> TransformPoint(const Matrix4G& mat, Point2D<double>& p);

  /* Return two points that lie on the line 'homoline' (described in homogeneous parameters).
     A vector from the origin that is perpendicular to the line is taken and from that point, two
     points at the given distance are returned. */
  void Line2PointPair(const Matrix4G& homoline, Point2D<double>* pts, double distance=10000.0);

  std::ostream& operator<<(std::ostream&, const Matrix4G&);
}

#endif
