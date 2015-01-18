/*
 * sysinfo.c
 *
 * Utility to get system information.
 *
 * (c) 2015 Herman verschooten
 */
#include <ruby.h>
#include <sysinfo.h>

VALUE Module= Qnil;

void Init_sysinfo() {
    Module = rb_define_module("Hvutils");
    rb_define_module_function(Module, "sysinfo", method_sysinfo, 0);
}

#ifdef _DARWIN_C_SOURCE
long uptime(void){
    struct timeval boottime;
    size_t len = sizeof(boottime);
    int mib[2] = { CTL_KERN, KERN_BOOTTIME };
    if(sysctl(mib, 2, &boottime, &len, NULL, 0) != -1) {
        time_t bsec = boottime.tv_sec, csec = time(NULL);
        return difftime(csec, bsec);
    }
    return -1;
}

long totalram(void){
    long totram;
    size_t len = sizeof(totram);
    int mib[2] = { CTL_HW, HW_MEMSIZE };
    if(sysctl(mib, 2, &totram, &len, NULL, 0) != -1)
        return totram;
    return -1;
}

/* int get_load(double loadavg[]) { */

/*     size_t len = sizeof(loadinfo); */
/*     printf("Size: %lu\n", len); */
/*     int mib[2] = { CTL_VM, VM_LOADAVG }; */
/*     return sysctl(mib,2, &loadinfo, &len, NULL, 0); */
/* } */

int
getloadavg(loadavg, nelem)
    double loadavg[];
    int nelem;
{
    struct loadavg loadinfo;
    int i, mib[2];
    size_t size;

    mib[0] = CTL_VM;
    mib[1] = VM_LOADAVG;
    size = sizeof(loadinfo);
    if (sysctl(mib, 2, &loadinfo, &size, NULL, 0) < 0)
        return (-1);

    nelem = MIN(nelem, sizeof(loadinfo.ldavg) / sizeof(fixpt_t));
    for (i = 0; i < nelem; i++) {
        loadavg[i] = (double) loadinfo.ldavg[i] / loadinfo.fscale;
    }
    return (nelem);
}
#endif
VALUE method_sysinfo(VALUE self) {
#ifdef __linux__
    struct sysinfo my_info;
    VALUE result, load;
    if(sysinfo(&my_info) != 0) {
        rb_raise(rb_eStandardError, "Could not retrieve system information");
    }
    result = rb_hash_new();
    load = rb_hash_new();

    rb_hash_aset(load, INT2NUM(1), rb_float_new(my_info.loads[0]));
    rb_hash_aset(load, INT2NUM(5), rb_float_new(my_info.loads[1]));
    rb_hash_aset(load, INT2NUM(15), rb_float_new(my_info.loads[2]));

    rb_hash_aset(result, ID2SYM(rb_intern("uptime")), LONG2NUM(my_info.uptime));
    rb_hash_aset(result, ID2SYM(rb_intern("load")), load);
    rb_hash_aset(result, ID2SYM(rb_intern("freeram")), LONG2NUM(my_info.freeram));
    rb_hash_aset(result, ID2SYM(rb_intern("totalram")), LONG2NUM(my_info.totalram));
    rb_hash_aset(result, ID2SYM(rb_intern("sharedram")), LONG2NUM(my_info.sharedram));
    rb_hash_aset(result, ID2SYM(rb_intern("bufferram")), LONG2NUM(my_info.bufferram));
    rb_hash_aset(result, ID2SYM(rb_intern("totalswap")), LONG2NUM(my_info.totalswap));
    rb_hash_aset(result, ID2SYM(rb_intern("freeswap")), LONG2NUM(my_info.freeswap));
    rb_hash_aset(result, ID2SYM(rb_intern("freehigh")), LONG2NUM(my_info.freehigh));
    rb_hash_aset(result, ID2SYM(rb_intern("totalhigh")), LONG2NUM(my_info.totalhigh));
    rb_hash_aset(result, ID2SYM(rb_intern("memunit")), INT2NUM(my_info.mem_unit));
    rb_hash_aset(result, ID2SYM(rb_intern("procs")), INT2NUM(my_info.procs));

    return result;
#elif defined(_DARWIN_C_SOURCE)
    VALUE result, load;

    double loadinfo[3];
    int nelem;

    load = rb_hash_new();

    if(getloadavg(loadinfo,nelem) > 0){
        rb_hash_aset(load, INT2NUM(1), rb_float_new(loadinfo[0]));
        rb_hash_aset(load, INT2NUM(5), rb_float_new(loadinfo[1]));
        rb_hash_aset(load, INT2NUM(15), rb_float_new(loadinfo[2]));
    }

    result = rb_hash_new();
    rb_hash_aset(result, ID2SYM(rb_intern("uptime")), LONG2NUM(uptime()));
    rb_hash_aset(result, ID2SYM(rb_intern("totalram")), LONG2NUM(totalram()));
    rb_hash_aset(result, ID2SYM(rb_intern("load")), load);

    return result;
#else
    return rb_hash_new();
#endif
}

