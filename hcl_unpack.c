/*
 * Minimal HCL archive extractor for Bonjour Madame (HCL, 1998).
 *
 * No third-party library is required. Archive and output files are accessed
 * through the C stdio API (fopen/fread/fwrite/fseek).
 *
 * Build examples:
 *   Microsoft C: cl /nologo /W4 /O2 /MT hcl_unpack.c /Fe:hcl_unpack.exe
 *   MinGW GCC:   gcc -std=c99 -Wall -Wextra -O2 -o hcl_unpack.exe hcl_unpack.c
 *
 * Usage:
 *   hcl_unpack BONJOUR.HCL BONJOUR
 */

#if defined(_MSC_VER)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define PATH_SEPARATOR '\\'
#else
#include <sys/types.h>
#define PATH_SEPARATOR '/'
#endif

#define HCL_MAX_ENTRIES 256u
#define HCL_NAME_SIZE 12u
#define HCL_ENTRY_SIZE 20u
#define HCL_HEADER_SIZE (4u + HCL_MAX_ENTRIES * HCL_ENTRY_SIZE)
#define COPY_BUFFER_SIZE 65536u

typedef struct HclEntry {
    char name[HCL_NAME_SIZE + 1u];
    uint32_t offset;
    uint32_t size;
} HclEntry;

static uint32_t read_le32(const unsigned char *bytes)
{
    return ((uint32_t)bytes[0]) |
           ((uint32_t)bytes[1] << 8) |
           ((uint32_t)bytes[2] << 16) |
           ((uint32_t)bytes[3] << 24);
}

static int read_exact(FILE *file, void *buffer, size_t size)
{
    return fread(buffer, 1u, size, file) == size;
}

static int ascii_equal_ignore_case(const char *left, const char *right)
{
    unsigned char a;
    unsigned char b;

    do {
        a = (unsigned char)*left++;
        b = (unsigned char)*right++;

        if (a >= (unsigned char)'A' && a <= (unsigned char)'Z') {
            a = (unsigned char)(a + ((unsigned char)'a' - (unsigned char)'A'));
        }
        if (b >= (unsigned char)'A' && b <= (unsigned char)'Z') {
            b = (unsigned char)(b + ((unsigned char)'a' - (unsigned char)'A'));
        }
    } while (a != 0u && a == b);

    return a == b;
}

static int valid_output_name(const char *name)
{
    const unsigned char *cursor = (const unsigned char *)name;

    if (name[0] == '\0' || strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
        return 0;
    }

    while (*cursor != 0u) {
        if (*cursor < 32u || *cursor == 127u ||
            *cursor == (unsigned char)'/' ||
            *cursor == (unsigned char)'\\' ||
            *cursor == (unsigned char)':') {
            return 0;
        }
        ++cursor;
    }

    return 1;
}

static int path_is_directory(const char *path)
{
#ifdef _WIN32
    struct _stat info;
    return _stat(path, &info) == 0 && (info.st_mode & _S_IFDIR) != 0;
#else
    struct stat info;
    return stat(path, &info) == 0 && S_ISDIR(info.st_mode);
#endif
}

static int path_exists(const char *path)
{
#ifdef _WIN32
    struct _stat info;
    return _stat(path, &info) == 0;
#else
    struct stat info;
    return stat(path, &info) == 0;
#endif
}

static int ensure_output_directory(const char *path)
{
    int result;

#ifdef _WIN32
    result = _mkdir(path);
#else
    result = mkdir(path, 0777);
#endif

    if (result == 0) {
        return 1;
    }

    if (errno == EEXIST && path_is_directory(path)) {
        return 1;
    }

    fprintf(stderr, "Error: cannot create output directory '%s': %s\n",
            path, strerror(errno));
    return 0;
}

static char *make_output_path(const char *directory, const char *name)
{
    size_t directory_length = strlen(directory);
    size_t name_length = strlen(name);
    int needs_separator;
    size_t total_length;
    char *path;

    needs_separator = directory_length > 0u &&
                      directory[directory_length - 1u] != '/' &&
                      directory[directory_length - 1u] != '\\';
    total_length = directory_length + (size_t)needs_separator + name_length + 1u;

    path = (char *)malloc(total_length);
    if (path == NULL) {
        return NULL;
    }

    memcpy(path, directory, directory_length);
    if (needs_separator) {
        path[directory_length++] = PATH_SEPARATOR;
    }
    memcpy(path + directory_length, name, name_length + 1u);
    return path;
}

