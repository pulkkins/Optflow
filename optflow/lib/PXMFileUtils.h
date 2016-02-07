
#ifndef PNMFILEUTILS_H

#include <fstream>
#include <string>

using namespace std;

/// Implements utility methods for reading PNM/PDVM/PSVM files.
class PNMFileUtils
{
public:
  /// Checks the magic number (the identifier at the beginning of the file).
  static void checkMagicNumber(ifstream &inputStream, const string &id);
  
  /// Extracts a token (a character sequence terminated by whitespace or end of row) from the given stream.
  static void extractToken(ifstream &inputStream, char *token, int maxSize);
};

#define PNMFILEUTILS_H

#endif
