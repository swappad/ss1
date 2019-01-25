#include <stralloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//#define DEBUG
#define HASHSTART 5381

#define T_SIZE 255
#define PI 3.14159265358979323846


typedef struct city {
    stralloc name;
    double latitude; // Breitengrad
    double longitude; // Längengrad
} City;

typedef struct node {
    unsigned int hash;
    City city;
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

int printCity(City city) {
    printf("%s B: %f L: %f \n", city.name.s, city.latitude, city.longitude);
    return 1;
}


double deg2rad(double deg) {
    return deg * (PI / 180);
}

// Haversine formula; Ideas for Implementation from stackoverflow user Chuck from 26 Aug 2008
double getDistance(City a, City b) {
    double R = 6317;
    double dLat = deg2rad(a.latitude - b.latitude);
    double dLon = deg2rad(a.longitude - b.longitude);
    double w = 
        sin(dLat/2) * sin(dLat/2) +
        cos(deg2rad(a.latitude)) * cos(deg2rad(b.latitude)) * 
        sin(dLon/2) * sin(dLon/2)
        ;
    double c = 2 * atan2(sqrt(w), sqrt(1-w));
    double d = R * c;
    return d;
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
    printf("%ld\n",s->len);
#endif
    if(s->s == NULL || s->len == 0) {
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
            if(cnt < 3){
                content[cnt] = cp + 1;
                cnt++;
            }
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

        Node* node;
        if((node = calloc(1 ,sizeof(Node))) == NULL) {
            fprintf(stderr, "unable to allocate mem for ew Node!\n");
            return 0;
        }

        City city = {0};

        
#ifdef DEBUG
        printf("%ld \n", strlen(content[0]));
#endif
        stralloc_copys(&(city.name), content[0]);
        stralloc_0(&(city.name));

        city.latitude = atof(content[1]);
        city.longitude = atof(content[2]);

        node->hash = compute_hash(&(city.name), city.name.len, T_SIZE);
        node->city = city;
#ifdef DEBUG
        printf("h: %d C: %p\n", node->hash, &node->city);
#endif
        
        insert(node, htab);
    }
    return 1;
}

City* findCity(stralloc s, HashTable* htab) {

    int hash = compute_hash(&s, s.len, T_SIZE);
    Node* node = htab->table[hash];
    while(node != NULL) {
        if(!stralloc_diff(&(node->city.name), &s)) {
            return &(node->city);
        }
        node = node->next;
    }
    return NULL;
}
                

int printList(Node* head) {
    Node* node = head;
    while(node != NULL) {
        printf("%s B: %f L: %f \n", node->city.name.s, node->city.latitude, node->city.longitude);
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



int main(int argc, char** argv) {
    if(argc != 3) {
        fprintf(stderr, "1invalid arguments!\n");
        exit(0);
    }

    


    FILE* file;
    if((file = fopen("gemeinden.txt", "r")) == NULL) {
        fprintf(stderr, "unable to open file!\n");
        return 0;
    }
    HashTable* htab;
    if((htab = calloc(1, sizeof(HashTable))) == NULL) {
        fprintf(stderr, "unable to allocate mem for hash table!\n");
    }
    readCitys(file, htab);
    //printTable(htab);

    stralloc sa,sb = {0};
    stralloc_copys(&sa, argv[1]);
    stralloc_0(&sa);
    stralloc_copys(&sb, argv[2]);
    stralloc_0(&sb);

    City* a = findCity(sa, htab);
    City* b = findCity(sb, htab);

    if(a == NULL || b == NULL) {
        fprintf(stderr, "2invalid arguments!\n");
        exit(0);
    }

    double dtotal = getDistance(*a, *b);
    double dtravels = 0;
    printf("Distance between cities: %f\n", dtotal);

    // Gameplay
    City x = *a;
    while(1) {
        printf("you are currently located in %s.\n Next town?", x.name.s);
        stralloc sx = {0};
        readline(stdin, &sx);
        char* c[3];
        if(!extract(&sx, c)) {
            fprintf(stderr, "invalid argument!\n");
            exit(0);
        }
        
        if(c[0] == NULL) {
            fprintf(stderr, "invalid argument!\n");
            exit(0);
        }


        stralloc_copys(&sx, c[0]);
        stralloc_0(&sx);

        City* xNext;
        if((xNext = findCity(sx, htab)) == NULL) {
            fprintf(stderr, "invalid argument!\n");
            exit(0);
        }

        

        double dnext = getDistance(x, *xNext);
        dtravels += dnext;
        if(!stralloc_diff(&(xNext->name), &(b->name))) {
            printf("Welcome to %s!\n Your travel distance was %f\n", b->name.s, dtravels);
            exit(1);
        }
        printf("Distance from %s to %s was %f\n", x.name.s, xNext->name.s, dnext);

        x = *xNext;

    }


    return 1;
}
