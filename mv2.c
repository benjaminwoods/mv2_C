// Modified Vigenere binary shift
// Varying incremental shifts on blocks of varying size in bytes
//
// Uses two keys: shifts (keyS) and bytesizes (keyB)
// !!!: keyS has each shift stored across 4 characters for non-ASCII input (max block size of 4 bytes)
//      For ASCII input, keyS has each letter shift stored as a capital letter.
//      keyB takes values of [1,2,3,4] (max block size is currently 4 bytes).
//
// Created by bjqw500 (first stable version, 22.12.16; last edited 22.12.16)
//
// Maximum block size is currently 4 bytes
// Maximum file size of 2 Gb (ftell)

#include <stdio.h> 
#include <stdlib.h>

typedef unsigned long block;
char* file_contents;
char *keyS, *keyB;

#define IN_NAME "in.txt"
#define OUT_NAME "out.txt"

long input_file_size;

// Function list
void capitalise(char* input,long size); 										// Takes ASCII input and produces capitalised output
void mv2(char* input,long size,char* keyS,char* keyB,long* els,int sign); 		// MV2 routine for non-ASCII input
void vig_ASCII(char* input,long size,char* keyS,long num_s,int sign);			// Vigenere routine for ASCII input
long keyS_toarray(char* keyS_file_name);										// Converts keyS (shifts) to char array
long keyB_toarray(char* keyB_file_name);										// Converts keyB (bytesizes) to char array
long file_toarray(char* input_file_name);										// Converts file to char array; returns file size in bytes
void array_tofile(char* output_file_name, long size);							// Converts char array to file

// Takes ASCII input and produces capitalised output
void capitalise(char* input,long size)
  {
	long n;
	
	for (n=0;n<size;n++)
	  {
		if ( ((input[n] >= 0) &&(input[n] < 65)) || ((input[n] > 90) && (input[n] < 97)) || ((input[n] > 122) && (input[n] < 128)) )
		  {
			input[n] = 32;
		  }
		else if ( (input[n] > 96) && (input[n] < 123) )
		  {
			input[n] = input[n] - 32;
		  }
	  }
  }

// Modified Vigenere two-key routine for non-ASCII input
//
// keyS = amount of shift (char array)
// keyB = number of bytes (char array)
// els = number of elements in keyS and keyB arrays
// sign = +1 for fwd shift
void mv2(char* input,long size,char* keyS,char* keyB,long* els,int sign)
  {
	long n;
	int p,q;
	block len,shift;
	block i;
	
	n=0; p=0; q=0;
	
	while (n<size)
	  {
		// Generate block length
		len = 1 << (8* (keyB[p % els[1]]- 48) ); //shift by 1 byte left for each byte (length is total number of values held by keyB[])
		
		// Generate shift
		shift = 0;
		for (q=0;q<4;q++) shift += keyS[(4*p+q) % els[0]] << (8*(3-q)); //concatenate 4 keyS[] chars into the shift for the block (looped via modulo)
	
		// Generate block
		i = 0;
		for (q=0;q<(keyB[p % els[1]]- 48);q++)	
		  {
			i += input[n+q] << (8*( (keyB[p % els[1]]- 48) -q-1)); //concatenate keyB[p] bytes into one temporary block (looped via modulo)
		  }
		
		//fprintf(stderr,"n=%i, i=%li\n",n,i);getchar();
		
		i = (i + len + sign*shift)%len; // Caesar shift
		
		// Retrieval
		for (q=0;q<(keyB[p % els[1]]- 48);q++)	
		  {
			input[n+q] = ( i >> (8*( (keyB[p % els[1]]- 48) -q-1) ) ) & (255); //mask off correct portion of block back into char
		  }
		 
		n += (keyB[p % els[1]]- 48); // Increment by bytes processed
		p++; // Increment to next value in keyB
	  }
  }

// Vigenere routine for ASCII input
//
// keyS = amount of shift (array)
// num_s = number of elements in shift array
// sign = +1 for fwd shift
void vig_ASCII(char* input,long size,char* keyS,long num_s,int sign)
{
	long n;
	int shift;
 
	capitalise(input,size);
	
	for (n=0;n<size;n++)
	  {
		shift = keyS[n % num_s] - 64; // For ASCII, keyS are capital letters, or @ for no shift
		if (input[n] != 32)	input[n] = 65 + ((input[n] - 65) + 26 + sign*shift)%26;
	  }
}

// Converts keyS (shifts) to char array
long keyS_toarray(char* keyS_file_name)
  {
	long Ssize;
	FILE *keyS_file = fopen(keyS_file_name, "rb");
	
	// Get keyS length (in bytes)
	fseek(keyS_file, 0, SEEK_END);
	Ssize = ftell(keyS_file);
	rewind(keyS_file);
	
	// Dynamically allocate memory
	keyS = (char*)malloc((Ssize) * sizeof(char));
		
	// Read file
	fread(keyS, sizeof(char), Ssize, keyS_file);
	
	// Close file stream
	fclose(keyS_file);
	
	return Ssize;
  }
  
// Converts keyB (bytesizes) to char array
long keyB_toarray(char* keyB_file_name)
  {
	long Bsize;
	FILE *keyB_file = fopen(keyB_file_name, "rb");
	
	// Get keyB length (in bytes)
	fseek(keyB_file, 0, SEEK_END);
	Bsize = ftell(keyB_file);
	rewind(keyB_file);
	
	// Dynamically allocate memory
	keyB = (char*)malloc((Bsize) * sizeof(char));
		
	// Read file
	fread(keyB, sizeof(char), Bsize, keyB_file);
	
	// Close file stream
	fclose(keyB_file);
	
	return Bsize;
  }

// Converts file to char array
long file_toarray(char* input_file_name)
  {
	long size,n;
	FILE *input_file = fopen(input_file_name, "rb");
	
	// Get file size
	fseek(input_file, 0, SEEK_END);
	size = ftell(input_file);
	rewind(input_file);
	
	// Dynamically allocate memory
	file_contents = (char*)malloc((size) * sizeof(char));
		
	// Read file
	fread(file_contents, sizeof(char), size, input_file);
	
	// Close file stream
	fclose(input_file);
	
	return size;
  }

// Converts char array to file
void array_tofile(char* output_file_name, long size)
  {
	FILE *output_file = fopen(output_file_name, "wb");
	
	// Write file
	fwrite(file_contents, sizeof(char), size, output_file);
	
	fclose(output_file);
  }

int main(int argc, char *argv[])
  {
	int sign;
	long num_byt;
	long size;
	long els[2]; // Elements in keyS and keyB respectively
	
	// arg initialisation
	if (argc == 1)
	  {
		return 0;
	  }
	else if (argc == 2)
	  {
		els[0] = keyS_toarray(argv[1]);
		sign = 1;
		els[1] = 0;
	  }
	else if (argc == 3)
	  {
		els[0] = keyS_toarray(argv[1]);
		sign = atoi(argv[2]);
		els[1] = 0;
	  }
	else
	  {
		els[0] = keyS_toarray(argv[1]);
		sign = atoi(argv[2]);
		els[1] = keyB_toarray(argv[3]);
	  }
	
	size = file_toarray(IN_NAME);
	
	// ASCII flag
	if (els[1] == 0)
	  { 	
		vig_ASCII(file_contents,size,keyS,els[0],sign);
	  }
	else
	  {
		mv2(file_contents,size,keyS,keyB,els,sign);
	  }
	
	array_tofile(OUT_NAME,size);
	
	return 0;
  }