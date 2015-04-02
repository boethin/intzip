#ifndef ___OPTIONS_H__
#define ___OPTIONS_H__

namespace intzip {

#define USAGE \
"usage: intzip "


struct options {
  bool compress;
  bool binary;
  const char *infile;
  
  options() : 
    compress(true),
    binary(false),
    infile(NULL)
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
    }
    
  }
  
  const char *usage() {
    return USAGE;
  }
  
  
};

}

#endif

