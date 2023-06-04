#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include <string.h>

void print_tree(const char* path,int level) {
    struct stat file_stat;
    if (lstat(path, &file_stat) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    char permissions[11];
    permissions[0] = (S_ISDIR(file_stat.st_mode)) ? 'd' : '-';
    permissions[1] = (file_stat.st_mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (file_stat.st_mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (file_stat.st_mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (file_stat.st_mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (file_stat.st_mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (file_stat.st_mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (file_stat.st_mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (file_stat.st_mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (file_stat.st_mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';

    struct passwd* pw = getpwuid(file_stat.st_uid);
    struct group* gr = getgrgid(file_stat.st_gid);

    for (size_t i = 0; i < (size_t)level; i++)
    {
        printf("|   ");
    }

    printf("├── %s %s %s %lld %s\n", permissions, pw->pw_name, gr->gr_name, (long long)file_stat.st_size, path);

    if (S_ISDIR(file_stat.st_mode)) {
        DIR* dir = opendir(path);
        if (dir == NULL) {
            perror("opendir");
            exit(EXIT_FAILURE);
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            char full_path[PATH_MAX];
            snprintf(full_path, PATH_MAX, "%s/%s", path, entry->d_name);
            print_tree(full_path, level + 1);
        }

        closedir(dir);
    }
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        char* path = argv[1];
        printf("path: %s\n",path);
        print_tree(path,0);
    } else {
        char* path = ".";
        print_tree(path,0);
    }
    return EXIT_SUCCESS;
}
