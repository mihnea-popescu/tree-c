#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

int dirtree(char *dirname, char *prefix) {
    struct dirent *de;
    char *pre = calloc(strlen(prefix) + 2, sizeof(char));
    int succes;

    if(pre == NULL) {
        return -1;
    }

    strcpy(pre, prefix);

    DIR *dr = opendir(dirname);

    if (dr == NULL) {
        succes = errno;
        free(pre);
        errno = succes;
        return -1;
    }

    while ((de = readdir(dr)) != NULL) {
        if((strcmp(de->d_name, ".") == 0) || (strcmp(de->d_name, "..") == 0)) continue;

        struct stat s;
        printf("%s%s\n", pre, de->d_name);


        char *filepath = calloc(strlen(dirname) + strlen(de->d_name) + 2, sizeof(char));

        if(filepath == NULL) {
            succes = errno;
            closedir(dr);
            free(pre);
            errno = succes;
            return -1;
        }

        strcat(filepath, dirname);
        if(strlen(filepath) > 0 && filepath[strlen(filepath) - 1] != '/') {
            strcat(filepath, "/");
        }
        strcat(filepath, de->d_name);

        
        if(stat(filepath,&s) == -1) {
            succes = errno;
            free(filepath);
            free(pre);
            closedir(dr);
            errno = succes;

            return -1;
        }

        if((s.st_mode & S_IFMT) == S_IFDIR) {
            char *newPrefix = calloc(strlen(dirname) + 2, sizeof(char));
        
            if(newPrefix == NULL) {
                succes = errno;
                free(filepath);
                free(pre);
                closedir(dr);
                errno = succes;
                return -1;
            }
            strcpy(newPrefix, pre);
            strcat(newPrefix, "--");
            if(dirtree(filepath, newPrefix) == -1) {
                succes = errno;
                free(filepath);
                free(newPrefix);
                free(pre);
                closedir(dr);
                errno = succes;
                return -1;
            }
            free(newPrefix);
        }

        free(filepath);
    }

    closedir(dr);
    free(pre);
    return 0;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        fprintf(stderr, "USE CASE: %s <path to directory>\n", argv[0]);
        return 200;
    }

    printf("%s%c",argv[1],'\n');
    if(dirtree(argv[1], "-") == -1) {
        perror(argv[1]);
        return 200;
    }

    return 0;

}