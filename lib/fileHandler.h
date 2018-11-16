#ifndef FILEHANDLER_LIB_H
#define FILEHANDLER_LIB_H
#include<stdlib.h>

inline bool file_exists (const char *file_name);
std::vector<char> file_read(char *file_name);
void file_write(char *data, char *file_name);

#endif //HTTPMESSAGE_LIB_H
