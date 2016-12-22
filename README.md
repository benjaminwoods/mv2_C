# mv2_C
A small C code that performs a variety of modified Vigenere two-key shifts on data; although simple, in theory some of these shifts are 'unbreakable'.

***
COMPILING AND RUNNING:
***

	It's pretty straightforward to compile, as an example:
	
	gcc mv2.c -o mv2
	
	The binary takes 1-3 arguments:
	
	mv2 KEYS SIGN KEYB
	- KEYS is the filename of the shift key
	- SIGN is which way (+1 increases all values by 1, -1 decreases)	[default of 1]
	- KEYB is the filename of the bytesize key							[if not specified, letter Vigenere cipher is performed]
	
	The input file is "in.txt", and the output file is "out.txt", but the files can be binaries; they
	do not have to be ASCII.

***
WHAT THE CODE ACTUALLY DOES:
***

1) Binary Vigenere shift

	The code takes the input and splits it into chunks of keyB[n] bytes, looping around.
	
	Currently, keyB can only take values 1,2,3 or 4. (Maximum bytes in a block is 4)
	
	i.e. for keyB = [1,3,2]
		input		[01110011 00010001 11001101 10101010 11100110 01001100 11111010]
					|-block0-|-        block 1         -|-    block 2    -|-block3-|
	
	In other words, bytesize[n] = (int)keyB[n] - 48.
	
	The code takes 4 bytes from keyS, and uses that to construct a shift:
	
	i.e.
		shift		[00000000 00000000 00000000 00110101 01201100 11010011 11111110 01101111]
					|-             shift 0             -|-             shift 1             -|
	
	The code then takes each block and increments it by a value of SIGN*keyS[n]. The block is wrapped using
	modulo so that the character set (defined by the length of a block) is looped.
	
	i.e. as keyS[0] = (00000000 00000000 00000000 00110101) which is (NUL NUL NUL 5) in ASCII:
	
	                 vvvvvvvv
		output		[10101000 00010001 11001101 10101010 11100110 01001100 11111010]
					|-block0-|-        block 1         -|-    block 2    -|-block3-|
	
	We then move to the next block, which has the bytesize keyB[1] and corresponding shift keyS[1]. The bytesizes
	and shifts are also looped using modulo.
	
	The output is then written to file. 

2) Letter Vigenere shift

	For the special case of KEYB undefined, the code takes the ASCII input, capitalises it, and replaces 
	all characters which are not capital letters with spaces.
	
	i.e. 
		input		[The quick brown dog jumped over the lazy dogs. What an odd sentence!]
		caps		[THE QUICK BROWN DOG JUMPED OVER THE LAZY DOGS  WHAT AN ODD SENTENCE ]
	
	The block size is restricted to 1 non-blank letter.
	
	The code produces the shifts from ASCII characters. Capital letter A corresponds to 1, B to 2, in accordance
	with a traditional Vigenere cipher. The character @ is reserved for no shift.
	
	In other words, shift[n] = (int)keyS[n] - 64.
	
	i.e.
		keyS		[DO@ANDROIDS@DREAM@OF@ELECTRIC@SHEEP]
		-> shift	(4,15,0,1,14,4,18,15,9,4,19,...)
	
	Then, it performs the Vigenere shift on each letter, moving the nth letter (nth block) to the letter in the alphabet
	a value of SIGN*shift[n] away. The block is wrapped using a 26 character set (the alphabet).
	
	i.e.
		output	[XWE EYART URSOS QOV JZYUHX XYEK YMU AAAM VDPW  AZFU AC OIP VYFCHNVM ]
