/*
 * file.c
 *
 *  Created on: Feb 10, 2011
 *  Altered on: Aug 27, 2011
 *      Author: ezequiel
 */
#include<math.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#include<dirent.h>
#include<sys/stat.h>

#include"file.h"

//input files don't have path or extension, just name
//output files don't have name, just path and extension
//both have mode : read/write
File * file_open(char * path, char * name, char * extension, const char * mode, int numberOfColumns){

	File * output = (File*)malloc(sizeof(File));
	output->numberOfColumns = numberOfColumns;

	if(!mode){
		printf("file_open. Error opening file.>%s%s%s %s\n. Use -h option", path, name, extension,mode);
		return NULL;
	}
	if(path){
		DIR * dir = opendir(path);

		if(dir==NULL){
			mkdir(path, 0777);
		}
	}
	if(path){
		output->path = (char*)malloc(sizeof(char)*(strlen(path)+1));
		sprintf(output->path, "%s", path);
	}else{
		output->path = "";
	}
	//resolving number of file
	int i = 0;
	char * number = "";
	if(path && !name){//use path to resolve number of file
		i = strcmp(path, TEMP_FOLDER)==0?temp_counter++:strcmp(path, OUTPUT_FOLDER)==0?output_counter++:0;
		number = (char*)malloc(sizeof(char)*(int)( log10((double)(i+1))+1 ));//the size of the string is associated with the order of the number
		sprintf(number, "%d", i);
	}

	extension = extension?extension:"";

	//resolving name
	if(name){
		output->name = (char*)malloc(sizeof(char)*(strlen(name)+1));
		sprintf(output->name, "%s", name);
	}else if (path){
		name = strcmp(path, TEMP_FOLDER)==0?TEMP_FILE_NAME:OUTPUT_FILE_NAME;
		output->name = (char*)malloc(sizeof(char)*(strlen(name)+strlen(number)+strlen(extension)+1));
		sprintf(output->name, "%s%s%s", name, number, extension);
	}else{
		output->name = "";
	}

	char * completeFilePathName = (char*)malloc(sizeof(char)*(strlen(output->path)+strlen(output->name)+2));//mais um fim de linha + um separador

	if(path){
		sprintf(completeFilePathName, "%s%s%s", output->path, PATH_SEPARATOR, output->name);
	}else{
		sprintf(completeFilePathName, "%s%s", output->path, output->name);
	}

	output->file = fopen(completeFilePathName, mode);

	free(completeFilePathName);

	if(output->file == NULL){
//		printf("file_open. Error opening file> %s %s .Use -h option\n", completeFilePathName, mode);
		return NULL;
	}
	return output;
}

void file_copy(File * input, File * output){
	char * line = NULL;
	unsigned int lineSize = 0;

	while(!feof(input->file)){
		getline(&line, &lineSize, input->file);
		line_clean(line);
		if(strlen(line)>0){
			fprintf(output->file, "%s%s", line, "\n");
		}
	}
	output->numberOfColumns = input->numberOfColumns;
}

void line_clean(char * line){
	int i;
	for(i=strlen(line);i>=0;i--){
		if(!isalnum(line[i]) && line[i]!=' '){
			line[i]='\0';
		}else{
			break;
		}
	}
}

//Frees the file descriptor, the object still exists in memory
void file_close(File * file){
	if(file!=NULL){
		if(fclose(file->file)){
			printf("Error closing input file.");
		}
	}
}
//Frees the file descriptors AND the memory is release too. The pointer is set to NULL
void file_delete(File * file){
	if(file!=NULL){
		if(fclose(file->file)){
			printf("Error closing input file.");
		}
		free(file->name);
		free(file->path);
		free(file);
	}
	file = NULL;
}
//REMOVES THE FILE FROM THE FILE SYSTEM DELETING EVEN THE FOLDER IF IT IS POSSIBLE
void file_purge(File * file){
//	deletes the file
	if(file==NULL){
		return;
	}
	char * completeFilePathName = (char*)malloc(sizeof(char)*(strlen(file->path)+strlen(file->name)+2));
	sprintf(completeFilePathName, "%s%s%s", file->path, PATH_SEPARATOR, file->name);
	remove(completeFilePathName);
	free(completeFilePathName);

//cleans the folder
	folder_remove_ifempty(file->path);
//	cleans in memory
	free(file->name);
	free(file->path);
	free(file);
}

void folder_remove_ifempty(char * path){
	struct dirent *ep;
	int empty = 1;
	DIR *dp = opendir (path);
	if (dp != NULL){
		while ((ep = readdir (dp))){
			if(strcmp(ep->d_name, ".")!=0 && strcmp(ep->d_name, "..")!=0 ){
				empty = 0;
			}
		}
		if(empty){
			remove(path);
		}
	}
}

void folder_remove(char * path){
	struct dirent *ep;
	char* temp;
	DIR* dp = opendir (path);
	if (dp != NULL){
		while ((ep = readdir (dp))){
			if(strcmp(ep->d_name, ".")!=0 && strcmp(ep->d_name, "..")!=0 ){
				temp = (char*)malloc(sizeof(char)*(strlen(path)+strlen(ep->d_name)+2));
				sprintf(temp, "%s%s%s", path, PATH_SEPARATOR, ep->d_name);
				remove(temp);
			}
		}
		remove(path);
	}
}
