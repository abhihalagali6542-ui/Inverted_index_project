#include "index.h"

int main(int argc,char *argv[])
{
    if(argc < 2)
    {
        printf("[ ERROR ] No input files provided\nUsage: ./a.out <file1> ...\n");
        return FAILURE;
    }

    file_t *files = NULL;
    mainnode_t *arr[27] = {0};

    validate_arguments(argv,&files);
    menu(&files,arr);
    
    return SUCCESS;
}