/*
 * onegin.c
 * не учитывалось сущетвование пустых строчек
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#define ERR -1



typedef struct line
{
    char *s;
    int len;
}line;

int cpy_file_in_str(char* poem, FILE* f_in, long size_byte);
void fill_lines(line *lines, char *poem, int size_byte);
void print(line* lines, int n_lines, FILE* f_out);
int n_count(char* poem, int size);
long seek_size(FILE* f_in);
int comparator_2(const void *q1, const void *p1);
int comparator_1(const void *q1, const void *p1);



int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("put name of f_in, f_out, otladka\n");
        return 1;
    }
    
    FILE *f_in  = fopen(argv[1], "r");
    if (f_in == NULL)
    {  // ввлжу названия файлов для записи с терминала
        perror("in fopen for r");
        return 1;
    }
    FILE *f_out = fopen(argv[2], "w");
    if (f_out == NULL)
    {
        perror("in fopen for w");
        return 1;
    }
    FILE *otladka = fopen("otladka", "w");
    if (otladka == NULL)
    {
        perror("in otladka");
        return 1;
    }
    
    long size_byte = seek_size(f_in);
    char* poem = (char* ) calloc(size_byte + 2, sizeof(char));


    if ((cpy_file_in_str(poem, f_in, size_byte)) == ERR)
        fprintf(stderr, "error in mystrcpy\n");
    int n_lines = n_count(poem, size_byte);

    fputs(poem, otladka);
    line *lines = (line*) calloc(n_lines, sizeof(line));
    fill_lines(lines, poem, size_byte);
    if (n_lines == ERR) printf("error in memcht in fill_lines\n");
    
    fputs("\n~~~~~~~~~~~~~~~~sort 1~~~~~~~~~~~~\n\n", f_out);
    qsort((void*) lines, n_lines, sizeof(line), comparator_1);
    print(lines, n_lines, f_out);
    fputs("\n\n~~~~~~~~~~~~~~sort 2~~~~~~~~~~~~\n\n", f_out);
    qsort((void*) lines, n_lines, sizeof(line), comparator_2);
    print(lines, n_lines, f_out);
    fputs("\n\n~~~~~~~~~~~~~~text~~~~~~~~~~~~~~\n\n", f_out);
    fputs(poem, f_out);
    
    fclose(f_in);
    fclose(f_out);
    fclose(otladka);
    free(poem);
    free(lines);
    
    return 0;
}

long seek_size(FILE* f_in)
{
    fseek(f_in, 0, SEEK_END);
    long size_byte = ftell(f_in);
    fseek(f_in, 0, SEEK_SET);
   // printf("s = %d\n", size_byte);
    return size_byte;
}

int n_count(char* poem, int size)
{
    int i = 0;
    int k = 0;
    while(i < size)
    {
        if (poem[i++] == '\n')
            k++;
    }
    return k;
}

void print(line* lines, int n_lines, FILE* f_out)
{
    int i = 0;
    //int fd = fileno(f_out); 
    for (i = 0; i < n_lines; i++)
    {
        fwrite(lines[i].s, sizeof(char), lines[i].len, f_out);
        //write(fd, lines[i].s, lines[i].len);
    }
}


void fill_lines(line* lines, char *poem, int n)
{
    int i = 0;
    (lines)->s = poem;
    char* adr_n = poem;
    char* adr_last = poem;
    
    adr_n = memchr(poem, '\n', n);
    (lines)->len =adr_n - adr_last + 1;
    adr_last = adr_n;
    for (i = 1; *(adr_n + 1) != '\0'; i++)
    {
        assert(adr_n);
            
        (lines + i)->s = adr_n + 1;
        adr_n = memchr(adr_n + 1, '\n', n - (adr_n + 1 - poem));
        (lines + i)->len = adr_n - adr_last;
        adr_last = adr_n;
    }
}

int cpy_file_in_str(char* poem, FILE* f_in, long size_byte)                         //дописать ошибки на -1
{
    int fd = fileno(f_in);
    assert(fd > 0);

    int n_read = read(fd, poem, (unsigned) size_byte);
    
    if (n_read != size_byte)
    {
        printf("error in func read: n_read != size_byte\n");
        return ERR;
    }
    return 0;
}

int comparator_1(const void *q1, const void *p1)
{
    const line* q = (const line*) q1;
    const line* p = (const line*) p1;
    char *s1 = q->s;
    char *s2 = p->s;
    int delta = 0;
    
    
    int k = 0;
    int l = 0;
    while ((delta == 0) && (k < q->len) && (l < p->len))
        {
            if (isalpha(*(s1 + k)) == 0)
            {
                k++;
                continue;
            }
            if (isalpha(*(s2 + l)) == 0)
            {
                l++;
                continue;
            }
            delta = toupper(*(s1 + k)) - toupper(*(s2 + l));
            k++;
            l++;
            //printf("%d\n", delta);
        }
    if (delta == 0)
    {
        if (q->len > p->len)
            return 1;
        if (p->len > q->len)
            return -1;
        if (q->len == p->len)
            return 0;
    }
    return delta;
};

int comparator_2(const void *q1, const void *p1)
{
    const line* q = (const line*) q1;
    const line* p = (const line*) p1;
    char *s1 = q->s;
    char *s2 = p->s;
    int delta = 0;
    
    int k = q->len - 1;
    int l = p->len - 1;
    while ((delta == 0) && (k >= 0) && (l >= 0))
        {
            if (isalpha(*(s1 + k)) == 0)
            {
                k--;
                continue;
            }
            if (isalpha(*(s2 + l)) == 0)
            {
                l--;
                continue;
            }
            delta = toupper(*(s1 + k)) - toupper(*(s2 + l));
            k--;
            l--;
            //printf("%d\n", delta);
        }
    if (delta == 0)
    {
        if (q->len > p->len)
            return 1;
        if (p->len > q->len)
            return -1;
        if (q->len == p->len)
            return 0;
    }
    return delta;
}