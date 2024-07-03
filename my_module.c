#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/module.h>

#define DEVICE_NAME "my_device"
#define MAJOR_NUM 100

// 定义IO控制命令
#define IOCTL_ALLOC_MEM _IOR(MAJOR_NUM, 0, size_t)
#define IOCTL_WRITE_DATA _IOW(MAJOR_NUM, 1, char *)
#define IOCTL_READ_DATA _IOR(MAJOR_NUM, 2, char *)

static char *kernel_memory = NULL;
static size_t memory_size = 0;

static long device_ioctl(struct file *file, unsigned int cmd,
                         unsigned long arg) {
  switch (cmd) {
  case IOCTL_ALLOC_MEM:
    // 分配内存
    memory_size = (size_t)arg;
    kernel_memory = kmalloc(memory_size, GFP_KERNEL);
    if (!kernel_memory)
      return -ENOMEM;
    break;
  case IOCTL_WRITE_DATA:
    // 存储用户态数据到内核空间
    if (copy_from_user(kernel_memory, (char *)arg, memory_size))
      return -EFAULT;
    break;
  case IOCTL_READ_DATA:
    // 导出内核数据到用户态
    if (copy_to_user((char *)arg, kernel_memory, memory_size))
      return -EFAULT;
    break;
  default:
    return -EINVAL;
  }
  return 0;
}

static struct file_operations fops = {
    .unlocked_ioctl = device_ioctl,
};

static int __init my_module_init(void) {
  // 注册设备
  int ret = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
  if (ret < 0) {
    printk(KERN_ALERT "Registering char device failed with %d\n", ret);
    return ret;
  }
  printk(KERN_INFO "My module loaded with device major number %d\n", MAJOR_NUM);
  return 0;
}

static void __exit my_module_exit(void) {
  // 注销设备
  if (kernel_memory)
    kfree(kernel_memory);
  unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
  printk(KERN_INFO "My module unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
