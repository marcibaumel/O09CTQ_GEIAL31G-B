#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>

#define HEADER_SIZE 54
#define FILE1_SIZE 1080000
#define FILE2_SIZE 4320000

void setHeaderTo600x600(unsigned char *);

int main(int argc, char const *argv[])
{
  int i;
  unsigned char *header = (unsigned char *)malloc(HEADER_SIZE * sizeof(unsigned char));
  unsigned char *space1 = (unsigned char *)malloc(FILE1_SIZE * sizeof(unsigned char));
  unsigned char *space2 = (unsigned char *)malloc((FILE2_SIZE / 2) * sizeof(unsigned char));
  unsigned char *mfSpace2 = (unsigned char *)malloc(FILE1_SIZE * sizeof(unsigned char));
  unsigned char *newSpace = (unsigned char *)malloc(FILE1_SIZE * sizeof(unsigned char));

  FILE *space_1_file = fopen("space_1.bmp", "rb");
  FILE *space_2_file = fopen("space_2.bmp", "rb");
  FILE *new_space_file = fopen("space_new.bmp", "wb");
  if (space_2_file == NULL || space_1_file == NULL || new_space_file == NULL)
  {
    printf("---\tERROR\t---\n");
    exit(1);
  }

  // printf("Reading and set headers\n");
  fread(header, sizeof(unsigned char), HEADER_SIZE, space_1_file);
  fseek(space_2_file, HEADER_SIZE, SEEK_SET);
  setHeaderTo600x600(header);

  // printf("Reading space1 data body\n");
  fread(space1, sizeof(unsigned char), FILE1_SIZE, space_1_file);
  i = 0;
  // printf("Reading space2 data body\n");
  for (i = 0; i < FILE2_SIZE; i += (1200 * 3))
  {
    fread(&space2[i], sizeof(unsigned char), 1200 * 3, space_2_file);
    if (fseek(space_2_file, 1200 * 3, SEEK_CUR) != 0)
    {
      break;
    };
  }

  // printf("Calculating space2 data body\n");
  int j;
  for (i = 0, j = 0; j < FILE1_SIZE; i += 6, j += 3)
  {
    mfSpace2[j] = (space2[i] + space2[i + 3]) / 2;
    mfSpace2[j + 1] = (space2[i + 1] + space2[i + 4]) / 2;
    mfSpace2[j + 2] = (space2[i + 2] + space2[i + 5]) / 2;
  }

  // printf("Calculating new space data body\n");
  for (i = 0; i < FILE1_SIZE; i += 32)
  {
    __m256i mm_a = _mm256_loadu_si256((__m256i *)&(space1[i]));
    __m256i mm_b = _mm256_loadu_si256((__m256i *)&(mfSpace2[i]));

    __m256i mm_c = _mm256_avg_epu8(mm_a, mm_b);

    _mm256_storeu_si256((__m256i *)&(newSpace[i]), mm_c);
  }

  // printf("Writing header data\n");
  fwrite(header, sizeof(unsigned char), sizeof(unsigned char) * HEADER_SIZE, new_space_file);

  // printf("Writing body data\n");
  fwrite(newSpace, sizeof(unsigned char), sizeof(unsigned char) * FILE1_SIZE, new_space_file);

  // printf("Free allocated memory space\n");
  free(header);
  free(newSpace);
  free(space1);
  free(space2);
  free(mfSpace2);
  fclose(space_1_file);
  fclose(space_2_file);
  fclose(new_space_file);
  return 0;
}

void setHeaderTo600x600(unsigned char *header)
{
  header[2] = 0xF6;
  header[3] = 0x7A;
  header[4] = 0x10;
  header[18] = 88;
  header[19] = 2;
  header[22] = 88;
  header[23] = 2;
  header[34] = 0xC0;
  header[35] = 0x7A;
  header[36] = 0x10;
  header[38] = 196;
  header[39] = 14;
  header[42] = 196;
  header[43] = 14;
}