static int ranges_overlap(const HclEntry *left, const HclEntry *right)
{
    uint64_t left_end = (uint64_t)left->offset + (uint64_t)left->size;
    uint64_t right_end = (uint64_t)right->offset + (uint64_t)right->size;

    return (uint64_t)left->offset < right_end &&
           (uint64_t)right->offset < left_end;
}

static int load_directory(FILE *archive,
                          uint64_t archive_size,
                          HclEntry entries[HCL_MAX_ENTRIES],
                          uint32_t *entry_count)
{
    unsigned char count_bytes[4];
    uint32_t count;
    uint32_t index;

    if (fseek(archive, 0L, SEEK_SET) != 0 ||
        !read_exact(archive, count_bytes, sizeof(count_bytes))) {
        fprintf(stderr, "Error: cannot read the HCL entry count.\n");
        return 0;
    }

    count = read_le32(count_bytes);
    if (count > HCL_MAX_ENTRIES) {
        fprintf(stderr, "Error: invalid HCL entry count %lu (maximum is %u).\n",
                (unsigned long)count, HCL_MAX_ENTRIES);
        return 0;
    }

    for (index = 0u; index < count; ++index) {
        unsigned char raw[HCL_ENTRY_SIZE];
        size_t name_length;
        uint64_t end;
        uint32_t previous;

        if (!read_exact(archive, raw, sizeof(raw))) {
            fprintf(stderr, "Error: truncated HCL directory at entry %lu.\n",
                    (unsigned long)index);
            return 0;
        }

        for (name_length = 0u;
             name_length < HCL_NAME_SIZE && raw[name_length] != 0u;
             ++name_length) {
            /* Find the mandatory NUL terminator. */
        }

        if (name_length == HCL_NAME_SIZE) {
            fprintf(stderr, "Error: entry %lu has no NUL-terminated name.\n",
                    (unsigned long)index);
            return 0;
        }

        memcpy(entries[index].name, raw, name_length);
        entries[index].name[name_length] = '\0';
        entries[index].offset = read_le32(raw + HCL_NAME_SIZE);
        entries[index].size = read_le32(raw + HCL_NAME_SIZE + 4u);

        if (!valid_output_name(entries[index].name)) {
            fprintf(stderr, "Error: entry %lu has an unsafe filename.\n",
                    (unsigned long)index);
            return 0;
        }

        end = (uint64_t)entries[index].offset + (uint64_t)entries[index].size;
        if ((uint64_t)entries[index].offset < (uint64_t)HCL_HEADER_SIZE ||
            end > archive_size) {
            fprintf(stderr,
                    "Error: entry %lu ('%s') is outside the archive "
                    "(offset=%lu, size=%lu).\n",
                    (unsigned long)index,
                    entries[index].name,
                    (unsigned long)entries[index].offset,
                    (unsigned long)entries[index].size);
            return 0;
        }

        for (previous = 0u; previous < index; ++previous) {
            if (ascii_equal_ignore_case(entries[index].name,
                                        entries[previous].name)) {
                fprintf(stderr,
                        "Error: duplicate filename '%s' in entries %lu and %lu.\n",
                        entries[index].name,
                        (unsigned long)previous,
                        (unsigned long)index);
                return 0;
            }
            if (ranges_overlap(&entries[index], &entries[previous])) {
                fprintf(stderr,
                        "Error: payloads for '%s' and '%s' overlap.\n",
                        entries[previous].name,
                        entries[index].name);
                return 0;
            }
        }
    }

    *entry_count = count;
    return 1;
}

