/* _start */
int main(void);

void __attribute__ ((constructor)) _start(void)
{
    main();
}
