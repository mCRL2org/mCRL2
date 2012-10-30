#include <assert.h>
#include <stdlib.h>
#include <stdexcept>

#include "mcrl2/atermpp/detail/utility.h"
#include "mcrl2/atermpp/detail/byteio.h"

namespace atermpp
{

static void resize_buffer(byte_writer* writer, size_t delta)
{
  size_t size_needed, new_size;

  assert(writer->type == STRING_WRITER);

  size_needed = writer->u.string_data.cur_size + delta;
  if (size_needed >= writer->u.string_data.max_size)
  {
    new_size = (std::max)(size_needed, writer->u.string_data.max_size*2);
    writer->u.string_data.buf = (unsigned char*)realloc(writer->u.string_data.buf, new_size);
    if (!writer->u.string_data.buf)
    {
      std::runtime_error("bafio: unable to resize buffer to " + to_string(new_size) + " bytes.");
    }
    writer->u.string_data.max_size = new_size;
  }
}

int write_byte(const int byte, byte_writer* writer, std::ostream &os)
{
  switch (writer->type)
  {
    case STRING_WRITER:
      resize_buffer(writer, 1);
      writer->u.string_data.buf[writer->u.string_data.cur_size++] = (char)byte;
      return byte;

    case FILE_WRITER:
      return fputc(byte, writer->u.file_data);

    case STREAM_WRITER:
      os.put(byte);
      return byte;

    default:
      abort();
  }
  return EOF;
}

size_t write_bytes(const char* buf, const size_t count, byte_writer* writer, std::ostream &os)
{
  switch (writer->type)
  {
    case STRING_WRITER:
      resize_buffer(writer, count);
      memcpy(&writer->u.string_data.buf[writer->u.string_data.cur_size], buf, count);
      writer->u.string_data.cur_size += count;
      return count;

    case FILE_WRITER:
      return fwrite(buf, 1, count, writer->u.file_data);

    case STREAM_WRITER:
      os.write(buf,count);
      return count;

    default:
      abort();
  }
  return 0;
}

int read_byte(byte_reader* reader, std::istream &is)
{
  size_t index;
  int c;

  c = EOF;
  switch (reader->type)
  {
    case STRING_READER:
      index = reader->u.string_data.index;
      if (index >= reader->u.string_data.size)
      {
        return EOF;
      }
      reader->bytes_read++;
      reader->u.string_data.index++;
      c = ((int)reader->u.string_data.buf[index]) & 0xFF;
      break;

    case FILE_READER:
      c = fgetc(reader->u.file_data);
      reader->bytes_read++;
      break;

    case STREAM_READER:
      c = is.get();
      reader->bytes_read++;
      break;

    default:
      abort();
  }

  return c;
}

size_t read_bytes(char* buf, size_t count, byte_reader* reader, std::istream &is)
{
  size_t index, size, left;
  size_t result;

  result = 0;
  switch (reader->type)
  {
    case STRING_READER:
      index = reader->u.string_data.index;
      size  = reader->u.string_data.size;
      left  = size-index;
      if (size <= index)
      {
        return 0;
      }
      if (left < count)
      {
        count = left;
      }
      memcpy(buf, &reader->u.string_data.buf[index], count);
      reader->u.string_data.index += count;
      reader->bytes_read += count;
      result = count;
      break;

    case FILE_READER:
      result = fread(buf, 1, count, reader->u.file_data);
      reader->bytes_read += count;
      break;

    case STREAM_READER:
      is.read(buf, count);
      result=count;
      reader->bytes_read += count;
      break;

    default:
      abort();
  }
  return result;
}

void init_stream_reader(byte_reader* reader, std::istream &is)
{
  reader->type = STREAM_READER;
  reader->bytes_read = 0;
}

} // namespace atermpp
