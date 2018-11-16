#include<stdlib.h>
#include<sys/stat.h>
#include "fileHandler.h"

inline bool file_exists (const char *file_name){
    struct stat buffer;   
    return (stat (file_name, &buffer) == 0); 
}

std::vector<char> file_read(char *file_name){
    std::vector<char> v;
    if (FILE *fp = fopen(file_name, "r"))
    {
        char buf[1024];
        while (size_t len = fread(buf, 1, sizeof(buf), fp))
            v.insert(v.end(), buf, buf + len);
        fclose(fp);
    }else{
        perror("Erorr, file can't be opened");
        return NULL;
    }
}


void file_write(char *data, char *file_name){

}

