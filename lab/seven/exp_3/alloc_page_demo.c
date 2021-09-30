#include<linux/module.h>
#include<linux/slab.h>
#include<linux/init.h>

static int __init alloc_pages_init(void) {
    static char *kbuf;
    static unsigned long order;
    int size;
    for (size = PAGE_SIZE, order = 0; order < MAX_ORDER; order++, size *= 2) {
        pr_info("order = %2ld, pages = %5ld, size = %8d", order, size / PAGE_SIZE, size);
        kbuf = (char *)__get_free_pages(GFP_ATOMIC, order);
        if (!kbuf) {
            pr_err("... __get_free_pages failed \n");
            break;
        }
        pr_info("... __get_free_pages succeed \n");
        free_pages((unsigned long)kbuf, order);
    }
    for (size = PAGE_SIZE, order = 0; order < MAX_ORDER; order++, size *= 2) {
        pr_info("order = %2ld, pages = %5ld, size = %8d", order, size / PAGE_SIZE, size);
        kbuf = kmalloc((size_t) size, GFP_ATOMIC);
        if (!kbuf) {
            pr_err("... kmalloc failed\n");
            break;
        }
        pr_info("... kmalloc succeed \n");
        kfree(kbuf);
    }
    return 0;
}

static void __exit alloc_pages_exit(void) {
    pr_info("Module unloading\n");
}

module_init(alloc_pages_init);
module_exit(alloc_pages_exit);
MODULE_AUTHOR("Peng Jinghui");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("alloc pages");