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
      unsigned char *buf;
      size_t   max_size;
      size_t   cur_size;
    } string_data;
  } u;
} byte_writer;

typedef struct
{
  int type;
  size_t bytes_read;
  union {
    FILE *file_data;
    struct {
      const unsigned char *buf;
      size_t   index;
      size_t   size;
    } string_data;
  } u;
} byte_reader;

int write_byte(int byte, byte_writer *writer);
size_t write_bytes(const char *buf, size_t count, byte_writer *writer);
int read_byte(byte_reader *reader);
size_t read_bytes(char *buf, size_t count, byte_reader *reader);
void init_file_reader(byte_reader *reader, FILE *file);
void init_string_reader(byte_reader *reader, const unsigned char *buf, size_t max_size);

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif
