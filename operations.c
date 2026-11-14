#include "index.h"

//-------------------------------------------------------------------------------------------------//

int insert_file(file_t **files, char *fname)
{
    file_t *newnode = malloc(sizeof(file_t));
    
    if(newnode == NULL)
        return FAILURE;
    strcpy(newnode->fname,fname);
    newnode->link = NULL;

    if(*files == NULL)
    {
        *files = newnode;
        return SUCCESS;
    }

    file_t *temp = *files;

    while(temp->link != NULL)
    {
        temp = temp->link;
    }
    temp->link = newnode;
    return SUCCESS;
}

//-------------------------------------------------------------------------------------------------//

int isduplicate_file(char *fname, file_t *files)
{
    while(files!=NULL)
    {
        if(strcmp(files->fname,fname) == 0)
            return SUCCESS;

        files = files->link;
    }
    return FAILURE;
}

//-------------------------------------------------------------------------------------------------//
