#include <stdio.h>
 
 
int main(int argc, char *argv[]) {
    printf("Hello, World!\n");
    FILE *file1 = fopen(argv[1], "rb");
    if (file1 == NULL) {
        printf("Error opening file 1\n");
        return 1;
    }
 
    FILE *file2 = fopen(argv[2], "rb");
    if (file2 == NULL) {
        printf("Error opening file 2\n");
        fclose(file1);
        return 1;
    }
 
    int byte_count1 = 0;
    int byte_count2 = 0;
    while (!feof(file2) && !feof(file1)) {
        unsigned char byte1 = fgetc(file1);
        unsigned char byte2 = fgetc(file2);
        byte_count1++;
        byte_count2++;
 
        if (byte1 != byte2) {
            printf("Files differ\n");
            fclose(file1);
            fclose(file2);
            return 1;
        }
    }
 
    printf("Files are identical with %d bytes compared\n", byte_count1);
    printf("Files are identical with %d bytes compared\n", byte_count2);
    fclose(file1);
    fclose(file2);
    return 0;
}