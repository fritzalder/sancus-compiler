#ifndef SPM_SUPPORT_H
#define SPM_SUPPORT_H

typedef unsigned spm_id;

struct Spm
{
    spm_id id;
    const char* name;
    void* public_start;
    void* public_end;
    void* secret_start;
    void* secret_end;
};

#define __PS(name) __spm_##name##_public_start
#define __PE(name) __spm_##name##_public_end
#define __SS(name) __spm_##name##_secret_start
#define __SE(name) __spm_##name##_secret_end

#define DECLARE_SPM(name)                                       \
    extern char __PS(name), __PE(name), __SS(name), __SE(name); \
    struct Spm name = {0, #name, &__PS(name), &__PE(name),      \
                       &__SS(name), &__SE(name)}

int protect_spm(struct Spm* spm);

#define always_inline inline __attribute__((always_inline))

always_inline void unprotect_spm()
{
    asm(".word 0x1380");
}

always_inline spm_id hmac_verify(const char* expected_hmac, struct Spm* spm)
{
    spm_id ret;
    asm("mov %1, r14\n\t"
        "mov %2, r15\n\t"
        ".word 0x1382\n\t"
        "mov r15, %0"
        : "=m"(ret)
        : "r"(spm->public_start), "r"(expected_hmac)
        : "r14", "r15");
    return ret;
}

always_inline spm_id hmac_write(char* dst, struct Spm* spm)
{
    spm_id ret;
    asm("mov %1, r14\n\t"
        "mov %2, r15\n\t"
        ".word 0x1383\n\t"
        "mov r15, %0"
        : "=m"(ret)
        : "r"(spm->public_start), "r"(dst)
        : "r14", "r15");
    return ret;
}

#define __ANNOTATE(x) __attribute__((annotate(x)))

#define SPM_FUNC(name)  __ANNOTATE("spm:" name)
#define SPM_ENTRY(name) __ANNOTATE("spm_entry:" name)
#define SPM_DATA(name)  SPM_FUNC(name)

#endif
