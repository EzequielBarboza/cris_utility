/*
 * file.h
 *
 *  Created on: Feb 10, 2011
 *      Author: ezequiel
 */

#ifndef FILE_H_
#define FILE_H_

#include<stdio.h>

#define TEMP_FOLDER		"temp_cris"
#define OUTPUT_FOLDER	"output_cris"
#define PATH_SEPARATOR	"/"
#define EXT_XYZ			"_.xyz"
#define EXT_INP			"MOLECULE.INP"
#define EXT_GJF			"_.gjf"
#define OUTPUT_FILE_NAME "output_"
#define TEMP_FILE_NAME	"temp_"

#define WRITE	"w"
#define READ	"r"

#define INPUT	0
#define TEMP	1
#define OUTPUT	2

static int temp_counter = 0;
static int output_counter = 0;

typedef struct File{
	FILE*	file;
	char*	path;
	char*	name;
	int		numberOfColumns;
}File;

//input files dont have path ou extension, just name
//output files dont have name, just path and extension
//both have mode
File*	file_open(char * path, char * name, char * ext, const char * mode, int numberOfColumns);

void	file_close(File * file);

void	file_copy(File * input, File * output);

void	file_remove(File * file);

void	file_purge(File * file);

void	file_delete(File * file);

void	line_clean(char * line);

void	folder_remove(char * path);

void	folder_remove_ifempty(char * path);

#endif /* FILE_H_ */
