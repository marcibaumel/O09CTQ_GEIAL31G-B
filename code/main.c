#include <stdio.h>
#include <immintrin.h>
#define N 8

int main(void)
{
	int a[] = {0, 1, 2, 3, 4, 5, 6, 7};
	int b[] = {8, 9, 10, 11, 12, 13, 14, 15};
	int c[N];

	int i;

	printf("a:\n");
	for (i = 0; i < N; i++)
		printf("%d\t", a[i]);
	printf("\n");

	printf("b:\n");
	for (i = 0; i < N; i++)
		printf("%d\t", b[i]);
	printf("\n");


	for (i = 0; i < N; i += 8) {
		__m256i mm_a = _mm256_loadu_si256((__m256i *)&(a[i])); //mm_a = <a[i], a[i+1], ..., a[i+7]>
		__m256i mm_b = _mm256_loadu_si256((__m256i *)&(b[i])); //mm_b = <b[i], b[i+1], ..., b[i+7]>

		__m256i mm_c = _mm256_add_epi32(mm_a, mm_b); //mm_c = mm_a + mm_b
		                                         //tehat: mm_c = <a[i] + b[i], a[i+1] + b[i+1], ..., a[i+7] + b[i+7]>

		_mm256_storeu_si256((__m256i *)&(c[i]), mm_c); //c[i..i+7] = mm_c
	}
	

	printf("c:\n");
	for (i = 0; i < N; i++)
		printf("%d\t", c[i]);
	printf("\n");

	return 0;
}