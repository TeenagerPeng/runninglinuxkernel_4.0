#include<linux/module.h>
#include<linux/mm.h>
#include<linux/slab.h>
#include<linux/init.h>

static char *kbuf;
static int size = 20;
static int align = 8;
static struct kmem_cache *slab_cache;

static int __init slab_demo_init(void) {

    slab_cache = kmem_cache_create("slab_cache", size, align, 0, NULL);

    if (!slab_cache) {
        pr_err("kmem_cache_create failed\n");
        return -ENOMEM;
    }

    pr_info("allocated memory cach correctly\n");

    kbuf = kmem_cache_alloc(slab_cache, GFP_ATOMIC);
    if (!kbuf) {
        pr_err("failed to create a cache object\n");
        (void)kmem_cache_destroy(slab_cache);
        return -1;
    }
    pr_info("create a cache object succeed\n");
    return 0;
}

static void __exit slab_demo_exit(void) {
    kmem_cache_free(slab_cache, kbuf);
    pr_info("destroyed a memory cache object\n");
    (void)kmem_cache_destroy(slab_cache);
}
module_init(slab_demo_init);
module_exit(slab_demo_exit);
MODULE_AUTHOR("Peng Jinghui");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("slab demo");