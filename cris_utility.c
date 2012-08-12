/*
 * cris_utility.c
 *
 *  Created on: 06/02/2011
 *  Altered on: 11/06/2011
 *  Altered on: 27/08/2011
 *  Author: ezequiel
 *
 *
 *  this program is intended to run on Unixes
 */
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>

#include<regex.h>
#include"transform_file.h"
#include"generate_dimers.h"
#include"file.h"

#define MAX_ALG 12//maior quantidade de dígitos depois da vírgula em um número decimal

const char * PERIODIC_TABLE[NUMBER_OF_ATOMS] = {
	"",//0
	"H",//1
	"He",//2
	"Li",//3
	"Be",//4
	"B",//5
	"C",//6
	"N",//7
	"O",//8
	"F",//9
	"Ne",//10
	"Na",//11
	"Mg",//12
	"Al",//13
	"Si",//14
	"P",//15
	"S",//16
	"Cl",//17
	"Ar",//18
	"K",//19
	"Ca",//20
	"Sc",//21
	"Ti",//22
	"V",//23
	"Cr",//24
	"Mn",//25
	"Fe",//26
	"Co",//27
	"Ni",//28
	"Cu",//29
	"Zn",//30
	"Ga",//31
	"Ge",//32
	"As",//33
	"Se",//34
	"Br",//35
	"Kr",//36
	"Rb",//37
	"Sr",//38
	"Y",//39
	"Zr",//40
	"Nb",//41
	"Mo",//42
	"Tc",//43
	"Ru",//44
	"Rh",//45
	"Pd",//46
	"Ag",//47
	"Cd",//48
	"In",//49
	"Sn",//50
	"Sb",//51
	"Te",//52
	"I"//53
};

void showHelp(){
	printf("This is cris_utility VERSION 5.0 28/08/2011 \n");
	printf("The goal of this program is to be a tool for filtering Gaussian files to extract exactly the values for the molecular geometry, interchange between various formats of chemical programs like ChemCraft, Guaussian\n");
	printf("Use: cris_utility <input_file_name> [-h] [-c] \n");
	printf("input_file_name: a formatted valid text file(use unix path name conventions).\nFiles with the name in format *.xyz are suposed to be the output of ChemCraft.\nFiles with the name in format *.log are suposed to be output of Gaussian(R)\n");
	printf("-h : shows this help\n");
	printf("-c : shows the changelog\n");
	printf("This program is free for you to use and is given as is.\n Authors: Ezequiel Barboza & Cristina Barboza\n");
	exit(1);
}

void showChangeLog(){
	printf("VERSION 5.0 28/08/2011 \n");
	printf("Merges two different molecules into one dimmer \n");
	printf("Bugfix: Error when loading files that don't match the pattern \\charge, spin\\ \n treatin characteres to be ignored between this pattern");
	exit(1);
}

int chooseOperation(){
	printf("Do you wanna generate DIMERS ?[y/n]");
	char * choice = NULL;
	unsigned int choiceSize = 0;
	getline(&choice, &choiceSize, stdin);
	if(toupper(choice[0])=='Y'){
		return 0;
//		generateDimers(inputFile);
	}else if(toupper(choice[0])=='N'){
		return 1;
//		transformFile(inputFile);
	}
	return 2;

}

