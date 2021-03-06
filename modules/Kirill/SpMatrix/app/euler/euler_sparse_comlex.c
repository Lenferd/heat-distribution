//

// Created by kirill on 10.11.16.
//
#include <stdio.h>
#include <math.h>

#include <stdlib.h>
#include <sp_mat.h>

#ifdef _COMPLEX_

int init(double *, double *, double *, double *, double *, double *, int *, TYPE **);
void createSpMat(spMatrix *, TYPE);

void printInFile(TYPE *UFin, FILE* fpRe, FILE* fpIm);
int final(TYPE *);

size_t nX;

size_t num = 0;

int main() {
  double xStart, xEnd;
  double sigma;
  double tStart, tFinal;
  double dt;
  int check;

  TYPE* U;

  //------------------------------------------------------------------------
  //                      Инициализация данных
  //------------------------------------------------------------------------

  init(&xStart, &xEnd, &sigma, &tStart, &tFinal, &dt, &check, &U);

  double step = fabs(xStart - xEnd) / nX;
  size_t sizeTime = (size_t)((tFinal - tStart) / dt);

  printf("TIMESIZE = %lu; NX = %lu\n", sizeTime, nX);

  //------------------------------------------------------------------------
  //              Заполнение значений и номера столбцов матрицы
  //------------------------------------------------------------------------

  spMatrix A;
  complex coeff = CMPLX(1.0, dt);

  createSpMat(&A, coeff);

  // -----------------------------------------------------------------------
  //                              Вычисления
  //------------------------------------------------------------------------

  TYPE* UNext = (TYPE*)malloc(sizeof(TYPE) * nX);
  TYPE* tmp;

  FILE* fpRe = fopen("./../../../../result/complex/Re.txt", "w");
  FILE* fpIm = fopen("./../../../../result/complex/Im.txt", "w");

  printInFile(U, fpRe, fpIm);

  double t0 = omp_get_wtime();
  for (int i = 1; i <= sizeTime; i++) {
    // UNext = A*U
    multMV(&UNext, A, U);

    tmp = U;
    U = UNext;
    UNext = tmp;
    if ( i%10000 == 0 )
      printInFile(U, fpRe, fpIm);

  }
  double t1 = omp_get_wtime();

  fclose(fpRe);
  fclose(fpIm);

  printf("finish!\n\n");

  //------------------------------------------------------------------------
  //                       Вывод результатов и чистка памяти
  //------------------------------------------------------------------------

  double diffTime = t1 - t0;
  printf("Time\t%.15lf\n", diffTime);

  printf("size\t%lu\n", num);

  final(U);

  free(U);
  free(UNext);

  freeSpMat(&A);
  return 0;

}

/*
____________________________________________________________________________

                          РЕАЛИЗАЦИЯ ФУНКЦИЙ
 ____________________________________________________________________________

*/


int init(double *xStart, double *xEnd, double *sigma, double *tStart,
         double *tFinal, double *dt, int *check, TYPE **U) {
  FILE *fp;
  if ((fp = fopen("./../../../../initial/INPUT.txt", "r")) == NULL) {
    printf("Не могу найти файл!\n");
    return -1;
  };

  fscanf(fp, "XSTART=%lf\n", xStart);
  fscanf(fp, "XEND=%lf\n", xEnd);
  fscanf(fp, "SIGMA=%lf\n", sigma);
  fscanf(fp, "NX=%lu\n", &nX);
  fscanf(fp, "TSTART=%lf\n", tStart);
  fscanf(fp, "TFINISH=%lf\n", tFinal);
  fscanf(fp, "dt=%lf\n", dt);
  fscanf(fp, "BC=%d\n", check);

  *U = (TYPE *) malloc(sizeof(TYPE) * nX);

  double re, im = 0.;

  // Заполнение функции в нулевой момент времени
  for(int i = 0; i < nX; i++) {
    fscanf(fp, "%lf", &re);
    (*U)[i] = CMPLX(re, 0);
  }
  fclose(fp);

  return 0;
}

void createSpMat(spMatrix *mat, TYPE coeff) {

  initSpMat(mat, nX, nX);

  int j = 0;
  for (int i = 0; i < nX; i ++) {
    mat->value[i] = coeff;
    mat->col[i] = j++;
  }

  mat->rowIndex[0] = 0;
  for (int i = 1; i < nX + 1; i++) {
    mat->rowIndex[i] = i;
  }

}

void printInFile(TYPE *UFin, FILE* fpRe, FILE* fpIm) {
  num++;
  for (int i = 0; i < nX; i++) {
    fprintf(fpRe, "%lf ", creal(UFin[i]));
    fprintf(fpIm, "%lf ", cimag(UFin[i]));
  }
  fprintf(fpRe, "\n");
  fprintf(fpIm, "\n ");
}


int final(TYPE *UFin) {
  FILE *fp;
  fp = fopen("./../../../../result/complex/EulerSparse.txt", "w");

  for (int i = 0; i < nX; i++)
    fprintf(fp, "%.15le\t%+.15lei\n", creal(UFin[i]), cimag(UFin[i]));

  fclose(fp);
}

#endif
