/*
 * life.c
 *
 *  Created on: Sep 15, 2013
 *      Author: sofia
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

char getvalue(int i, int j,int dimi,int dimj,char **array){
	if(i>=dimi)
		i = 0;
	if(j>=dimj)
		j = 0;
	if(i >= 0 && j >= 0){
		return array[i][j];
	}
	else if(i<0 && j>=0){
		return array[dimi-1][j];
	}
	else if(j<0 && i>=0){
		return array[i][dimj-1];
	}
	else{
		return array[dimi-1][dimj-1];
	}
}

char computePosition(int i, int j,int dimi,int dimj,char **starting){
	char value;
	char upright;
	char up;
	char upleft;
	char right;
	char left;
	char down;
	char downright;
	char downleft;
	int sum;

	upright = getvalue(i-1,j-1,dimi,dimj,starting);
	upleft = getvalue(i-1,j+1,dimi,dimj,starting);
	up = getvalue(i-1,j,dimi,dimj,starting);
	right = getvalue(i,j-1,dimi,dimj,starting);
	left = getvalue(i,j+1,dimi,dimj,starting);
	downright = getvalue(i+1,j-1,dimi,dimj,starting);
	downleft = getvalue(i+1,j+1,dimi,dimj,starting);
	down = getvalue(i+1,j,dimi,dimj,starting);
	sum = upright+upleft+up+left+right+downleft+downright+down-8*48;

	if(getvalue(i,j,dimi,dimj,starting) == '1'){
		if( sum <= 1)
			value = '0';
		else if(sum == 2 ||
				sum == 3)
			value = '1';
		else if(sum <= 8 &&
				sum >= 4)
			value = '0';


	}

	else{
		if(sum == 3){

			value = '1';

		}
		else
			value = '0';

	}
	return value;
}


int main(int argc, char* argv[]){

	int rep ,counter;
	int dimi,dimj,generations;
	int j,i;
	char **starting;
	char **helping;
	char *buffer;
	struct timeval t1,t2;
	FILE *input,*timeplot;

	if(argc == 8){ 			/* read the command line arguments */
		for(i = 1 ; i < argc ; i++)
			if(strcmp(argv[i], "-d") == 0){
				dimi = atoi(argv[i+1]);
				dimj = atoi(argv[i+2]);
				i += 2;
			}
			else if(strcmp(argv[i], "-g") == 0){
				generations = atoi(argv[i+1]);
				i++;
			}
			else if(strcmp(argv[i], "-i") == 0){
				if((input = fopen(argv[i+1],"r")) == NULL){ 		/* the connector reads the input file and then scatters it*/
					fprintf(stderr,"There was a problem opening the file %s.\n",argv[3]);
					perror(NULL);
				}
				i++;
			}

	}

	else{
		printf("Usage: %s -d [dimension i] [dimension j] -g [generations] -i [input file] -o [output file]\n",argv[0]);
		return 0;
	}

	if((buffer = malloc(sizeof(char)*dimj+1)) == NULL){ /* allocate memory for the matrix */
		fprintf(stderr,"There was a problem allocating memory.\n");
		perror(NULL);
	}


	if((starting = malloc(sizeof(char *)*dimi)) == NULL){ /* allocate memory for the matrix */
		fprintf(stderr,"There was a problem allocating memory.\n");
		perror(NULL);
	}



	if((helping = malloc(sizeof(char *)*dimi)) == NULL){ /* allocate memory for the matrix */
		fprintf(stderr,"There was a problem allocating memory.\n");
		perror(NULL);
	}

	for(i = 0 ; i < dimi ; i++){
		if((starting[i] = malloc(sizeof(char)*dimj)) == NULL){
			fprintf(stderr,"There was a problem allocating memory.\n");
			perror(NULL);
		}
		if((helping[i] = malloc(sizeof(char)*dimj)) == NULL){
			fprintf(stderr,"There was a problem allocating memory.\n");
			perror(NULL);
		}
	}


	for(i = 0 ; i < dimi ; i++){
		fgets(buffer, dimj+1, input);
		fgetc(input);
		for(j = 0 ; j < dimj ; j++)
			starting[i][j] = buffer[j];
	}

	fclose(input);
	free(buffer);

	counter=1;
	gettimeofday(&t1,NULL);

	for( rep = 0 ; rep < generations ;rep++){
		if(counter==0){
			break;
		}
		for( i = 0 ;i<dimi; i++){
			for( j =0 ; j <dimj;j++){
				helping[i][j] = computePosition(i,j,dimi,dimj,starting);
			}
		}
		counter = 0;

		for( i = 0;i < dimi;i++){
			for( j = 0;j < dimj;j++){
				if(helping[i][j]=='1'){
					counter++;
				}
			}
		}
		for( i = 0;i < dimi;i++){
			for( j = 0;j < dimj;j++){
				starting[i][j]=helping[i][j];
			}
		}

	}
	gettimeofday(&t2,NULL);
	
	if((timeplot = fopen("./statistics/SERIAL_time.plot","a+")) == NULL){
		fprintf(stderr,"Problem in SERIAL_time.plot.\n");
	}
	else{
		fprintf(timeplot,"%d, %f\n",dimi,(t2.tv_sec-t1.tv_sec)*1000.0f + (t2.tv_usec - t1.tv_usec)/1000.f);
		fclose(timeplot);
	}

	for( i = 0 ; i < dimi;i++){
		for( j = 0 ; j < dimj;j++){
			printf("%c",starting[i][j]);
		}
		printf("\n");
	}


	for(i=0; i<dimi; i++){
		free(starting[i]);
		free(helping[i]);
	}

	free(starting);
	free(helping);

	return 0;
}


