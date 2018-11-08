#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int printField(unsigned long int x, int size);
int removeKayles(unsigned long int *x,int position, int hits);
int input = 0;
int position = 0;
int hits = 0;
int player=0;
unsigned long int field=1; //the field is saved from left to right<->least to most significant bit
int size=10;    
int main(){
    //setup
    srand(time(0));
    size = (rand()%(30-10))+10;
    printf("%d",size);
    field = (1<<(size))-1;
    printField(field, size);

    //Gameplay
    while(field){
        switch(player){
            case 0: printf("\nYour move: ");
                
                    if(scanf("%d",&input)!=1) return 1;

                    if((~size+1)<input && input<0){ //Case where input is less than zero hits two kyles
                        position = ~input+1;
                        hits = 2;
                        if(removeKayles(&field, position, hits)==-1){
                            printf("Shot into Space");
                            player=0;
                            break;
                        }
                        printField(field,size);
                        player^=1;
                        break;

                    }else if(size>=input && input>0){ //Case where input is greater than zero hits only one kayle
                        position = input;
                        hits = 1;
                        if(removeKayles(&field, position, hits)==-1){
                            printf("Shot into Space");
                            player=0;
                            break;
                        }
                        printField(field,size);
                        player^=1;
                        break;
                         
                    }else{
                        printf("\nInvalid input!");
                        player=0;
                        break;
                    }
            case 1: printf("\nMy move: ");
                    int mypos = size/2;
                    int cnt=0;
                    while(field && removeKayles(&field, mypos, 2)==-1 && removeKayles(&field, mypos, 1)==-1){
                       mypos=size-cnt;
                       cnt++;
                    } 
                    printField(field,size);
                    player^=1;
        }
    }
    player ? printf("You won!\n") : printf("You lost!\n");
}

int printField(unsigned long int x,int size){
    printf("\n"); 
    for(int i=0; i<size; i++){
        (x & (1<<i)) ? (printf("/\\ ")) : (printf("   "));
    }
    printf("\n");
    for(int i=1; i<=size; i++){
        printf("%02d ",i);
    }
    printf("\n");
    return 0;
}

int removeKayles(unsigned long int *x, int position, int hits){
    int tmp = ( (1<<hits)-1)<<(position-1); 
    if((*x & tmp)==tmp) { 
        *x ^=tmp; 
    }else{
        return -1; //Checking wether kayles weren't already hit
    }
    return 0;
}

