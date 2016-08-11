#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>
#include "types.h"

const char* WallEnumToStr[Wall_Size] = {
       "back",
       "left",
       "bottom",
       "right",
       "top",
       "front" };


