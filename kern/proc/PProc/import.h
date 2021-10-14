#ifndef _KERN_PROC_PPROC_H_
#define _KERN_PROC_PPROC_H_

#ifdef _KERN_

unsigned int get_curid(void);
void set_pdir_base(unsigned int index);
unsigned int thread_spawn(void *entry, unsigned int id,
                          unsigned int quota);
//for fork
unsigned int container_get_quota(unsigned int id);
unsigned int container_get_usage(unsigned int id);

#endif  /* _KERN_ */

#endif  /* !_KERN_PROC_PPROC_H_ */
