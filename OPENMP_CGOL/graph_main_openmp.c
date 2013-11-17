#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <error.h>
#include <math.h>
#include <sys/time.h>
#include "mpi.h"

char **charalloc2d(int n, int m) {
	int i;
	char *data = malloc(n*m*sizeof(char));   // We need to create a dynamic matrix that
	char **array = malloc(n*sizeof(char *)); // lies in continuous memory for MPI_Scatter to use

	for (i=0; i<n; i++)
		array[i] = &(data[i*m]);

	return array;
}

void charfree2d(char **array) {
	free(array[0]);
	free(array);
	return;
}

int calculateProcess(int row, int column,int sizep){
	if(row < 0){
		row = sizep-1;
	}
	else if(row >=sizep)
	{
		row = 0;
	}

	if(column < 0)
		column = sizep-1;
	else if(column >= sizep)
		column = 0;

	return row*sizep+column;
}

char getvalue(int i,int j,int sizei,int sizej,char UpRight,char *Uprow,char UpLeft,char *Right,char *Left,char LoRight,char *Lorow,char LoLeft,char **curr_matrix){
	if(i<0 && j<0){
		return UpRight;
	}
	else if(i<0 && j>=0 && j<sizej){
		return Uprow[j];
	}
	else if(i<0 && j>=sizej){
		return UpLeft;
	}
	else if(i>=0 && i<sizei && j<0){
		return Right[i];
	}
	else if(i>=0 && i<sizei && j>=sizej){
		return Left[i];
	}
	else if(i>=sizei && j<0){
		return LoRight;
	}
	else if(i>=sizei && j>=0 && j<sizej){
		return Lorow[j];
	}
	else if(i>=sizei && j>=sizej){
		return LoLeft;
	}
	else
		return curr_matrix[i][j];
}

char computePosition(int i, int j,int sizei,int sizej,char UpRight,char *Uprow,char UpLeft,char *Right,char *Left,char LoRight,char *Lorow,char LoLeft,char **curr_matrix){
	char value;
	int upright;
	int up;
	int upleft;
	int right;
	int left;
	int down;
	int downright;
	int downleft;
	int sum;
#pragma omp parallel
	{
#pragma omp sections
		{
#pragma omp section
			up = getvalue(i-1,j,sizei,sizej,UpRight,Uprow,UpLeft,Right,Left,LoRight,Lorow,LoLeft,curr_matrix) == '1' ? 1 : 0 ;
#pragma omp section
			upleft = getvalue(i-1,j+1,sizei,sizej,UpRight,Uprow,UpLeft,Right,Left,LoRight,Lorow,LoLeft,curr_matrix) == '1' ? 1 : 0 ;
#pragma omp section
			upright = getvalue(i-1,j-1,sizei,sizej,UpRight,Uprow,UpLeft,Right,Left,LoRight,Lorow,LoLeft,curr_matrix) == '1' ? 1 : 0 ;
#pragma omp section
			right = getvalue(i,j-1,sizei,sizej,UpRight,Uprow,UpLeft,Right,Left,LoRight,Lorow,LoLeft,curr_matrix) == '1' ? 1 : 0 ;
#pragma omp section
			left = getvalue(i,j+1,sizei,sizej,UpRight,Uprow,UpLeft,Right,Left,LoRight,Lorow,LoLeft,curr_matrix) == '1' ? 1 : 0 ;
#pragma omp section
			downright = getvalue(i+1,j-1,sizei,sizej,UpRight,Uprow,UpLeft,Right,Left,LoRight,Lorow,LoLeft,curr_matrix) == '1' ? 1 : 0 ;
#pragma omp section
			downleft = getvalue(i+1,j+1,sizei,sizej,UpRight,Uprow,UpLeft,Right,Left,LoRight,Lorow,LoLeft,curr_matrix) == '1' ? 1 : 0 ;
#pragma omp section
			down = getvalue(i+1,j,sizei,sizej,UpRight,Uprow,UpLeft,Right,Left,LoRight,Lorow,LoLeft,curr_matrix) == '1' ? 1 : 0 ;
		}
	}
	sum = upright+upleft+up+left+right+downleft+downright+down;
	if(getvalue(i,j,sizei,sizej,UpRight,Uprow,UpLeft,Right,Left,LoRight,Lorow,LoLeft,curr_matrix) == '1'){

		if(sum <= 1)
			value = '0';
		else if(sum == 2 || sum == 3)
			value = '1';
		else if(sum <= 8 && sum >= 4)
			value = '0';
	}

	else{

		if(sum == 3)
			value = '1';
		else
			value = '0';
	}

	return value;
}


