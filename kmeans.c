#define PY_SSIZE_T_CLEAN
#include<Python.h>
#include <stdbool.h>
/**
 * Python Stuff
 **/
void k_mean(int , int ,double** , double ** , int , int );
static PyObject* fit(PyObject *self, PyObject *args){ 
	//Args from python:
	int k,max_iter,data_len,vector_len;
	PyObject *initial_centroids, *data;
	PyArg_ParseTuple(args, "iiiiOO", &k,&max_iter,&data_len,&vector_len,&initial_centroids,&data);
	printf("k=%d max_iter=%d datalen=%d, vectorlen=%d\n",k,max_iter,data_len,vector_len);
	
	Py_ssize_t i, j;
	
	//Define C initial_centroids vector:
	double** c_initials = (double**)calloc(k,sizeof(double*));
	for (i=0; i<k; ++i){
		c_initials[i]= (double*)calloc(vector_len,sizeof(double));
	}
	//Build C initials_centroids vector
	for (i=0; i<k; ++i){
		PyObject *vector = PyList_GetItem(initial_centroids, i);
		if (!PyList_Check(vector)) printf("$$$ bug1\n");
		for (j=0; j<vector_len; ++j){
			PyObject* coordinate = PyList_GetItem(vector, j);
			PyArg_Parse(coordinate, "d", &c_initials[i][j]);
		}
	}

	//Define C data Matrice:
	double** c_data = (double**)calloc(data_len,sizeof(double*));
	for (i=0; i<data_len; ++i){
		c_data[i]= (double*)calloc(vector_len,sizeof(double));
	}
	//Build C data matrice:
	for (i=0; i<data_len; ++i){
		PyObject *vector = PyList_GetItem(data, i);
		if (!PyList_Check(vector)) printf("$$$ bug1\n");
		for (j=0; j<vector_len; ++j){
			PyObject* coordinate = PyList_GetItem(vector, j);
			PyArg_Parse(coordinate, "d", &c_data[i][j]);
		}
	}
	
	//Run kmeans (It modifies c_initials to the final result)
	/*printf("Before:\n");
	for(int i1=0;i1<k;i1++)
		{
			for(int j1=0;j1<vector_len;j1++) printf("%lf ", c_initials[i1][j1]);
			printf("\n");
		}*/
	k_mean(k, max_iter, c_initials, c_data, data_len, vector_len);
	printf("After:\n");
	for(int i1=0;i1<k;i1++)
		{
			for(int j1=0;j1<vector_len;j1++) printf("%lf ", c_initials[i1][j1]);
			printf("\n");
		}

	//Modify "initials" (which is pyobject) to hold the result:
	PyObject* result = PyList_New(PyList_Size(initial_centroids));
	if (!PyList_Check(result)) printf("bug2!!!\n");
	for (i=0; i<k; ++i){
		double* line = c_initials[i];
		PyObject *vector = PyList_New(vector_len);
		PyList_SetItem(result, i, vector);
		for (j=0; j<vector_len; ++j){
			PyObject* value;
			value = Py_BuildValue("d", line[j]);
			PyList_SetItem(vector, j, value);
		}
	}

	//free c_data & c_initials
	for (i=0; i<k; ++i) free(c_initials[i]);
	for (i=0; i<data_len; ++i) free(c_data[i]);
	free(c_initials);free(c_data);
    return result;
}

static PyMethodDef kmeansspMethods[] = {
	{"fit",
	(PyCFunction) fit,
	METH_VARARGS,
	PyDoc_STR("The link between my python code and c code EA15")},
	{NULL,NULL,0,NULL}
};

static struct PyModuleDef moduledef = {
	PyModuleDef_HEAD_INIT,
	"mykmeanssp",
	NULL,
	-1,
	kmeansspMethods
};

PyMODINIT_FUNC
PyInit_mykmeanssp(void) {
	PyObject *m;
	m = PyModule_Create(&moduledef);
	if (!m) return NULL;
	return m;
}

/**
 * Code itself
 **/
void printArr (double * arr, int s) {
	int i;
	for (i=0; i<s; ++i) {
		printf("%f",arr[i]);
		i+1<s ? printf(","):printf("\n");
	}
}

