#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

int DWCi_EncSessionProcess(void);
void DWCi_EncSessionShutdown(void);
// HYPOTHESIS: parameter types guessed from register use in SBS callers.
void DWCi_EncSessionInitialize(int mode, const char* name);
int DWCi_EncSessionGetAsync(const char* request, void* key, void* data, int size, int flag);

#ifdef __cplusplus
}
#endif
