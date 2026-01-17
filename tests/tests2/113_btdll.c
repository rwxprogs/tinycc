int tcc_backtrace(const char*, ...);
#define hello() \
    tcc_backtrace("hello from %s() / %s:%d",__FUNCTION__,__FILE__,__LINE__)

#define __declspec(n)

#if DLL==1
__declspec(dllexport) int f_1()
{
    hello();
    return 0;
}


#elif DLL==2
__declspec(dllexport) int f_2()
{
    hello();
    return 0;
}


#else

int f_1();
int f_2();
int f_main()
{
    hello();
    return 0;
}

int main ()
{
    f_1();
    f_2();
    f_main();
    return 0;
}

#endif
