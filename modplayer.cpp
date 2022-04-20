#include <stdio.h>
#include "modfile.h"

#include <string>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <numeric>

const int SAMPLE_NAME_LENGTH = 22;
const int SAMPLE_SAMPLE_LENGTH_LENGTH = 2;
const int SAMPLE_FINETUNE_LENGTH = 1;
const int SAMPLE_VOLUME_LENGTH = 1;
const int SAMPLE_REPEAT_OFFSET_LENGTH = 2;
const int SAMPLE_REPEAT_LENGTH = 2;

const int FILE_NAME_LENGTH = 20;
const int FILE_NUMBER_OF_PATTERNS_LENGTH = 1;
const int FILE_SONG_END_LENGTH = 1;
const int FILE_PATTERN_TABLE_LENGTH = 128;
const int FILE_FORMAT_TAG_LENGTH = 4;

const int SAMPLE_LENGTH = (
  SAMPLE_NAME_LENGTH +
  SAMPLE_SAMPLE_LENGTH_LENGTH +
  SAMPLE_FINETUNE_LENGTH +
  SAMPLE_VOLUME_LENGTH +
  SAMPLE_REPEAT_OFFSET_LENGTH +
  SAMPLE_REPEAT_LENGTH
);

const int NUMBER_OF_SAMPLES = 32;

typedef unsigned char BYTE;
using namespace std;

vector<BYTE> readFile(string filename) {
  // https://stackoverflow.com/questions/15366319/how-to-read-the-binary-file-in-c
  // open the file
  streampos fileSize;
  ifstream file(filename, ios::binary);

  // get its size:
  file.seekg(0, ios::end);
  fileSize = file.tellg();
  file.seekg(0, ios::beg);

  // read the data:
  vector<BYTE> fileData(fileSize);
  file.read((char*) &fileData[0], fileSize);

  return fileData;
}

int main() {
  string aaa = "rainysum.mod";

  cout << "I am reading the file... \n\n";

  vector<BYTE> fileData = readFile(aaa);
  int fileLengths[5] = {
    FILE_NAME_LENGTH,
    FILE_NUMBER_OF_PATTERNS_LENGTH,
    FILE_SONG_END_LENGTH,
    FILE_PATTERN_TABLE_LENGTH,
    FILE_FORMAT_TAG_LENGTH
  };
  int sampleLengths[6] = {
    SAMPLE_NAME_LENGTH,
    SAMPLE_SAMPLE_LENGTH_LENGTH,
    SAMPLE_FINETUNE_LENGTH,
    SAMPLE_VOLUME_LENGTH,
    SAMPLE_REPEAT_OFFSET_LENGTH,
    SAMPLE_REPEAT_LENGTH
  };

  Modfile the_file(fileData, fileLengths, sampleLengths, NUMBER_OF_SAMPLES);

  cout << "File has been read and loaded into object. Sample bank name test...\n";

  Sample sample_bank_example = the_file.get_sample_bank(0);

  cout << sample_bank_example.get_name();

  cout << "\nFollowed by the file name test...\n";

  cout << the_file.get_name();

  cout << "\nFollowed by the int test...\n";

  cout << sample_bank_example.get_length();

  return 0;
}
