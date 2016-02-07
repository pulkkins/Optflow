
#ifdef WITH_CGAL

#include "PXMFileUtils.h"
#include "SparseVectorField.h"
#include "SparseVectorFieldIO.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>

static const int TOKEN_SIZE = 100;

void SparseVectorFieldIO::readVectorField(const string &inFileName,
                                          SparseVectorField &V)
{
  int numVectors;
  char token[TOKEN_SIZE];
  float x, y;
  float u, v;
  
  ifstream inputStream(inFileName.c_str(), ios::binary | ios::in);
  if(!inputStream)
    throw runtime_error("File not found.");
  
  PNMFileUtils::checkMagicNumber(inputStream, "PSV");
  
  // get the number of vectors
  PNMFileUtils::extractToken(inputStream, token, TOKEN_SIZE);
  numVectors = atoi(token);
  char ch;
  inputStream.read(&ch, sizeof(char));
  
  for(int i = 0; i < numVectors; i++)
  {
    inputStream.read((char *)&x, sizeof(float));
    inputStream.read((char *)&y, sizeof(float));
    inputStream.read((char *)&u, sizeof(float));
    inputStream.read((char *)&v, sizeof(float));
    
    V.addVector(Point(x, y), Point(x + u, y + v));
  }
  
  inputStream.close();
}

void SparseVectorFieldIO::writeVectorField(const SparseVectorField &V,
                                           const string &outFileName)
{
  float x, y;
  float u, v;
  Point p1;
  Point p2;
  
  ofstream outputStream(outFileName.c_str(), ios::binary | ios::out);
  if(!outputStream)
    throw runtime_error("Error creating file");
  
  char header[100];
  sprintf(header, "PSV\n%d\n", V.getNumVectors());
  outputStream.write(header, strlen(header));
  
  for(int i = 0; i < V.getNumVectors(); i++)
  {
    p1 = V.getStartPoint(i);
    x = p1.x();
    y = p1.y();
    outputStream.write((const char *)&x, sizeof(float));
    outputStream.write((const char *)&y, sizeof(float));
    p2 = V.getEndPoint(i);
    u = p2.x() - p1.x();
    v = p2.y() - p1.y();
    outputStream.write((const char *)&u, sizeof(float));
    outputStream.write((const char *)&v, sizeof(float));
  }
  
  outputStream.close();
}

#endif
