CXX=mpicxx
LD=${CXX}
#CXXFLAGS+=-Wall -Wextra -Werror -pedantic -std=c++11 -I${OPENBLAS_ROOT}/include -O1
CXXFLAGS+=-Wall -Wextra -Werror -pedantic -std=c++11 -O0
#LDFLAGS+=-lm $(CXXFLAGS) -L${OPENBLAS_ROOT}/lib -lopenblas
LDFLAGS+=-lm $(CXXFLAGS) -L${OPENBLAS_ROOT}/lib -lcblas

OBJS=cg.o matrix.o matrix_coo.o mmio.o

all: cgsolver_sparse

cgsolver_sparse: cg_main_sparse.o $(OBJS)
	$(LD) -o $@ cg_main_sparse.o $(OBJS) $(LDFLAGS)

clean:
	rm -f cgsolver_sparse *.o *~