int main(int argc, char* argv[]){
	int  my_rank;                                   /* rank of process */
	int  numOfProcesses;                            /* number of processes */
	int dimi,dimj;                                  /* dimensions of matrix */
	int i,j,k,rep;
	int generations;                                /* generations to produce */
	int sizep ;             /*size of process martrix sizep x sizep*/
	int row,column;                                 /*row and column of process*/
	int nreq;
	char **matrix;                                  /* the matrix */
	char **curr_matrix;                             /* the matrix of the current generation */
	char **next_matrix;                             /* the matrix of the next generation */
	char *buffer;
	int sizei,sizej;
	int *index;       /*Helping array for MPI_create_graph*/
	int  *edges;                                    /*matrix of edges in graph*/
	char *rightcolumn;                              /*sending and receiving array without changing the curr_matrix*/
	char *leftcolumn;                               /*>>*/
	char *upperrow;                                 /*>>*/
	char *lowerrow;                         /*>>*/
	char upperRight,upperLeft,lowerRight,lowerLeft; /*receiving corners */
	int neigh[8];
	FILE *input,*timeplot;                           /*input file stream */
	int *shared_counter;
	struct timeval t1,t2;
	MPI_Datatype _matrixrow_, _receivedrow_;        /* the row of the matrix */
	MPI_Datatype _matrix_,_receivedmatrix_;
	MPI_Comm new_comm;                              /*new communicator for the graph*/
	MPI_Datatype _passingcells_;                    /*the column of the matrix*/
	MPI_Request r[16];                               /*request array for every piece of the matrix */
	MPI_Status statuses[16];                         /*statuses for each request*/


	MPI_Init(&argc, &argv);

	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);


	sizep = sqrt(numOfProcesses);

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
				if(my_rank == 0){
					if((input = fopen(argv[i+1],"r")) == NULL){ 		/* the connector reads the input file and then scatters it*/
						fprintf(stderr,"There was a problem opening the file %s.\n",argv[3]);
						perror(NULL);
					}
					i++;
				}
			}

	}

	else{
		printf("Usage: %s -d [dimension i] [dimension j] -g [generations] -i [input file]\n",argv[0]);
		return 0;
	}

	sizej = (int)sqrt((dimj*dimj)/numOfProcesses);   /* the column size of the matrix of each thread */
	sizei = (int)sqrt((dimi*dimi)/numOfProcesses);   /* the row size of the matrix of each thread */

	if(my_rank == 0){
		printf("sizei = %d\tsizej = %d\n",sizei,sizej);	
	}
	MPI_Type_contiguous(sizei ,MPI_CHAR ,&_passingcells_);
	MPI_Type_commit(&_passingcells_);

	if((shared_counter = malloc(numOfProcesses*sizeof(int))) == NULL){
		fprintf(stderr,"There was a problem allocating memory.\n");
		perror(NULL);
	}
	if((rightcolumn = malloc(sizei * sizeof(char))) == NULL){
		fprintf(stderr,"There was a problem allocating memory.\n");
		perror(NULL);
	}
	if((leftcolumn = malloc(sizei * sizeof(char))) == NULL){
		fprintf(stderr,"There was a problem allocating memory.\n");
		perror(NULL);
	}
	if((upperrow = malloc(sizej* sizeof(char))) == NULL){
		fprintf(stderr,"There was a problem allocating memory.\n");
		perror(NULL);
	}
	if((lowerrow = malloc(sizej * sizeof(char))) == NULL){
		fprintf(stderr,"There was a problem allocating memory.\n");
		perror(NULL);
	}

	if((edges = malloc(numOfProcesses*8*sizeof(int))) == NULL){
		fprintf(stderr,"There was a problem allocating memory.\n");
		perror(NULL);
	}
	if((index = malloc(numOfProcesses*sizeof(int))) == NULL){
		fprintf(stderr,"There was a problem allocating memory.\n");
		perror(NULL);
	}
	MPI_Type_vector(sizej,1, sizej*numOfProcesses, MPI_CHAR, &_matrixrow_);
	MPI_Type_commit(&_matrixrow_);

	MPI_Type_vector(sizej,1, sizej, MPI_CHAR, &_receivedrow_);
	MPI_Type_commit(&_receivedrow_);

	MPI_Type_create_resized(_receivedrow_,0,sizeof(char),&_receivedmatrix_);
	MPI_Type_commit(&_receivedmatrix_);
	//MPI_Type_contiguous(sizei, _matrixrow_, &_matrix_);
	MPI_Type_create_resized(_matrixrow_, 0, sizeof(char), &_matrix_);
	MPI_Type_commit(&_matrix_);


	curr_matrix = charalloc2d(sizei,sizej);
	next_matrix = charalloc2d(sizei,sizej);


	matrix = charalloc2d(sizei,sizej*numOfProcesses);


	if(my_rank == 0){


		if((buffer = malloc(sizeof(char)*dimj+1)) == NULL){ /* allocate memory for the matrix */
			fprintf(stderr,"There was a problem allocating memory.\n");
			perror(NULL);
		}


		for(k = 0 ; k < sizej*numOfProcesses/dimj ; k++){       /* writes the input matrix in a serial matrix for scatter to use */
			for(i = 0 ; i < sizei ; i++){
				fgets(buffer, dimj+1, input);
				fgetc(input);
				for(j = 0 ; j < dimj ; j++)
					matrix[i][k*dimj+j] = buffer[j];
			}
		}
		free(buffer);
		fclose(input);
	}
	 MPI_Barrier(MPI_COMM_WORLD);
	         gettimeofday(&t1,NULL); 

	for(i = 0 ; i < numOfProcesses ; i++){
		index[i] = 8*(i+1);
	}
