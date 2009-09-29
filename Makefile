# If we want to use cuda gramma, compiler should be switched to nvcc

CC			=	gcc
#OPTIMIZE	=	-O2 -W -Wall
OPTIMIZE	=	-O2 
GSL_INCL	=	-I/usr/local/include
GSL_LIBS	=	-L/usr/local/lib
CUFFT_INCL	=	-I/usr/local/cuda/include
CUFFT_LIBS	=	-L/usr/local/cuda/lib

OPTIONS		=	$(OPTIMIZE) -DLINKLIST

EXEC		=	cuco

OBJS		=	allvars.o begrun.o init.o read_ic.o gravlist.o \
				main.o driftfac.o longrange.o pm_periodic.o \
				run.o predict.o accel.o io.o timestep.o

INCL		=	allvars.h proto.h Makefile

CFLAGS		=	$(OPTIONS) $(GSL_INCL) $(CUFFT_INCL)

LIBS		=	$(GSL_LIBS) -lgsl -lgslcblas -lm \
				$(CUFFT_LIBS) -lcudart -lcufft

$(EXEC)	:	$(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(EXEC)

$(OBJS)	:	$(INCL)

clean:
	rm -f $(OBJS) $(EXEC)
