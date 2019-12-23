#include "mach_override.h"
#include "helpers.h"
#include "offsets.h"
#include "payload.h"
#include "UI/ui.h"
#include "UI/openGL/Interface.h"
#define DWORD unsigned int

void commandPatches(){
    printf("[CommandHanlder] Command Handler Patches injected\n");
    void *dst = (void *)(AC_BASE + commandHandlerOffset);
    *(uint64_t *)dst = commandOverwrites;
    void (*objc_load_addr_orig)(void);
    objc_load_addr_orig = dst;
    objc_load_addr_orig();
}

int applyPatches(){
    void *src = (void *)((AC_BASE + commandHandlerOffset));
    void *dst = (void *)&commandPatches;
    printf("[PATCHER] %p -> %p\n", src, dst);
    commandOverwrites = patch_jmp(src, dst);
    printf("[PATCHER] Command-Overwrite return: 0x%02llx\n", commandOverwrites);
    return 0;
}

__attribute__((constructor)) static void ctor(void) {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 5 * NSEC_PER_SEC), dispatch_get_main_queue(), ^{
        if(applyPatches() != 0){
            printf("[ERROR] Error occured while trying to apply patches.\n");
            exit(1);
        }else {
            printf("[PATCHER] applied!\n");
        }
        if(menuType == 1){
            mach_override_ptr(dlsym(RTLD_DEFAULT, "CGLFlushDrawable"), CGLFlushDrawableOverride, (void **)&Original_CGLFlushDrawable);
        }
        printf("[PATCHER] Everything set\n");
    });
}

