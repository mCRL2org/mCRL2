#ifndef BYTEIO_H
#define BYTEIO_H

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

#define FILE_WRITER   0
#define STRING_WRITER 1

#define FILE_READER   0
#define STRING_READER 1

typedef struct
{
  int type;
  union {
    FILE *file_data;
    struct {
      char *buf;
      int   max_size;
      int   cur_size;
    } string_data;
  } u;
} byte_writer;

typedef struct
{
  int type;
  long bytes_read;
  union {
    FILE *file_data;
    struct {
      char *buf;
      int   index;
      int   size;
    } string_data;
  } u;
} byte_reader;

int write_byte(int byte, byte_writer *writer);
int write_bytes(const char *buf, int count, byte_writer *writer);
int read_byte(byte_reader *reader);
int read_bytes(char *buf, int count, byte_reader *reader);
void init_file_reader(byte_reader *reader, FILE *file);
void init_string_reader(byte_reader *reader, char *buf, int max_size);

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif
