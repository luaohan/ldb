// ikv_str.h (2014-11-17)
// WangPeng (1245268612@qq.com)

#ifndef _IKV_STR_H_
#define _IKV_STR_H_

#define ONE_M_ 1024 * 1024 // 1M

typedef struct ikv_str_t 
{
    int len;    //str 的长度, buf 数组中已经使用字节的数量
    int free;   //buf 中的剩余容量
    char    buf[];  //字节数组
} ikv_str_t;


ikv_str_t *ikv_create_str(const char *str, int strlen);
void ikv_free_str_t( ikv_str_t *str );
void ikv_free_str(char *str);

int ikv_str_len( const ikv_str_t *str );
int ikv_str_avail( const ikv_str_t *str );

void ikv_str_clear( ikv_str_t *str );

//成功返回 0, 失败返回 -1
int ikv_str_cat( ikv_str_t *istr, const char *str);
int ikv_str_cat_str( ikv_str_t *str1, ikv_str_t *str2);

//对 istr 左右两端进行修剪，清除其中 str 指定的所有字符
//比如 ikv_str_trim(xxyyabcyyxy, "xy") 将返回 "abc"
//本函数中有 malloc 记住 free
char *ikv_str_trim( char *istr, const char *str);

int ikv_str_cmp( const ikv_str_t *str1, const ikv_str_t *str2);

//把str, 用delim 分开，将放在 tokens 数组中
//并且把最后的空格给去掉, *num 是 token 数组的大小
int strs2tokens(char *strs, const char *delim, char *tokens[], int *num);

void str2lower(char *str);

#endif
