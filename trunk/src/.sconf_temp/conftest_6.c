
            
            #include <assert.h>
            
                #ifdef __cplusplus
                extern "C"
                #endif
                char setenv();

            int main() {
                    #if defined (__stub_setenv) || defined (__stub___setenv)
                    fail fail fail
                    #else
                    setenv();
                    #endif

                    return 0;
                }

