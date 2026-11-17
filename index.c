#include "index.h"

//-------------------------------------------------------------------------------------------------//

void display_file(file_t *head)
{
    if(head == NULL)
    {
        printf("List is empty\n");
        return;
    }
    printf("\nhead -> ");
    while(head!=NULL)
    {
        printf("%s -> ",head->fname);
        head = head->link;
    }
    printf("NULL\n");
}

//-------------------------------------------------------------------------------------------------//

int validate_update_file(char *fname)
{
    char start,end;

    char *dot=strrchr(fname,'.');

    if(dot == NULL || strcmp(dot,".txt")!=0)
    {
        printf("[ ERROR ] Invalid file format. Only '.txt' files are allowed.\n");
        return FAILURE;
    }

    FILE *fptr = fopen(fname,"r");

    if(fptr == NULL)
    {
        fclose(fptr);
        printf("[ ERROR ] Unable to open %s file\n",fname);
        return FAILURE;
    }
    if(fgetc(fptr)==EOF)
    {
        fclose(fptr);
        printf("[ ERROR ] backup file %s is an empty file\n",fname);
        return FAILURE;
    }
        
    rewind(fptr);
    start = fgetc(fptr);
    fseek(fptr,-1,SEEK_END);
    end = fgetc(fptr);
    if(end == '\n')
    {
        fseek(fptr, -2, SEEK_END);
        end = fgetc(fptr);
    }
        
    if(start != '#' || end != '#')
    {
        printf("start char: %c\n end char: %c\n",start,end);
        fclose(fptr);
        printf("[ ERROR ] Backup file '%s' has an invalid format: Both the first and last characters must be '#'.\n", fname);
        return FAILURE;
    }

    fclose(fptr);
    return SUCCESS;
}

//-------------------------------------------------------------------------------------------------//

int validate_arguments(char *argv[],file_t **files)
{
    for(int i=1;argv[i]!=0;i++)
    {
        char *dot=strrchr(argv[i],'.');
        if(dot == NULL || strcmp(dot,".txt")!=0)
        {
            printf("[ ERROR ] %s file should be \".txt\" type\n",argv[i]);
            continue;
        }

        FILE *fptr = fopen(argv[i],"r");

        if(fptr == NULL)
        {
            fclose(fptr);
            printf("[ ERROR ] Unable to open %s file\n",argv[i]);
            continue;
        }
        if(fgetc(fptr)==EOF)
        {
            printf("[ ERROR ] %s is an empty file\n",argv[i]);
            continue;
        }
        if(isduplicate_file(argv[i], *files)==SUCCESS)
        {
            printf("[ ERROR ] File %s is already Exists\n",argv[i]);
            continue;
        }

        insert_file(files,argv[i]);
        fclose(fptr);
    }
    display_file(*files);
}

//-------------------------------------------------------------------------------------------------//

int Create_Database(file_t *files, mainnode_t *arr[])
{
    char word[size];
    while(files!=NULL)
    {
        FILE *fptr = fopen(files->fname,"r");
        if(fptr == NULL)
        {
            printf("[ ERROR ] Unable to open %s file\n",files->fname);
            files = files->link;
            continue;
        }
        while(fscanf(fptr,"%s",word) != EOF)
        {
            insert_word(files->fname,word,arr);
        }
        fclose(fptr);
        files = files->link;
    }
    return SUCCESS;
}

//-------------------------------------------------------------------------------------------------//

int Display_Database (mainnode_t *arr[])
{
    for(int i=0;i<27;i++)
    {
        if(arr[i] == NULL)
            continue;

        mainnode_t *M_temp = arr[i];
        while(M_temp!=NULL)
        {
            printf("%d\t%s\t%d\t",i,M_temp->word,M_temp->file_count);
            if(M_temp->sub_link!=NULL)
            {
                subnode_t *S_temp = M_temp->sub_link;
                while(S_temp!=NULL)
                {
                    printf("%s\t%d\t",S_temp->file_name,S_temp->word_count);
                    S_temp = S_temp->link;
                }
                printf("\n");
            }
            M_temp = M_temp->link;
        }
    }
}

//-------------------------------------------------------------------------------------------------//

