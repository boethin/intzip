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

// void intzip::chunk<T>::to_string(char buf[]) const;
namespace intzip {

  template<>
  void intzip::chunk<uint16_t>::to_string(char buf[]) const
  {
    sprintf(buf,"%#04" PRIx16 " [base=%" PRIu16 ", maxdiff=%" PRIu16 ", %" PRIu16 "*%" PRId8 " bits: cost=%" PRIu16 "]",
      this->first,this->base,this->maxdiff,this->len,this->bits,this->calculate_cost());
  }

  template<>
  void chunk<uint32_t>::to_string(char buf[]) const
  {
    sprintf(buf,"%#08" PRIx32 " [base=%" PRIu32 ", maxdiff=%" PRIu32 ", %" PRIu32 "*%" PRId8 " bits: cost=%" PRIu32 "]",
      this->first,this->base,this->maxdiff,this->len,this->bits,this->calculate_cost());
  }

  template<>
  void chunk<uint64_t>::to_string(char buf[]) const
  {
    sprintf(buf,"%#016" PRIx64 " [base=%" PRIu64 ", maxdiff=%" PRIu64 ", %" PRIu64 "*%" PRId8 " bits: cost=%" PRIu64 "]",
      this->first,this->base,this->maxdiff,this->len,this->bits,this->calculate_cost());
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

  sprintf(buf,"%#04" PRIx16,n);
  printf_tracelog(mark,buf);
}

template<>
void printf_tracelog_args(const char *mark, uint32_t n)
{
  char buf[TRACE_BUFSIZE];

  sprintf(buf,"%#08" PRIx32,n);
  printf_tracelog(mark,buf);
}

template<>
void printf_tracelog_args(const char *mark, uint64_t n)
{
  char buf[TRACE_BUFSIZE];

  sprintf(buf,"%#016" PRIx64,n);
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
void printf_tracelog_args(const char *mark, const chunk<T> &chunk)
{
  char buf[TRACE_BUFSIZE];

  chunk.to_string(buf);
  printf_tracelog(mark,buf);
}

// -- specializations --

template void printf_tracelog_args(const char *mark, uint16_t n);
template void printf_tracelog_args(const char *mark, const intzip::chunk<uint16_t> &chunk, const char* format, ...);
template void printf_tracelog_args(const char *mark, const intzip::chunk<uint16_t> &chunk);

template void printf_tracelog_args(const char *mark, uint32_t n);
template void printf_tracelog_args(const char *mark, const intzip::chunk<uint32_t> &chunk, const char* format, ...);
template void printf_tracelog_args(const char *mark, const intzip::chunk<uint32_t> &chunk);

template void printf_tracelog_args(const char *mark, uint64_t n);
template void printf_tracelog_args(const char *mark, const intzip::chunk<uint64_t> &chunk, const char* format, ...);
template void printf_tracelog_args(const char *mark, const intzip::chunk<uint64_t> &chunk);

