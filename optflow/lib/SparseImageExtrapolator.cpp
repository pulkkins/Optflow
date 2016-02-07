
#ifdef WITH_CGAL

#include "SparseImageExtrapolator.h"
#include "SparseVectorField.h"

#include "CImg_config.h"
#include <CImg.h>

void SparseImageExtrapolator::constructUniformMesh(int w, int h, int n,
                                                   vector< Point > &points,
                                                   list< int > &indices) const
{
  const int POLYWIDTH = w / n;
  const int POLYHEIGHT = h / n;
  
  for(int y = 0; y <= h; y += POLYHEIGHT)
    for(int x = 0; x <= w; x += POLYWIDTH)
      points.push_back(Point(x, y));
  
  for(int yi = 0; yi < n; yi++)
  {
    for(int xi = 0; xi < n; xi++)
    {
      indices.push_back(xi + yi * (n + 1));
      indices.push_back(xi + 1 + yi * (n + 1));
      indices.push_back(xi + 1 + (yi + 1) * (n + 1));
      
      indices.push_back(xi + yi * (n + 1));
      indices.push_back(xi + 1 + (yi + 1) * (n + 1));
      indices.push_back(xi + (yi + 1) * (n + 1));
    }
  }
}

void SparseImageExtrapolator::extrapolate(const CImg< unsigned char > &I0,
                                          const CImg< double > &V,
                                          double multiplier,
                                          double spacing,
                                          CImg< unsigned char > &Ie) const
{
  vector< Point > points1;
  vector< Point > points2;
  list< int > indices;
  double u, v;
  int x, y;
  
  constructUniformMesh(I0.width(), I0.height(), spacing, points1, indices);
  
  points2.resize(points1.size());
  for(int i = 0; i < points1.size(); i++)
  {
    x = points1[i].x();
    y = points1[i].y();
    u = V(x, y, 0);
    v = V(x, y, 1);
    points2[i] = Point(x + multiplier * u, y + multiplier * v);
  }
  
  extrapolate_(I0, points1, points2, indices, 1.0, Ie);
}

void SparseImageExtrapolator::extrapolate(const CImg< unsigned char > &I0,
                                          const SparseVectorField &V,
                                          double multiplier,
                                          CImg< unsigned char > &Ie) const
{
  extrapolate_(I0, V.getStartPoints(), V.getEndPoints(),
               V.getTriIndices(), multiplier, Ie);
}

void SparseImageExtrapolator::extrapolate_(const CImg< unsigned char > &I0,
                                           const vector< Point > &featurePoints1,
                                           const vector< Point > &featurePoints2,
                                           const list< int > &triIndices,
                                           double multiplier,
                                           CImg< unsigned char > &Ie) const
{
  int index[3];
  int x1[3], y1[3];
  int x2[3], y2[3];
  CImg< unsigned char > texture;
  double v1x, v1y, v2x, v2y;
  
  Ie = CImg< unsigned char >(I0.width(), I0.height(), 1, 1);
  Ie.fill(0);
  
  for(list< int > ::const_iterator i = triIndices.begin(); i != triIndices.end(); )
  {
    for(int ii = 0; ii < 3; ii++)
    {
      index[ii] = *i;
      x1[ii] = featurePoints1[*i].x();
      y1[ii] = featurePoints1[*i].y();
      x2[ii] = x1[ii] + multiplier * (featurePoints2[*i].x() - x1[ii]);
      y2[ii] = y1[ii] + multiplier * (featurePoints2[*i].y() - y1[ii]);
      i++;
    }
    
    // Draw a warped triangle to the destination image with 
    // the source image as a texture. Only triangles 
    // with counterclockwise vertex order are drawn.
    
    // Use the cross product formula.
    v1x = x1[1] - x1[0];
    v1y = y1[1] - y1[0];
    v2x = x1[2] - x1[0];
    v2y = y1[2] - y1[0];
    
    if(v1x * v2y - v1y * v2x > 0.0)
      Ie.draw_triangle(x2[0], y2[0], x2[1], y2[1], x2[2], y2[2], I0,
                       x1[0], y1[0], x1[1], y1[1], x1[2], y1[2]);
  }
}

#endif
