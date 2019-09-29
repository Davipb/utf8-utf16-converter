#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <inttypes.h>

#include "converter.h"

static int help(const char* base)
{
    printf("Usage: %s <mode> <input> <output>\n", base);
    printf("\n");
    printf("mode:\n");
    printf("'utf8' if the input file is in UTF-8, 'utf16' if the input file is in UTF-16.\n");
    printf("The output file will be in the opposite encoding\n");
    printf("\n");
    printf("input:\n");
    printf("Input file to be read. Must be in the encoding specified by 'mode'\n");
    printf("\n");
    printf("output:\n");
    printf("Output file where the converted result will be stored.\n");
    printf("Its contents will be completely overwritten.\n");
    return EXIT_SUCCESS;
}

// Prints a rounded human-readable byte size to stdout
// size: The size do print, in bytes
static void print_size(size_t size)
{
    static char const* const suffixes[] =
    {
        "b",
        "KiB",
        "MiB",
        "GiB"
    };
    static size_t const suffixes_len = sizeof suffixes / sizeof suffixes[0];

    size_t suffix = 0;
    while (size >= 1024 && suffix < suffixes_len - 1)
    {
        suffix++;
        size /= 1024;
    }

    printf("%zu %s", size, suffixes[suffix]);
}

// Prints a precise human-readable time to stdout
// seconds: The time to print, in seconds
static void print_time(double seconds)
{
    static char const* const suffixes[] =
    {
        "ns",
        "us",
        "ms",
        "s"
    };
    static size_t const suffixes_len = sizeof suffixes / sizeof suffixes[0];

    uint64_t nanos = (uint64_t)(seconds * 1e9);

    bool any_printed = false;

    size_t suffix = suffixes_len;
    while (suffix-- > 0)
    {
        uint64_t unit = nanos / (uint64_t)pow(1000.0, suffix);

        if (suffix < suffixes_len - 1)
            unit %= 1000;

        if (unit > 0 || (suffix == 0 && !any_printed))
        {
            any_printed = true;
            printf("%"PRIu64"%s ", unit, suffixes[suffix]);
        }
    }
}

// Opens a file and reads its contents fully
//
// path: The path of the file to be read
// buffer: Pointer to a variable that will receive the file content
// buffer_len: Pointer to a variable that will receive the buffer length
//
// return: If the file was read successfully. 
static bool read_file(char const* path, char** buffer, size_t* buffer_len)
{
    FILE* file = fopen(path, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Unable to open input file %s", path);
        return false;
    }

    size_t len = 4096; // Total length, in chars, of the input buffer
    size_t next_index = 0; // First free index of the input buffer

    char* input = malloc(len);
    if (input == NULL)
    {
        fprintf(stderr, "Unable to allocate enough memory to read file %s", path);
        return false;
    }
        

    size_t read;
    do {
        read = fread(input + next_index, sizeof(char), len - next_index, file);
        next_index += read;

        if (next_index == len)
        {
            len *= 2;
            char* new_input = realloc(input, len);
            if (new_input == NULL)
            {
                fprintf(stderr, "Unable to allocate enough memory to read file %s. Is the file too big, or are you running out of memory?", path);
                free(input);
                return false;
            }

            input = new_input;
        }

    } while (read > 0);


    if (ferror(file))
    {
        fprintf(stderr, "Unable to read file %s", path);
        free(input);
        return false;
    }

    fclose(file);

    char* trimmed_input = realloc(input, next_index);
    if (trimmed_input != NULL)
        input = trimmed_input;

    *buffer = input;
    *buffer_len = next_index;
    return true;
}

int main(int argc, char const* argv[]) 
{
    if (argc != 4)
        return help(argv[0]);
    
    char const* mode = argv[1];

    bool is_utf8;
    if (strcmp(mode, "utf8") == 0) { is_utf8 = true; }
    else if (strcmp(mode, "utf16") == 0) { is_utf8 = false; }
    else
    {
        fprintf(stderr, "Mode must be either 'utf8' or 'utf16', case-sensitive");
        return EXIT_FAILURE;
    }

    char* input;
    size_t input_len;
    if (!read_file(argv[2], &input, &input_len))
        return EXIT_FAILURE;
    
    if (input_len == 0)
    {
        fprintf(stderr, "Input file is empty");
        return EXIT_FAILURE;
    }
    
    char* output;
    size_t output_len;
    if (is_utf8)
        output_len = sizeof(utf16_t) * utf8_to_utf16(input, input_len / sizeof(utf8_t), NULL, 0);
    else
        output_len = sizeof(utf8_t) * utf16_to_utf8(input, input_len / sizeof(utf16_t), NULL, 0);

    output = malloc(output_len);
    if (output == NULL)
    {
        fprintf(stderr, "Unable to allocate enough memory to write converted output. Is the input too big, or are you running out of memory?");
        return EXIT_FAILURE;
    }


    time_t time_start = time(NULL);
    clock_t clock_start = clock();

    if (is_utf8)
        utf8_to_utf16(input, input_len / sizeof(utf8_t), output, output_len / sizeof(utf16_t));
    else
        utf16_to_utf8(input, input_len / sizeof(utf16_t), output, output_len / sizeof(utf8_t));

    time_t time_end = time(NULL);
    clock_t clock_end = clock();
        
    free(input);


    FILE* output_file = fopen(argv[3], "wb");
    if (output_file == NULL)
    {
        fprintf(stderr, "Unable to open output file %s", argv[3]);
        return EXIT_FAILURE;
    }

    size_t written = fwrite(output, sizeof(char), output_len, output_file);
    if (written < output_len)
    {
        fprintf(stderr, "Unable to write output to file %s", argv[3]);
        return EXIT_FAILURE;
    }

    double time_seconds = difftime(time_end, time_start);
    uint64_t clock_delta = (uint64_t)clock_end - clock_start;
    double clock_seconds = ((double)clock_delta) / CLOCKS_PER_SEC;

    if (is_utf8)
        printf("UTF-8 to UTF-16");
    else
        printf("UTF-16 to UTF-8");

    printf("\n");

    printf("Input: ");
    print_size(input_len);
    printf("\n");

    printf("Output: ");
    print_size(input_len);
    printf("\n");

    printf("\n");

    printf("Real time: ");
    print_time(time_seconds);
    printf("\n");

    printf("CPU time:  ");
    print_time(clock_seconds);
    printf("\n");

    printf("CPU ticks: %"PRIu64"\n", clock_delta);
}
