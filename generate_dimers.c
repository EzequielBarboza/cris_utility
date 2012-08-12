/*
 * dimer_scan.c
 *
 *  Created on: 29/01/2011
 *  Author: ezequiel
 *
 *
 *  this program is intended to run on Unixes
 */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#include"transform_file.h"
#include"file.h"

void generateDimers(File * input1, File * input2){

	File * inputFile1 = file_open(input1->path, input1->name, NULL, READ, input1->numberOfColumns);
	File * inputFile2 = input2==NULL ? NULL : file_open(input2->path, input2->name, NULL, READ, input2->numberOfColumns);
	char * line = NULL;
	unsigned int lineSize = 0;
	int number_of_files;
	double delta, increment;
	char * extension = NULL;
	File * intermediare = NULL;

CHOOSE_NOF://number of files to be generated
	printf("NUMBER OF OUTPUT FILES=");
	getline(&line, &lineSize, stdin);
	if((number_of_files=atoi(line))==0){
		printf("Invalid choice\n");
		goto CHOOSE_NOF;
	}

CHOOSE_DELTA://DELTA: distancia entre um monômero e outro medido em angstrons
	printf("DELTA=");
	getline(&line, &lineSize, stdin);
	if((delta=atof(line))==0.0){
		printf("Invalid choice\n");
		goto CHOOSE_DELTA;
	}

CHOOSE_INC://number of files
	printf("INCREMENT=");
	getline(&line, &lineSize, stdin);
	if((increment=atof(line))==0.0){
		printf("Invalid choice\n");
		goto CHOOSE_INC;
	}

CHOOSE_OUTPUT:
	printf("Generate dimers in GAUSSIAN(.gjf) format? [y/n]");
	getline(&line, &lineSize, stdin);
	if(toupper(line[0])=='Y'){
		extension = EXT_GJF;
		intermediare = includeGaussianColumn(inputFile1);
		file_delete(inputFile1);
	}else if(toupper(line[0])=='N'){
		printf("Generate dimers in DALTON(MOLECULE.INP) format? [y/n]");
		getline(&line, &lineSize, stdin);
		if(toupper(line[0])=='Y'){
			extension = EXT_INP;
			intermediare = inputFile1;
		}else if(toupper(line[0])=='N'){
			printf("Generate dimers in CHEMCRAFT(output.xyz) format? [y/n]");
			getline(&line, &lineSize, stdin);
			if(toupper(line[0])=='Y'){
				extension = EXT_XYZ;
				intermediare = inputFile1;
			}else if(toupper(line[0])=='N'){
				printf("Nothin was built. Quiting\n");
				return;
			}
		}
	}
	if (!extension) {
		printf("Invalid choice\n");
		goto CHOOSE_OUTPUT;
	}

//alocating the temp files
	File ** tempFiles = (File**)malloc(sizeof(File*)*number_of_files);
//instantiating the temp files
	int i;
	for(i=0;i<number_of_files;i++){//gerar n arquivos temporários do tipo xyz
		tempFiles[i] = file_open(TEMP_FOLDER, NULL, EXT_XYZ, WRITE, 0);
		file_copy(intermediare, tempFiles[i]);
		rewind(intermediare->file);
	}
// treats the generation of dimmers of different molecules
	if(inputFile2!=NULL){
		file_delete(intermediare);//frees the previous file
		if(extension==EXT_GJF){
			intermediare = includeGaussianColumn(inputFile2);//clones the file addin one more column
			file_delete(inputFile2);//frees the inputFile
		}else{
			intermediare = inputFile2;
		}
	}

//	GENERATIN DIMERS
	char* Line = NULL;
	char* token;
	int column;
	int zColumn = extension==EXT_GJF?intermediare->numberOfColumns-1:intermediare->numberOfColumns;
	double zValue;
	while(!feof (intermediare->file)){
		getline(&line, &lineSize, intermediare->file);
		Line = (char*)malloc(sizeof(char)*lineSize);
		strcpy(Line, line);
		token = strtok(Line, " \t\n\r");
		if(token==NULL){//pula linha em branco || !isalpha(token[0])
			continue;
		}else{
			column = 0;
			zValue = 0.0;
			while(token != NULL){
				column++;
				for(i=0;i<number_of_files;i++){
					if(column==zColumn+1){
						fprintf(tempFiles[i]->file, "%d%s", (atoi(token)+1), "\t");
					}else if(column==zColumn){//se estou na coluna certa (z), incremento para cada arquivo, senão somente escrevo
						if(i==0){
							zValue = atof(token)+delta;
						}else{
							zValue += increment;
						}
						fprintf(tempFiles[i]->file, "%f%s", zValue, "\t");
					}else{
						fprintf(tempFiles[i]->file, "%s%s", token, "\t");
					}
				}
				token = strtok(NULL, " \t\n\r");
			}
			for(i=0;i<number_of_files;i++){
				fprintf(tempFiles[i]->file, "%s", "\n");
			}
		}
	}

	for(i=0;i<number_of_files;i++){//fechar arquivos temporários do tipo xyz
		fputs("\n", tempFiles[i]->file);
		file_close(tempFiles[i]);
	}


//END GENERATING DIMERS

//	TRANSFORMING TO THE FINAL FORMAT
	if(extension==EXT_XYZ){
		for(i=0;i<number_of_files;i++){//simplesmente copiar os temporários para a pasta output
			toChemCraft(tempFiles[i]);
			file_purge(tempFiles[i]);
		}
	}else if(extension==EXT_INP){
		for(i=0;i<number_of_files;i++){//simplesmente copiar os temporários para a pasta output
			toDalton(tempFiles[i]);
			file_purge(tempFiles[i]);
		}
	}else if(extension==EXT_GJF){
		File * header = enterGaussianHeader();

		//opens the script file just to append the line for executing this new gaussian file
		File * script = file_open(OUTPUT_FOLDER, "script", NULL, "a", 0);
		fprintf(script->file, "%s", "# \n");

		for(i=0;i<number_of_files;i++){//simplesmente copiar os temporários para a pasta output
			toGaussian(header, tempFiles[i], script);
			file_purge(tempFiles[i]);
		}
		fprintf(script->file, "%s", "done \n");
		file_delete(script);
		file_purge(header);
	}
	file_delete(intermediare);
}
