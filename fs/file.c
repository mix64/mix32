#include <file.h>
#include <init.h>
#include <intr.h>
#include <macro.h>
#include <console.h>

struct file *fget(int index)
{
    if (index > NELEM(file)) {
        panic("fget");
    }
    disable_intr();
    file[index].refcnt += 1;
    enable_intr();
    return &file[index];
}

struct file *falloc()
{
    struct file *fp = 0;
    disable_intr();
    for (int i = 0; i < NELEM(file); i++) {
        if (file[i].refcnt == 0) {
            fp = &file[i];
            fp->refcnt = 1;
            break;
        }
    }
    enable_intr();
    return fp;
}

void ffree(struct file *f)
{
    if (f == 0) {
        panic("ffree");
    }
    disable_intr();
    if (f->refcnt < 1) {
        panic("ffree-2");
    }
    --f->refcnt;
    enable_intr();
}

struct file *fdup(struct file *f)
{
    if (f == 0 || f->refcnt < 1) {
        panic("fdup");
    }
    disable_intr();
    f->refcnt++;
    enable_intr();
    return f;
}
