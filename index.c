#include "index.h"


int menu(file_t **files, mainnode_t *arr[])
{
    int choice;
    char word[FILE_SIZE];
    short create_flag = 0,update_flag = 0;

    printf("\n1. Create Database\n2. Display Database\n3. Search Database\n4. Update Database\n5. Save Database\n6. Exit\n");
    
    while(1)
    {
        printf("\nEnter your choice: ");
        scanf("%d",&choice);
        switch(choice)
        {
            case 1:
                if(create_flag == 0)
                {
                    Create_Database(*files,arr);
                    printf("\n[ SUCCESS ] Database created\n");
                    create_flag = 1;
                }
                else
                    printf("\n[ FAILURE ] Database already created\n");
            break;
            case 2:
                Display_Database(arr);
            break;
            case 3:
                printf("Enter word you want to search: ");
                scanf("%s",word);
                Search_Database(word, arr);
            break;
            case 4:
                if(update_flag == 0)
                {
                    if(create_flag == 1)
                    {
                        printf("\n[ FAILURE ] Database update is not allowed after database creation.\n");
                    }
                    else
                    {
                        while(Update_Database(files,arr)==FAILURE)
                        {
                            printf("[ INFO ] Re-enter valid File\n");
                        }
                            printf("\n[ SUCCESS ] Database updated\n");
                            update_flag = 1;
                    }
                }
                else
                {
                    printf("\n[ FAILURE ] Database already updated\n");
                }
            break;
            case 5:
                if(Save_Database(arr)==SUCCESS)
                    printf("\n[ SUCCESS ] Database saved\n");
            break;
            case 6:
                printf("\nExiting...\n");
                return SUCCESS;
            break;
            default:
                printf("\n[ ERROR ] Invalid choice! Re-try:\n");
        }
    }
}

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
        printf("\n[ ERROR ] Invalid file format. Only '.txt' files are allowed.\n");
        return FAILURE;
    }

    FILE *fptr = fopen(fname,"r");

    if(fptr == NULL)
    {
        fclose(fptr);
        printf("\n[ ERROR ] Unable to open %s file\n",fname);
        return FAILURE;
    }
    if(fgetc(fptr)==EOF)
    {
        fclose(fptr);
        printf("\n[ ERROR ] backup file %s is an empty file\n",fname);
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
        // printf("start char: %c\n end char: %c\n",start,end);
        fclose(fptr);
        printf("\n[ ERROR ] Backup file '%s' has an invalid format: Both the first and last characters must be '#'\n", fname);
        return FAILURE;
    }

    fclose(fptr);
    return SUCCESS;
}

//-------------------------------------------------------------------------------------------------//

void validate_arguments(char *argv[],file_t **files)
{
    for(int i=1;argv[i]!=0;i++)
    {
        char *dot=strrchr(argv[i],'.');
        if(dot == NULL || strcmp(dot,".txt")!=0)
        {
            printf("\n[ ERROR ] %s file should be \".txt\" type\n",argv[i]);
            continue;
        }

        FILE *fptr = fopen(argv[i],"r");

        if(fptr == NULL)
        {
            fclose(fptr);
            printf("\n[ ERROR ] Unable to open %s file\n",argv[i]);
            continue;
        }
        if(fgetc(fptr)==EOF)
        {
            printf("\n[ ERROR ] %s is an empty file\n",argv[i]);
            continue;
        }
        if(isduplicate_file(argv[i], *files)==SUCCESS)
        {
            printf("\n[ ERROR ] File %s is already Exists\n",argv[i]);
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
    char word[FILE_SIZE];
    while(files!=NULL)
    {
        FILE *fptr = fopen(files->fname,"r");
        if(fptr == NULL)
        {
            printf("\n[ ERROR ] Unable to open %s file\n",files->fname);
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
    int empty=0;

    for(int i=0;i<27;i++)
    {
        if(arr[i] == NULL)
            continue;

        mainnode_t *M_temp = arr[i];
        while(M_temp!=NULL)
        {
            if(empty == 0)
            {
                printf("\n---------------------------------------------------------------------------------\n");
                printf("Index\t|\tInformation");
                printf("\n---------------------------------------------------------------------------------\n");
                empty = 1;
            }
            printf("  %d\t|\t%s\t%d\t",i,M_temp->word,M_temp->file_count);
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

    if(empty == 0)
    {
       printf("\n[ INFO ] Database is empty. No records are available to display\n");
        return FAILURE;
    }
    printf("---------------------------------------------------------------------------------\n");

    return SUCCESS;
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
        printf("\n[ FAILURE ] Word %s not found\n",word);
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
        printf("\n[ FAILURE ] Word '%s' was not found in any of the files.\n", word);
        return FAILURE;
    }
    printf("\n");
    return SUCCESS;
}

//-------------------------------------------------------------------------------------------------//

int Update_Database(file_t **files, mainnode_t *arr[])
{
    char fname[FILE_SIZE],word[FILE_SIZE];
    int index,file_count,word_count;

    printf("\nEnter the backup file name to update the database: ");
    scanf("%s",fname);
    if(validate_update_file(fname) == FAILURE)
    {
        return FAILURE;
    }
    
    FILE *fptr = fopen(fname,"r");
    if(fptr == NULL)
    {
        fclose(fptr);
        printf("\n[ ERROR ] Unable to open %s file\n",fname);
        return FAILURE;
    }

    while(fscanf(fptr," #%d;%[^;];%d;",&index,word,&file_count)==3)
    {
        mainnode_t *M_node = update_mainnode(arr,index,word,file_count);
        // printf("Word: %s | File count: %d\n", word, file_count);
        char filename[FILE_SIZE];
        int word_count;
        
        for(int i = 0; i < file_count; i++)
        {
            if(fscanf(fptr, "%[^;];%d;", filename, &word_count) != 2)
                break;

            remove_duplicate_file(files,filename);

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
        fscanf(fptr, "#");
    }
    return SUCCESS;
}

//-------------------------------------------------------------------------------------------------//

int Save_Database(mainnode_t *arr[])
{
    int empty=0;
    char fname[FILE_SIZE];
    printf("\nEnter the backup file name to save the database: ");
    scanf("%s",fname);

    char *dot=strrchr(fname,'.');
    if(dot == NULL || strcmp(dot,".txt")!=0)
    {
        printf("\n[ ERROR ] Invalid file format. Only '.txt' files are allowed.\n");
        return FAILURE;
    }

    FILE *fptr = fopen(fname,"w+");
    if(fptr == NULL)
    {
        printf("\n[ ERROR ] Unable to open Database.csv file\n");
        return FAILURE;
    }

    for(int i=0;i<27;i++)
    {
        if(arr[i] == NULL)
            continue;

        mainnode_t *M_temp = arr[i];
        while(M_temp!=NULL)
        {
            empty = 1;
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
    if(empty == 0)
    {
        remove(fname);
        printf("\n[ INFO ] Database is empty. No records are available to save\n");
        return FAILURE;
    }
    return SUCCESS;
}

//-------------------------------------------------------------------------------------------------//
