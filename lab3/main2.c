#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>

#define BUF_SIZE 1024

void create_directory(char* path) {
    if (mkdir(path, 0777) == -1) {
        perror("Error creating directory");
    }
}

void read_directory(char* path) {
    DIR* dirp;
    struct dirent* directp;
    dirp = opendir(path);
    if (dirp != NULL) {        
        while ((directp = readdir(dirp)) != NULL) {
            printf("%s\n", directp->d_name);
        }
        closedir(dirp);
    } else {
        perror("Error reading directory");
    }
}

void delete_directory(char* path) {
    if (rmdir(path) == -1) {
        perror("Error deleting directory");
    }
}

void create_file(char* path) {
    int fd = open(path, O_CREAT | O_WRONLY, 0777);
    if (fd == -1) {
        perror("Error creating file");
    } else {
        close(fd);
    }
}

void read_file(char* path) {
    int fd_src = open(path, O_RDONLY);
    if (fd_src == -1) {
		perror("Error opening file");
    }
    char* buf = (char*)malloc(BUF_SIZE);
	size_t  bytes_read;
	while((bytes_read = read(fd_src, buf, BUF_SIZE)) > 0) {
        printf("%.*s", bytes_read, buf);
	}
    close(fd_src);
}

void delete(char* path) {
    if (unlink(path) == -1) {
        perror("Error deleting file");
    }
}

void create_symbolic_link(char* old_path, char* new_path) {
    if (symlink(old_path, new_path) == -1) {
        perror("Error creating symbolic link");
    }
}

void read_symbolic_link(char* path) {
    char buffer[BUF_SIZE];
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        printf("%s\n", buffer);
    } else {
        perror("Error reading symbolic link");
    }
}

void read_file_through_symbolic_link(char* path) {
    char buffer[BUF_SIZE];
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        read_file(buffer);
    } else {
        perror("Error reading symbolic link");
    }
}

void create_hard_link(char* old_path, char* new_path) {
    if (link(old_path, new_path) == -1) {
        perror("Error creating hard link");
    }
}

void print_file_permissions_and_hard_links(char* path) {
    struct stat file_stat;
    if (stat(path, &file_stat) == -1) {
        perror("Error getting file info");
    } else {
        printf("Permissions: %o\n", file_stat.st_mode & 0777);
        printf("Hard links: %ld\n", file_stat.st_nlink);
    }
}

void change_file_permissions(char* path, mode_t mode) {
    if (chmod(path, mode) == -1) {
        perror("Error changing file permissions");
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Not enough args!\n");
        return 1;
    }
    char* action = argv[1];
    char* path = argv[2];

    if (strcmp(action, "1") == 0) {
        create_directory(path);
    } else if (strcmp(action, "2") == 0) {
        read_directory(path);
    } else if (strcmp(action, "3") == 0) {
        delete_directory(path);
    } else if (strcmp(action, "4") == 0) {
        create_file(path);
    } else if (strcmp(action, "5") == 0) {
        read_file(path);
    } else if (strcmp(action, "6") == 0) {
        delete(path);
    } else if (strcmp(action, "7") == 0 && argc >= 4) {
        create_symbolic_link(path, argv[3]);
    } else if (strcmp(action, "8") == 0) {
        read_symbolic_link(path);
    } else if (strcmp(action, "9") == 0) {
        read_file_through_symbolic_link(path);
    } else if (strcmp(action, "10") == 0 && argc >= 4) {
        create_hard_link(path, argv[3]);
    } else if (strcmp(action, "11") == 0) {
        print_file_permissions_and_hard_links(path);
    } else if (strcmp(action, "12") == 0 && argc >= 4) {
        mode_t mode = strtoul(argv[3], NULL, 8);
        change_file_permissions(path, mode);
    } else {
        printf("Unknown action: %s\n", action);
    }
}
