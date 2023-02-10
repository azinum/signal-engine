// arg_parser.h
// TODO:
//  - parse arguments with more than 1 parameter
//  - add parse errors
//  - optional evaluation of numeric arguments? (think: ./your_program --some-flag '12+7*2')

#ifndef _ARG_PARSER_H
#define _ARG_PARSER_H

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

typedef int32_t i32;
typedef uint32_t u32;
typedef int16_t i16;
typedef uint16_t u16;
typedef int8_t i8;
typedef uint8_t u8;
typedef float f32;

#define ARRAY_LENGTH(ARR) (sizeof(ARR) / sizeof(ARR[0]))

typedef enum Arg_type {
  ArgInt = 0,
  ArgFloat,
  ArgString,
  ArgBuffer,

  MAX_ARG_TYPE,
} Arg_type;

static const char* arg_type_desc[MAX_ARG_TYPE] = {
  "integer",
  "float",
  "string",
  "buffer",
};

typedef enum {
  ArgParseError = -1,
  ArgParseHelp,
  ArgParseOk,
} ParseResult;

typedef struct {
  char flag;  // Single char to identify the argument flag
  const char* long_flag; // Long string to identify the argument flag
  const char* desc; // Description of this flag
  Arg_type type;  // Which type the data argument is to be
  u32 num_args;  // Can be either one or zero for any one flag
  void* data; // Reference to the data which is going to be overwritten by the value of the argument(s)
} Parse_arg;

struct {
  u32 print_arg_type_always;
  u32 description_padding;
  u32 indent_size;
} arg_parser_opts = {
  .print_arg_type_always = 0,
  .description_padding = 0,
  .indent_size = 4,
};

void arg_parser_init(u32 print_arg_type_always, u32 description_padding, u32 indent_size);

void args_print_help(FILE* fp, Parse_arg* args, u32 num_args, char** argv);

ParseResult parse_args(Parse_arg* args, u32 num_args, u32 argc, char** argv);

void indent(FILE* fp);

void arg_parser_init(u32 print_arg_type_always, u32 description_padding, u32 indent_size) {
  arg_parser_opts.print_arg_type_always = print_arg_type_always;
  arg_parser_opts.description_padding = description_padding;
  arg_parser_opts.indent_size = indent_size;
}

void args_print_help(FILE* fp, Parse_arg* args, u32 num_args, char** argv) {
  u32 longest_arg_length = 0;
  // Find the longest argument (longflag)
  for (u32 arg_index = 0; arg_index < num_args; ++arg_index) {
    Parse_arg* arg = &args[arg_index];
    if (!arg->long_flag) {
      continue;
    }
    u32 arg_length = strlen(arg->long_flag);
    if (longest_arg_length < arg_length) {
      longest_arg_length = arg_length;
    }
  }
  longest_arg_length += arg_parser_opts.description_padding;

  fprintf(fp, "USAGE:\n");
  indent(fp);
  fprintf(fp, "%s [FLAGS]\n\n", argv[0]);
  fprintf(fp, "FLAGS:\n");
  for (u32 arg_index = 0; arg_index < num_args; ++arg_index) {
    Parse_arg* arg = &args[arg_index];
    indent(fp);
    if (arg->flag) {
      fprintf(fp, "-%c", arg->flag);
    }
    if (arg->flag && arg->long_flag) {
      fprintf(fp, ", --%-*s", longest_arg_length, arg->long_flag);
    }
    else if (!arg->flag && arg->long_flag) {
      fprintf(fp, "--%-*s", longest_arg_length, arg->long_flag);
    }
    if (arg->num_args > 0 || arg_parser_opts.print_arg_type_always) {
      fprintf(fp, " ");
      fprintf(fp, "<%s>", arg_type_desc[arg->type]);
    }
    fprintf(fp, " %s\n", arg->desc);
  }
  indent(fp);
  fprintf(fp, "-h, --%-*s show help menu\n\n", longest_arg_length, "help");
}

ParseResult parse_args(Parse_arg* args, u32 num_args, u32 argc, char** argv) {
  if (!argv) {
    return ArgParseOk;
  }
  for (u32 index = 1; index < argc; ++index) {
    char* arg = argv[index];
    u8 long_flag = 0;
    u8 found_flag = 0;

    if (*arg == '-') {
      arg++;
      if (*arg == '-') {
        long_flag = 1;
        arg++;
      }
      if (*arg == 'h' && !long_flag) {
        args_print_help(stdout, args, num_args, argv);
        return ArgParseHelp;
      }
      if (long_flag) {
        if (!strcmp(arg, "help")) {
          args_print_help(stdout, args, num_args, argv);
          return ArgParseHelp;
        }
      }
      Parse_arg* parse_arg = NULL;
      // Linear search over the array of user defined arguments
      for (u32 arg_index = 0; arg_index < num_args; ++arg_index) {
        parse_arg = &args[arg_index];
        if (long_flag) {
          if (parse_arg->long_flag) {
            if (!strcmp(parse_arg->long_flag, arg)) {
              found_flag = 1;
              break;
            }
          }
        }
        else {
          if (parse_arg->flag == *arg) {
            // We found the flag
            found_flag = 1;
            break;
          }
        }
      }

      if (found_flag) {
        if (parse_arg->num_args > 0) {
          if (index + parse_arg->num_args < argc) {
            char* buffer = argv[++index];
            assert(buffer != NULL);
            assert(parse_arg);
            switch (parse_arg->type) {
              case ArgInt: {
                sscanf(buffer, "%i", (i32*)parse_arg->data);
                break;
              }
              case ArgFloat: {
                sscanf(buffer, "%f", (f32*)parse_arg->data);
                break;
              }
              case ArgString: {
                char** str = parse_arg->data;
                *str = buffer;
                break;
              }
              case ArgBuffer: {
                sscanf(buffer, "%s", (char*)parse_arg->data);
                break;
              }
              default:
                assert(0);
                break;
            }
          }
          else {
            fprintf(stderr, "Missing parameter after flag '%c'\n", *arg);
            return ArgParseError;
          }
        }
        else {
          switch (parse_arg->type) {
            case ArgInt: {
              *(i32*)parse_arg->data = 1;
              break;
            }
            case ArgFloat: {
              *(f32*)parse_arg->data = 1.0f;
              break;
            }
            default:
              break;
          }
        }
      }
      else {
        fprintf(stderr, "Flag '%s' not defined\n", arg);
        return ArgParseError;
      }
    }
    else {
      Parse_arg* parse_arg = NULL;
      for (u32 arg_index = 0; arg_index < num_args; ++arg_index) {
        parse_arg = &args[arg_index];
        if (parse_arg->flag == 0 && parse_arg->long_flag == NULL) {
          switch (parse_arg->type) {
            case ArgString: {
              char** str = parse_arg->data;
              *str = argv[index];
              break;
            }
            case ArgBuffer: {
              strcpy((char*)parse_arg->data, argv[index]);
            }
            default:
              break;
          }
          break;
        }
      }
    }
  }
  return ArgParseOk;
}

inline void indent(FILE* fp) {
  for (u32 i = 0; i < arg_parser_opts.indent_size; ++i, fprintf(fp, " "));
}
#endif // _ARG_PARSER_H
