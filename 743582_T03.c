#include <stdio.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

/* Inclusão da biblioteca pthread */
#include <pthread.h>

/* VARIÁVEIS GLOBAIS */
double h, *val, max, min;
int n, nval, i, *vet, size;
pthread_mutex_t mutex;

/* funcao que calcula o minimo valor em um vetor */
double min_val(double *vet,int nval) {
	int i;
	double min;

	min = FLT_MAX;

	for(i=0;i<nval;i++) {
		if(vet[i] < min)
			min =  vet[i];
	}

	return min;
}

/* funcao que calcula o maximo valor em um vetor */
double max_val(double * vet, int nval) {
	int i;
	double max;

	max = FLT_MIN;

	for(i=0;i<nval;i++) {
		if(vet[i] > max)
			max =  vet[i];
	}

	return max;
}

/* conta quantos valores no vetor estao entre o minimo e o maximo passados como parametros */
void *count(void *parametro) {

	/* Inicia o mutex */
	// pthread_mutex_init(&mutex, NULL);

	/* Numero de bins por thread */
	long nthreads = (long) parametro;
	int tam = (n / size) + 1;

	int prim, ult;
	prim = nthreads * tam;
	ult = (prim + tam);
	if(ult > n){
		ult = n;
	}

	int i, j, count;
	double min_t, max_t;

	for(j=prim;j<ult;j++) {
		count = 0;
		min_t = min + j*h;
		max_t = min + (j+1)*h;
		for(i=0;i<nval;i++) {
			if( (val[i] <= max_t && val[i] > min_t) || (j == 0 && val[i] <= min_t) ) {
				// pthread_mutex_lock(&mutex);
				count++;
				// pthread_mutex_unlock(&mutex);
			}
		}

		vet[j] = count;
	}
	return vet;
}

int main(int argc, char * argv[]) {
	long unsigned int duracao;
	long thread;
	pthread_t *thread_handles;
	struct timeval start, end;

	/* entrada do número de threads */
	scanf("%d",&size);

	/* aloca de acordo com o número de threads */
	thread_handles = malloc (size * sizeof (pthread_t));

	/* entrada do numero de dados */
	scanf("%d",&nval);

	/* numero de barras do histograma a serem calculadas */
	scanf("%d",&n);

	/* vetor com os dados */
	val = (double *)malloc(nval*sizeof(double));
	vet = (int *)malloc(n*sizeof(int));

	/* entrada dos dados */
	for(i=0;i<nval;i++) {
		scanf("%lf",&val[i]);
	}

	/* calcula o minimo e o maximo valores inteiros */
	min = floor(min_val(val,nval));
	max = ceil(max_val(val,nval));

	/* calcula o tamanho de cada barra */
	h = (max - min)/n;

	gettimeofday(&start, NULL);

	/* executa paralelamente a função count */
	for (thread = 0; thread < size; thread++)
		pthread_create(&thread_handles[thread], NULL, count, (void *) thread);

	for (thread = 0; thread < size; thread++)
		pthread_join(thread_handles[thread], NULL);

	gettimeofday(&end, NULL);

	duracao = ((end.tv_sec * 1000000 + end.tv_usec) - \
	(start.tv_sec * 1000000 + start.tv_usec));

	printf("%.2lf",min);
	for(i=1;i<=n;i++) {
		printf(" %.2lf",min + h*i);
	}
	printf("\n");

	/* imprime o histograma calculado */
	printf("%d",vet[0]);
	for(i=1;i<n;i++) {
		printf(" %d",vet[i]);
	}
	printf("\n");


	/* imprime o tempo de duracao do calculo */
	printf("%lu\n",duracao);

	free(vet);
	free(val);
	free(thread_handles);

	return 0;
}
