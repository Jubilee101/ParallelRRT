/*this file is from repo: https://github.com/Ignacio-Perez/openmprrt/blob/master/src/pgm.c for pgm read and write */
/*  OPENMP-RRT
    Copyright (C) 2019  Ignacio Perez-Hurtado
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pgm.h"

char last_error[256];

int next_line(FILE* fp, char* buffer, PGM* pgm) {
	int i,error;
	buffer[0]=0;
	int counter=0;
	do{
		i=0;
		do{
			if ( (error = fscanf(fp,"%c",&buffer[i++])) != EOF) {
				if (buffer[i-1]=='\n') {
					buffer[i-1]=0;
				}
				counter++;
			} 
		} while(error!=EOF && buffer[i-1]!=0);
	} while(error!=EOF && (buffer[0]=='#' || buffer[0]==0));
	
	if (error==EOF) {
		counter=0;
		fclose(fp);
		sprintf(last_error,"Error reading file.");
		free(pgm);
	}
	return counter;
}

PGM* load_pgm(const char* file)
{
	PGM* pgm=NULL;
	char buffer[256];
	int bytes;
	int offset=0;
	unsigned raster_size;
	FILE* fp = fopen(file,"r");
	if (fp==NULL) {
		sprintf(last_error,"Error: Cannot open %s.",file);
		return NULL;
	}
	pgm = (PGM*)malloc(sizeof(PGM));
	strcpy(pgm->file,file);
	
	if ((bytes= next_line(fp,buffer,pgm))==0) {
		return NULL;
	} else {
		offset+=bytes;
	}
	if (strcmp(buffer,"P5")!=0) {
		fclose(fp);
		sprintf(last_error,"Error: Invalid PGM file.");
		free(pgm);
		return NULL;
	}
	if ((bytes= next_line(fp,buffer,pgm))==0) {
		return NULL;
	} else {
		offset+=bytes;
	}	
	sscanf(buffer,"%d %d",&(pgm->width),&(pgm->height));
	
	if (pgm->width<=0 || pgm->width>=65536  || pgm->height<=0 || pgm->width>=65536) {
		fclose(fp);
		sprintf(last_error,"Error: Invalid raster size.");
		free(pgm);
		return NULL;
	}
	
	if ((bytes= next_line(fp,buffer,pgm))==0) {
		return NULL;
	} else {
		offset+=bytes;
	}	
	
	sscanf(buffer,"%d",&(pgm->maxval));
	
	if (pgm->maxval<=0 || pgm->maxval>=256) {
		fclose(fp);
		sprintf(last_error,"Error: Invalid maximum gray value.");
		free(pgm);
		return NULL;
	}
	
	fclose(fp);
	
	raster_size = pgm->width * pgm->height;
	
	pgm->raster = (unsigned char*)malloc(raster_size);
	
	fp=fopen(file,"rb");
	fseek(fp,offset,SEEK_SET);
	
	if (fread(pgm->raster,1,raster_size,fp) != raster_size) {
		fclose(fp);
		sprintf(last_error,"Error reading raster data.");
		free(pgm->raster);
		free(pgm);
		return NULL;
	}
	
	fclose(fp);
	return pgm;
}


int save_pgm(PGM* pgm)
{
	FILE* fp = fopen(pgm->file,"w");
	if (fp==NULL) {
		sprintf(last_error,"Cannot save file.");
		return 0;
	}
	if (fprintf(fp,"P5\n# PGM file\n%d %d\n255\n",pgm->width,pgm->height)<0) {
		sprintf(last_error,"Cannot save file.");
		fclose(fp);
		return 0;
	}
	unsigned raster_size = pgm->width * pgm->height;
	if (fwrite(pgm->raster,1,raster_size,fp) != raster_size) {
		sprintf(last_error,"Cannot save file.");
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}


void destroy_pgm(PGM* pgm)
{
	if (pgm!=NULL) {
		free(pgm->raster);
		free(pgm);
	}
}

void inflate_obstacles(PGM* pgm, double radius)
{
	int o = ceil(radius);
 	unsigned char* raster = (unsigned char*)malloc(pgm->width * pgm->height);
 	memset(raster,255,pgm->width * pgm->height);
	for (int i=0;i<pgm->height;i++) {
		for (int j=0;j<pgm->width;j++) {
			if (!IS_OBSTACLE(pgm,i,j)) {
				continue;
			}
			for (int a = i-o; a<= i+o; a++) {
				for (int b= j-o; b<=j+o; b++) {
					if (a>=0 && a<pgm->height && b>=0 && b< pgm->width) {
						double d = sqrt( (a-i)*(a-i) + (b-j)*(b-j));
						if (d<=radius) {
							raster[a*pgm->width+b] = 0;
						}
					}
				}
			} 
		}
	}
	free(pgm->raster);
	pgm->raster = raster;
}

void inflate_obstacles_parallel(PGM* pgm, double radius)
{
	int o = ceil(radius);
 	unsigned char* raster = (unsigned char*)malloc(pgm->width * pgm->height);
 	memset(raster,255,pgm->width * pgm->height);
	#pragma omp parallel for schedule(dynamic, 16)
	for (int index=0;index<pgm->height*pgm->width;index++) {
		int i= index/pgm->width;
		int j= index%pgm->width;
		if (!IS_OBSTACLE(pgm,i,j)) {
			continue;
		}
		for (int a = i-o; a<= i+o; a++) {
			for (int b= j-o; b<=j+o; b++) {
				if (a>=0 && a<pgm->height && b>=0 && b< pgm->width) {
					double d = sqrt( (a-i)*(a-i) + (b-j)*(b-j));
					if (d<=radius) {
						raster[a*pgm->width+b] = 0;
					}
				}
			}
		} 
	}
	free(pgm->raster);
	pgm->raster = raster;
}

void remove_inner_obstacles(PGM* pgm)
{
	unsigned char* raster = (unsigned char*)malloc(pgm->width * pgm->height);
	for (int i=0;i<pgm->height;i++) {
		for (int j=0;j<pgm->width;j++) {
			
			if (IS_INNER_OBSTACLE(pgm,i,j)) {
				raster[i*pgm->width+j] = 255;
			} else {
				raster[i*pgm->width+j] = pgm->raster[i*pgm->width+j];
			}
		}
	}
	free(pgm->raster);
	pgm->raster = raster;
}


void draw_line(PGM* pgm, int x0, int y0, int x1, int y1, unsigned char color)
{
	double x = x0;
	double y = y0;
	
	double gx = x1;
	double gy = y1;

	double d1 = (gx-x)*(gx-x)+(gy-y)*(gy-y);
	double d2 = d1;
		
	double d = sqrt(d1);
	
	double ix = (gx-x)/d;
	double iy = (gy-y)/d;
	
	do {
		d1 = d2;
		pgm->raster[(int)y * pgm->width + (int)x] = color;
		x += ix;
		y += iy;
		d2 = (gx-x)*(gx-x)+(gy-y)*(gy-y);
	}while (d2<d1);
	
}

// int detect_obstacle(PGM* pgm, int x0, int y0, int x1, int y1, unsigned char threshold)
// {
// 	int obstacle = 0;
// 	double x = x0;
// 	double y = y0;
	
// 	double gx = x1;
// 	double gy = y1;

// 	double d1 = (gx-x)*(gx-x)+(gy-y)*(gy-y);
// 	double d2 = d1;
		
// 	double d = sqrt(d1);
	
// 	double ix = (gx-x)/d;
// 	double iy = (gy-y)/d;
	
// 	do {
// 		d1 = d2;
// 		if (pgm->raster[(int)y * pgm->width + (int)x] < threshold) {
// 			obstacle = 1;
// 		}
// 		x += ix;
// 		y += iy;
// 		d2 = (gx-x)*(gx-x)+(gy-y)*(gy-y);
// 	}while (d2<d1);
	
// 	return obstacle;
// }
int detect_obstacle(PGM* pgm, int x0, int y0, int x1, int y1, unsigned char threshold)
{
	int obstacle = 0;
	// double x = x0;
	// double y = y0;
	
	double gx = x1;
	double gy = y1;

	double d1 = (gx-x0)*(gx-x0)+(gy-y0)*(gy-y0);
	// double d2 = d1;
		
	int d = (int)sqrt(d1);
	// printf("d:%d\n",d);
	double ix = (gx-x0)/d;
	double iy = (gy-y0)/d;
	#pragma omp for
	for(int i=0;i<=d;i++){
		if (obstacle == 1) {
			continue;
		}
		double x=x0+i*ix;
		double y=y0+i*iy;
		if (pgm->raster[(int)y * pgm->width + (int)x] < threshold) {
			obstacle = 1;
			// return obstacle;
			obstacle=1;
		}
	}		
	return obstacle;
}


