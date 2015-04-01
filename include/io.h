#ifndef ___IO_H__
#define ___IO_H__

#include <vector>

namespace intzip {

template<class T>
void read_stdin_hex(std::vector<T> &in);

template<class T>
void read_stdin_bin(std::vector<T> &in);

template<class T>
void write_stdout_hex(const std::vector<T> &out);

}

#endif
