SWIG   = swig
CC     = gcc 
AR     = ar
RM     = rm -f
DEPEND = makedepend
MAKE   = make


ifeq ($(OSTYPE),cygwin)
	OS_DEFINES = -D__CYGWIN -DOLD_C_COMPILER
else 
	OS_DEFINES = -std=gnu99 -fno-stack-protector
endif

CDEBUGFLAGS     = -Wall -pedantic -g
#COPTIMFLAGS     = -O3  -ftree-vectorize
#COPTIMFLAGS     = -O3  -ftree-vectorize -march=nocona
COPTIMFLAGS     = -O3 
#COPTIMFLAGS     = -O0
EXTRA_CFLAGS    = -fPIC -pipe 
EXTRA_DEFINES   = -DUSE_GSHHS_20 -DOPTIMIZE_GSHHS_READ -DSAVE_MEMORY -DVLM_BUILD_DATE=\""`date -u +%Y-%m-%d\ T\ %T\ %Z`"\"
#EXTRA_DEFINES   = -DSAVE_MEMORY 
VLMC_INCLUDES   = -I../
EXTRA_INCLUDES  = $(MYSQL_INCLUDES) $(VLMC_INCLUDES)
VLM_DEFINES     = -DVLM_MINOR_VERSION=11 -DSAFE_LINE_CHECK -DPARANOID_COAST_CHECK

CFLAGS = $(CDEBUGFLAGS) $(COPTIMFLAGS) $(EXTRA_CFLAGS) $(OS_DEFINES) $(EXTRA_DEFINES) $(VLM_DEFINES) $(EXTRA_INCLUDES) #-DDEBUG

LD_FLAGS_OSX    = -bundle -flat_namespace -undefined suppress
LD_FLAGS_LINUX  = -shared

ARFLAGS = -rc

ifeq ($(OSTYPE),darwin)
       LD_FLAGS=$(LD_FLAGS_OSX)
       EXTRA_LIBRARIES = `mysql_config5 --libs_r` 
       MYSQL_INCLUDES  = `mysql_config5 --cflags`
else	
       LD_FLAGS=$(LD_FLAGS_LINUX)
       EXTRA_LIBRARIES = `mysql_config --libs_r` 
       MYSQL_INCLUDES  = `mysql_config --cflags`
endif

CFLAGS = $(CDEBUGFLAGS) $(COPTIMFLAGS) $(EXTRA_CFLAGS) $(OS_DEFINES) $(EXTRA_DEFINES) $(VLM_DEFINES) $(EXTRA_INCLUDES) #-DDEBUG
ifeq ($(OSTYPE),cygwin)

endif

SRCS = engine.c timings.c races.c boats.c utils.c

OBJS = engine.o timings.o races.o boats.o utils.o

PROGRAM = engine

all:: engine

engine: $(OBJS) ../libvlmc.a ../wgrib/libgrib.a
	$(RM) $@
	$(CC) -o $@ $(CFLAGS) $(OBJS) $(EXTRA_LIBRARIES) ../libvlmc.a ../wgrib/libgrib.a

../libvlmc.a:
	cd .. && $(MAKE) libvlmc.a

../wgrib/libgrib.a:
	cd .. && $(MAKE) wgrib/libgrib.a

clean::
	$(RM) engine *.CKP *.ln *.BAK *.bak *.o core errs ,* *~ *.a .emacs_* tags TAGS make.log MakeOut *.so

# after a make depend, run this:
# mv Makefile Makefile.bak ; cat Makefile.bak | sed -e '/^[a-z]*\.o: / s,/[0-9A-Z_a-z/]*.h,,g' -e '^[a-z]*\.o: *$/ d' > Makefile

# DO NOT DELETE

boats.o: ../defs.h
boats.o: ../types.h
boats.o: ../polar.h ../shmem.h 
boats.o: ../useshmem.h ../vlm.h ../boat.h ../winds.h
boats.o: ../lines.h ../waypoint.h ../ortho.h ../vmg.h ../loxo.h enginedefs.h
boats.o: enginetypes.h timings.h races.h utils.h
engine.o: ../defs.h
engine.o: ../types.h ../gshhs.h ../grib.h ../polar.h ../shmem.h
engine.o: ../useshmem.h
engine.o: ../context.h ../winds.h enginetypes.h
engine.o: timings.h races.h boats.h utils.h
races.o: ../defs.h
races.o: ../types.h
races.o: ../polar.h ../shmem.h
races.o: ../useshmem.h ../vlm.h ../boat.h enginetypes.h timings.h races.h
utils.o: ../defs.h
utils.o: ../types.h
utils.o: enginedefs.h enginetypes.h
