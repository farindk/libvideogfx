/********************************************************************************
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

#include "libvideogfx/graphics/geometry/matrix.hh"
#include <libvideogfx/error.hh>
#include <math.h>
#include <iomanip>

namespace videogfx {
  using namespace std;

  Matrix4G::Matrix4G()
  {
    d_rows=d_columns=0;
  }

  Matrix4G::Matrix4G(const Matrix4G& m)
  {
    d_rows=m.d_rows;
    d_columns=m.d_columns;
    for (int r=0;r<d_rows;r++)
      for (int c=0;c<d_columns;c++)
	d_mat[r][c] = m[r][c];
  }

  Matrix4G::Matrix4G(int rows,int columns)
  {
    d_rows=rows;
    d_columns=columns;

    for (int r=0;r<d_rows;r++)
      for (int c=0;c<d_columns;c++)
	d_mat[r][c] = ((r==c) ? 1 : 0);
  }

  Matrix4G   Matrix4G::operator+(const Matrix4G& m) const
  {
    AssertDescr(d_rows    == m.d_rows,    "matrix dimensions differ (rows)");
    AssertDescr(d_columns == m.d_columns, "matrix dimensions differ (columns)");

    Matrix4G mat(d_rows,d_columns);

    for (int r=0;r<d_rows;r++)
      for (int c=0;c<d_columns;c++)
	mat.d_mat[r][c] = d_mat[r][c] + m.d_mat[r][c];

    return mat;
  }

  Matrix4G   Matrix4G::operator-(const Matrix4G& m) const
  {
    AssertDescr(d_rows    == m.d_rows,    "matrix dimensions differ (rows)");
    AssertDescr(d_columns == m.d_columns, "matrix dimensions differ (columns)");

    Matrix4G mat(d_rows,d_columns);

    for (int r=0;r<d_rows;r++)
      for (int c=0;c<d_columns;c++)
	mat.d_mat[r][c] = d_mat[r][c] - m.d_mat[r][c];

    return mat;
  }

  Matrix4G   Matrix4G::operator*(const Matrix4G& b) const
  {
    const Matrix4G& a = *this;

    AssertDescr(a.d_columns == b.d_rows, "matrix sizes are incompatible for multiplication");

    Matrix4G mat(a.d_rows,b.d_columns);

    for (int i=0;i<a.d_rows;i++)
      for (int j=0;j<b.d_columns;j++)
	{
	  double sum=0.0;
	  for (int k=0;k<a.d_columns;k++)
	    {
	      sum += a.d_mat[i][k] * b.d_mat[k][j];
	    }

	  mat.d_mat[i][j] = sum;
	}

    return mat;
  }

  Matrix4G Matrix4G::operator*(double factor) const
  {
    Matrix4G mat(d_rows,d_columns);

    for (int i=0;i<d_rows;i++)
      for (int j=0;j<d_columns;j++)
	{
	  mat.d_mat[i][j] = d_mat[i][j] * factor;
	}

    return mat;
  }

  Matrix4G Matrix4G::operator/(double factor) const
  {
    //AssertDescr(factor != 0.0, "Cannot divide matrix through zero.");

    Matrix4G mat(d_rows,d_columns);

    for (int i=0;i<d_rows;i++)
      for (int j=0;j<d_columns;j++)
	{
	  mat.d_mat[i][j] = d_mat[i][j] / factor;
	}

    return mat;
  }

  const Matrix4G& Matrix4G::operator=(const Matrix4G& m)
  {
    d_rows    = m.d_rows;
    d_columns = m.d_columns;

    Assert(d_rows<=4);
    Assert(d_columns<=4);

    for (int i=0;i<d_rows;i++)
      for (int j=0;j<d_columns;j++)
	d_mat[i][j] = m.d_mat[i][j];

    return *this;
  }

  Matrix4G Matrix4G::Transpose() const
  {
    Matrix4G mat(d_columns, d_rows);

    for (int i=0;i<d_rows;i++)
      for (int j=0;j<d_columns;j++)
	mat.d_mat[j][i] = d_mat[i][j];

    return mat;
  }

  double   Matrix4G::Det() const
  {
    if (d_rows==3 && d_columns==3)
      {
	return (d_mat[0][0]*(d_mat[1][1]*d_mat[2][2] - d_mat[1][2]*d_mat[2][1]) +
		d_mat[0][1]*(d_mat[1][2]*d_mat[2][0] - d_mat[2][2]*d_mat[1][0]) +
		d_mat[0][2]*(d_mat[1][0]*d_mat[2][1] - d_mat[2][0]*d_mat[1][1]));
      }
    else
      {
	NotImplemented;
      }
  }

  double   Matrix4G::Trace() const
  {
    AssertDescr(d_columns == d_rows, "matrix sizes are incompatible for multiplication");

    double sum=0.0;
    for (int i=0; i<d_rows; i++)
      sum += d_mat[i][i];

    return sum;
  }

  Matrix4G Matrix4G::Inverse() const
  {
    AssertDescr(d_rows == d_columns, "matrix must be square for inverse computation");

    Matrix4G inv(d_rows,d_rows);

    if (d_rows==2)
      {
	double det = d_mat[0][0]*d_mat[1][1] - d_mat[0][1]*d_mat[1][0];
	inv[0][0] = d_mat[1][1]/det;
	inv[0][1] = d_mat[0][1]/det;
	inv[1][0] = d_mat[1][0]/det;
	inv[1][1] = d_mat[0][0]/det;

	return inv;
      }
    else if (d_rows==3)
      {
	inv[0][0] = +(d_mat[1][1]*d_mat[2][2] - d_mat[1][2]*d_mat[2][1]);
	inv[0][1] = -(d_mat[2][2]*d_mat[0][1] - d_mat[2][1]*d_mat[0][2]);
	inv[0][2] = +(d_mat[1][2]*d_mat[0][1] - d_mat[1][1]*d_mat[0][2]);

	inv[1][0] = -(d_mat[2][2]*d_mat[1][0] - d_mat[2][0]*d_mat[1][2]);
	inv[1][1] = +(d_mat[2][2]*d_mat[0][0] - d_mat[2][0]*d_mat[0][2]);
	inv[1][2] = -(d_mat[1][2]*d_mat[0][0] - d_mat[1][0]*d_mat[0][2]);

	inv[2][0] = +(d_mat[2][1]*d_mat[1][0] - d_mat[2][0]*d_mat[1][1]);
	inv[2][1] = -(d_mat[2][1]*d_mat[0][0] - d_mat[2][0]*d_mat[0][1]);
	inv[2][2] = +(d_mat[1][1]*d_mat[0][0] - d_mat[1][0]*d_mat[0][1]);

	return inv/Det();
      }
    else
      {
	AssertDescr(0,"TODO: write something that does not depend on GSL");
#if 0
	gsl_matrix* matrix   = gsl_matrix_alloc(d_rows,d_rows);
	gsl_matrix* inverse  = gsl_matrix_alloc(d_rows,d_rows);
	gsl_permutation* per = gsl_permutation_calloc(d_rows);

	// build linear equation system to be solved

	for (int i=0;i<d_rows;i++)
	  for (int j=0;j<d_columns;j++)
	    {
	      gsl_matrix_set(matrix,i,j, d_mat[i][j]);
	    }

	// solve the equation system using LU-decomposition

	int signum=0;
	gsl_linalg_LU_decomp(matrix,per,&signum);
	gsl_linalg_LU_invert(matrix,per, inverse);

	Matrix4G inv(d_rows,d_columns);

	for (int i=0;i<d_rows;i++)
	  for (int j=0;j<d_columns;j++)
	    {
	      inv[i][j] = gsl_matrix_get(inverse,i,j);
	    }

	// cleanup

	gsl_permutation_free(per);
	gsl_matrix_free(matrix);
	gsl_matrix_free(inverse);

	return inv;
#endif
      }
  }


  Matrix4G Matrix4G::PseudoInverse() const
  {
    Matrix4G T = Transpose();
    return (T*(*this)).Inverse() * T;
  }


  void Matrix4G::Threshold(double t)
  {
    for (int i=0;i<d_rows;i++)
      for (int j=0;j<d_columns;j++)
	if (d_mat[i][j]>-t && d_mat[i][j]<t)
	  d_mat[i][j]=0.0;
  }

  double Matrix4G::Norm() const
  {
    double sum = 0.0;
    
    for (int i=0;i<d_rows;i++)
      for (int j=0;j<d_columns;j++)
	{
	  sum += d_mat[i][j] * d_mat[i][j];
	}

    return sqrt(sum);
  }

  void Matrix4G::DeleteColumn(int column)
  {
    d_columns--;

    for (int j=column;j<d_columns;j++)
      for (int i=0;i<d_rows;i++)
	d_mat[i][j] = d_mat[i][j+1];
  }

  void Matrix4G::DeleteRow(int row)
  {
    d_rows--;

    for (int i=row;i<d_rows;i++)
      for (int j=0;j<d_columns;j++)
	d_mat[i][j] = d_mat[i+1][j];
  }

  void Matrix4G::AddColumn()
  {
    assert(d_columns<4);
    d_columns++;
  }

  void Matrix4G::AddRow()
  {
    assert(d_rows<4);
    d_rows++;
  }

  Matrix4G Matrix4G::SubMatrix(int row0,int col0, int h,int w) const
  {
    Matrix4G sub(w,h);

    for (int r=0;r<h;h++)
      for (int c=0;c<w;c++)
	sub[r][c]=d_mat[r+row0][c+col0];

    return sub;
  }


  Matrix4G CrossProductMatrix(const Matrix4G& v)
  {
    assert(v.AskRows()==3);
    assert(v.AskColumns()==1);

    Matrix4G mat(3,3);
    mat[0][0]=    0   ; mat[0][1]=-v[2][0]; mat[0][2]= v[1][0];
    mat[1][0]= v[2][0]; mat[1][1]=    0   ; mat[1][2]=-v[0][0];
    mat[2][0]=-v[1][0]; mat[2][1]= v[0][0]; mat[2][2]=    0   ;

    return mat;
  }

  Matrix4G GivensRotation(int row1,int row2, double angle,int dimension)
  {
    Matrix4G mat(dimension,dimension);

    mat[row1][row1] = mat[row2][row2] = cos(angle);
    double s = sin(angle);
    mat[row2][row1] = -s;
    mat[row1][row2] =  s;

    return mat;
  }

  Matrix4G CCWRotationX(double angle,int dim) { return GivensRotation(1,2,angle,dim); }
  Matrix4G CCWRotationY(double angle,int dim) { return GivensRotation(2,0,angle,dim); }
  Matrix4G CCWRotationZ(double angle,int dim) { return GivensRotation(0,1,angle,dim); }

  Matrix4G CWRotationX(double angle,int dim) { return GivensRotation(2,1,angle,dim); }
  Matrix4G CWRotationY(double angle,int dim) { return GivensRotation(0,2,angle,dim); }
  Matrix4G CWRotationZ(double angle,int dim) { return GivensRotation(1,0,angle,dim); }

  Matrix4G Translation3D(double tx,double ty,double tz)
  {
    Matrix4G t(4,4);
    t[0][3]=tx;
    t[1][3]=ty;
    t[2][3]=tz;
    return t;
  }

  Matrix4G Translation2D(double tx,double ty)
  {
    Matrix4G t(3,3);
    t[0][2]=tx;
    t[1][2]=ty;
    return t;
  }

  Matrix4G CrossProduct(const Matrix4G& vec1, const Matrix4G& vec2)
  {
    assert(vec1.AskRows()==3);
    assert(vec1.AskColumns()==1);
    assert(vec2.AskRows()==3);
    assert(vec2.AskColumns()==1);

    Matrix4G result(3,1);

    result[0][0] = vec1[1][0]*vec2[2][0] - vec2[1][0]*vec1[2][0];
    result[1][0] = vec1[2][0]*vec2[0][0] - vec2[2][0]*vec1[0][0];
    result[2][0] = vec1[0][0]*vec2[1][0] - vec2[0][0]*vec1[1][0];

    return result;
  }

  Point2D<double> TransformPoint(const Matrix4G& mat, Point2D<double>& p)
  {
    assert(mat.AskRows()==3);
    assert(mat.AskColumns()==3);

    double x = p.x*mat[0][0] + p.y*mat[0][1] + mat[0][2];
    double y = p.x*mat[1][0] + p.y*mat[1][1] + mat[1][2];
    double w = p.x*mat[2][0] + p.y*mat[2][1] + mat[2][2];

    Point2D<double> newpoint;
    newpoint.x = x/w;
    newpoint.y = y/w;

    return newpoint;
  }

  void Line2PointPair(const Matrix4G& homoline, Point2D<double>* pts, double distance)
  {
    assert(homoline.AskRows()==3);
    assert(homoline.AskColumns()==1);

    double n[3]; // line parameters, normalized such that n_0^2 + n_1^2 = 1

    double len = sqrt(homoline[0][0]*homoline[0][0] + homoline[1][0]*homoline[1][0]);
    for (int i=0;i<3;i++)
      n[i] = homoline[i][0]/len;

    // find vector from origin to line that is perpendicular to line
    pts[0].x = -n[2]*n[0];
    pts[0].y = -n[2]*n[1];

    // move from that point the 'distance' along the line in both directions
    pts[1].x = pts[0].x - distance*n[1];
    pts[1].y = pts[0].y + distance*n[0];
    pts[0].x += distance*n[1];
    pts[0].y -= distance*n[0];
  }

  ostream& operator<<(ostream& ostr, const Matrix4G& mat)
  {
    for (int i=0;i<mat.AskRows();i++)
      {
	for (int j=0;j<mat.AskColumns();j++)
	  {
	    ostr << setprecision(3) << setw(6) << mat[i][j] << ' ';
	  }

	ostr << endl;
      }

    return ostr;
  }

}
