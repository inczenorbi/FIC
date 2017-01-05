#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100

void leftTrim(char *s, char *str) {
	int i = 0;

	while(*s == ' ' || *s == '\n' || *s == '\t')
		++s;
	
	while(*s != '\0')
		str[i++] = *s++;
	
	str[i] = '\0';
}

int isDataTransferInstr(char *s) {
	const int row = 8;
	const char trans[8][5] = {"mov", "push", "pop", "xchg", "xlat", "lea", "lds", "les"};
	int i, j;
	
	for(i = 0; i < row; i++) {
		j = 0;
		while((s[j] == trans[i][j]))
			++j;
		
		if(trans[i][j] == '\0')
			return 1;
	}
		
	return 0;
}

int isAritmInstr(char *s) {
	const int row = 18;
	const char arit[18][5] = {"add", "adc", "inc", "aaa", "daa", "sub", "sbb", "dec", "neg", "aas", "das", "cbw", "cwd", "mul", "imul", "aam", "div", "aad"};
	int i, j;
	
	for(i = 0; i < row; i++) {
		j = 0;
		while((s[j] == arit[i][j]))
			++j;
		
		if(arit[i][j] == '\0')
			return 1;
	}
		
	return 0;
}

void printStatistic(int transNumber, int aritNumber, int jmpNumber, int otherInstr) {
	printf("Numar total de instructiuni: %d\n", transNumber + aritNumber + jmpNumber + otherInstr);
	printf("Numar de instructiuni de transfer de date: %d\n", transNumber);
	printf("Numar de instructiuni aritmetice: %d\n", aritNumber);
	printf("Numar de instructiuni de salt (conditionat sau direct): %d\n", jmpNumber);
	printf("Numar de alte instructiuni: %d\n", otherInstr);
	system("pause");
}

void modFile(const char *inputFileName) {
	char outputFileName[50];
	char aux[MAX_LEN], auxTrimed[MAX_LEN];
	FILE *f_in, *f_out;
	int listener = 0;
	int transNumber = 0, aritNumber = 0, jmpNumber = 0, otherInstr = 0; 
	
	f_in = fopen(inputFileName, "rt");
	if(!f_in) {
		printf("Can't open the file!\n");
		exit(1);
	}
	
	strcpy(outputFileName, "m_");
	strcat(outputFileName, inputFileName);
	f_out = fopen(outputFileName, "wt");
	if(!f_out) {
		printf("Can't open the file!\n");
		exit(1);
	}
	
	while(fgets(aux, MAX_LEN, f_in)) {
		leftTrim(aux, auxTrimed);
		
		if(listener) {
			if(auxTrimed[0] == 'r' && auxTrimed[1] == 'e' && auxTrimed[2] == 't') { // end of function
				++jmpNumber;
				listener = 0;
				fprintf(f_out, "%s", aux);
			}
			else if(aux[0] == '\t' && aux[1] == '.')
				; // skiping the useless lines
			else {
				fprintf(f_out, "%s", aux); // printing the code
				// Instruction counting:
				if(auxTrimed[0] == 'j')
					++jmpNumber;
				else if(isAritmInstr(auxTrimed))
					++aritNumber;
				else if(isDataTransferInstr(auxTrimed))
					++transNumber;
				else
					++otherInstr;
			}
		}
		else if((aux[0] >= 'a' && aux[0] <= 'z') || (aux[0] >= 'A' && aux[0] <= 'Z')) {
			fprintf(f_out, "%s", aux);
			listener = 1;
		}
	}
	
	printStatistic(transNumber, aritNumber, jmpNumber, otherInstr);
	
	fclose(f_in);
	fclose(f_out);
}

int main(void) {
	char inputFileName[50];
	
	printf("Enter the name of the file: ");
	scanf("%s", inputFileName);
	
	modFile(inputFileName);
	return 0;
}