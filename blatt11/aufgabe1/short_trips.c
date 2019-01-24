#include <stralloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG
#define HASHSTART 5381

#define T_SIZE 255

typedef struct city {
    stralloc name;
    double latitude; // Breitengrad
    double longitude; // Längengrad
} City;

typedef struct node {
    unsigned int hash;
    City* city;
    struct node* next;
} Node;

typedef struct hashTable {
    Node* table[T_SIZE];
} HashTable;


static unsigned int compute_hash(stralloc* s, int len, int t_size) {
    int hashval = HASHSTART;
    for(unsigned int index = 0; index < len; ++index) {
        char ch = s->s[index];
        hashval += hashval << 5;
        hashval ^= ch;
    }
    return ((unsigned int) hashval) % t_size;
}

int printCity(City* city) {
    printf("%s B: %f L: %f \n", city->name.s, city->latitude, city->longitude);
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
#ifdef DEBUG
    printf("extracting\n");
#endif
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

// just for the case, that bucket table should become more complex
int insert(Node* node, HashTable* htab) {
    node->next = htab->table[node->hash];
    htab->table[node->hash] = node;
 
    return 1;
}


int readCitys(FILE* file, HashTable* htab) {
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
        if((city = calloc(1,sizeof(City))) == NULL) {
            fprintf(stderr, "unable to allocate mem for new City!\n");
            return 0;
        }
        
#ifdef DEBUG
        printf("%ld \n", strlen(content[0]));
#endif
        stralloc_copys(&(city->name), content[0]);
        stralloc_0(&(city->name));

        city->latitude = atof(content[1]);
        city->longitude = atof(content[2]);

        printCity(city);
        Node* node;
        if((node = calloc(1 ,sizeof(Node))) == NULL) {
            fprintf(stderr, "unable to allocate mem for ew Node!\n");
            return 0;
        }

        node->hash = compute_hash(&(city->name), city->name.len, T_SIZE);
        node->city = city;
#ifdef DEBUG
        printf("h: %d C: %p\n", node->hash, node->city);
#endif
        
        insert(node, htab);
    }
    return 1;
}

int printList(Node* head) {
    Node* node = head;
    printf("Head: %p\n", head->next);
    while(node != NULL) {
        printf("Node %p\n", node);
        printf("%s B: %f L: %f \n", node->city->name.s, node->city->latitude, node->city->longitude);
        node = node->next;
    }
    return 1;
}

int printTable(HashTable* htab) {
    for(int cnt=0; cnt < T_SIZE; cnt++){
        printList(htab->table[cnt]);
    }
    return 1;
}


int main() {
    FILE* file;
    if((file = fopen("ss1/blatt11/aufgabe1/gemeinden.txt", "r")) == NULL) {
        fprintf(stderr, "unable to open file!\n");
        return 0;
    }
    HashTable* htab;
    if((htab = calloc(1, sizeof(htab))) == NULL) {
        fprintf(stderr, "unable to allocate mem for hash table!\n");
    }
    readCitys(file, htab);
    printTable(htab);
    return 1;
}
