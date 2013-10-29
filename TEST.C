/*  8*7: 3.21: 2.73: 2.42: 2.16:	*/
/* 16*7: 4.85: 4.12: 3.75:     :	*/
/*
int	scyp;
int	edit_scr;

void	main()
{
	int	a,b,i;
	CRTMOD(16);G_CLR_ON();H_INIT();scyp=0;edit_scr=0;

	a=ONTIME();
	for(i=0;i<999;i++){g_print(100,10,"ABCDEFG",15);}
	b=ONTIME();b_iprint(b-a);

	a=ONTIME();
	for(i=0;i<999;i++){g_print(100,10,"‚`‚a‚b‚c‚d‚e‚f",15);}
	b=ONTIME();b_iprint(b-a);

}
*/
/*303*/
void	main()
{
	int	a,b,i;
	CRTMOD(16);G_CLR_ON();
	HOME(0,0,504);
child("command.x /c dir");
	a=ONTIME();
	for(i=0;i<49;i++){tg_copy(0);}
	b=ONTIME();b_iprint(b-a);


}
/*
void	main()
{
while(1){
int i;
for(i=0;i<16;i++){
	b_iprint(BITSNS(i));
}
	b_sprint("\r\n");
}
}
*/
