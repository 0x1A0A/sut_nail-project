#ifndef __SAVE_CSV_H__
#define __SAVE_CSV_H__

#include <stdio.h>
#include <list>
#include "utils.h"
#include "raylib.h"
#include <ctype.h>

void saveListToCSV(const std::list<imgAVG>& list, const char *filename)
{
	FILE *fp;
	
	if ( list.size() == 0) {
		TraceLog(LOG_WARNING, "list empty create empty file");
	}

	char validname[256];
	int c = 0;
	for ( int i=0; filename[i]; ++i ) {
		if ( isalnum( filename[i] ) ) {
			validname[c++] = filename[i];
		} else {
			validname[c++] = '_';
		}
	}

	validname[c++] = '.';
	validname[c++] = 'c';
	validname[c++] = 's';
	validname[c++] = 'v';
	validname[c++] = '\0';

	TraceLog(LOG_INFO, "saving to file %s", validname);

	fp = fopen( validname, "w" );
	
	if (!fp) {
		fprintf(stderr, "can't open file for writing %s", validname);
		return;
	}

	for ( const imgAVG &i:list ) {
		fprintf(fp,"%f,%f\n", i.time, i.avg);
	}
	fclose(fp);

	TraceLog(LOG_INFO, "succesfully saved data to %s", validname);
}

#endif