int strtoint (char * str){
	int res,i;	
	char c;

	res = i = 0;
	while((c=str[i])!='\0'){
		assert('0'<=c && c<='9');
		res = 10*res + c-'0';
		++i;
	}
	return res;
}
double strtodouble (char * str){
	double integer,fraction,exp;
	int i, sign, d;
	char c;

	integer = fraction = 0;	
	i = 0; sign = 1;	
	c = str[i];

	if (c == '-') {
		sign = -sign;
		++i;
	}
	while((c=str[i])!='\0' && c!='.'){
		integer = 10*integer + c-'0';
		++i;
	}
	++i;
	d=0;
	while((c=str[i])!='\0'){
		fraction = fraction*10+ (c-'0');
		d++;
		++i;
	}
	exp=1;
	while(d--) {
		exp *= 0.1;
	}
	fraction *= exp;
	return sign*(integer+fraction);
}
int calcSize (char * str){
	int cnt, i;	
	char c;

	cnt = i = 0;
	while((c=str[i])!='\0'){
		if (c==',') ++cnt;
		++i;
	}
	return cnt+1;
}
int lenstr(char * str) {
	char c;
	int i;
	i = 0;
	while ((c=str[i])!='\0') {
		++i;
	}
	return i;
}
double * lineToVector (char * str, int *d){

	int size , k, j, i;
	double *res;
	char *tmp;

	size = calcSize(str);
	if(*d == -1) *d = size;
	
	res = (double *)calloc(size,sizeof(double));
	assert(res!=NULL);
	j = 0;
	tmp = (char *)calloc(lenstr(str),sizeof(char));
	assert(tmp!=NULL);

	k = 0;
	for (i=0;i<size;++i){
		k = 0;
		while(str[j]!=',' && str[j]!='\0'){
			tmp[k] = str[j];
			++j;++k;
		}++j;
		tmp[k] = '\0';
		res[i] = strtodouble(tmp);
	}

	free(tmp);
	return res;
}
char* enlarge (char * a, int asize){
	int i;
	char *b; 
	b = (char*)(calloc(2*asize,sizeof(char)));
	assert(b!=NULL);
	for (i=0;i<asize;++i){
		b[i] = a[i];
	}
	free(a);
	return b;
}
char *dynamicLine (FILE *f, int* flag){
	int cnt, i, bound;
	char c, *line;

	cnt = i = 0; bound = 256;
	line = (char*)calloc(bound,sizeof(char));
	assert(line!=NULL);

	while((c=getc(f))!='\n' && c!=EOF){
		if (cnt+2==bound) {
			line = enlarge(line,bound);
			bound *=2;
		}
		line[i] = c;
		++cnt;
		++i;
	}
	line[i] = '\0'; 
	if (c==EOF) *flag = 1;

	return line;
}

double **enlarger (double ** a, int asize){
	double **b;
	int i;
	b = (double**)(calloc(2*asize,sizeof(double*)));
	for (i=0;i<asize;++i){
		b[i] = a[i];
	}
	free(a);
	return b;
}



double **readLines(FILE *f,int *len, int *d){
	int bound, i, flag;
	char *line;
	double **mat;

	i = flag = 0; bound = 512;
	mat = (double**)calloc(bound,sizeof(double*));
	assert(mat!=NULL);
	
	do {
		line = dynamicLine(f,&flag);
		if (i+2==bound){
			mat = enlarger(mat, bound);
			bound *= 2;
		}
		mat[i] = lineToVector(line, d);
		free(line);
		++i;
	}while (flag!=1);
	if (i+2==bound){
		mat = enlarger(mat, bound);
		bound *= 2;
	}
	*len=i-1;
	return mat;
}



double ** slice (int k, double ** x,int dim) {
	int i, j;
	double ** res;
	res =  (double**)calloc(k,sizeof(double *));
	assert(res!=NULL);
	
	for (i=0;i<k;++i) {
		res[i] = (double*)calloc(dim,sizeof(double));
		assert(res[i]!=NULL);
	}
	
	for(i=0; i<k; ++i) {
		for (j=0;j <dim; ++j) {
			res[i][j] = x[i][j];
		}
	} 
	return res;
}
double *** createClusters(int k, int init) {
	int i;
	double *** res;
	res = (double***)calloc(k,sizeof(double **));
	assert(res!=NULL);
	for (i=0;i<k; ++i) {
		res[i] = calloc(init,sizeof(double*));
		assert (res[i]!=NULL);
	}
	return res;
}

