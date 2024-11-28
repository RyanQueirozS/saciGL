#include <assert.h>
#include "saci-test/saci-testing.h"

extern void saci_TestWindowing(void);
extern void saci_TestModelLoading(void);
extern void saci_TestRendering(void);

int main(void) {
    // saci_Test_PrintPassed(true); // do not toggle this on, unlees you want info about the checks that passed

    {
        saci_TestWindowing();
        saci_TestModelLoading();
        saci_TestRendering();
    }

    saci_Test_End();

    return 0;
}
