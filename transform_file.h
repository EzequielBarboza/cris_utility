/*
 * transform_file.h
 *
 *  Created on: Feb 6, 2011
 *      Author: ezequiel
 */

#ifndef TRANSFORM_FILE_H_
#define TRANSFORM_FILE_H_

#define NUMBER_OF_ATOMS 54

#include<stdio.h>

#include"file.h"

File*	enterGaussianHeader();

File*	includeGaussianColumn(File* in);

void	transformFile(File* file);//entry point to this functionality

void	toChemCraft(File* input);

void	toDalton(File* input);

void	toGaussian(File* header, File* input, File * script);

#endif /* TRANSFORM_FILE_H_ */
