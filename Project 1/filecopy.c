// Class      : Operating Systems
// Assignment : Project 1
// Authors    : Gabriel Amodeo Elbling & Felixander Kery
// File       : filecopy.c -> copies files from one place to another

#include <stdio.h>

int main(int argc, char *argv[]) {
    // check for correct usage
    if (argc != 3) {
        printf("Usage: filecopy source_file target_file\n");
        return 1;
    }

    // open source file and return if cannot open
    FILE *source_file = fopen(argv[1], "rb");
    if (source_file == NULL) {
        printf("filecopy: couldn't open %s: No such file or directory.\n", argv[1]);
        return 1;
    }

    // open target file and return if cannot open
    FILE *target_file = fopen(argv[2], "wb");
    if (target_file == NULL) {
        printf("filecopy: couldn't open %s for writing.\n", argv[2]);
        fclose(source_file);
        return 1;
    }

    // set vars and buffer
    int byte_count = 0;
    char buffer[4096];
    size_t read_bytes;

    // reading loop that copies the contents over until EOF
    while ((read_bytes = fread(buffer, 1, sizeof(buffer), source_file)) > 0) {
        size_t written_bytes = fwrite(buffer, 1, read_bytes, target_file);

        // check to see if there has been a read/write error
        if (written_bytes != read_bytes) {
            printf("filecopy: write error.\n");
            fclose(source_file);
            fclose(target_file);
            return 1;
        }

        // add written bytes to current byte count
        byte_count += written_bytes;
    }

    // close target and source files
    fclose(source_file);
    fclose(target_file);

    printf("filecopy: copied %d bytes from %s to %s\n", byte_count, argv[1], argv[2]);
    return 0;
}
