#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct{
    int file_version;  // ���1�ł�
    int revisions;     // �ۑ������o�[�W�����̐�
}VersionFileheader;
typedef struct{
    int revision;      // �o�[�W����
    int file_length;   // �t�@�C���T�C�Y
}VersionFileRevision;
int main(int arugument_count, char* arugument[]){
    if(arugument_count == 0){
        return 0;
    }
    VersionFileheader x;
    VersionFileRevision y;
    x.file_version = 1;
    x.revisions = 1;
    y.revision = 1;
    FILE* fp = fopen(arugument[1], "r+");
    if(!fp){
            printf("Failed to commit %s to version control: No such file or directory",arugument[0]);
        }
    if(strcmp(arugument[0],"commit") == 0){
        if(!fp){
            printf("Failed to commit %s to version control: No such file or directory",arugument[0]);
        }
        else{

        }
    }
    fseek(fp, 0L, SEEK_END);
    y.file_length = ftell(fp);
    rewind(fp);
    fwrite(&x,sizeof(x),1,fp);
    fwrite(&y,sizeof(y),1,fp); 
    return 0;
}
