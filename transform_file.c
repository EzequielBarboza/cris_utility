/*
 * transform_file.c
 *
 *  Created on: Feb 6, 2011
 *      Author: ezequiel
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#include"transform_file.h"
#include"file.h"

File * enterGaussianHeader(){
	printf("Please provide de header for the .gjf file\n");
	File * output = file_open(TEMP_FOLDER, NULL, EXT_GJF, WRITE, 0);
	if(output == NULL)
	{
		printf(">>Error processing files. Verify your permissions in the folder/format of input<<\n");
		return output;
	}

	char * line = NULL;
	unsigned int lineSize = 0;
//////////////
GETMEM:
	printf("%s", "%mem=");
	getline(&line, &lineSize, stdin);
	int memValue = atoi(line);
	if(memValue==0){
		printf("Invalid value, please enter a valid number\n");
		goto GETMEM;
	}
	fprintf(output->file, "%s%d%s", "%mem=", memValue, "GB\n");
	printf("\n");
////////////////
GETNPROCSHARE:
	printf("%s", "%nprocshare=");
	getline(&line, &lineSize, stdin);
	int nprocshareValue = atoi(line);
	if(nprocshareValue==0){
		printf("\nInvalid value, please enter a valid number\n");
		goto GETNPROCSHARE;
	}
	fprintf(output->file, "%s%d%s", "%nprocshare=", nprocshareValue, "\n");
	printf("\n");
//////////////
	printf("%s", "# ");
	getline(&line, &lineSize, stdin);
	line_clean(line);
	fprintf(output->file, "%s%s%s", "# ", line, "         \n");

	fprintf(output->file, "%s", "                     \n");

	printf("Enter title:\t");
	getline(&line, &lineSize, stdin);
	line_clean(line);
	fprintf(output->file, "%s%s", line, "           \n");

	fprintf(output->file, "%s", "                        \n");

	printf("Enter charge and multiplicity:\t");
	getline(&line, &lineSize, stdin);
	line_clean(line);
	fprintf(output->file, "%s%s", line, "      \n");
	fprintf(output->file, "%s", "\n");
	printf("\n");

	file_close(output);

	return output;
}

File* includeGaussianColumn(File* in){
	File * input = file_open(in->path, in->name, NULL, READ, in->numberOfColumns);
		File * output = file_open(TEMP_FOLDER, NULL, EXT_XYZ, WRITE, 0);
		if(output == NULL || input == NULL)
		{
			printf(">>Error processing files. Verify your permissions in the folder/format of input<<\n");
			return NULL;
		}

		char * line = NULL;
		unsigned int lineSize = 0;

		while(!feof(input->file)){
			getline(&line, &lineSize, input->file);
			line_clean(line);
			if(strlen(line)>0){
				fprintf(output->file, "%s\t%d\t%s", line, 1, "\n");
			}
		}

		fputs("\n", output->file);
		output->numberOfColumns = input->numberOfColumns+1;
		file_close(output);

	file_delete(input);
	return file_open(output->path, output->name, NULL, READ, output->numberOfColumns);//RETORNO O ARQUIVO AGORA COM DIREITO DE LEITURA
}

void toGaussian(File * h, File * in, File * script){
	File * header = file_open(h->path, h->name, NULL, READ, h->numberOfColumns);
		File * input = file_open(in->path, in->name, NULL, READ, in->numberOfColumns);
			File * output = file_open(OUTPUT_FOLDER, NULL, EXT_GJF, WRITE, 0);
			if(output == NULL || input == NULL)
			{
				printf(">>Error processing files. Verify your permissions in the folder/format of input<<\n");
				return;
			}

			file_copy(header, output);

			file_copy(input, output);

			fputs("\n", output->file);

			if(script){
				//appends a line in the script for executing the gaussian file in nohup mode
				fprintf(script->file, "%s %s %s", "nohup g03", output->name, "\n");
			}
			file_delete(output);//deletes only in the memory scope
		file_delete(input);
	file_delete(header);
}

void toDalton(File * in){
File * input = file_open(in->path, in->name, NULL, READ, in->numberOfColumns);
	File * output = file_open(OUTPUT_FOLDER, NULL, EXT_INP, WRITE, 0);

	if(output == NULL || input == NULL)
	{
		printf(">>Error processing files. Verify your permissions in the folder/format of input<<\n");
	}
	char * line = NULL;
	unsigned int lineSize = 0;

	int i;
	char ** listAtoms = (char**)malloc(sizeof(char*)*NUMBER_OF_ATOMS);
	for(i=0;i<NUMBER_OF_ATOMS;i++){
		listAtoms[i]=(char*)malloc(sizeof(char)*3);
	}
	char * atom = NULL;
	int repetido = 0;
	int atomCounter = 0;
	while(!feof(input->file)){
		getline(&line, &lineSize, input->file);
		atom = strtok(line, "\n\t ");
		if(atom!=NULL){
			for(i=0;i<NUMBER_OF_ATOMS && i<=atomCounter;i++){
				if(strcmp(listAtoms[i], atom)==0){
					repetido = 1;
					break;
				}
			}
			if(repetido){
				repetido = 0;
				continue;
			}else{
				strcpy(listAtoms[atomCounter], atom);
				atomCounter++;
			}
		}
	}
	rewind(input->file);
	int current = 0;
	while(current < atomCounter){
		while(!feof(input->file)){
			getline(&line, &lineSize, input->file);
			char * Line = (char*)malloc(sizeof(char)*lineSize);
				strcpy(Line, line);
				atom = strtok(Line, "\t ");
				if(atom!=NULL){
					if(strcmp(atom, listAtoms[current])==0){
						line_clean(line);
						if(strlen(line)>0){
							fprintf(output->file, "%s%s", line, "\n");
						}
					}
				}
				free(Line);
		}
		current++;
		fputs("                  \n", output->file);
		rewind(input->file);
	}
	fputs("\n", output->file);
	file_delete(output);
file_delete(input);
}

//simplesmente copia da pasta temp para output
void toChemCraft(File * in){
File * input = file_open(in->path, in->name, NULL, READ, in->numberOfColumns);
	File * output = file_open(OUTPUT_FOLDER, NULL, EXT_XYZ, WRITE, 0);
	if(output->file == NULL)
	{
		printf(">>Error processing files. Verify your permissions in the folder/format of input<<\n");
	}

	file_copy(input, output);

	fputs("\n", output->file);
	file_delete(output);
file_delete(input);
}


//receives a file on the format .xyz and prompts the user for the output format
void transformFile(File * input){//recebendo sempre um arquivo .xyz ouinputFile
	char * choice = NULL;
	unsigned int choiceSize = 0;
BEGIN:
	printf("Create GAUSSIAN(output.gjf) file? [y/n]");
	getline(&choice, &choiceSize, stdin);
	if(toupper(choice[0])=='Y'){
		printf("Startin conversion...");
		File * header = enterGaussianHeader();
		toGaussian(header, input, NULL);
		return;
	}else if(toupper(choice[0])=='N'){
		printf("Create DALTON(MOLECULE.INP) file? [y/n]");
		getline(&choice, &choiceSize, stdin);
		if(toupper(choice[0])=='Y'){
			toDalton(input);
			return;
		}else if(toupper(choice[0])=='N'){
			printf("Create CHEMCRAFT(output.xyz) file? [y/n]");
			getline(&choice, &choiceSize, stdin);
			if(toupper(choice[0])=='Y'){
				toChemCraft(input);
				return;
			}else if(toupper(choice[0])=='N'){
				printf("Nothing was built. Quiting\n");
				exit(0);
			}
		}
	}
	printf("Invalid choice\n");
	goto BEGIN;
}
