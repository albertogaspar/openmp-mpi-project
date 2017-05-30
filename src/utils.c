#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _XOPEN_SOURCE

char *zstring_strtok(char *str, const char *delim) {
    static char *static_str=0;    	/* var to store last address */
    int index=0, strlength=0;      	/* integers for indexes */
    int found = 0;              	/* check if delim is found */

    /* delimiter cannot be NULL
    * if no more char left, return NULL as well
    */
    if (delim==0 || (str == 0 && static_str == 0))
        return 0;

    if (str == 0)
        str = static_str;

    /* get length of string */
    while(str[strlength])
        strlength++;

    /* find the first occurrence of delim */
    for (index=0;index<strlength;index++)
        if (str[index]==delim[0]) {
            found=1;
            break;
        }

    /* if delim is not contained in str, return str */
    if (!found) {
        static_str = 0;
        return str;
    }

    /* check for consecutive delimiters
    *if first char is delim, return delim
    */
    if (str[0]==delim[0]) {
        static_str = (str + 1);
        return (char *)delim;
    }

    /* terminate the string
    * this assignment requires char[], so str has to
    * be char[] rather than *char
    */
    str[index] = '\0';

    /* save the rest of the string */
    if ((str + index + 1)!=0)
        static_str = (str + index + 1);
    else
        static_str = 0;

        return str;
}

char* paxtok (char *str, char *seps) {
    static char *tpos, *tkn, *pos = NULL;
    static char savech;

    // Specific actions for first and subsequent calls.

    if (str != NULL) {
        // First call, set pointer.
        pos = str;
        savech = 'x';
    } else {
        // Subsequent calls, check we've done first.
        if (pos == NULL)
            return NULL;
        // Then put character back and advance.
        while (*pos != '\0')
            pos++;
        *pos++ = savech;
    }

    // Detect previous end of string.
    if (savech == '\0')
        return NULL;

    // Now we have pos pointing to first character.
    // Find first separator or nul.
    tpos = pos;
    while (*tpos != '\0') {
        tkn = strchr (seps, *tpos);
        if (tkn != NULL)
            break;
        tpos++;
    }

    savech = *tpos;
    *tpos = '\0';

    return pos;
}

char *strtok_new(char * string, char const * delimiter){
   static char *source = NULL;
   char *p, *riturn = 0;
   if(string != NULL){
       source = string;
   }
   if(source == NULL){
       return NULL;
   }
   if((p = strpbrk (source, delimiter)) != NULL) {
      *p  = 0;
      riturn = source;
      source = ++p;
   }
return riturn;
}


char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            //assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        //assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}


void str_split_2(char* a_str, const char a_delim, char* result[])
{
    //char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[1];
    delim[0] = a_delim;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);
    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    //result = malloc(sizeof(char*) * count);
    //char* result[7];
    if (result)
    {
        size_t idx  = 0;
        char* token = paxtok(a_str, delim);
        char *my = NULL;
        while (token)
        {
            //assert(idx < count);
            printf("Token %ld : %s\n", idx, token);
            char* tkn = strdup(token);
            result[idx++] = (*tkn)? tkn : "";
            token = paxtok(NULL, delim);
        }
        int j=0;
        for(j=0; j<7; j++){
            printf("%s\n", result[j]);
        }
        //assert(idx == count - 1);
        //*(result + idx) = 0;
    }
}

int utils_compare_int(int first, int second){
	if(first > second)
		return 1;
	if(second > first)
		return -1;
	return 0;
}