/*runs over a .log file searching for the sequence of characters //*/
File * parseGaussian(File * from)
{
//	create new temp file in the xyz format
	File * to = file_open(TEMP_FOLDER, NULL, EXT_XYZ, WRITE, 0);
	int numberOfColumns = 1;//acummulating the number of columns of this file
	int countingColumns = 1;//if I am still counting columns for this file
//	parsear caracter a caracter
	while(!feof(from->file)){
		char ptr = fgetc(from->file);
//		pegou a primeira sequencia
		while( !feof(from->file) && ptr!=92 ){
			ptr = fgetc(from->file);
		}

		if(feof(from->file)){
			printf("Erro no arquivo de input\n");
			return NULL;
		}

		//ignore list - put this before each check
		ptr = fgetc(from->file);
		while((ptr == '\n' || ptr == '\t' || ptr == ' ') && !feof(from->file)){
			ptr = fgetc(from->file);
		}
		////////

		if(ptr != 92){//tenta pegar mais um backslash
			continue;
		}

		//ignore list - put this before each check
		ptr = fgetc(from->file);
		while((ptr == '\n' || ptr == '\t' || ptr == ' ') && !feof(from->file)){
			ptr = fgetc(from->file);
		}
		////////

		if(!isdigit(ptr) && ptr!='-'){//tentar pegar um número inteiro
			continue;
		}

		//ignore list - put this before every check
//		ptr = fgetc(from->file);
		while((ptr == '\n' || ptr == '\t' || ptr == ' ') && !feof(from->file)){
			ptr = fgetc(from->file);
		}
		////////

//		passou porque não é um número real, se for um número negativo, tenta completar o número
		if(ptr=='-'){
			ptr = fgetc(from->file);
			if(!isdigit(ptr)){
				continue;
			}
		}

		//ignore list - put this before every check
		ptr = fgetc(from->file);
		while((ptr == '\n' || ptr == '\t' || ptr == ' ') && !feof(from->file)){
			ptr = fgetc(from->file);
		}
		////////

		if(ptr != ','){//tenta pegar uma virgula
			continue;
		}

		//ignore list - put this before every check
		ptr = fgetc(from->file);
		while((ptr == '\n' || ptr == '\t' || ptr == ' ') && !feof(from->file)){
			ptr = fgetc(from->file);
		}
		////////

		if(!isdigit(ptr)){//tenta pegar um numero qualquer
			continue;
		}

		//ignore list - put this before every check
		ptr = fgetc(from->file);
		while((ptr == '\n' || ptr == '\t' || ptr == ' ') && !feof(from->file)){
			ptr = fgetc(from->file);
		}
		////////

		if(ptr != 92){//tenta pegar um backslash
			continue;
		}
		//pegou a sequencia correta
		ptr = fgetc(from->file);
		while(ptr != 92 && !feof(from->file)){//enquanto nao encontra a sequencia final \ continua
			if(ptr==92 || feof(from->file)){
				break;//acabou o parsing
			}
			else if(isalpha(ptr)){//se é uma letra : C, H, ... escreva. este é o simbolo do átomo
				fputc(ptr, to->file);
			}
			else if(ptr==','){//troca as virgulas por marcas de tabulacao
				fputc('\t', to->file);
			}
			else if(isdigit(ptr) || ptr=='-'){
				int counter = 0;//
				while( (!feof(from->file) && ptr!=',' && ptr!=92) || ptr=='\n' || ptr==' '||ptr=='\t'){
					if(ptr!='\n' && ptr!=' ' && ptr!='\t'){// os numeros sao os valores, enquanto nao achar um sinal estranho percorre escrevendo
						fputc(ptr, to->file);
						counter++;//escrevi um numero, incremento o contador
					}
					ptr = fgetc(from->file);
				}
				while(counter<MAX_ALG){
					fputc('0', to->file);//escrever 0 ate completar quantidade mínima de dígitos
					counter++;//escrevi um numero, incremento o contador
				}
				if(countingColumns){
					numberOfColumns++;
				}
				if(ptr == 92){
					fputc('\n', to->file);
					countingColumns = 0;
				}
				else if(ptr == ','){
					fputc('\t', to->file);
				}
			}
			ptr = fgetc(from->file);
		}
		if(ptr==92 || feof(from->file)){
			break;
		}
	}
	fputs("\n", to->file);
	to->numberOfColumns = numberOfColumns;
	file_close(to);//closes the descriptor of the file
	return to;
}

