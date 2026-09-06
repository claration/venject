#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char PackageJson[] =
    "{"
    "  \"name\": \"discord\","
    "  \"main\": \"index.js\""
    "}";

static int write_u32_le(FILE *f, uint32_t value)
{
    unsigned char bytes[4] = {
        (unsigned char)(value & 0xff),
        (unsigned char)((value >> 8) & 0xff),
        (unsigned char)((value >> 16) & 0xff),
        (unsigned char)((value >> 24) & 0xff),
    };

    return fwrite(bytes, 1, sizeof(bytes), f) == sizeof(bytes) ? 0 : -1;
}

static unsigned char *read_file(
    const char *path,
    size_t *size
) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return NULL;
    }

    long len = ftell(f);
    if (len < 0) {
        fclose(f);
        return NULL;
    }

    if (fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        return NULL;
    }

    *size = (size_t)len;

    unsigned char *data = malloc(*size);

    if (*size != 0 && !data) {
        fclose(f);
        return NULL;
    }

    if (*size != 0 &&
        fread(data, 1, *size, f) != *size) {
        free(data);
        fclose(f);
        return NULL;
    }

    fclose(f);

    return data;
}

static int write_asar(
    const char *js_file,
    const char *out_file
) {
    size_t js_size;
    unsigned char *js = read_file(
        js_file,
        &js_size
    );

    if (!js && js_size != 0) return -1;

    const uint32_t indexJsSize = (uint32_t)js_size;
    const uint32_t packageJsonSize = (uint32_t)strlen(PackageJson);

    char header[4096];

    int headerStringSize = snprintf(
        header,
        sizeof(header),
        "{\"files\":{"
            "\"index.js\":{"
                "\"size\":%u,"
                "\"offset\":\"0\""
            "},"
            "\"package.json\":{"
                "\"size\":%u,"
                "\"offset\":\"%u\""
            "}"
        "}}",
        indexJsSize,
        packageJsonSize,
        indexJsSize
    );

    if (headerStringSize < 0 ||
        (size_t)headerStringSize >= sizeof(header)) {
        free(js);
        return -1;
    }

    const uint32_t headerSizeBytes = (uint32_t)headerStringSize;
    const uint32_t alignedSize = (headerSizeBytes + 3) & ~3u;
    const uint32_t headerSize = alignedSize + 8;
    const uint32_t headerObjectSize = alignedSize + 4;

    memset(
        header + headerSizeBytes,
        '0',
        alignedSize - headerSizeBytes
    );

    FILE *f = fopen(out_file, "wb");
    if (!f) {
        free(js);
        return -1;
    }

    if (write_u32_le(f, 4) != 0 ||
        write_u32_le(f, headerSize) != 0 ||
        write_u32_le(f, headerObjectSize) != 0 ||
        write_u32_le(f, headerSizeBytes) != 0) {
        fclose(f);
        free(js);
        return -1;
    }

    if (fwrite(
            header,
            1,
            alignedSize,
            f
        ) != alignedSize) {
        fclose(f);
        free(js);
        return -1;
    }

    if (fwrite(
            js,
            1,
            js_size,
            f
        ) != js_size) {
        fclose(f);
        free(js);
        return -1;
    }

    if (fwrite(
            PackageJson,
            1,
            packageJsonSize,
            f
        ) != packageJsonSize) {
        fclose(f);
        free(js);
        return -1;
    }

    free(js);

    return fclose(f) == 0 ? 0 : -1;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.js> <output.asar>\n", argv[0]);
        return 1;
    }

    if (write_asar(argv[1], argv[2]) != 0) {
        fprintf(stderr, "failed to create %s\n", argv[2]);
        return 1;
    }

    return 0;
}
