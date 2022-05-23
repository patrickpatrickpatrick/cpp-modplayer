#include "modfile.h"
#include "RtAudio.h"

#include <stdio.h>
#include <cstdlib>
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
const int FILE_PATTERN_DATA_LENGTH = 1024;

const int FILE_PATTERN_LENGTH = 64;

const int SAMPLE_LENGTH = (
  SAMPLE_NAME_LENGTH +
  SAMPLE_SAMPLE_LENGTH_LENGTH +
  SAMPLE_FINETUNE_LENGTH +
  SAMPLE_VOLUME_LENGTH +
  SAMPLE_REPEAT_OFFSET_LENGTH +
  SAMPLE_REPEAT_LENGTH
);

const int NUMBER_OF_SAMPLES = 31;

// finetune
// seems to be like subtracting 12 / octave

// period table
// this is octave 1
// if you divide by 2 and round to ceil
// you can get the value of higher octave
const int period[12] = {
  1712, //c
  1616, //c#
  1524, //d
  1440, //d#
  1356, //e
  1280, //f
  1208, //f#
  1140, //g
  1076, //g#
  1016, //a
  960, //a#
  906 //b
};

// magic number ! ! ! !
// PAL:   7093789.2 / (amigaval * 2)
// to get sample rate


typedef unsigned char BYTE;
using namespace std;

// Two-channel sawtooth wave generator.
int saw( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData )
{
  unsigned int i, j;
  double *buffer = (double *) outputBuffer;
  double *lastValues = (double *) userData;
  if ( status )
    std::cout << "Stream underflow detected!" << std::endl;
  // Write interleaved audio data.
  for ( i=0; i<nBufferFrames; i++ ) {
    for ( j=0; j<2; j++ ) {
      *buffer++ = lastValues[j];
      lastValues[j] += 0.005 * (j+1+(j*0.1));
      if ( lastValues[j] >= 1.0 ) lastValues[j] -= 2.0;
    }
  }
  return 0;
}

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
  int fileLengths[6] = {
    FILE_NAME_LENGTH,
    FILE_NUMBER_OF_PATTERNS_LENGTH,
    FILE_SONG_END_LENGTH,
    FILE_PATTERN_TABLE_LENGTH,
    FILE_FORMAT_TAG_LENGTH,
    FILE_PATTERN_DATA_LENGTH
  };
  int sampleLengths[6] = {
    SAMPLE_NAME_LENGTH,
    SAMPLE_SAMPLE_LENGTH_LENGTH,
    SAMPLE_FINETUNE_LENGTH,
    SAMPLE_VOLUME_LENGTH,
    SAMPLE_REPEAT_OFFSET_LENGTH,
    SAMPLE_REPEAT_LENGTH
  };

  Modfile the_file(fileLengths, sampleLengths, NUMBER_OF_SAMPLES, fileData);
  vector<Sample> sample_bank = vector<Sample>(NUMBER_OF_SAMPLES);
  set_sample_bank(sampleLengths, fileLengths[0], sample_bank, fileData, NUMBER_OF_SAMPLES);

  // number_of_samples = n_o_s;

  cout << "File has been read and loaded into object. Sample bank name test...\n";

  Sample sample_bank_example = sample_bank[0];

  cout << sample_bank_example.get_name();

  cout << "\nFollowed by the file name test...\n";

  cout << the_file.get_name();

  cout << "\nFollowed by the int test...\n";

  cout << sample_bank_example.get_length();

  // http://web.mit.edu/carrien/Public/speechlab/marc_code/ADAPT_VC/rtaudio/doc/html/index.html
  // https://gist.github.com/nagachika/165241

  RtAudio dac;
  if ( dac.getDeviceCount() < 1 ) {
    std::cout << "\nNo audio devices found!\n";
    exit( 0 );
  }
  RtAudio::StreamParameters parameters;
  parameters.deviceId = dac.getDefaultOutputDevice();
  parameters.nChannels = 2;
  parameters.firstChannel = 0;
  unsigned int sampleRate = 44100;
  unsigned int bufferFrames = 256; // 256 sample frames
  double data[2] = {0, 0};
  try {
    dac.openStream( &parameters, NULL, RTAUDIO_FLOAT64,
                    sampleRate, &bufferFrames, &saw, (void *)&data );
    dac.startStream();
  }
  catch ( RtAudioError& e ) {
    e.printMessage();
    exit( 0 );
  }

  char input;
  std::cout << "\nPlaying ... press <enter> to quit.\n";
  std::cin.get( input );
  try {
    // Stop the stream
    dac.stopStream();
  }
  catch (RtAudioError& e) {
    e.printMessage();
  }
  if ( dac.isStreamOpen() ) dac.closeStream();
  return 0;
}
