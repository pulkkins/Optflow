
#include "VectorFieldIllustrator.h"

#include "CImg_config.h"
#include <CImg.h>
#include <stdexcept>

#include <iostream>

using namespace cimg_library;

/*double complement(double v)
{
  return 255.0 - v;
}*/

void VectorFieldIllustrator::renderDenseVectorField(const CImg< double > &V,
                                                    CImg< unsigned char > &I,
                                                    int spacing)
{
  const int W = V.width();
  const int H = V.height();
  
  CImg< double > qualityMap(V.get_shared_channel(2).get_equalize(256).normalize(0.0, 255.0));
  CImg< double > qualityMapComplement = 255.0 - qualityMap; //.get_apply(complement);
  
  CImg< unsigned char > colorMap(W, H, 1, 3);
  colorMap.get_shared_channel(0).assign(qualityMapComplement);
  colorMap.get_shared_channel(1).assign(qualityMap);
  colorMap.get_shared_channel(2).fill(127);
  
  I.draw_quiver(V.get_shared_channels(0, 1), colorMap, 1, spacing, 1.0f);
}

#ifdef WITH_CGAL
void VectorFieldIllustrator::renderSparseVectorField(const SparseVectorField &V,
                                                     CImg< unsigned char > &I)
{
  vector< Point >::const_iterator fpIter1 = V.getStartPoints().begin();
  vector< Point >::const_iterator fpIter2 = V.getEndPoints().begin();
  
  unsigned char color[3];
  
  /*Delaunay dt;
    list< Point_2 > dtPoints;
    for(fpIter1 = featurePoints1.begin(); fpIter1 != featurePoints1.end(); fpIter1++)
    dtPoints.push_back(Point_2(fpIter1->getX(), fpIter1->getY()));
    dt.insert(dtPoints.begin(), dtPoints.end());
  
    Delaunay::Finite_edges_iterator eIter;*/
  //Delaunay::Face_iterator fIter;
  
  color[0] = 120;
  color[1] = 60;
  color[2] = 60;
  
  for(list< int >::const_iterator iIter = V.getTriIndices().begin(); iIter != V.getTriIndices().end(); iIter++)
  {
    int x1 = V.getStartPoint(*iIter).x();
    int y1 = V.getStartPoint(*iIter).y();
    iIter++;
    int x2 = V.getStartPoint(*iIter).x();
    int y2 = V.getStartPoint(*iIter).y();
    iIter++;
    int x3 = V.getStartPoint(*iIter).x();
    int y3 = V.getStartPoint(*iIter).y();
    
    I.draw_line(x1, y1, x2, y2, color);
    I.draw_line(x2, y2, x3, y3, color);
    I.draw_line(x3, y3, x1, y1, color);
  }
	
  color[0] = 100;
  color[1] = 220;
  color[2] = 100;
  
  for(; fpIter1 != V.getStartPoints().end(); )
  {
    I.draw_arrow(fpIter1->x(), fpIter1->y(),
                 fpIter2->x(), fpIter2->y(),
                 color, 1, 40, -25);
    
    fpIter1++;
    fpIter2++;
  }
	
  /*for(eIter = dt.finite_edges_begin(); eIter != dt.finite_edges_end(); eIter++)
    {
    Delaunay::Edge e = *eIter;
    Point_2 p = e.first->vertex(dt.cw(e.second))->point();
    int x1 = p.x();
    int y1 = p.y();
    p = e.first->vertex(dt.ccw(e.second))->point();
    int x2 = p.x();
    int y2 = p.y();
		
    dest.draw_line(x1, y1, x2, y2, color);
    }*/
	
  // vertex coordinates
  /*std::map<int, Delaunay::Vertex_handle> V;
    int inum = 0;
    for(Delaunay::Vertex_iterator vit = dt.vertices_begin(); 
    vit != dt.vertices_end(); 
    ++vit) {
    //V[inum++] = vit;
    int x = vit->point();
    }*/

  /*for(fIter = dt.faces_begin(); fIter != dt.faces_end(); fIter++)
    {
    for(int i = 0; i < 3; i++)
    {
    int x = fIter->vertex(i);
    int y = fIter->vertex(i);
    }
    //int x = dt->;
    }*/
	
  /*typedef CGAL::Triangulation_2< CGAL::Exact_predicates_inexact_constructions_kernel > Triangulation;
    Triangulation triangulation;
    Triangulation::Point point;
	
    list< Triangulation::Point > points;
    for(fpIter1 = featurePoints1.begin(); fpIter1 != featurePoints1.end(); fpIter1++)
    points.push_back(Triangulation::Point(fpIter1->getX(), fpIter1->getY()));
	
    triangulation.insert(points.begin(), points.end());
	
    Triangulation::Vertex_circulator vc = triangulation.incident_vertices(triangulation.infinite_vertex()), done(vc);*/
}

/*void VectorFieldIllustrator::renderVectorField(const VectorField &V, 
					       CImg< unsigned char > &I)
{
  const DenseVectorField *V_dense = dynamic_cast< const DenseVectorField * >(&V);
  const SparseVectorField *V_sparse = dynamic_cast< const SparseVectorField * >(&V);
	
  if(V_dense != NULL)
    renderDenseVectorField(*V_dense, I);
  else if(V_sparse != NULL)
    renderSparseVectorField(*V_sparse, I);
  else
    throw invalid_argument("invalid vector field type");
}*/
#endif