File * parseChemCraft(File * from){
//	create new temp file in the temp folder with the extension xyz
	File * to = file_open(TEMP_FOLDER, NULL, EXT_XYZ, WRITE, 0);
	if(to == NULL || from == NULL)
	{
		printf(">>Error processing files. Verify your permissions in the folder/format of input<<\n");
		return to;
	}
	char * line = NULL;
	unsigned int lineSize = 0;
	while(!feof (from->file)){
		getline(&line, &lineSize, from->file);
		line_clean(line);
		if(isdigit(line[0])){//nem todos os arquivos chemcraft tem a coluna 0 como simbolos atomicos
			char * Line = (char*)malloc(sizeof(char)*lineSize);
			strcpy(Line, line);
			char * atom = strtok(Line, " ");
			sprintf(line, "%s%s%s", PERIODIC_TABLE[atoi(atom)], " ", strtok(NULL, ""));
			free(Line);
		}
		if(strlen(line)>0){
			fprintf(to->file, "%s%s", line, "\n");
		}
	}
	fputs("\n", to->file);
	to->numberOfColumns = 4;
//	fechar o arquivo e devolver em modo de leitura
	file_close(to);
	return to;
}

int main(int argc, char * argv[]){

	File * input1, * input2;

//	limpar execuções prévias
	folder_remove(OUTPUT_FOLDER);
	folder_remove(TEMP_FOLDER);

	if(argc<2){
		printf("Use cris_utility -h\n");
		exit(1);
	}
	if(strcmp(argv[1],"-h")==0){
		showHelp();
	}
	if(strcmp(argv[1],"-c")==0){
		showChangeLog();
	}
	//try to open the first input file
	if((input1 = file_open(NULL, argv[1], NULL, READ, 0))==NULL) {
		printf("First file name is invalid\n");
		exit(-1);
	}

	//try to open the second file to create a dimmer with two different molecules

	input2 = file_open(NULL, argv[2], NULL, READ, 0);

	regex_t gaussianRE;
	regex_t chemcraftRE;

	if(	REG_ESPACE==regcomp(&gaussianRE, "\.log$", REG_ICASE | REG_NOSUB )	){
		printf("Memory Alocation Error: Alocating space for regular expression");
	}
	if(	REG_ESPACE==regcomp(&chemcraftRE, "\.xyz$", REG_ICASE | REG_NOSUB)	){
		printf("Memory Alocation Error: Alocating space for regular expression");
	}

	File * temp1 = NULL;
	if(input1){
		if(!regexec(&gaussianRE, argv[1], 0, NULL, 0)){
			temp1 = parseGaussian(input1);//transformar o arquivo que está no formato .log - Gaussian
		}else if(!regexec(&chemcraftRE, argv[1], 0, NULL, 0)){
			temp1 = parseChemCraft(input1);//transformar o arquivo que está no formato .xyz
		}else{
			printf("First file name is invalid. Use -h option for help\n");
		}
	}
	File * temp2 = NULL;
	if(input2){
		if(!regexec(&gaussianRE, argv[2], 0, NULL, 0)){
			temp2 = parseGaussian(input2);//transformar o arquivo que está no formato .log - Gaussian
		}else if(!regexec(&chemcraftRE, argv[2], 0, NULL, 0)){
			temp2 = parseChemCraft(input2);//transformar o arquivo que está no formato .xyz
		}else{
			printf("Second file name is invalid. Using only the first file\n");
		}
	}

//	file_close(temp);

	if(temp1){//temp1 is mandatory
		switch(chooseOperation()){
		case 0:
			generateDimers(temp1, temp2);
			break;
		case 1:
			transformFile(temp1);
			break;
		default:
			printf("Invalid Option. Quiting\n");
			break;
		}
	}
	//close the input files, do not purge them or they will desapear from the users folder
	file_close(input1);
	file_close(input2);
	//purge the temporary files
	file_purge(temp1);
	file_purge(temp2);
	regfree(&gaussianRE);
	regfree(&chemcraftRE);
	printf("Done\n");
	return 0;
}
