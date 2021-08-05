/*!gcc {0} -I. -I../../include/ -I../include/ -I../../../libcester/include -o out; ./out */
/*!clang++ {0} -I. -I../../include/ -I../include/ -I../../../libcester/include -o out.exe; ./out.exe */
/*!clang -ansi -pedantic-errors {0} -I. -I../../include/ -I../include/ -I../../../libcester/include -o out.exe; ./out.exe */
/*!clang {0} -I. -I../../include/ -I../include/ -I../../../libcester/include -o out.exe; ./out.exe */
/*!g++ -std=c++11 {0} -I. -I../../include/ -I../include/ -I../../../libcester/include -o out; ./out */
/*!g++ -ansi -pedantic-errors {0} -I. -I../../include/ -I../include/ -I../../../libcester/include -o out; ./out */
/*!gcc  -ansi -pedantic-errors {0} -I. -I../../include/ -I../include/ -I../../../libcester/include -o out; ./out */

#include <exotic/cline/font_effect.h>
#include <exotic/cline/cliarg.h>
#include <exotic/fio/fs.h>
#include <exotic/xtd/xtypes_bounds.h>
#include <dirent.h>
#ifndef fio_is_regular_file
#include <sys/stat.h>
#include <sys/types.h>
#endif

typedef struct Slocer {
    bool recurse;
    bool verbose;
    long line_count;
    long source_line_count;
    bool last_char_is_whitespace;
    XAllocator allocator;
} Slocer;

int fatal_error2(char *msg, int code) {
    fprintf(stderr, "%s%s: %d%s\n", CLINE_FE(CLINE_FE_FOREGROUND_RED), msg, code, CLINE_FE(CLINE_FE_RESET));
    return EXIT_FAILURE;
}

int fatal_error(char *msg) {
    fprintf(stderr, "%s%s%s\n", CLINE_FE(CLINE_FE_FOREGROUND_RED), msg, CLINE_FE(CLINE_FE_RESET));
    return EXIT_FAILURE;
}

void warn(char *msg, char *sup1) {
    fprintf(stdout, "%s%s%s%s\n", CLINE_FE(CLINE_FE_FOREGROUND_YELLOW), msg, sup1, CLINE_FE(CLINE_FE_RESET));
}

void debug(char *msg, char *sup1) {
    fprintf(stdout, "%s%s%s%s\n", CLINE_FE(CLINE_FE_FOREGROUND_CYAN), msg, sup1, CLINE_FE(CLINE_FE_RESET));
}

#ifndef fio_is_regular_file
bool fio_is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
#endif

#ifndef fio_is_directory
bool fio_is_directory(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}
#endif

bool read_file_char(void *param, char ch) {
    Slocer *slocer = (Slocer *) param;
    if (ch == '\n') {
        slocer->source_line_count++;
        if (!slocer->last_char_is_whitespace) slocer->line_count++;
    }
    slocer->last_char_is_whitespace = xbound_char_is_white_space(ch);
    return TRUE;
}

void read_file_or_directory(Slocer *slocer, char *path);

// TODO add is_parent to check if is parent folder in 

void read_directory(Slocer *slocer, char *path) {
    char *full_path;
    struct dirent *directory_child;
    DIR *directory = opendir(path);
    if (!directory) {
        warn("Unable to read the directory: ", path);
        return;
    }
    while ((directory_child = readdir(directory)) != NULL) {
        if (xstring_cstr_equals(directory_child->d_name, ".") || 
            xstring_cstr_equals(directory_child->d_name, "..")) continue;
        full_path = xstring_cstr_concat_cstr(slocer->allocator, path, "/");
        full_path = xstring_cstr_concat_cstr_free_old(slocer->allocator, full_path, directory_child->d_name);
        read_file_or_directory(slocer, full_path);
        //debug("Reading the file: ", full_path);
        slocer->allocator.memory_free(full_path);
    }
    closedir(directory);
}

