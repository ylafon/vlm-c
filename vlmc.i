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
    coast_zone shoreline[3601][1800];
    winds_prev windtable;
%}

/* les inclusions pour g�n�rer le wrapper python */
typedef long time_t;

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
extern coast_zone shoreline[3601][1800];
extern winds_prev windtable;
