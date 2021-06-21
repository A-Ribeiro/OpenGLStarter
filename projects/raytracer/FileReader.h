#ifndef FileReader_H
#define FileReader_H

#include <stdio.h>
#include <string>

class FileReader {
    
    FILE* fileDescriptor;

    void skipWhiteSpacesAndComments() {
        int c = -1;

        c = getc(fileDescriptor);

        while (!feof(fileDescriptor) && (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '#' )) {
            
            if (c == '#') {
                while (!feof(fileDescriptor) && c != '\n')
                    c = getc(fileDescriptor);
            } else
                c = getc(fileDescriptor);

        }

        if (c != EOF)
            ungetc(c, fileDescriptor);
    }

public:

    FileReader(const std::string &filename) {
        //remove all comments
        fileDescriptor = fopen(filename.c_str(), "r");
        
        ARIBEIRO_ABORT(
            !fileDescriptor,
            "Error to open file: %s\n", filename.c_str());

        //if (!fileDescriptor) {
        //    fprintf(stderr, "Error to open file: %s\n", filename.c_str());
        //    exit(-1);
        //}

    }

    ~FileReader() {
        if (fileDescriptor)
            fclose(fileDescriptor);
    }

    int readInt() {
        if (!fileDescriptor || feof(fileDescriptor))
            return 0;
        
        skipWhiteSpacesAndComments();

        int result;
        fscanf(fileDescriptor, "%i", &result);

        return result;
    }

    float readFloat() {
        if (!fileDescriptor || feof(fileDescriptor))
            return 0;

        skipWhiteSpacesAndComments();

        float result;
        fscanf(fileDescriptor, "%f", &result);

        return result;
    }

    std::string readString() {
        if (!fileDescriptor || feof(fileDescriptor))
            return 0;

        skipWhiteSpacesAndComments();

        std::string result;

        int c;

        c = getc(fileDescriptor);
        while (c != ' ' && c != '\t' && c != '\n' && c != '\r' && c != EOF && c != '#' ) {
            result = result + (char)c;
            c = getc(fileDescriptor);
        }

        if (c == '#')
            ungetc(c, fileDescriptor);

        return result;
    }
};


#endif