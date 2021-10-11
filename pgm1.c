/***************************************************************************
* File: pgm1.c
* Author: David Song
* Procedures: 
* main - display system information. use fork and pipe for client-server communication.
* magicSquare - a function that checks if them matrix is a magic square.
* remove_spaces - a function to remove whitespaces in a string.
***************************************************************************/

//Header files
#include <stdio.h>											// prototypes for standard input/output functions
#include <string.h>											// required for string functions
#include <stdlib.h>											// utility library
#include <unistd.h>											// general purpose header
#include <sys/wait.h>										// for blocking the calling process
#include <sys/utsname.h>									// needed for printing system info

//Function definitions
char * magicSquare(int* arr);	
void remove_spaces(char* s);

/***************************************************************************
* int main(void)
* Author: David Song
* Date: 11 September 2021
* Description: Display System Information using uname. Use fork and pipe to let client-server communicate.
* Client sends a text formatted values to server and check if it is a magic square.
* Finally, server verifies if the result is correct and sends back the response.
*
* Parameters:
* 	main 	O/P 	int 	Status code (not currently used)
**************************************************************************/
int main() {

   	struct utsname buf;										// Initialized uname buf
   	char hostname[100];										// For storing hostname
   	char domainname[100];									// For storing domainname

   	if (uname(&buf) < 0) {									// Check for uname errors
   	   perror("Failed to uname");
   	}

   	printf("System name = %s\n", buf.sysname);				//Print system info
   	gethostname(hostname, 100);								// get host name 
   	printf("Host name   = %s\n", hostname);					// Print host name
   	printf("Release     = %s\n", buf.release);				// Print release
   	printf("Version     = %s\n", buf.version);				// Print version
   	printf("Machine     = %s\n", buf.machine);				// Print machine
   
   	getdomainname(domainname, sizeof(domainname));			// get domain name
   	printf("Domain name = %s\n", domainname);				// Print domain name

	printf("\nMagic Square Checker \n");					
	int p12[2];												// Read/Write pipe from client to server
	int p21[2]; 											// Read/Write pipe from server to client
	
	if (pipe(p12) == -1 || pipe(p21) == -1) {				//Check for errors in pipes
		printf("An error occurred when opening the pipe\n");
		return 1;
	}

	int id = fork();										// assign a fork to an int for simple use
	if (id == -1) {											// Check fork for error
		printf("An error occured with fork\n");
		return 2;
	}
	else if (id > 0) {										// client - parent process
		close(p12[0]);										//close reading pipe not in used
		char input_digit[100];								// for storing user input
		char reply_str[100];								// for storing string from server
		printf("Input 9 numbers in order from left to right, top down, numbers ONLY\n");	//Prompt User
		printf("(e.g 8 1 6 3 5 7 4 9 2) where 8 1 6 is 1st row, 3 5 7 is 2nd row, so on\n"); 		//example of prompt
		printf("Input here: ");
		scanf("%[^\n]s", input_digit);						//Scan inputs
		if (write(p12[1], input_digit, strlen(input_digit)+1) == -1) {	//Write input digits to server. Included error handling
			printf("An error occured writing to pipe");
			return 3;
		}
		close(p12[1]);										// close writing pipe
		wait(NULL);   										// Wait for child to send a string
		close(p21[1]);										// close pipe
      	if (read(p21[0], reply_str, 100) == -1 ) {			// Read string from server/child. Included error handling
      		printf("An error occured reading from pipe");
			return 4;
      	}
      	printf("Magic Square Response from Server is: %s\n", reply_str);
      	close(p21[0]);										//close reading end
      	
	} else {												// server - child process
		close(p12[1]);										// close reading pipe not in used
		char output_digit[100];								// for storing values from client
		char reply_str[100];								// for storing reply back to client
		if (read(p12[0], output_digit, 100) == -1 ) {		// Read string from client/parent. Included error handling
			printf("An error occured reading from pipe");
			return 5;
		}
		remove_spaces(output_digit);						// Remove whitespaces in the text formatted values
		
		int i = 0;											// Track chars in a string to print each char
		int row, col; 										// Matrix constraints
		int size = 3;										// Matrix size
		int matrix[3][3];									// Matrices to store values
		
		for (row = 0; row < size; row++) { 					// Assign values from a string into matrix
    		for (col = 0; col < size; col++)  
    		{
        		matrix[row][col] = output_digit[i++] - '0';	// convert char to int, then save values into matrix

        	}
    	}	
    	char result[10]; 									// for storing magic square T/F response
    	strcpy(result, magicSquare(matrix[0])); 			// pass an array of matrices in magic square and save result
		close(p12[0]);										// close reading pipe
		close(p21[0]);										// close reading pipe
		if (write(p21[1], result, strlen(result)+1) == -1) {// Write result back to client. Included error handling
			printf("An error occured writing to pipe");
			return 6;
		}
		close(p21[1]);										//close writing pipe
	}
	
	return 0;												//exit 
}

