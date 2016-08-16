#include "fft.h"

int main()
{
	int n = 8;
	float x[8] = {1,2,3,4,5,6,0,0};
	float y[8] = {0};
	
	fft(x, y, n);
	for (int i = 0; i < n; i++)
	{
		printf("%f  %f\n", x[i], y[i]);
	}
	printf("\n\n");
	fft(x, y, -n);
	for (int i = 0; i < n; i++)
	{
		printf("%f  %f\n", x[i], y[i]);
	}

	return 0;
}
