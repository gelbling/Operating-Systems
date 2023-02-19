// Class      : Operating Systems
// Assignment : Project 1
// Authors    : Gabriel Amodeo Elbling & Felixander Kery
// File       : treecopy.c -> recursively copies directories

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#define BUFFER_SIZE 4096

// define function definitions
void copy_file(const char* source, const char* target);
void copy_directory(const char* source, const char* target);

// initialize global counter variables
int num_directories = 0;
int num_files       = 0;
long num_bytes      = 0;

int main(int argc, char* argv[]) {

    // check if there are too many arguments
    if (argc != 3) {
        fprintf(stderr, "treecopy: incorrect number of arguments!\n");
        fprintf(stderr, "usage: treecopy <sourcefile> <targetfile>\n");
        return 1;
    }

    // initialize pointers to source and target destinations
    char* source = argv[1];
    char* target = argv[2];

    // check if source directory can be accessed
    struct stat source_stat;
    if (stat(source, &source_stat) < 0) {
        fprintf(stderr, "treecopy: couldn't access source %s: %s\n", source, strerror(errno));
        return 1;
    }

    // check if it is a regular file or directory
    struct stat target_stat;
    if (stat(target, &target_stat) == 0) {
        // check if file or directory already exists
        if (S_ISREG(target_stat.st_mode)) {
            fprintf(stderr, "treecopy: target %s already exists and is a file\n", target);
            return 1;
        } else if (!S_ISDIR(target_stat.st_mode)) {
            fprintf(stderr, "treecopy: target %s already exists and is not a directory\n", target);
            return 1;
        }
    } else {
        if (mkdir(target, 0777) < 0) {
            fprintf(stderr, "treecopy: couldn't create directory %s: %s\n", target, strerror(errno));
            return 1;
        }
    }

    // check if source is file or directory or something else
    if (S_ISREG(source_stat.st_mode)) {
        copy_file(source, target);
    } else if (S_ISDIR(source_stat.st_mode)) {
        copy_directory(source, target);
    } else {
        fprintf(stderr, "treecopy: source %s is not a regular file or directory\n", source);
        return 1;
    }

    // print summary of the copy
    printf("treecopy: copied %d directories, %d files, and %ld bytes from %s to %s\n",num_directories, num_files, num_bytes, argv[1], argv[2]);

    return 0;
}

// function that copies a source file into a new target file
void copy_file(const char* source, const char* target) {

    // open file
    int source_fd = open(source, O_RDONLY);

    // check if it was able to be opened
    if (source_fd < 0) {
        fprintf(stderr, "treecopy: couldn't open file %s: %s\n", source, strerror(errno));
        exit(1);
    }

    // check if getting information about the file is possible
    struct stat source_stat;
    if (fstat(source_fd, &source_stat) < 0) {
        fprintf(stderr, "treecopy: couldn't get information about file %s: %s\n", source, strerror(errno));
        close(source_fd);
        exit(1);
    }

    // open and create target file
    int target_fd = open(target, O_WRONLY | O_CREAT | O_TRUNC, source_stat.st_mode);

    // check if file creation was successful
    if (target_fd < 0) {
        fprintf(stderr, "treecopy: couldn't create file %s: %s\n", target, strerror(errno));
        close(source_fd);
        exit(1);
    }

    // creater copying buffer
    char buffer[BUFFER_SIZE];

    // copy file contents into bytes_read variable 
    ssize_t bytes_read;
    while ((bytes_read = read(source_fd, buffer, BUFFER_SIZE)) > 0) {
        // copy the file, if the return from write is less than zero, there was an error
        // if not, file will be copied successfuly
        if (write(target_fd, buffer, bytes_read) < 0) {
            fprintf(stderr, "treecopy: couldn't write to file %s: %s\n", target, strerror(errno));
            close(source_fd);
            close(target_fd);
            exit(1);
        }
        // increment number of bytes copied
        num_bytes += bytes_read;
    }

    // check if reading the file is possible
    if (bytes_read < 0) {
        fprintf(stderr, "treecopy: couldn't read from file %s: %s\n", source, strerror(errno));
        close(source_fd);
        close(target_fd);
        exit(1);
    }

    // close source file, if it doesn't work exit with error
    if (close(source_fd) < 0) {
        fprintf(stderr, "treecopy: couldn't close file %s: %s\n", source, strerror(errno));
        exit(1);
    }

    // close target file, if it doesn't work exit with error
    if (close(target_fd) < 0) {
        fprintf(stderr, "treecopy: couldn't close file %s: %s\n", target, strerror(errno));
        exit(1);
    }

    // increment file number counter 
    num_files += 1;

    // print what is being currently copied 
    printf("%s -> %s\n", source, target);

}


// function that copies a directory recursively
void copy_directory(const char* source, const char* target) {

    // opening directory
    DIR* dir = opendir(source);

    // checking if directory was opened successfuly
    if (dir == NULL) {
        fprintf(stderr, "treecopy: couldn't open directory %s: %s\n", source, strerror(errno));
        exit(1);
    }

    // increment directories counter
    num_directories += 1;

    // read directory contents
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {

        // skip over and ignore current and parent directory
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        // construct full path of the source
        char source_path[BUFFER_SIZE];
        snprintf(source_path, BUFFER_SIZE, "%s/%s", source, entry->d_name);

        // construct full path of the target
        char target_path[BUFFER_SIZE];
        snprintf(target_path, BUFFER_SIZE, "%s/%s", target, entry->d_name);

        // check if accessing the source is possible
        struct stat source_stat;
        if (stat(source_path, &source_stat) < 0) {
            fprintf(stderr, "treecopy: couldn't access source %s: %s\n", source_path, strerror(errno));
            exit(1);
        }

        // if the given source is a file, call the function that will copy that file
        if (S_ISREG(source_stat.st_mode)) {
            copy_file(source_path, target_path);
        } 
        // if the given source is a directory, try to create a new directory
        // then recursively call the copy directory function in order to copy all elements in
        // that given directory
        else if (S_ISDIR(source_stat.st_mode)) {
            if (mkdir(target_path, source_stat.st_mode) < 0) {
                fprintf(stderr, "treecopy: couldn't create directory %s: %s\n", target_path, strerror(errno));
                exit(1);
            }
            // recursive call
            copy_directory(source_path, target_path);
        }
    }

    // try to close directory, if possible
    if (closedir(dir) < 0) {
        fprintf(stderr, "treecopy: couldn't close directory %s: %s\n", source, strerror(errno));
        exit(1);
    }
}



