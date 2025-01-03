#include <assert.h>
#include <stdlib.h>

#define SACI_TEST_IMPLEMENTATION
#include "saci-testing.h"

extern void saci_TestWindowing(void);
extern void Test_Unit_Model_Loading(void);
extern void Test_Integration_Model_Loading(void);
extern void saci_TestRendering(void);

int main(void) {
    srand(time(NULL)); // for randomly generated data
    // saci_Test_PrintPassed(true); // do not toggle this on, unlees you want info about the checks that passed

    {
        saci_TestWindowing();
        Test_Unit_Model_Loading();
        Test_Integration_Model_Loading();
        saci_TestRendering();
    }

    saci_Test_End();

    return 0;
}
