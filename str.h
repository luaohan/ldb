// ldb_str.h (2014-11-17)
// WangPeng (1245268612@qq.com)

#ifndef _ldb_STR_H_
#define _ldb_STR_H_

#define ONE_M_ 1024 * 1024 // 1M

typedef struct ldb_str_t 
{
    int len;    //str 的长度, buf 数组中已经使用字节的数量
    int free;   //buf 中的剩余容量
    char    buf[];  //字节数组
} ldb_str_t;


ldb_str_t *ldb_create_str(const char *str, int strlen);
void ldb_free_str_t( ldb_str_t *str );
void ldb_free_str(char *str);

int ldb_str_len( const ldb_str_t *str );
int ldb_str_avail( const ldb_str_t *str );

void ldb_str_clear( ldb_str_t *str );

//成功返回 0, 失败返回 -1
int ldb_str_cat( ldb_str_t *istr, const char *str);
int ldb_str_cat_str( ldb_str_t *str1, ldb_str_t *str2);

//对 istr 左右两端进行修剪，清除其中 str 指定的所有字符
//比如 ldb_str_trim(xxyyabcyyxy, "xy") 将返回 "abc"
//本函数中有 malloc 记住 free
char *ldb_str_trim( char *istr, const char *str);

int ldb_str_cmp( const ldb_str_t *str1, const ldb_str_t *str2);

//把str, 用delim 分开，将放在 tokens 数组中
//并且把最后的空格给去掉, *num 是 token 数组的大小
int strs2tokens(char *strs, const char *delim, char *tokens[], int *num);

void str2lower(char *str);

#endif
