#include "matrix_coo.hh"
#include <iostream>
extern "C" {
#include "mmio.h"
}

void MatrixCOO::read(const std::string & fn) {
  int nz;

  int ret_code;
  MM_typecode matcode;
  FILE * f;

  if ((f = fopen(fn.c_str(), "r")) == NULL) {
    printf("Could not open matrix");
    exit(1);
  }

  if (mm_read_banner(f, &matcode) != 0) {
    printf("Could not process Matrix Market banner.\n");
    exit(1);
  }

  // Matrix is sparse
  if (not(mm_is_matrix(matcode) and mm_is_coordinate(matcode))) {
    printf("Sorry, this application does not support ");
    printf("Market Market type: [%s]\n", mm_typecode_to_str(matcode));
    exit(1);
  }

  if ((ret_code = mm_read_mtx_crd_size(f, &m_m, &m_n, &nz)) != 0) {
    exit(1);
  }

  irn.resize(nz);
  jcn.resize(nz);
  a.resize(nz);
  // Store matrix properties
  m_nz = nz;
  m_is_sym = mm_is_symmetric(matcode);

  // Allocate Device Memory
  cudaMalloc(&irn_storage,nz*sizeof(int));
  cudaMalloc(&jcn_storage,nz*sizeof(int));
  cudaMalloc(&a_storage,nz*sizeof(double));
  //cudaMalloc(&m_is_sym_storage,sizeof(bool));
  //cudaMalloc(&nz_storage,sizeof(size_t));

  // Store Matrix properties in unified memory
  //cudaMemcpy(m_is_sym_storage,&m_is_sym,sizeof(bool),cudaMemcpyHostToDevice);
  //cudaMemcpy(nz_storage,&m_nz,sizeof(size_t),cudaMemcpyHostToDevice);
  std::cout << "Number of nonzero elements: " << nz << std::endl;

  // Temporary storage
  //int irn[nz];
  //int jcn[nz];
  std::cout << irn[0] << std::endl;
  //double a[nz];
  

  // Store Matrix Data in shared memory
  //  NOTE: when reading in doubles, ANSI C requires the use of the "l"  /
  //   specifier as in "%lg", "%lf", "%le", otherwise errors will occur /
  //  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            /
  for (int i = 0; i < nz; i++) {
    int I, J;
    double a_;

    fscanf(f, "%d %d %lg\n", &I, &J, &a_);
    I--; // adjust from 1-based to 0-based /
    J--;

    irn[i] = I;
    jcn[i] = J;
    a[i] = a_;
  }
  cudaMemcpy(irn_storage,irn.data(),nz*sizeof(int),cudaMemcpyHostToDevice);
  cudaMemcpy(jcn_storage,jcn.data(),nz*sizeof(int),cudaMemcpyHostToDevice);
  cudaMemcpy(a_storage,a.data(),nz*sizeof(double),cudaMemcpyHostToDevice);

  /*
  // reserve memory for matrices //
  irn.resize(nz);
  jcn.resize(nz);
  a.resize(nz);

  //  NOTE: when reading in doubles, ANSI C requires the use of the "l"  /
  //   specifier as in "%lg", "%lf", "%le", otherwise errors will occur /
  //  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            /
  m_is_sym = mm_is_symmetric(matcode);
  for (int i = 0; i < nz; i++) {
    int I, J;
    double a_;

    fscanf(f, "%d %d %lg\n", &I, &J, &a_);
    I--; // adjust from 1-based to 0-based /
    J--;

    irn[i] = I;
    jcn[i] = J;
    a[i] = a_;
  }
  */
  

  if (f != stdin) {
    fclose(f);
  }
}
