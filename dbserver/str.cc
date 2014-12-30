#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include "str.h"

//extern long long TOTAL_MALLOC;

namespace ldb {
namespace dbserver {

str_t *create_str(const char *str, int str_len)
{
    int total = sizeof(str_t) + str_len + 1;

    str_t *p = (str_t *)malloc( total );
    if (p == NULL) {
        return NULL;
    }
    
    //TOTAL_MALLOC += total;
//    fprintf(stderr, "sizeof(str_t) + str_len + 1 : %d\n", total);

    p->len = str_len;
    p->free = 0;
    
    if (str != NULL)
        memcpy( p->buf, str, str_len );

    p->buf[str_len] = '\0';

    return p;
}

void free_str_t(str_t *str)
{
    assert(str != NULL);
#if 0
    if( TOTAL_MALLOC > 0){
        TOTAL_MALLOC -= (sizeof(str_t) + str->len + str->free + 1);
    }
#endif
    free(str);
}

void free_str(char *str)
{
    assert(str != NULL);
#if 0 
    if( TOTAL_MALLOC > 0){
        TOTAL_MALLOC -= (strlen(str) + 1);
    }
#endif
    free(str);
}

int str_len( const str_t *str )
{
    assert(str != NULL);

    return str->len;
}

int str_avail(const str_t *str)
{
    assert(str != NULL);

    return str->free;
}

void str_clear(str_t *str)
{
    str->free += str->len;
    str->len = 0;
    str->buf[0] = '\0';
}

int str_cat( str_t *istr, const char *str)
{
    assert(istr != NULL && str != NULL);
    
    int len = istr->len;

    istr->len += strlen(str);

    if ( strlen(str) > (size_t)istr->free ) {

        if (istr->len < ONE_M_) {

            str_t *p = (str_t *)realloc(istr, sizeof(str_t) + 2 * istr->len + 1);

            if (p == NULL) {
                return -1;
            }

            //TOTAL_MALLOC += (2 * istr->len - len - istr->free);
            if (p != istr) {
            //    free_str_t(istr);
                istr = p;
            }
            istr->free = istr->len;

        } else if (istr->len >= ONE_M_) {

            str_t *p = (str_t *)realloc(istr, sizeof(str_t) + ONE_M_ + istr->len + 1);
            if (p == NULL) {
                return -1;
            }

            //TOTAL_MALLOC += (ONE_M_ + istr->len  - len - istr->free);
            if (p != istr) {
            //    free_str_t(istr);
                istr = p;
            }
            istr->free = ONE_M_;
        }
    } else {
        istr->free -= strlen(str);
    }
    
    //strcat(istr->buf, str);
    memcpy(istr->buf + len, str, strlen(str));
    
    istr->buf[istr->len] = '\0';

    return 0;
}

int str_cat_str( str_t *str1, str_t *str2)
{
    assert (str1 != NULL && str2 != NULL);

    return str_cat(str1, str2->buf);
}

char *str_trim( char *istr, const char *str)
{
    assert(istr != NULL && str != NULL);

    int start_p = 0;
    int end_p = strlen(istr) - 1;

    while( strchr(str, istr[start_p]) != 0){
        start_p++;
    }

    while( strchr(str, istr[end_p]) != 0) {
        end_p--;
    }

    int len = end_p - start_p + 1;
    if( len <= 0 ) {
        return NULL;
    }
#if 0 
    char *new_str = (char *)malloc(len + 1);
    memcpy(new_str, istr + start_p, len);
    new_str[len] = '\0';
    TOTAL_MALLOC += (len + 1);
    return new_str;
#endif 
    memmove(istr, istr + start_p, len);
    istr[len] = '\0';

    return istr;
}

int str_cmp( const str_t *str1, const str_t *str2)
{
    assert(str1 != NULL && str2 != NULL);
    
    return strcmp(str1->buf, str2->buf);
}

int strs2tokens(char *strs, const char *delim, char *tokens[], int *num)
{
    assert(strs != NULL && delim != NULL && tokens != NULL);

    char *token = strtok(strs, delim);

    int i = 0;
    while (token){
        if (token[0] == '\n') {
            break;
        }

        tokens[i++] = token;
        token = strtok(NULL, delim);
    }   
    tokens[i] = NULL;
    
    if (num != NULL) *num = i;

    return 0;
}

void str2lower(char *str)
{
    assert(str != NULL);
    while (*str != '\0') {
        *str = (char)tolower(*str);
        str++;
    }

}

} /*namespace ldb*/
} /*namespace dbserver*/

