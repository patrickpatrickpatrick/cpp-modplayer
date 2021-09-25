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

struct file_index_and_length {
  int index;
  int length;
};

class Base {
  public:
    string get_string_from_bytes(
      file_index_and_length x,
      vector<BYTE> fileData
    ) {
      string string_name;

      BYTE* string_name_pointer = &fileData[x.index];

      string_name.assign(string_name_pointer, string_name_pointer + x.length);
      // maybe this should just return the string?
      // would kind of make sense tbh
      return string_name;
    }
};

class Sample: public Base {
  file_index_and_length name, length, finetune, linear_volume, repeat_offset, repeat_length, sample_data;
  vector<BYTE> fileData;

  public:
    Sample() {};
    void set_all(
      file_index_and_length *a,
      file_index_and_length *b,
      file_index_and_length *c,
      file_index_and_length *d,
      file_index_and_length *e,
      file_index_and_length *f,
      vector<BYTE> x
    ) {
      name = *a;
      length = *b;
      finetune = *c;
      linear_volume = *d;
      repeat_offset = *e;
      repeat_length = *f;
      fileData = x;
    }

    string get_name() {
      return get_string_from_bytes(name, fileData);
    }
};

class Modfile: public Base {
  file_index_and_length name, number_of_patterns, song_end_jump_pos, pattern_table, file_format_tag, pattern_data;
  Sample sample_bank[NUMBER_OF_SAMPLES];
  vector<BYTE> fileData;

  public:
    Modfile(
      vector<BYTE> a
    ) {
      fileData = a;
    }
    void set_file_attributes(
      file_index_and_length *a,
      file_index_and_length *b,
      file_index_and_length *c,
      file_index_and_length *d,
      file_index_and_length *e,
      file_index_and_length *f
    ) {
      name = *a;
      number_of_patterns = *b;
      song_end_jump_pos = *c;
      pattern_table = *d;
      file_format_tag = *e;
      pattern_data = *f;
    }
    void set_sample_bank() {
      int sampleLengths[6] = {
        SAMPLE_NAME_LENGTH,
        SAMPLE_SAMPLE_LENGTH_LENGTH,
        SAMPLE_FINETUNE_LENGTH,
        SAMPLE_VOLUME_LENGTH,
        SAMPLE_REPEAT_OFFSET_LENGTH,
        SAMPLE_REPEAT_LENGTH
      };

      for (int i = 0; i < NUMBER_OF_SAMPLES; i++) {
        file_index_and_length name, length, finetune, volume, repeat_offset, repeat_length;
        file_index_and_length *name_p, *length_p, *finetune_p, *volume_p, *repeat_offset_p, *repeat_length_p;

        // name is of type file_index_and_length
        // set name_p to point to address at name
        name_p = &name;
        length_p = &length;
        finetune_p = &finetune;
        volume_p = &volume;
        repeat_offset_p = &repeat_offset;
        repeat_length_p = &repeat_length;

        // -> in C++ is a function used to assign values to a pointer
        name_p->index = (20 + (30 * i));
        name_p->length = 22;

        length_p->index = ((20 + (30 * i)) + accumulate(sampleLengths, sampleLengths + 1, 0));
        length_p->length = 2;

        finetune_p->index = ((20 + (30 * i)) + accumulate(sampleLengths, sampleLengths + 2, 0));
        finetune_p->length = 1;

        volume_p->index = ((20 + (30 * i)) + accumulate(sampleLengths, sampleLengths + 3, 0));
        volume_p->length = 1;

        repeat_offset_p->index = ((30 + (30 * i)) + accumulate(sampleLengths, sampleLengths + 4, 0));
        repeat_offset_p->length = 2;

        repeat_length_p->index = ((30 + (30 * i)) + accumulate(sampleLengths, sampleLengths + 5, 0));
        repeat_length_p->length = 2;

        sample_bank[i].set_all(
          name_p,
          length_p,
          finetune_p,
          volume_p,
          repeat_offset_p,
          repeat_length_p,
          fileData
        );
      }
    }
    Sample get_sample_bank(int index) {
      cout << "yy";
      return sample_bank[index];
    }
};

int main() {
  string aaa = "rainysum.mod";

  cout << "I am reading the file... \n\n";

  vector<BYTE> fileData = readFile(aaa);

  Modfile the_file(fileData);

  file_index_and_length name, number_of_patterns, song_end_jump_pos, pattern_table, file_format_tag, pattern_data;
  file_index_and_length *name_p, *number_of_patterns_p, *song_end_jump_pos_p, *pattern_table_p, *file_format_tag_p, *pattern_data_p;

  name_p = &name;
  number_of_patterns_p = &number_of_patterns;
  song_end_jump_pos_p = &song_end_jump_pos;
  pattern_table_p = &pattern_table;
  file_format_tag_p = &file_format_tag;
  pattern_data_p = &pattern_data;

  name_p->index = 0;
  name_p->length = 20;

  number_of_patterns_p->index = (NUMBER_OF_SAMPLES * SAMPLE_LENGTH) + 20;
  number_of_patterns_p->length = 1;

  song_end_jump_pos_p->index = (NUMBER_OF_SAMPLES * SAMPLE_LENGTH) + 20 + 1;
  song_end_jump_pos_p->length = 1;

  pattern_table_p->index = (NUMBER_OF_SAMPLES * SAMPLE_LENGTH) + 20 + 1 + 1;
  pattern_table_p->length = 128;

  file_format_tag_p->index = (NUMBER_OF_SAMPLES * SAMPLE_LENGTH) + 20 + 1 + 1 + 128;
  file_format_tag_p->length = 4;

  the_file.set_file_attributes(
    name_p,
    number_of_patterns_p,
    song_end_jump_pos_p,
    pattern_table_p,
    file_format_tag_p,
    pattern_data_p
  );

  // BYTE* fileName = &fileData[0];

  cout << "File has been read. Beginning to load variables.\n\n";

  Sample sample_bank_example = the_file.get_sample_bank(0);

  cout << "NAH";

  // cout << sample_bank_example.get_name();

  // Sample sample_bank[32];

  // cout << sample_bank[1].get_name(fileData);

  return 0;
}
