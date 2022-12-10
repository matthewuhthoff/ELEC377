#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/ktime.h>


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
#define HAVE_PROC_OPS
#endif

static int lab1_show(struct seq_file *m, void *v) {

  struct task_struct *control_block = current;
  seq_printf(m, "Current Process PCB Information\n");
  seq_printf(m, "Name = %s\n", control_block->comm);
  seq_printf(m, "PID = %d\n", control_block->pid);
  seq_printf(m, "PPID = %d\n", task_ppid_nr(control_block));

  if (control_block->state == 0x0000)
    seq_printf(m, "State = Running\n");
  else if(control_block->state == 0x0001 || control_block->state == 0x0002)
    seq_printf(m, "State = Waiting\n");
  else
    seq_printf(m, "State = Stopped");

  seq_printf(m, "Real UID = %d\n", control_block->cred->uid);
  seq_printf(m, "Effective UID = %d\n", control_block->cred->euid);
  seq_printf(m, "Saved UID = %d\n", control_block->cred->suid);
  seq_printf(m, "Real GID = %d\n", control_block->cred->gid);
  seq_printf(m, "Effective GID = %d\n", control_block->cred->egid);
  seq_printf(m, "Saved GID = %d\n", control_block->cred->sgid);
  return 0;
}

static int lab1_open(struct inode *inode, struct  file *file) {
  return single_open(file, lab1_show, NULL);
}

#ifdef HAVE_PROC_OPS
static const struct proc_ops lab1_fops = {
  .proc_open = lab1_open,
  .proc_read = seq_read,
  .proc_lseek = seq_lseek,
  .proc_release = single_release,
};
#else
static const struct file_operations lab1_fops = {
  .owner = THIS_MODULE,
  .open = lab1_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
};
#endif

static int __init lab1_init(void) {
  proc_create("lab1", 0, NULL, &lab1_fops);
  printk(KERN_INFO "lab1mod in\n");
  return 0;
}

static void __exit lab1_exit(void) {
  remove_proc_entry("lab1", NULL);
  printk(KERN_INFO "lab1mod out\n");
}

MODULE_LICENSE("GPL");
module_init(lab1_init);
module_exit(lab1_exit);
