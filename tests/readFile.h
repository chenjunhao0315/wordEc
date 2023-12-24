#ifndef READFILE_H
#define READFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>

std::string readFile(const char *filepath) {
    std::string context;
    FILE *f = fopen (filepath, "r");
    if (f) {
        fseek (f, 0, SEEK_END);
        size_t length = ftell (f);
        fseek (f, 0, SEEK_SET);
        char *buffer = (char*)malloc(length);
        if (buffer) {
            fread (buffer, 1, length, f);
        }
        fclose (f);
        context = std::string(buffer);
        free(buffer);
    }
    return context;
}

#endif // READFILE_H