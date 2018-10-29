#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int randCase();

int main(){
    printf("***banca francesca*** \n"
            "In jeder Runde koennen Sie  einen Teil Ihres Geldes auf eine der folgenden Kombinationen setzen: \n"
            "1: Aces, Augensumme: 3 mit einer Gewinnquote von 1:61 \n"
            "2: Pequeno, Augensumme:5 6 7 mit einer Gewinnquote von 1:1 \n"
            "3: Grande, Augensumme: 14 15 16 mit einer Gewinnquote von 1:1 \n");
    int kontostand=1000;
    int einsatz=0;
    int kombination=-1;

    while(1==1){
       if(kontostand<=0) return 0; 
       printf("Kontostand: %d\n",kontostand);
       printf("Einsatz und Kombination:");

       if(scanf("%d %d",&einsatz, &kombination)==2 && kombination<=3 && kombination>=1 && einsatz>=0){

         if(einsatz==0) return 0;
         if(!(kontostand-einsatz<0)){
        
             int tmp=randCase();
             if(tmp==0){
                 printf("Nichts passiert! \n");
             }else if(tmp==kombination){
                 switch(tmp){
                     case 1: kontostand+=einsatz*61; 
                             printf("Gewonnen! Aces\n");
                             break;
                     case 2: printf("Gewonnen! Pequeno\n");
                     case 3: printf("Gewonnen! Grande\n");
                     default : kontostand+=kontostand;
                 }
             }else{
                 kontostand-=einsatz;
                 printf("Verloren! \n");
             }

         }else printf("Einsatz zu hoch! \n");
       }else{
           fflush(stdin);
           printf("Ungueltige Eingabe! \n");
       }
    } 

}

int randCase(){
    srand(time(0));
    int  r= rand()%7 + rand()%7 + rand()%7 + 3;
    if(r==3) return 1;
    if(r==5||r==6||r==7) return 2;
    return ((r==14||r==15||r==16) ? 1:0);
    return 0;
}
    
