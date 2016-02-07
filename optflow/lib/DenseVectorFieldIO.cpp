
#include "DenseVectorFieldIO.h"
#include "PXMFileUtils.h"

#include "CImg_config.h"
#include <CImg.h>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>

static const int TOKEN_SIZE = 100;

void DenseVectorFieldIO::readVectorField(const string &inFileName,
                                         CImg< double > &V)
{
  char token[TOKEN_SIZE];
  int width, height;
  float u, v;
  int x, y;
  float q;
  int qc;
  int numQualityChannels;
  
  ifstream inputStream(inFileName.c_str(), ios::binary | ios::in);
  if(!inputStream)
    throw runtime_error("File not found.");
  
  PNMFileUtils::checkMagicNumber(inputStream, "PDV");
  
  // get dimensions
  PNMFileUtils::extractToken(inputStream, token, TOKEN_SIZE);
  width = atoi(token);
  PNMFileUtils::extractToken(inputStream, token, TOKEN_SIZE);
  height = atoi(token);
  
  // read the number of quality channels
  PNMFileUtils::extractToken(inputStream, token, TOKEN_SIZE);
  numQualityChannels = atoi(token);
  char ch;
  inputStream.read((char *)&ch, sizeof(char));
  
  V = CImg< double >(width, height, 1, 2 + numQualityChannels);
  
  // read data
  for(y = 0; y < height; y++)
  {
    for(x = 0; x < width; x++)
    {
      inputStream.read((char *)&u, sizeof(float));
      inputStream.read((char *)&v, sizeof(float));
      V(x, y, 0) = u;
      V(x, y, 1) = v;
      
      for(qc = 0; qc < numQualityChannels; qc++)
      {
        inputStream.read((char *)&q, sizeof(float));
        V(x, y, 2 + qc) = q;
      }
    }
  }
  
  inputStream.close();
}

void DenseVectorFieldIO::writeVectorField(const CImg< double > &V,
                                          const string &outFileName)
{
  float u, v;
  int x, y;
  float q;
  int qc;
  
  ofstream outputStream(outFileName.c_str(), ios::binary | ios::out);
  if(!outputStream)
    throw runtime_error("Error creating file");
  
  char header[100];
  sprintf(header,"PDV\n%d %d\n%d\n", V.width(), V.height(), V.spectrum() - 2);
  outputStream.write(header, strlen(header));
  
  // write data
  for(y = 0; y < V.height(); y++)
  {
    for(x = 0; x < V.width(); x++)
    {
      u = V(x, y, 0);
      v = V(x, y, 1);
      outputStream.write((const char *)&u, sizeof(float));
      outputStream.write((const char *)&v, sizeof(float));
      for(qc = 0; qc < V.spectrum() - 2; qc++)
      {
        q = V(x, y, 2 + qc);
        outputStream.write((const char *)&q, sizeof(float));
      }
    }
  }
  
  outputStream.close();
}
