/**
 * This file is part of intzip.
 *
 * Copyright (C) 2015 Sebastian Boethin <boethin@xn--domain.net>
 *
 * intzip is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * intzip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with intzip.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef ___INTZIP_OPTIONS_H___
#define ___INTZIP_OPTIONS_H___

namespace intzip {

// PACKAGE_STRING from config.h
#define VERSION_STRING \
PACKAGE_STRING "\n" \
"Copyright (C) 2015 Sebastian Boethin\n" \
"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n" \
"This is free software: you are free to change and redistribute it.\n" \
"There is NO WARRANTY, to the extent permitted by law."

#define USAGE_STRING \
"Usage: " PACKAGE " [options]... [file]\n" \
"Fast compression of integer sets.\n" \
"Options:\n" \
"  -h, --help               Display this information.\n" \
"  -v, --version            Display version information.\n" \
"  -c, --compress           Compress a given integer list (default behavior).\n" \
"  -d, --decompress         Decompress a compressed integer list.\n" \
"  -b, --binary             Input and output in binary (network order / big endian) mode.\n" \
"If the [file] argument is omitted, input is read from stdin.\n\n" \
"See <https://github.com/boethin/intzip/blob/master/README.md> for more information."

struct options {
  bool compress;
  bool binary;
  const char *infile;
  bool usage;
  bool version;

  options() : 
    compress(true),
    binary(false),
    infile(NULL),
    usage(false)
  {}
  
  void parse(int argc, char **argv)
  {
    for (int i = 1; i < argc; ++i)
    {
      if (argv[i][0] == '-' && argv[i][1] != '\0') {
        if (argv[i][1] == '-') {
          if (0 == strcmp(argv[i],"--compress"))
            handle('c');
          else if (0 == strcmp(argv[i],"--decompress"))
            handle('d');
          else if (0 == strcmp(argv[i],"--binary"))
            handle('b');
          else if (0 == strcmp(argv[i],"--version"))
            handle('v');
          else
            handle('h');
        }
        else {
          for (char *c = argv[i] + 1; *c; c++) {
            handle(*c);
          }
        }
      }
      else {
        infile = argv[i];
      }
    }
  }
  
  void handle(char cmd)
  {
    switch (cmd) {
      case 'c':
        compress = true;
        break;
      case 'd':
        compress = false;
        break;
      case 'b':
        binary = true;
        break;
      case 'v':
        version = true;
        break;
      case 'h':
        usage = true;
        break;
    }
    
  }

  const char *get_version() {
    return VERSION_STRING;
  }

  const char *get_usage() {
    return USAGE_STRING;
  }

  
  
};

}

#endif

