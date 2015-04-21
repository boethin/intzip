#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "intzip-stdint.h"
#include "intzip-trace.h"
#include "intzip-def.h"
#include "intzip-chunk.h"

#include <stdio.h>
#include <stdarg.h>

using namespace std;

extern bool enable_trace;

#define T_FORMAT_u16 "%" PRIu16
#define T_FORMAT_u32 "%" PRIu32
#define T_FORMAT_u64 "%" PRIu64
#define T_FORMAT(u) T_FORMAT_u ## u

#define T_xFORMAT_u16 "0x%04" PRIu16
#define T_xFORMAT_u32 "0x%08" PRIu32
#define T_xFORMAT_u64 "0x%016" PRIu64
#define T_xFORMAT(u) T_xFORMAT_u ## u

#define _sprintf_chunkdata(buf,cp,xform,uform) \
  sprintf (buf, \
    xform " [base=" uform ", " uform "*%" PRId8 " bits]", \
      cp->first,cp->base,cp->len,cp->bits)
#define sprintf_chunkdata(buf,cp,u) _sprintf_chunkdata(buf,cp,T_xFORMAT(u),T_FORMAT(u))

#define _sprintf_chunk(buf,cp,xform,uform) \
  sprintf (buf, \
    xform " [base=" uform ", maxdiff=" uform ", " uform "*%" PRId8 " bits: cost=" uform "]", \
      cp->first,cp->base,cp->maxdiff,cp->len,cp->bits,cp->calculate_cost())
#define sprintf_chunk(buf,cp,u) _sprintf_chunk(buf,cp,T_xFORMAT(u),T_FORMAT(u))


// void intzip::chunk<T>::to_string(char buf[]) const;
namespace intzip {

  template<>
  void chunkdata<uint16_t>::to_string(char buf[]) const
  {
    sprintf_chunkdata(buf,this,16);
  }

  template<>
  void chunkdata<uint32_t>::to_string(char buf[]) const
  {
    sprintf_chunkdata(buf,this,32);
  }

  template<>
  void chunkdata<uint64_t>::to_string(char buf[]) const
  {
    sprintf_chunkdata(buf,this,64);
  }

  template<>
  void chunk<uint16_t>::to_string(char buf[]) const
  {
    sprintf_chunk(buf,this,16);
  }

  template<>
  void chunk<uint32_t>::to_string(char buf[]) const
  {
    sprintf_chunk(buf,this,32);
  }

  template<>
  void chunk<uint64_t>::to_string(char buf[]) const
  {
    sprintf_chunk(buf,this,64);
  }

} // namespace intzip

using namespace intzip;

#define TRACE_BUFSIZE 0x100 // must be large enough for any log line

static void printf_tracelog(const char *mark, const char* log)
{
  if (enable_trace)
    fprintf(stderr,"# %-12s %s\n",mark,log);
}

template<>
void printf_tracelog_args(const char *mark, uint16_t n)
{
  char buf[TRACE_BUFSIZE];

  sprintf(buf,T_xFORMAT_u16,n);
  printf_tracelog(mark,buf);
}

template<>
void printf_tracelog_args(const char *mark, uint32_t n)
{
  char buf[TRACE_BUFSIZE];

  sprintf(buf,T_xFORMAT_u32,n);
  printf_tracelog(mark,buf);
}

template<>
void printf_tracelog_args(const char *mark, uint64_t n)
{
  char buf[TRACE_BUFSIZE];

  sprintf(buf,T_xFORMAT_u64,n);
  printf_tracelog(mark,buf);
}

void printf_tracelog_args(const char *mark, const char* format, ...)
{
  va_list args;
  char buf[TRACE_BUFSIZE];

  va_start(args, format);
  vsprintf(buf, format, args);
  va_end(args);

  printf_tracelog(mark,buf);
}

template<class T>
void printf_tracelog_args(const char *mark, const chunkdata<T> &chunk, const char* format, ...)
{
  va_list args;
  char buf[TRACE_BUFSIZE], buf2[TRACE_BUFSIZE], buf3[TRACE_BUFSIZE];

  chunk.to_string(buf);

  va_start(args, format);
  vsprintf(buf2, format, args);
  va_end(args);

  sprintf(buf3,"%s %s",buf,buf2);

  printf_tracelog(mark,buf3);
}

template<class T>
void printf_tracelog_args(const char *mark, const chunk<T> &chunk, const char* format, ...)
{
  va_list args;
  char buf[TRACE_BUFSIZE], buf2[TRACE_BUFSIZE], buf3[TRACE_BUFSIZE];

  chunk.to_string(buf);

  va_start(args, format);
  vsprintf(buf2, format, args);
  va_end(args);

  sprintf(buf3,"%s %s",buf,buf2);

  printf_tracelog(mark,buf3);
}

template<class T>
void printf_tracelog_args(const char *mark, const chunkdata<T> &chunk)
{
  char buf[TRACE_BUFSIZE];

  chunk.to_string(buf);
  printf_tracelog(mark,buf);
}

template<class T>
void printf_tracelog_args(const char *mark, const chunk<T> &chunk)
{
  char buf[TRACE_BUFSIZE];

  chunk.to_string(buf);
  printf_tracelog(mark,buf);
}

// -- specializations --

template void printf_tracelog_args(const char *mark, uint16_t n);
template void printf_tracelog_args(const char *mark, const intzip::chunkdata<uint16_t> &chunk, const char* format, ...);
template void printf_tracelog_args(const char *mark, const intzip::chunkdata<uint16_t> &chunk);
template void printf_tracelog_args(const char *mark, const intzip::chunk<uint16_t> &chunk, const char* format, ...);
template void printf_tracelog_args(const char *mark, const intzip::chunk<uint16_t> &chunk);

template void printf_tracelog_args(const char *mark, uint32_t n);
template void printf_tracelog_args(const char *mark, const intzip::chunkdata<uint32_t> &chunk, const char* format, ...);
template void printf_tracelog_args(const char *mark, const intzip::chunkdata<uint32_t> &chunk);
template void printf_tracelog_args(const char *mark, const intzip::chunk<uint32_t> &chunk, const char* format, ...);
template void printf_tracelog_args(const char *mark, const intzip::chunk<uint32_t> &chunk);

template void printf_tracelog_args(const char *mark, uint64_t n);
template void printf_tracelog_args(const char *mark, const intzip::chunkdata<uint64_t> &chunk, const char* format, ...);
template void printf_tracelog_args(const char *mark, const intzip::chunkdata<uint64_t> &chunk);
template void printf_tracelog_args(const char *mark, const intzip::chunk<uint64_t> &chunk, const char* format, ...);
template void printf_tracelog_args(const char *mark, const intzip::chunk<uint64_t> &chunk);