#pragma omp parallel shared(edges) private(i,row,column)
	{
#pragma omp for schedule(static)  

		for(i = 0 ; i < numOfProcesses ; i++){

			row = i/sizep;            /*Which is the row of the process in process matrix*/
			column = i%sizep;         /*Which is the column of the process in process matrix*/

			//#pragma omp sections
			//{
			//	#pragma omp section
			edges[8*i+0]= calculateProcess(row-1,column-1,sizep);           /*calculate Upper right neighboor*/
			//	#pragma omp section
			edges[8*i+1]= calculateProcess(row-1,column  ,sizep);           /*calculate Upper       neighboor*/
			//	#pragma omp section
			edges[8*i+2]= calculateProcess(row-1,column+1,sizep);           /*calculate Upper  left neighboor*/
			//	#pragma omp section
			edges[8*i+3]= calculateProcess(row  ,column-1,sizep);           /*calculate right       neighboor*/
			//	#pragma omp section
			edges[8*i+4]= calculateProcess(row  ,column+1,sizep);           /*calculate left        neighboor*/
			//	#pragma omp section
			edges[8*i+5]= calculateProcess(row+1,column-1,sizep);           /*calculate Lower right neighboor*/
			//	#pragma omp section
			edges[8*i+6]= calculateProcess(row+1,column  ,sizep);           /*calculate Lower       neighboor*/
			//	#pragma omp section
			edges[8*i+7]= calculateProcess(row+1,column+1,sizep);           /*calculate Lower  left neighboor*/

			//}
		}
	}

	MPI_Scatter(&(matrix[0][0]), sizei, _matrix_, &(curr_matrix[0][0]), sizei, _receivedmatrix_, 0, MPI_COMM_WORLD);

	/*Calculate neighboor processes*/
	//for(j=8;j<16;j++){
	// edges[j]= my_rank;                                      /*the rest of the edges is the my rank process*/
	//}


	MPI_Graph_create(MPI_COMM_WORLD,numOfProcesses,index,edges,0,&new_comm); //Create the common comunicator for the neighbors of a process

	//Send the needed pieces of the matrix to the neigbor processes

	for(rep=0 ;rep<generations ;rep++){

		MPI_Comm_rank(new_comm, &my_rank);
		nreq = 0;                                                    /*Initialize to zero*/

		MPI_Graph_neighbors(new_comm, my_rank, 8, neigh);

		MPI_Isend(&(curr_matrix[0][0]),1,MPI_CHAR,neigh[0],0,new_comm,&r[nreq++]);				//send upper left square
		MPI_Isend(&(curr_matrix[0][0]),1,_passingcells_,neigh[1],1,new_comm,&r[nreq++]);		//send first row
		MPI_Isend(&(curr_matrix[0][sizej-1]),1,MPI_CHAR,neigh[2],2,new_comm,&r[nreq++]);			//send upper right square

		for(j=0;j<sizei;j++){
			rightcolumn[j] = curr_matrix[j][0];
		}

		MPI_Isend(&rightcolumn[0],1,_passingcells_,neigh[3],3,new_comm,&r[nreq++]);					//send left column
		for(j=0;j<sizei;j++){
			leftcolumn[j] = curr_matrix[j][sizej-1];
		}

		MPI_Isend(&leftcolumn[0],1,_passingcells_,neigh[4],4,new_comm,&r[nreq++]);					//send right column
		//	}
		//	#pragma omp section
		MPI_Isend(&(curr_matrix[sizei-1][0]),1,MPI_CHAR,neigh[5],5,new_comm,&r[nreq++]);			//send bottom right square
		//	#pragma omp section
		MPI_Isend(&(curr_matrix[sizei-1][0]),1,_passingcells_,neigh[6],6,new_comm,&r[nreq++]);	//send last row
		//	#pragma omp section
		MPI_Isend(&(curr_matrix[sizei-1][sizej-1]),1,MPI_CHAR,neigh[7],7,new_comm,&r[nreq++]);		//send bottom right square
		//	}
		//}
		//#pragma omp parallel shared(r,nreq)
		//{
		//	#pragma omp sections
		//		{
		//Set the request array to its beginning


		//MPI_Barrier(MPI_COMM_WORLD);


		//	#pragma omp section
		MPI_Irecv(&upperRight,1,MPI_CHAR,neigh[0],7,new_comm,&r[nreq++]);		            //receive lower left corner
		//	#pragma omp section
		MPI_Irecv(&(upperrow[0]),1,_passingcells_,neigh[1],6,new_comm,&r[nreq++]);	        //receive lower row
		//	#pragma omp section
		MPI_Irecv(&upperLeft,1,MPI_CHAR,neigh[2],5,new_comm,&r[nreq++]);			        //receive lower right corner
		//	#pragma omp section
		MPI_Irecv(&(rightcolumn[0]),1,_passingcells_,neigh[3],4,new_comm,&r[nreq++]);			//receive left column
		//	#pragma omp section
		MPI_Irecv(&(leftcolumn[0]),1,_passingcells_,neigh[4],3,new_comm,&r[nreq++]);			//receive right column
		//	#pragma omp section
		MPI_Irecv(&lowerRight,1,MPI_CHAR,neigh[5],2,new_comm,&r[nreq++]);			        //receive upper left corner
		//	#pragma omp section
		MPI_Irecv(&(lowerrow[0]),1,_passingcells_,neigh[6],1,new_comm,&r[nreq++]);			//receive upper row
		//	#pragma omp section
		MPI_Irecv(&lowerLeft,1,MPI_CHAR,neigh[7],0,new_comm,&r[nreq++]);					//receive upper right corner
		//	}

		//}
		//}
		MPI_Waitall( nreq, r, statuses );					      				//wait for all requests

		shared_counter[my_rank] = 1;	
		#pragma omp parallel shared(next_matrix,curr_matrix,shared_counter) private(i,j)
		{
	#pragma omp for schedule(static)  
		/*Compute the effect of the neighboors in curr matrix and copy it to next_matrix*/
		for( i = 0 ; i<sizei; i++){
			for( j =0 ; j<sizej; j++){
				next_matrix[i][j] = computePosition(i,j,sizei,sizej,upperRight,upperrow,upperLeft,rightcolumn,leftcolumn,lowerRight,lowerrow,lowerLeft,curr_matrix);
				if(next_matrix[i][j] == '1'){
					shared_counter[my_rank] = 0;
				}
			}
		}
	#pragma omp for schedule(static)
		/*Copy next generation matrix to current matrix*/
		for( i = 0 ; i < sizei; i++){
			for( j = 0 ; j < sizej; j++){
				curr_matrix[i][j] = next_matrix[i][j];
			}
		}
	}
	for(i = 0 ; i < numOfProcesses ; i++){
		MPI_Bcast(&shared_counter[i], 1, MPI_INT, i, MPI_COMM_WORLD);
		if(shared_counter[i] == 0){
			break;
		}
	}
	if( i == numOfProcesses){
		break;
	}

	}
	MPI_Barrier(MPI_COMM_WORLD);
	gettimeofday(&t2,NULL);
	if(my_rank == 0){
		if((timeplot = fopen("./statistics/OPENMP_time.plot","a+")) == NULL){
			fprintf(stderr,"Problem in OPENMP_time.plot.\n");
		}
		else{
			fprintf(timeplot,"%d, %f\n",dimi,(t2.tv_sec-t1.tv_sec)*1000.0f + (t2.tv_usec - t1.tv_usec)/1000.f);
			fclose(timeplot);
		}
	}

	MPI_Gather(&curr_matrix[0][0],sizei,_receivedmatrix_,&matrix[0][0],sizei, _matrix_, 0,MPI_COMM_WORLD);
	if(my_rank == 0){
		for(k = 0 ; k < sizej*numOfProcesses/dimj ; k++){       /* writes the input matrix in a serial matrix for scatter to use */
			for(i = 0 ; i < sizei ; i++){
				for(j = 0 ; j < dimj ; j++)
					printf("%c",matrix[i][k*dimj+j]);
				printf("\n");
			}

		}
	}
	free(shared_counter);
	free(edges);
	free(index);
	free(rightcolumn);
	free(leftcolumn);
	free(upperrow);
	free(lowerrow);
	charfree2d(curr_matrix);
	charfree2d(next_matrix);
	charfree2d(matrix);

	MPI_Type_free(&_passingcells_);
	MPI_Type_free(&_matrixrow_);
	MPI_Type_free(&_receivedrow_);
	MPI_Type_free(&_receivedmatrix_);
	MPI_Type_free(&_matrix_);
	MPI_Comm_free(&new_comm);
	MPI_Finalize();
	return 0;
	}


