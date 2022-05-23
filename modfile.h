#include <string>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <numeric>

using namespace std;
typedef unsigned char BYTE;

struct file_index_and_length {
  int index;
  int length;
};


int get_integer_from_bytes(file_index_and_length z, vector<BYTE>& fileData) {
  // i worked this out last time...
  // (all_instruments->length[0] << 8 | all_instruments->length[1]) * 2

  // the length needs to be 2 otherwise this will not work
  // BIG ENDIAN!!!!
  // MSB is first

  cout << ((*(&fileData[z.index]) << 8) | *(&fileData[z.index + 1])) * 2;

  return ((*(&fileData[z.index]) << 8) | *(&fileData[z.index + 1])) * 2;

}

string get_string_from_bytes(file_index_and_length x, vector<BYTE>& fileData) {
  string string_name;

  string_name.assign(&fileData[x.index], &fileData[x.index] + x.length);
  // maybe this should just return the string?
  // would kind of make sense tbh

  return string_name;
}

class Sample {
  file_index_and_length name, length, finetune, linear_volume, repeat_offset, repeat_length, sample_data;
  vector<BYTE> *file_data_ref;

  public:
    Sample() {};
    void set_all(
      file_index_and_length *a,
      file_index_and_length *b,
      file_index_and_length *c,
      file_index_and_length *d,
      file_index_and_length *e,
      file_index_and_length *f,
      file_index_and_length *g,
      vector<BYTE> *fileData
    ) {
      name = *a;
      length = *b;
      finetune = *c;
      linear_volume = *d;
      repeat_offset = *e;
      repeat_length = *f;
      sample_data = *g;
      file_data_ref = fileData;
    }

    string get_name() {
      return get_string_from_bytes(name, *file_data_ref);
    }
    int get_length() {
      // idk why i need 2 multiply the data
      return get_integer_from_bytes(length, *file_data_ref);
    }
};

class Modfile {
  vector<BYTE> *fileData;
  file_index_and_length file_name, number_of_patterns, song_end_jump_pos, pattern_table, file_format_tag, pattern_data;
  vector<Sample> sample_bank;
  int total_sample_length;
  int number_of_samples;

  public:
    Modfile(
      int fileLengths[6],
      int sampleLengths[6],
      int number_of_samples,
      vector<BYTE> &fd
    ) {
      fileData = &fd;

      file_index_and_length file_name, number_of_patterns, song_end_jump_pos, pattern_table, file_format_tag, pattern_data;
      file_index_and_length *file_name_p, *number_of_patterns_p, *song_end_jump_pos_p, *pattern_table_p, *file_format_tag_p, *pattern_data_p;

      file_name_p = &file_name;
      number_of_patterns_p = &number_of_patterns;
      song_end_jump_pos_p = &song_end_jump_pos;
      pattern_table_p = &pattern_table;
      file_format_tag_p = &file_format_tag;
      pattern_data_p = &pattern_data;

      file_name_p->index = 0;
      file_name_p->length = fileLengths[0];

      // set_sample_bank(sampleLengths, fileLengths[0]);

      int offset_for_file_info = (number_of_samples * accumulate(sampleLengths, sampleLengths + 6, 0));

      number_of_patterns_p->index = offset_for_file_info + accumulate(fileLengths, fileLengths + 1, 0);
      number_of_patterns_p->length = fileLengths[1];

      song_end_jump_pos_p->index = offset_for_file_info + accumulate(fileLengths, fileLengths + 2, 0);
      song_end_jump_pos_p->length = fileLengths[2];

      pattern_table_p->index = offset_for_file_info + accumulate(fileLengths, fileLengths + 3, 0);
      pattern_table_p->length = fileLengths[3];

      file_format_tag_p->index = offset_for_file_info + accumulate(fileLengths, fileLengths + 4, 0);
      file_format_tag_p->length = fileLengths[4];

      pattern_data_p->index = offset_for_file_info + accumulate(fileLengths, fileLengths + 5, 0);
      pattern_data_p->length = fileLengths[5];

      set_file_attributes(
        file_name_p,
        number_of_patterns_p,
        song_end_jump_pos_p,
        pattern_table_p,
        file_format_tag_p,
        pattern_data_p
      );
    }
    void set_file_attributes(
      file_index_and_length *a,
      file_index_and_length *b,
      file_index_and_length *c,
      file_index_and_length *d,
      file_index_and_length *e,
      file_index_and_length *f
    ) {
      file_name = *a;
      number_of_patterns = *b;
      song_end_jump_pos = *c;
      pattern_table = *d;
      file_format_tag = *e;
      pattern_data = *f;
    }
    string get_name() {
      return get_string_from_bytes(file_name, *fileData);
    }
};

void set_sample_bank(int sampleLengths[6], int lengthOfFileLengths, vector<Sample> & sample_bank, vector<BYTE> & fileData, int number_of_samples) {
  int total_sample_length_acc = 0;
  int lengthOfSampleLengths = accumulate(sampleLengths, sampleLengths + 6, 0);

  for (int i = 0; i < number_of_samples; i++) {
    file_index_and_length name, length, finetune, volume, repeat_offset, repeat_length, file_data;
    file_index_and_length *name_p, *length_p, *finetune_p, *volume_p, *repeat_offset_p, *repeat_length_p, *file_data_p;

    // name is of type file_index_and_length
    // set name_p to point to address at name
    name_p = &name;
    length_p = &length;
    finetune_p = &finetune;
    volume_p = &volume;
    repeat_offset_p = &repeat_offset;
    repeat_length_p = &repeat_length;
    file_data_p = &file_data;


    // -> in C++ is a function used to assign values to a pointer
    name_p->index = (lengthOfFileLengths + (lengthOfSampleLengths * i));
    name_p->length = sampleLengths[0];

    length_p->index = ((lengthOfFileLengths + (lengthOfSampleLengths * i)) + accumulate(sampleLengths, sampleLengths + 1, 0));
    length_p->length = sampleLengths[1];

    finetune_p->index = ((lengthOfFileLengths + (lengthOfSampleLengths * i)) + accumulate(sampleLengths, sampleLengths + 2, 0));
    finetune_p->length = sampleLengths[2];

    volume_p->index = ((lengthOfFileLengths + (lengthOfSampleLengths * i)) + accumulate(sampleLengths, sampleLengths + 3, 0));
    volume_p->length = sampleLengths[3];

    repeat_offset_p->index = ((lengthOfFileLengths + (lengthOfSampleLengths * i)) + accumulate(sampleLengths, sampleLengths + 4, 0));
    repeat_offset_p->length = sampleLengths[4];

    repeat_length_p->index = ((lengthOfFileLengths + (lengthOfSampleLengths * i)) + accumulate(sampleLengths, sampleLengths + 5, 0));
    repeat_length_p->length = sampleLengths[5];

    // calculate offset for samples
    file_data_p->index = (lengthOfFileLengths + (lengthOfSampleLengths * number_of_samples)) + total_sample_length_acc;
    file_data_p->length = get_integer_from_bytes(length, fileData) * 2;

    sample_bank[i].set_all(
      name_p,
      length_p,
      finetune_p,
      volume_p,
      repeat_offset_p,
      repeat_length_p,
      file_data_p,
      &fileData
    );

    cout << "\nget sample length\n";
    printf ("%i\n", total_sample_length_acc);
    printf ("%d\n", sample_bank[i].get_length());

    sample_bank[i].get_name();

    total_sample_length_acc = total_sample_length_acc + sample_bank[i].get_length();
  }
}