
/* Implemented functions */

#include "vmc_sysctrl.h"
#include "vmc_sysmem.h"
#include "vmc_custom.h"

#include "vmc_add.h"
#include "vmc_sub.h"
#include "vmc_mul.h"
#include "vmc_div_mod.h"
#include "vmc_neg_not.h"

#ifdef VM_MATH_SUPPORT
#include "vmc_exp_abs.h"
#endif

#include "vmc_and.h"
#include "vmc_or.h"
#include "vmc_xor.h"
#include "vmc_shi_rot.h"
#include "vmc_sel.h"
#include "vmc_limit.h"
#include "vmc_max.h"
#include "vmc_min.h"
#include "vmc_gtge.h"
#include "vmc_ltle.h"
#include "vmc_eqne.h"
#include "vmc_mux.h"
#include "vmc_conv.h"

#include "vmc_datetime.h"

#include "vmc_pointers.h"

#ifdef VM_STRING_SUPPORT
#include "vmc_string.h"
#endif