int Search_Database(char *word, mainnode_t *arr[])
{
    int index, Found=0;

    if(isupper(word[0]))
        index = word[0] - 'A';
    else if(islower(word[0]))
        index = word[0] - 'a';
    else
        index = 26;

    if(arr[index] == NULL)
    {
        printf("%s not found\n",word);
        return FAILURE;
    }
    mainnode_t *M_temp = arr[index];

    while(M_temp!=NULL)
    {
        if(strcmp(M_temp->word,word) == 0)
        {
            Found = 1;
            printf("\nWord '%s' is present in %d files\n",word,M_temp->file_count);
            if(M_temp->sub_link!=NULL)
            {
                subnode_t *S_temp = M_temp->sub_link;
                while(S_temp!=NULL)
                {
                    printf("In %s : %d times",S_temp->file_name,S_temp->word_count);
                    S_temp = S_temp->link;
                    if(S_temp!=NULL)
                        printf("\n");
                }
            }
        }
        M_temp = M_temp->link;
    }
    
    if(Found == 0)
    {
        printf("[ FAILURE ] Word '%s' was not found in any of the files.\n", word);
        return FAILURE;
    }
    printf("\n");
    

}

//-------------------------------------------------------------------------------------------------//

int Update_Database(file_t *files, mainnode_t *arr[])
{
    char fname[size],word[size];
    int index,file_count,word_count;

    printf("Enter the backup file name to update the database: ");
    scanf("%s",fname);
    if(validate_update_file(fname) == FAILURE)
    {
        return FAILURE;
    }
    
    FILE *fptr = fopen(fname,"r");
    if(fptr == NULL)
    {
        fclose(fptr);
        printf("[ ERROR ] Unable to open %s file\n",fname);
        return FAILURE;
    }

    while(fscanf(fptr," #%d;%[^;];%d;",&index,word,&file_count)==3)
    {
        mainnode_t *M_node = update_mainnode(arr,index,word,file_count);
        // printf("Word: %s | File count: %d\n", word, file_count);
        int i = 0;
        char filename[size];
        int word_count;
        
        for(int i = 0; i < file_count; i++)
        {
            if(fscanf(fptr, "%[^;];%d;", filename, &word_count) != 2)
                break;

            subnode_t *S_new = update_subnode(arr, index, filename, word_count);

            if(M_node->sub_link == NULL)
                M_node->sub_link = S_new;
            else
            {
                subnode_t *S_temp = M_node->sub_link;
                while(S_temp->link != NULL)
                    S_temp = S_temp->link;
                S_temp->link = S_new;
            }
        }
        // int c = fgetc(fptr);
        // printf("next char: %d\n", c);
        // ungetc(c, fptr);
        fscanf(fptr, "#");
    }
    return SUCCESS;
}

//-------------------------------------------------------------------------------------------------//

int Save_Database(mainnode_t *arr[])
{
    char fname[size];
    printf("Enter the backup file name to save the database: ");
    scanf("%s",fname);

    char *dot=strrchr(fname,'.');
    if(dot == NULL || strcmp(dot,".txt")!=0)
    {
        printf("[ ERROR ] Invalid file format. Only '.txt' files are allowed.\n");
        return FAILURE;
    }

    FILE *fptr = fopen(fname,"w+");
    if(fptr == NULL)
    {
        printf("[ ERROR ] Unable to open Database.csv file\n");
        return FAILURE;
    }

    for(int i=0;i<27;i++)
    {
        if(arr[i] == NULL)
            continue;

        mainnode_t *M_temp = arr[i];
        while(M_temp!=NULL)
        {
            fprintf(fptr,"#%d;%s;%d;",i,M_temp->word,M_temp->file_count);
            if(M_temp->sub_link!=NULL)
            {
                subnode_t *S_temp = M_temp->sub_link;
                while(S_temp!=NULL)
                {
                    fprintf(fptr,"%s;%d;",S_temp->file_name,S_temp->word_count);
                    S_temp = S_temp->link;
                }
                fprintf(fptr,"#\n");
            }
            M_temp = M_temp->link;
        }
    }
    fclose(fptr);
    return SUCCESS;
}

//-------------------------------------------------------------------------------------------------//