double distance(double * u, double * v, int dim) {
	double res; int i;
	res = 0;
	for (i=0;i<dim ; ++i) res+=(u[i]-v[i])*(u[i]-v[i]);
	return res;
}


void add (double *** si, double *u,int * bound, int *cnt){
	int lim, i;
	double **b;
	if (*cnt == *bound) {
		lim = 2*(*bound);
		b = (double**)(calloc(lim,sizeof(double*)));
		assert(b!=NULL);
		for (i=0;i<*bound;++i){
			b[i] = (*si)[i];
		}
		free(*si);
		*si = b;
		*bound = lim;
	}
	(*si)[*cnt] = u;
	*cnt = *cnt+1;
}



bool v_eq (double * u, double * v, int dim) {
	while(dim--) if (u[dim]!=v[dim]) return false;
	return true;
}

double * calc_cent (double **si, int dim, int cnt, double * v){
	double * mean; int i, j;
	if(cnt>0){
		mean = (double*)calloc(dim,sizeof(double));
		assert(mean!=NULL);
		for (i=0; i<cnt; ++i){
			for (j=0;j<dim;++j) mean[j]+=si[i][j];
		}
		for (i=0;i<dim;++i) mean[i] = mean[i]/cnt;
		return mean;
	}
	else return v;
}
void Result (double **m, int k, int dim){
	int i, j;
	i = 0;
	while (i<k){
			for (j=0;j<dim;++j){
				printf("%.4f",m[i][j]);
				if (j<dim-1){
					printf(",");
				} 
				else {
					printf("\n");
				}			
		}
		++i;
	}
}

void k_mean(int k, int max_iter,double** initials, double ** data, int mat_len, int dim){
	double** x = data;
	double **m = initials;
	double ***s, min_dis, temp, *temp_cent;
	int init, *bounds, *counts, argmin, i, j;
	bool changed =true;

	init = 128;
	changed = true;

	bounds = (int *)calloc(k,sizeof(int));
	assert(bounds!=NULL);
	counts = (int *)calloc(k,sizeof(int));
	assert(counts!=NULL);

	while (changed && max_iter ){
		changed = false;

		s = (double***)calloc(k,sizeof(double **));
		assert(s!=NULL);
		for (i = 0; i< k; i++) {
			s[i] = (double**)calloc(init,sizeof(double*));
			bounds[i] = init;
			counts[i]=0;
		}

		for (i=0;i<mat_len;++i){
			argmin = 0;
			min_dis = distance(x[i], m[0], dim);
			for (j=1 ; j<k; ++j){
				temp = distance(x[i], m[j] ,dim);
				if (temp < min_dis) {
					min_dis = temp;
					argmin = j;
				}
			}
			add(&s[argmin],x[i], &bounds[argmin], &counts[argmin]);
		}
		for (i=0; i<k; ++i){
			temp_cent = calc_cent(s[i], dim, counts[i],m[i]);
			if (!v_eq(temp_cent,m[i],dim)) {
				changed = true;
				for(j=0;j<dim;++j) m[i][j] = temp_cent[j];
			}
			free(temp_cent);
		}
		max_iter-=1;

		for (i=0;i<k; ++i) free(s[i]);
		free(s);
	}
	
	

	free(counts);free(bounds);
	//for(i=0;i<k;++i) free(m[i]);
	//free(m);
	//for (i=0;i<mat_len; ++i) free(x[i]);
	
	//Result(m, k, dim);
	//return m;
}

int main(int argc, char* argv []){
	int i, k, max_iter, mat_len, dim;
	double **mat;
	FILE *f;
	char address[512];

	assert(argc>=2 || argc<=3);
	k = strtoint(argv[1]);
	dim = -1;
	scanf("%s", address);
	f = fopen(address,"r");
	if (argc == 2) {
		max_iter = 200;	
	}
	else {
		max_iter = strtoint(argv[2]);
	}

	printf("k=%d, max=%d, filename=%s \n",k,max_iter,address);

	mat = readLines(f, &mat_len, &dim);
	assert (max_iter>=0 && mat_len>=k);

	//k_mean(k, max_iter, mat, mat_len, dim);

	for (i=0;i<mat_len;i++) free(mat[i]);
	
	free(mat);
	fclose(f);
	return 0;
}
