

char* concat(const char *s1,const char *s2)
{
    char *result;
    result = malloc(strlen(s1)+strlen(s2)+1);
    if(result == NULL)
    {
        printf("Error: malloc failed in concat\n");
        exit(EXIT_FAILURE);
    }
    strcpy(result,s1);
    strcat(result,s2);
    return result;
}


char *trim_white_space(char *str)
{
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end+1) = '\0';

    return str;
}