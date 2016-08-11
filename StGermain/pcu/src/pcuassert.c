#include <stdlib.h>
#include "pcuassert.h"

extern int pcu_jump_ready = 0;
extern jmp_buf pcu_jump_env;
extern jmp_buf pcu_rollback_env;
extern char* pcu_assert_cur = NULL;


