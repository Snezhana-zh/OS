#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 1024

int lastIndexOf(const char* str, char ch) {
	int index = -1;
	for (int i = 0; i < strlen(str); ++i) {
		if (str[i] == ch) {
			index = i;
		}
	}
	return index;
}

char* subString(char* input, int startIndex, int endIndex) {
	if (startIndex > endIndex) {
		return NULL;
	}
	int len = endIndex - startIndex + 1;

	char* dest = (char*)malloc(sizeof(char) * (1 + len));
	if (!dest) {
		perror("malloc() failed!");
		fprintf(stderr, "%s", strerror(errno));
		return NULL;
	}
	strncpy (dest, input + startIndex, len);
	return dest;
}

char* reverseStr(const char* input) {
	int len = strlen(input);
	if (len <= 0) {
		printf("Size < 0!\n");
		return NULL;
	}

	char* dest = malloc(len + 1);
	if (!dest) {
		perror("malloc() failed!");
		fprintf(stderr, "%s", strerror(errno));
		return NULL;
	}

	for (int i = 0; i < len; ++i) {
		dest[i] = input[len - i - 1];
	}
	dest[len] = '\0';
	return dest;
}

void reverseBuf(char* input, size_t len) {
	for (int i = 0; i < len / 2; ++i) {
		char ch = input[i];
		input[i] = input[len - i - 1];
		input[len - i - 1] = ch;		
	}
}

char* concat(char* s1, char* s2) {
	if (s1 == NULL) {
		return s2;
	}
	if (s2 == NULL) {
		return s1;
	}

	int len1 = strlen(s1);
	int len2 = strlen(s2);                      

	char* result = malloc(len1 + len2 + 1);

	if (!result) {
		perror("malloc() failed!");
		fprintf(stderr, "%s", strerror(errno));
		return NULL;
	}

	memcpy(result, s1, len1);
	memcpy(result + len1, s2, len2 + 1);    

	return result;
}

void copy_files(char* src, char* dest) {
	DIR* dirp;
	struct dirent* directp;

	dirp = opendir(src);
	if (dirp == NULL) {
		perror("Can't create a dir!");
		fprintf(stderr, "%s", strerror(errno));
		return;
	}
	while(true) {
		directp = readdir(dirp);
		if (directp == NULL) {
			break;
		}

		if(directp->d_type == DT_REG) {
			char* filename = directp->d_name;
			char* new_filename = concat(concat(dest, "/"), reverseStr(filename));

			int fd_src = open(concat(concat(src, "/"), filename), O_RDONLY);
			char* src_name = concat(concat(src, "/"), filename);

			if(fd_src == -1) {
				perror("Error in opening srs file in cope_files()");
				fprintf(stderr, "%s", strerror(errno));
				break;
			}
			
			int fd_dest = open(new_filename, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			if(fd_dest == -1) {
				perror("Error in opening dest file in cope_files()");
				fprintf(stderr, "%s", strerror(errno));
				break;
			}
			
			char* buf = (char*)malloc(BUF_SIZE);
			size_t  bytes_read;
			while(true) {
				if ((bytes_read = read(fd_src, buf, BUF_SIZE)) <= 0) {
					break;
				}
				reverseBuf(buf, bytes_read);
				if (write(fd_dest, buf, bytes_read) == -1) {
					perror("Error in writing dest file in cope_files()");
					fprintf(stderr, "%s", strerror(errno));
					break;						
				}
			}
			close(fd_dest);
			close(fd_src);
			free(new_filename);
		}
	}
	closedir(dirp);
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Not enough args!\n");
		return 1;
	}
	char* path = argv[1];
	int startIndex = lastIndexOf(path, '/');
	char* nameDir = subString(path, startIndex + 1, strlen(path) - 1);
	printf("%s", nameDir);
	printf("\n");
	reverseBuf(nameDir, strlen(nameDir));
	printf("%s", nameDir);
	printf("\n");

	char* dest = concat(subString(path, 0, startIndex), nameDir);
	
	if (mkdir(dest, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
		printf("%s", dest);
		perror("Error in dir name!");
		fprintf(stderr, "%s", strerror(errno));
		return 1;
	}

	copy_files(path, dest);

	free(nameDir);
}
