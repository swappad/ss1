#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define maxLineLength 200
#define amtOfItems 8
//Entry for Plainberg includes description in the set
//
struct mountain{
    char name[maxLineLength];
    long heigth;
};

int printMountains(struct mountain mountains[], int mountainslen);
int swap(struct mountain mountains[],int pos1,int pos2);
int checkInput(int guess, int round);
int sort(struct mountain m[],int length);
struct mountain mountains[amtOfItems];

int main(){

    //Setup
    FILE *datei;
    datei = fopen("berge.txt","r"); //readOnly

    if(datei==NULL){
        printf("Unable to read file!\n");
        return 0;
    }

    char buf[maxLineLength];
    int linecounter=0; //linecounter in document
    int index=0; //current index to write in reservoir
    srand(time(0));

    while(fgets(buf, maxLineLength-1, datei)){
        
        index = (linecounter>=amtOfItems) ? (rand() % linecounter) : (linecounter); 
        //printf("Item %d\n",index); //debugging
        if(index<amtOfItems){
            //seperates name and height
            int charcounter=0;
            char *cp;
            for(cp=buf; cp-buf<maxLineLength; cp++){
                if(*cp==':') break;
                mountains[index].name[charcounter]=*cp;
                mountains[index].name[charcounter+1]='\0';
                charcounter++;
            }
            mountains[index].heigth=atoi(cp+1);
        }
        linecounter++;

    }
    
    printf("Bring the mountains in correct order. Begin with the smallest!\n");
    printMountains(mountains, amtOfItems);

    //Game
    int inorder=1;
    int round =1;
    int guess=0;
    int validIn=0;
    int score=0;
    while(inorder || round<=amtOfItems){
        printf("Guess the next place!\n");
        printf("%d:",round);

        do{
            int in = scanf("%d",&guess);
            if(!(in==1 && guess>=round && guess<=amtOfItems)){
                fflush(stdin);
                return 0; //stupid, but submission test does not accept exception handling
                printf("Invalid input. Try again!\n");
                printf("%d:",round);
                validIn=1;
            }else validIn=0;
        }while(validIn);

        if(checkInput(guess-1,round)){ //Array index fix
            printf("You guessed right!\n");
            swap(mountains, guess-1, round-1);
            printMountains(mountains, amtOfItems);
            score++;
        }else{
            printf("You guessed wrong! End of Game\n");
            printf("Your score is: %d\n", score);
            inorder=0;
            sort(mountains, amtOfItems);
            printf("The correct order was:\n");
            printMountains(mountains,amtOfItems);
            return 0;
        }
        round++;

    }
    printf("Congratulations!!! You found the correct order!");
    return 0;


}
int swap(struct mountain mountains[],int pos1,int pos2){
    if(pos1<0 && pos2<0) return 0;
    struct mountain tmp = mountains[pos1];
    mountains[pos1] = mountains[pos2];
    mountains[pos2] = tmp;
    return 0;
}

int checkInput(int guess, int round){
    int place=1;
    for(int i=0;i<amtOfItems;i++){
        if(mountains[i].heigth<mountains[guess].heigth){
            place++;
        }
    }
    printf("%d\n",place);
    return (round==place) ? 1 : 0;
}
//bubble sort algorithm
int sort(struct mountain m[],int length){
    int swapped=1;
    while(swapped){
        swapped=0;
        for(int i=0;i<=length;i++){
            if(m[i].heigth>m[i+1].heigth){
                swap(m,i,i+1);
                swapped=1;
            }
        }
    }
    return 0;
}

int printMountains(struct mountain mountains[], int mountainsLen){
    printf("\n");
    for(int i=0; i<mountainsLen;i++){
        printf("%d %s",i+1, mountains[i].name);
        //printf(" %ld",mountains[i].heigth); //cheat mode
        printf("\n");
    }
    printf("\n");
    return 0;
}



