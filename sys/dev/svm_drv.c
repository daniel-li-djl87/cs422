#include <lib/debug.h>
#include <lib/gcc.h>
#include <lib/string.h>
#include <lib/types.h>
#include <lib/x86.h>

#include "svm_drv.h"
#include "vmcb.h"

#define PAGESIZE		4096

#define CPUID_FEATURE_FUNC	0x80000001
# define CPUID_SVM_FEATURE_FUNC	0x8000000a
#define CPUID_FEATURE_SVM	(1<<2)
# define CPUID_SVM_LOCKED	(1<<2)

#define MSR_VM_CR		0xc0010114
# define MSR_VM_CR_SVMDIS	(1<<4)
# define MSR_VM_CR_LOCK		(1<<3)
# define MSR_VM_CR_DISA20	(1<<2)
# define MSR_VM_CR_RINIT	(1<<1)
# define MSR_VM_CR_DPD		(1<<0)

#define MSR_VM_HSAVE_PA		0xc0010117

#define SVM_STGI()					\
	do {						\
		__asm __volatile("stgi");		\
	} while (0)

#define SVM_CLGI()				\
	do {					\
		__asm __volatile("clgi");	\
	} while (0)

static uint8_t hsave_area[PAGESIZE] gcc_aligned(PAGESIZE);

/*
 * Check whether the machine supports SVM. (Sec 15.4, APM Vol2 r3.19)
 *
 * @return TRUE if SVM is supported; otherwise FALSE
 */
static bool
svm_check(void)
{
	/* check CPUID 0x80000001 */
	uint32_t feature, dummy;
	cpuid(CPUID_FEATURE_FUNC, &dummy, &dummy, &feature, &dummy);
	if ((feature & CPUID_X_FEATURE_SVM) == 0) {
		KERN_DEBUG("The processor does not support SVM.\n");
		return FALSE;
	}

	/* check MSR VM_CR */
	if ((rdmsr(MSR_VM_CR) & MSR_VM_CR_SVMDIS) == 0) {
		return TRUE;
	}

	/* check CPUID 0x8000000a */
	cpuid(CPUID_SVM_FEATURE_FUNC, &dummy, &dummy, &dummy, &feature);
	if ((feature & CPUID_SVM_LOCKED) == 0) {
		KERN_DEBUG("SVM maybe disabled by BIOS.\n");
		return FALSE;
	} else {
		KERN_DEBUG("SVM maybe disabled with key.\n");
		return FALSE;
	}

	KERN_DEBUG("SVM is available.\n");

	return TRUE;
}

/*
 * Enable SVM. (Sec 15.4, APM Vol2 r3.19)
 */
static void
svm_enable(void)
{
	/* set MSR_EFER.SVME */
	uint64_t efer;

	efer = rdmsr(MSR_EFER);
	efer |= MSR_EFER_SVME;
	wrmsr(MSR_EFER, efer);

	KERN_DEBUG("SVM is enabled.\n");
}

int
svm_drv_init(void)
{
	/* check whether the processor supports SVM */
	if (svm_check() == FALSE)
		return 1;
	/* enable SVM */
	svm_enable();
	KERN_DEBUG("Host state-save area is at %x.\n", hsave_area);
	memzero(hsave_area, PAGESIZE);
	wrmsr(MSR_VM_HSAVE_PA, (uintptr_t) hsave_area);
	return 0;
}