void read_file(Slocer *slocer, char *file) {
    if (slocer->verbose) {
        debug("Reading the file: ", file);
    }
    if (fio_read_file_chars_cb_from_path2(file, read_file_char, slocer) != XTD_OK) {
        warn("Unable to read the file: ", file);
    } else {
        slocer->line_count++;
        slocer->source_line_count++;
    }    
}

void read_file_or_directory(Slocer *slocer, char *path) {
    if (fio_is_directory(path) && (slocer->recurse || slocer->line_count == 0)) {
        read_directory(slocer, path);
    } else if (fio_is_regular_file(path)) {
        read_file(slocer, path);
    }
}

void count_lines_in_files_and_folders(Slocer *slocer, char **values, size_t size) {
    size_t index;
    for (index = 0; index < size; index++) {
        read_file_or_directory(slocer, values[index]);
    }
}

int main(int argc, char **argv) {
    enum x_stat status;
    size_t size;
    char **values;
    char *help_text;
    XAllocator allocator;
    ClineArgs *cline_arg;
    Slocer *slocer;

    init_xallocator(&allocator);
    if (init_cline_arg(&allocator, &cline_arg, "slocer") != XTD_OK) return fatal_error("Failed to initialize argument parser");
    if (cline_arg_set_description(cline_arg, "Count the number of line in your project or source file, This does not give regard to comment.") != XTD_OK) goto fail_cline_arg;
    if (cline_arg_add_option(cline_arg, XTD_NULL, "-h<:>--help", "Print this help message", FALSE) != XTD_OK) goto fail_cline_arg;
    if (cline_arg_add_option(cline_arg, XTD_NULL, "-r<:>--recurse", "Recursively count the lines in file and sub folder recursively", FALSE) != XTD_OK) goto fail_cline_arg;
    if (cline_arg_add_option(cline_arg, XTD_NULL, "-v<:>--verbose", "Print verbose detail in the terminal", FALSE) != XTD_OK) goto fail_cline_arg;
    if (cline_arg_collect_orphans(cline_arg, "source", TRUE) != XTD_OK) goto fail_cline_arg;
    if (cline_arg_section_help(cline_arg, XTD_NULL, XTD_NULL, &help_text) != XTD_OK) goto fail_cline_arg;
    if ((status = cline_arg_parse_in_range(cline_arg, 1, argc, argv)) != XTD_OK) goto fail_cline_arg_parser;
    if (cline_arg_has_option(cline_arg, XTD_NULL, "-h") != XTD_OK) { fprintf(stdout, "%s", help_text); return EXIT_SUCCESS; }
    
    slocer = (Slocer *) allocator.memory_calloc(1, sizeof(Slocer));
    slocer->line_count = 0;
    slocer->source_line_count = 0;
    slocer->allocator = allocator;
    slocer->last_char_is_whitespace = FALSE;
    slocer->verbose = cline_arg_has_option(cline_arg, XTD_NULL, "-v");
    slocer->recurse = cline_arg_has_option(cline_arg, XTD_NULL, "-r");
    if (!slocer) goto fail_slocer_init;
    size = cline_arg_get_orphan_values(cline_arg, &values);
    count_lines_in_files_and_folders(slocer, values, size);

    fprintf(stdout, "%sTotal (LOC): %d%s\n", CLINE_FE(CLINE_FE_FOREGROUND_GREEN), slocer->line_count, CLINE_FE(CLINE_FE_RESET));
    fprintf(stdout, "%sTotal (SLOC): %d%s\n", CLINE_FE(CLINE_FE_FOREGROUND_GREEN), slocer->source_line_count, CLINE_FE(CLINE_FE_RESET));
    allocator.memory_free(slocer);
    return EXIT_SUCCESS;

    fail_cline_arg:
        destroy_cline_arg(cline_arg);
        return fatal_error("Error occur while initializing argument parser");
    
    fail_cline_arg_parser:
        destroy_cline_arg(cline_arg);
        return fatal_error2("Error occur while parsing cli arg", status);
    
    fail_slocer_init:
        destroy_cline_arg(cline_arg);
        return fatal_error("Unable to initialize the slocer object");
}