/***************************************************************************
* char * magicSquare(int* arr)
* Author: David Song
* Date: 11 September 2021
* Description: a function that checks if the matrix is a magic square.
*
* Parameters:
* 	arr 			I/P 	int * 	input matrix
* 	magicSquare 	O/P 	char * 	Status code (not currently used)
***************************************************************************/
char * magicSquare(int* arr) {
	int size = 3; 											// assuming 3x3 = 9 as it matches the 9 digits input
    int row, column = 0; 									// starting matrices index
    int sum, sum1, sum2;									// sum initialization
    int flag = 0;											// flag for checking T or F
    int matrix[3][3]; 										// magic square example = {{4,9,2},{3,5,7},{8,1,6}};
   
	memcpy(matrix, arr, sizeof(matrix)); 					//copy content of arr parameter into matrix

    printf("Entered matrix is: ");
    for (row = 0; row < size; row++) {						// Print entered matrix
        printf("\n");
        for (column = 0; column < size; column++)
        {
            printf("%d  ", matrix[row][column]);
        }
        printf("\n");
    }

    sum = 0;
    for (row = 0; row < size; row++) 						// Check diagonal elements
    {
        for (column = 0; column < size; column++)			// sum diagonal elements
        {
            if (row == column)
                sum = sum + matrix[row][column];
        }
    }

    for (row = 0; row < size; row++) 						// Check elements as Rows
    {
        sum1 = 0;
        for (column = 0; column < size; column++) 			// sum of row elements
        {
            sum1 = sum1 + matrix[row][column];
        }
        if (sum == sum1)									// compare if sum of diagonal = row equal, flag 1 for True
            flag = 1;
        else {												// flag 0 for false
            flag = 0;
            break;
        }
    }

    for (row = 0; row < size; row++) 						// Check elements as Columns
    {
        sum2 = 0;
        for (column = 0; column < size; column++) 			// sum of columns elements
        {
            sum2 = sum2 + matrix[column][row];
        }
        if (sum == sum2)									// compare if sum of digonal = column equal, flag 1 for true
            flag = 1;
        else {												// flag 0 for false
            flag = 0;
            break;
        }
    }

    if (flag == 1) {										// Flag T when all sums equal in every row, col, diagonal
        return "T";
    }
    else {													// else Flag F 
        return "F";
    }
}


/***************************************************************************
* void remove_spaces(char* s)
* Author: David Song
* Date: 11 September 2021
* Description: a function to remove any whitespaces in a string
*
* Parameters:
* 	s				I/P 	char * 	input string
* 	remove_spaces	O/P 	void 	Status code (not currently used)
***************************************************************************/
void remove_spaces(char* s) {	
    char* d = s;											// Remove any whitespaces
    do {
        while (*d == ' ') {									
            ++d;
        }
    } while (*s++ = *d++);
}

