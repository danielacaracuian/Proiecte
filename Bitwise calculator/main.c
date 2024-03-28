#include <stdio.h>
#include <stdlib.h>

typedef void (*operations)(int *n1, int *n2, int *rez);

int * getBits(size_t const size, void const * const ptr, int *binArray)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j, l = 0;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {         
            byte = (b[i] >> j) & 1;
            binArray[l++] = (int)byte;
            printf("%u", byte);
        }
    }
    printf("\n");
    return binArray;
}

void add3Bits(int bit1, int bit2, int bit3, int *rez) {
	int r = bit1 + bit2 + bit3;
	if(r == 3) {
		rez[0] = 1;
		rez[1] = 1;
	}
	if(r == 2) {
		rez[0] = 0;
		rez[1] = 1;
	}
	if(r == 1) {
		rez[0] = 1;
		rez[1] = 0;
	}
	if(r == 0) {
		rez[0] = 0;
		rez[1] = 0;
	}
}

void add4BitNumbers(int *n1, int *n2, int *rez) {
	int r[2] = {0, 0};
	// add last bit
	add3Bits(n1[3], n2[3], 0, r);
	rez[3] = r[0];
	rez[2] = r[1];
	// add the third bit
	add3Bits(n1[2], n2[2], rez[2], r);
	rez[2] = r[0];
	rez[1] = r[1];
	// add the second bit
	add3Bits(n1[1], n2[1], rez[1], r);
	rez[1] = r[0];
	rez[0] = r[1];
	// add the first bit
	add3Bits(n1[0], n2[0], rez[0], r);
	rez[0] = r[0];
}

int toDec(int bit1, int bit2, int bit3, int bit4) {
	return bit1 * 8 + bit2 * 4 + bit3 * 2 + bit4; 
}

void exchange(int *n1, int *n2, int *rez) {
	for(int i = 0; i < 4; i++) {
		rez[i] = n1[i];
	}
	int firstIndex = toDec(0, 0, n2[0], n2[1]);
	int secondIndex = toDec(0, 0, n2[2], n2[3]);
	int temp = rez[firstIndex];
	rez[firstIndex] = rez[secondIndex];
	rez[secondIndex] = temp;
}

void rotation(int *n1, int *n2, int *rez) {
	for(int i = 0; i < 4; i++) {
		rez[i] = n1[i];
	}
	int rotationNr = toDec(n2[0], n2[1], n2[2], n2[3]);
	for(int i = 0; i < rotationNr; i++) {
		int temp = rez[0];
		for(int j = 0; j < 3; j++) {
			rez[j] = rez[j+1];
		}
		rez[3] = temp;
	}
}

void XOR(int *n1, int *n2, int *rez) {
	for (int i = 0; i < 4 ; i++) {   
		rez[i] = n1[i] == n2[i] ? 0 : 1;
	}
}

void getBitsFromBinarry(int *binArray, int start, int length, int *rez) {
	for(int i = start, j = 0; i < length; i++, j++) {
		rez[j] = binArray[i];
	}
}

void copyArray(int *a, int *b) {
	for (int i = 0; i < 4 ; i++) {   
		b[i] = a[i];
	}
}

int main(int argv, char* argc[])
{
	int N = 3;
    int M = 3035218;
    int bArray[32];
	getBits(sizeof(M), &M, bArray);
    
	int dataLength = N*6 + 4;
	int bData[dataLength];
	for (int i = 32 - dataLength, j = 0; i < 32 ; i++, j++) {   
		bData[j] = bArray[i];
	}
	printf("\bData to be used:\n");
	for (int i = 0; i < dataLength ; i++) {   
		printf("%d", bData[i]);
	}
	/*
	int n1[4] = {0,0,0,1};
	int n2[4] = {1,1,0,0};
	int rez[4] ={0,0,0,0};
   	add4BitNumbers(n1, n2, rez);
   	
   	printf("\n");
   	for (int i = 0; i < 4 ; i++) {   
		printf("%d", rez[i]);
	}
	printf("\nExchange\n");
	
	exchange(n1, n2, rez);
   	for (int i = 0; i < 4 ; i++) {   
		printf("%d", rez[i]);
	}
	
	printf("\nRotation\n");
	//rotatie pe 1011 si 0101 = 5, rezultatul va fi 0111
	int rn1[4] = {1,0,1,1};
	int rn2[4] = {0,1,0,1};
	rotation(rn1, rn2, rez);
   	for (int i = 0; i < 4 ; i++) {   
		printf("%d", rez[i]);
	}
	
	printf("\nXOR\n");
	//XOR pentru 1011 si 0101, va rezulta 1110
	int xn1[4] = {1,0,1,1};
	int xn2[4] = {0,1,0,1};
	int xrez[4] ={0,0,0,0};
	XOR(xn1, xn2, xrez);
   	for (int i = 0; i < 4 ; i++) {   
		printf("%d", xrez[i]);
	}
	*/
	printf("\n = = = = = = = = = = = = = = = =\n");
	operations func[4] = {&add4BitNumbers, &exchange, &rotation, &XOR}; 
	//for (int i = 0; i < 4; ++i) 
	//	 func[i](xn1, xn2, xrez);
	
	int firstNumber[4] = {0, 0, 0, 0}; 
	int secondNumber[4] = {0, 0, 0, 0}; 
	int result[4] = {0, 0, 0, 0}; 
	int binOperation[2] = {0, 0};
	int operation = 0;
	
	// first operation
	getBitsFromBinarry(bData, 0, 4, firstNumber);
	getBitsFromBinarry(bData, 4, 6, binOperation);	 
	getBitsFromBinarry(bData, 6, 10, secondNumber);
	operation = toDec(0, 0, binOperation[0], binOperation[1]);
	printf("\nOperation nr: %d \n", operation);
	
	func[operation](firstNumber, secondNumber, result);
	for (int i = 0; i < 4 ; i++) {   
		printf("%d", result[i]);
	}
	
	// second operation
	getBitsFromBinarry(bData, 10, 12, binOperation);
	getBitsFromBinarry(bData, 12, 16, secondNumber);
	operation = toDec(0, 0, binOperation[0], binOperation[1]);
	printf("\nOperation nr: %d \n", operation);
	copyArray(result, firstNumber);
	
	func[operation](firstNumber, secondNumber, result);
	for (int i = 0; i < 4 ; i++) {   
		printf("%d", result[i]);
	}
	
	// thrid operation
	getBitsFromBinarry(bData, 16, 18, binOperation);
	getBitsFromBinarry(bData, 18, 22, secondNumber);
	operation = toDec(0, 0, binOperation[0], binOperation[1]);
	printf("\nOperation nr: %d \n", operation);
	copyArray(result, firstNumber);
	
	func[operation](firstNumber, secondNumber, result);
	for (int i = 0; i < 4 ; i++) {   
		printf("%d", result[i]);
	}
	
	printf("\nOutput: %d", toDec(result[0], result[1], result[2], result[3]));
    return 0;
}
