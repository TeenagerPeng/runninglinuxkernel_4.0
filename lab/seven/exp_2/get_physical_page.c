#include<linux/version.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/mm.h>

#define PRT(a, b) pr_info("%-15s=%10d %10ld %8ld\n", \
            a, b, (PAGE_SIZE*b)/1024, (PAGE_SIZE*b)/1024/1024)

static int __init get_pages_init(void) {
    struct page *p;
    int i;
    unsigned long pfn, valid = 0;
    int free = 0, locked = 0, reserved = 0, swapcache = 0,
        referenced = 0, slab = 0, private = 0, uptodate = 0,
        dirty = 0, active = 0, writeback = 0, mappedtodisk = 0;

    unsigned long num_physpages;
    num_physpages = get_num_physpages();
    for (i = 0; i < num_physpages; i++) {
        pfn = i + ARCH_PFN_OFFSET;
        if (!pfn_valid(pfn)) continue;
        valid++;
        p = pfn_to_page(pfn);
        if (!page_count(p)) {
            free++;
            continue;
        }
        if (PageLocked(p)) locked++;
        if (PageReserved(p)) reserved++;
        if (PageSwapCache(p)) swapcache++;
        if (PageReferenced(p)) referenced++;
        if (PageSlab(p)) slab++;
        if (PagePrivate(p)) private++;
        if (PageUptodate(p)) uptodate++;
        if (PageDirty(p)) dirty++;
        if (PageActive(p)) active++;
        if (PageWriteback(p)) writeback++;
        if (PageMappedToDisk(p)) mappedtodisk++;

    }

    pr_info("\nExamining %ld pages (num_phys_pages) = %ld MB\n", num_physpages, num_physpages * PAGE_SIZE / 1024 / 1024);
    pr_info("Pages with valid PFN's = %ld, = %ld MB\n", valid, valid * PAGE_SIZE / 1024 / 1024);

    pr_info("\n                     Pages         KB       MB\n\n");

    PRT("free", free);
    PRT("locked", locked);
    PRT("reserved", reserved);
    PRT("swapcache", swapcache);
    PRT("referenced", referenced);
    PRT("slab", slab);
    PRT("private", private);
    PRT("uptodate", uptodate);
    PRT("dirty", dirty);
    PRT("active", active);
    PRT("writeback", writeback);
    PRT("mappedtodisk", mappedtodisk);
    return 0;
}

static void __exit get_pages_exit(void) {
    pr_info("Module Unloading\n");
}

module_init(get_pages_init);
module_exit(get_pages_exit);

MODULE_AUTHOR("Peng Jinghui");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("get pages");