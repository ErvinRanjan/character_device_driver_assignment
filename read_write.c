#include <stdio.h>
#include <stdlib.h>

int main(){
	FILE* fptr;
	char buf[16];

	fptr = fopen("/dev/character_device_1","r+");
	fprintf(fptr,"ERVIN_B220027CS");
	fscanf(fptr,"%s",buf);
	printf("%s\n",buf);
	fclose(fptr);

	return 0;
}