static int extract_entry(FILE *archive,
                         const HclEntry *entry,
                         const char *output_directory,
                         unsigned char *copy_buffer)
{
    char *output_path;
    FILE *output;
    uint32_t remaining;

    output_path = make_output_path(output_directory, entry->name);
    if (output_path == NULL) {
        fprintf(stderr, "Error: out of memory while building an output path.\n");
        return 0;
    }

    if (path_exists(output_path)) {
        fprintf(stderr, "Error: refusing to overwrite existing path '%s'.\n",
                output_path);
        free(output_path);
        return 0;
    }

    if (entry->offset > (uint32_t)LONG_MAX ||
        fseek(archive, (long)entry->offset, SEEK_SET) != 0) {
        fprintf(stderr, "Error: cannot seek to '%s' in the archive.\n",
                entry->name);
        free(output_path);
        return 0;
    }

    output = fopen(output_path, "wb");
    if (output == NULL) {
        fprintf(stderr, "Error: cannot create '%s': %s\n",
                output_path, strerror(errno));
        free(output_path);
        return 0;
    }

    remaining = entry->size;
    while (remaining > 0u) {
        size_t chunk = remaining < COPY_BUFFER_SIZE
                           ? (size_t)remaining
                           : (size_t)COPY_BUFFER_SIZE;

        if (fread(copy_buffer, 1u, chunk, archive) != chunk) {
            fprintf(stderr, "Error: short read while extracting '%s'.\n",
                    entry->name);
            fclose(output);
            remove(output_path);
            free(output_path);
            return 0;
        }
        if (fwrite(copy_buffer, 1u, chunk, output) != chunk) {
            fprintf(stderr, "Error: cannot write '%s': %s\n",
                    output_path, strerror(errno));
            fclose(output);
            remove(output_path);
            free(output_path);
            return 0;
        }

        remaining -= (uint32_t)chunk;
    }

    if (fclose(output) != 0) {
        fprintf(stderr, "Error: cannot finalize '%s': %s\n",
                output_path, strerror(errno));
        remove(output_path);
        free(output_path);
        return 0;
    }

    printf("Extracted %-12s %10lu bytes\n",
           entry->name, (unsigned long)entry->size);
    free(output_path);
    return 1;
}

static void print_usage(const char *program_name)
{
    fprintf(stderr,
            "Usage: %s <archive.hcl> <output-directory>\n"
            "Example: %s BONJOUR.HCL BONJOUR\n",
            program_name, program_name);
}

int main(int argc, char **argv)
{
    FILE *archive;
    long archive_length;
    HclEntry entries[HCL_MAX_ENTRIES];
    uint32_t entry_count;
    uint32_t index;
    unsigned char *copy_buffer;
    int result = EXIT_FAILURE;

    if (argc != 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    archive = fopen(argv[1], "rb");
    if (archive == NULL) {
        fprintf(stderr, "Error: cannot open archive '%s': %s\n",
                argv[1], strerror(errno));
        return EXIT_FAILURE;
    }

    if (fseek(archive, 0L, SEEK_END) != 0 ||
        (archive_length = ftell(archive)) < 0L) {
        fprintf(stderr, "Error: cannot determine the size of '%s'.\n", argv[1]);
        fclose(archive);
        return EXIT_FAILURE;
    }

    if ((uint64_t)archive_length < (uint64_t)HCL_HEADER_SIZE) {
        fprintf(stderr, "Error: '%s' is too small to be an HCL archive.\n", argv[1]);
        fclose(archive);
        return EXIT_FAILURE;
    }

    if (!load_directory(archive,
                        (uint64_t)archive_length,
                        entries,
                        &entry_count)) {
        fclose(archive);
        return EXIT_FAILURE;
    }

    printf("Archive: %s\n", argv[1]);
    printf("Entries: %lu\n", (unsigned long)entry_count);
    printf("Output:  %s\n\n", argv[2]);

    if (!ensure_output_directory(argv[2])) {
        fclose(archive);
        return EXIT_FAILURE;
    }

    copy_buffer = (unsigned char *)malloc(COPY_BUFFER_SIZE);
    if (copy_buffer == NULL) {
        fprintf(stderr, "Error: cannot allocate the copy buffer.\n");
        fclose(archive);
        return EXIT_FAILURE;
    }

    for (index = 0u; index < entry_count; ++index) {
        if (!extract_entry(archive, &entries[index], argv[2], copy_buffer)) {
            goto cleanup;
        }
    }

    printf("\nDone: extracted %lu files.\n", (unsigned long)entry_count);
    result = EXIT_SUCCESS;

cleanup:
    free(copy_buffer);
    fclose(archive);
    return result;
}
