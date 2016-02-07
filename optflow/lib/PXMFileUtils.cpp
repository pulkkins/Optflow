
#include "PXMFileUtils.h"

#include <iostream>
#include <stdexcept>
#include <string.h>

static const int TOKEN_SIZE = 100;

void PNMFileUtils::checkMagicNumber(ifstream &inputStream, const string &id)
{
  char token[TOKEN_SIZE];
  
  extractToken(inputStream, token, TOKEN_SIZE);
  if(strcmp(token, id.c_str()) != 0)
  {
    cerr<<"Magic number "<<token<<" != "<<id<<endl;
    throw runtime_error("Bad magic number");
  }
}

void PNMFileUtils::extractToken(ifstream &inputStream,
                                char *token,
                                int maxSize)
{
  int count = 0;
  char ch;
  
  // skip whitespace and comments
  do
  {
    inputStream.read((char *)&ch, sizeof(char));
    // if a comment char is found then read till the next "\n"
    if(ch == '#')
      while(ch != '\n')
        inputStream.read((char *)&ch, sizeof(char *));
  }
  while(ch == ' ' || ch == '\t' || ch == '\n');
  
  // copy data into token
  do
  {
    if(count >= maxSize - 1)
      throw runtime_error("Token too large");
    token[count++] = ch;
    inputStream.read((char *)&ch, sizeof(char));
  }
  while(ch != ' ' && ch != '\t' && ch != '\n' && ch != '.');
  
  inputStream.putback(ch);
  token[count] = '\0';
}
