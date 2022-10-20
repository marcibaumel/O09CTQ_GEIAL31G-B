#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>

int main(int argc, char const *argv[])
{
    //Need variables 
    int headerSize = 112;
    int fileSize1 = 1080000;
    int fileSize2 = 4320000;
    char header[headerSize];
    int i;
    int j;


    //Allocated the memory space
    char *file1 = (char *)malloc(fileSize1 * sizeof(char));
    char *file2= (char *)malloc((fileSize2) * sizeof(char));
    char *mfSpace2 = (char *)malloc(fileSize1 * sizeof(char));
    char *newBody = (char *)malloc(fileSize1 * sizeof(char));

    FILE *space_1 = fopen("space_1.bmp", "rb");
    FILE *space_2 = fopen("space_2.bmp", "rb");
    FILE *new_space_file = fopen("space_merged.bmp", "wb");

    //Check the files
    if (space_2 == NULL || space_1 == NULL || new_space_file == NULL)
    {
        printf("FILE ERROR");
        return 0;
    }

    // Reading and set headers from file1
    fread(header, sizeof(char), headerSize, space_1);
    fseek(space_2, headerSize, SEEK_SET);

    // Read the file1 and file2 picture data
    fread(file1, sizeof(char), fileSize1, space_1);
    fread(file2, sizeof(char), fileSize2, space_2);

    // Recalculate the second picture body
    char *file2Column = (char *)malloc(fileSize1 * sizeof(char));
    char *file2Row = (char *)malloc(fileSize1 * sizeof(char));

    // Check the color value
    for (i = 0, j = 0; j < fileSize1 - 3600; i += 6, j += 3)
    {
        //See the column
        file2Column[j] = file2[i];

        //See the row element value
        file2Row[j] = file2[i + 3600];

        //The second value
        file2Column[j + 1] = file2[i + 1];
        file2Row[j + 1] = file2[i + 3601];

        //The third value
        file2Column[j + 2] = file2[i + 2];
        file2Row[j + 2] = file2[i + 3602];

        //Check end of the row
        if ((i % 3600) == 0)
        {
            i += 3600;
        }
    }

    // Merging the body for the new image
    for (i = 0; i < fileSize1; i += 32)
    {
        //Moves integer values from unaligned memory location to a destination vector
        __m256i columnData = _mm256_loadu_si256((__m256i *)&file2Column[i]);
        __m256i rowData = _mm256_loadu_si256((__m256i *)&file2Row[i]);

        //Computes the average of unsigned 8/16-bit integer data elements of two vectors. 
        __m256i avgData = _mm256_avg_epu8(columnData, rowData);

        //Moves integer values from unaligned memory location to a destination vector
        __m256i requiredSize = _mm256_loadu_si256((__m256i *)&(file1[i]));

        //Computes the average of unsigned 8/16-bit integer data elements of two vectors. 
        __m256i newImage = _mm256_avg_epu8(requiredSize, avgData);

        //Moves values from a integer vector to an unaligned memory location.
        _mm256_storeu_si256((__m256i *)&(newBody[i]), newImage);
    }

    // Make the header for the merged image
    fwrite(header, sizeof(char), sizeof(char) * headerSize, new_space_file);

    // Make the picture for the merged image
    fwrite(newBody, sizeof(char), sizeof(char) * fileSize1, new_space_file);

    // Liberate the allocated elements
    free(newBody);
    free(file1);
    free(file2);
    free(mfSpace2);
    fclose(space_1);
    fclose(space_2);
    fclose(new_space_file);
    return 0;
}