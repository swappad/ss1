#include <stralloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG
#define HASHSTART 5381

typedef struct city {
    char* name;
    double latitude; // Breitengrad
    double longitude; // Längengrad
} City;

typedef struct node {
    int hash;
    City* city;
    struct node* next;
} Node;

typedef struct cityList {
    Node* head;
} CityList;


static unsigned int compute_hash(char* s, int len) {
    int hashval = HASHSTART;
    for(unsigned int index = 0; index < len; ++index) {
        char ch = s[index];
        hashval += hashval << 5;
        hashval ^= ch;
    }
    return hashval;
}

int printCity(City* city) {
    printf("%s B: %f L: %f \n", city->name, city->latitude, city->longitude);
    return 1;
}


int readline(FILE* file, stralloc* s) {

#ifdef DEBUG
    printf("reading line...\n");
#endif
    s->len = 0;
    for(;;) {
        if(!stralloc_readyplus(s,1)) {
            fprintf(stderr, "unable to allocate mem for new char!\n");
            return 0;
        }
        if(fread(s->s + s->len, sizeof(char), 1, file) <= 0) {
            return 0;
        }
        if(s->s[s->len] == '\n') {
            ++s->len;
#ifdef DEBUG
            printf("line read!\n");
#endif
            return 1;
        }
        ++s->len;
    }
}


/* Function: extract
 * --------------------------------------
 *  extracts strings from a list of strings
 *  replaces seperator with '0' to make normal string operations possible
 *
 *  ':' is seperator
 *  '\n' indicates end
 *  max 3 strings per stralloc
 *  
 *  *s: stralloc
 *  *content[3]: char* array of size 3
 *
 *  return: 1 found three strings and '\n' indicator
 *          0 did not find '\n' indicator; strings might be corrupted
 *          0 parameter is NULL
 */
int extract(stralloc* s, char* content[3]) {
    if(s->s == NULL) {
        fprintf(stderr, "parameter NULL!\n");
        return 0;
    }
    char* cp = s->s;
    int cnt = 0;
    content[cnt] = s->s;
    cnt++;

    for(; cp <= (s->s + s->len); cp++) {
        if(*cp == ':') {
            *cp = 0;
            content[cnt] = cp + 1;
            cnt++;
        }
        if(*cp == '\n') {
            *cp = 0;
            return 1;
        }
    }
    return 0;
}

// just for the case, that list should become more complex
int append(Node* node, CityList* list) {
    node->next = list->head;
    list->head = node;
    return 1;
}

int readCitys(FILE* file, CityList* list) {
    if(file == NULL) {
        fprintf(stderr, "file pointer is NULL!\n");
        return -1;
    }
    stralloc s = {0};
    while(readline(file, &s)) {
        char* content[3];

        if(!extract(&s, content)) {
            return 0;
        }

        City* city;
        if((city = malloc(sizeof(City))) == NULL) {
            fprintf(stderr, "unable to allocate mem for new City!\n");
            return 0;
        }
        
#ifdef DEBUG
        printf("%ld \n", strlen(content[0]));
#endif
        if((city->name = malloc(strlen(content[0]) * sizeof(char)) ) == NULL) {
            fprintf(stderr, "unable to allocate mem for city name!\n");
            return 0;
        }
        strcpy(city->name, content[0]);

        city->latitude = atof(content[1]);
        city->longitude = atof(content[2]);

        printCity(city);
        Node* node;
        if((node = malloc(sizeof(Node))) == NULL) {
            fprintf(stderr, "unable to allocate mem for ew Node!\n");
            return 0;
        }

        node->hash = compute_hash(city->name, strlen(city->name));
        node->city = city;
        
        append(node, list);
    }
    return 1;
}

int printList(CityList* list) {
    Node* node = list->head;
    while(node != NULL) {
        printf("%s B: %f L: %f \n", node->city->name, node->city->latitude, node->city->longitude);
        node = node->next;
    }
    return 1;
}
        

int main() {
    FILE* file;
    if((file = fopen("ss1/blatt11/aufgabe1/gemeinden.txt", "r")) == NULL) {
        fprintf(stderr, "unable to open file!\n");
        return 0;
    }
    CityList citylist = {0};
    readCitys(file, &citylist);
    printList(&citylist);
    return 1;
}
