#include "BCMHostInit.h"

#include <bcm_host.h>

#include <stdio.h>

BMCHost::BMCHost(){
    initialized = false;
}

BMCHost* BMCHost::Instance(){
    static BMCHost instance;
    return &instance;
}

/*
void showLicenseStatus()
{
   int gencmdHandle = vc_gencmd_init();

   if (gencmdHandle != 0) {
      fprintf(stderr,"Error vc_gencmd_init. Error=%d.\n", gencmdHandle);
      return;
   }

   char *response = malloc(1024);
   memset(response, 0, 1024);

   int vc_error = vc_gencmd(response, 1023, "codec_enabled H264");
   if (vc_error != 0) {
      fprintf(stderr,"Error vc_gencmd. Error=%d.\n", vc_error);
      free(response);
      return;
   }

   fprintf(stderr,"vc_gencmd response: %s.\n", response);
   free(response);
}*/

void BMCHost::init(){
    if (initialized)
        return;
    initialized = true;
    bcm_host_init();
    fprintf(stderr,"BMCHost::init OK\n");
}
