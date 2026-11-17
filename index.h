#ifndef INDEX_H
#define INDEX_H

#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>

#define FILE_SIZE 25
#define SUCCESS 0
#define FAILURE -1

typedef struct f_node
{
    char fname[FILE_SIZE];
    struct f_node *link;
}file_t;

typedef struct s_node
{
    char file_name[FILE_SIZE];
    int word_count;
    struct s_node *link;
}subnode_t;

typedef struct m_node
{
    char word[FILE_SIZE];
    int file_count;
    subnode_t *sub_link;
    struct m_node *link;
}mainnode_t;

/* Function Prototypes */
//-------------------------------------------------------------------------------------------------//

int insert_file(file_t **, char *);
int Save_Database(mainnode_t *arr[]);
int isduplicate_file(char *,file_t *);
int validate_update_file(char *fname);
int menu(file_t **, mainnode_t *arr[]);
int Display_Database (mainnode_t *arr[]);
int remove_duplicate_file(file_t **, char *);
int Search_Database(char *, mainnode_t *arr[]);
void validate_arguments(char *argv[],file_t **);
int insert_word(char *,char *,mainnode_t *arr[]);
int Create_Database(file_t *, mainnode_t *arr[]);
int Update_Database(file_t **, mainnode_t *arr[]);
subnode_t *update_subnode(mainnode_t *arr[],int,char *,int);
mainnode_t *update_mainnode(mainnode_t *arr[],int,char *,int);

//-------------------------------------------------------------------------------------------------//

#endif