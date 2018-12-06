#include <stdio.h>
#include <stdlib.h>
#include <string.h>



struct quadTree{
    int data;
    struct quadTree *elem[4];
};

typedef struct quadTree Node;
int size=4;

int insert(Node *node, int data, int size, int x, int y){
    if(size<2){
      node->data = data;
      return 0;
    }

    int i = 0;
    size = size/2;
    if(x>=size){
        if(y>=size) i=3;
        else i=1;
    }else{
        if(y>=size) i=2;
        else i=0;
    }

    if((node)->elem[i]==NULL){
        for(int k=0; k<4; k++){
            node->elem[k] = (Node *)calloc(1, sizeof(Node));
        }
    }
    if(node->elem[i]==NULL){
        printf("cannot allocate mem!");
        exit(1);
    }

    insert(node->elem[i], data, size, x % size, y % size); //unten links
    //mem optimization
    int allWhite = 1;
    for(int k=0; k<4; k++){
        if(node->elem[k]->data==0){
            allWhite = 0;
            break;
        }
    }
    if(allWhite){
        for(int k=0; k<4; k++){
            free(node->elem[k]);
            node->elem[k] = NULL;
        }
        node->data = 1;
    }
    return 0;

}

int printTree(Node *node){
    if(node->elem[0]==NULL){ //equivalent to other nodes
       switch(node->data){
           case 0:
               printf("B");
               return 0;
           case 1:
               printf("W");
               return 0;
       }
    }
    printf("[");
    for(int k=0; k<4; k++){
        printTree(node->elem[k]);
    }
    printf("]");
    
    return 0;
}
   


int colorAt(Node *node, int size, int x, int y){
    if(node->elem[0]==NULL){ //equivalent to other nodes
        switch(node->data){
            case 0:
                printf("#");
                break;
            case 1:
                printf(" ");
                break;
        }
        return 0;
    }
    int i=0;
    size = size/2;
    if(x>=size){
        if(y>=size) i=3;
        else i=1;
    }else{
        if(y>=size) i=2;
        else i=0;
    }

    colorAt(node->elem[i], size, x % size, y % size); 
    return 0;
}

int printImage(Node *node, int size){
    for(int row=0; row<size; row++){
        for(int col=0; col<size; col++){
            colorAt(node, size, col, row);
        }
        printf("\n");
    }
    return 0;
}

int main(){
    char buf[8];
    unsigned int size=0;
    Node *node = (Node *) calloc(1,sizeof(Node));
    if(scanf("%u", &size)!=1){
        printf("invalid input!");
        exit(0);
    }

    while(scanf("%s", buf)==1){
        //printf("%s",buf);
        if(strcmp(buf, "PUT")==0){
            unsigned int row,col;
            if(scanf("%u %u", &row, &col)!=2){
                printf("invalid input!");
                exit(1);
            }
            insert(node, 1, size, col, row);
        }
        if(strcmp(buf, "PRINT")==0){
            if(scanf("%s",buf)!=1){
                printf("invalid input!");
                exit(0);
            }
            if(strcmp(buf,"IMAGE")==0){
                printImage(node, size);
            }
            if(strcmp(buf,"TREE")==0){
                printTree(node);
                printf("\n");
            }
        }

    }

}

    
    
