/*
 pour tester :
 python
 import vlmc
*/

/* le nom du module python, qui doit correspondre
 * - au nom du fichier interface (ce fichier)
 * - au nom de la lib dynamique (_vlmc.so)
 */
%module vlmc
%feature("autodoc", "1");

/* Le code C concern� */
%{
    #include "defs.h"
    #include "types.h"
    #include "boat.h"
    #include "ortho.h"
    #include "loxo.h"
    #include "polar.h"
    #include "vmg.h"
    #include "gshhs.h"
    #include "winds.h"
    #include "grib.h"
    #include "lines.h"
    #include "util.h"
    #include "context.h"
    #include "vlm.h"
    #include "move.h"
    #include "waypoint.h"
    vlmc_context global_vlmc_context;
%}

/* les inclusions pour g�n�rer le wrapper python */
typedef long time_t;

%include "cpointer.i"
%import "defs.h"
%include "types.h"
%include "gshhs.h"
%include "boat.h"
%include "ortho.h"
%include "loxo.h"
%include "polar.h"
%include "vmg.h"
%include "winds.h"
%include "grib.h"
%include "lines.h"
%include "util.h"
%include "context.h"
%include "vlm.h"
%include "move.h"
%include "waypoint.h"

%pointer_class(double, doublep)

extern vlmc_context global_vlmc_context;

