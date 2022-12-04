/**
 * Kostra programu pro 2. projekt IZP 2022/23
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h>
#include <ctype.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap) {
    assert(c != NULL);
    assert(cap >= 0);

    c->size = 0;
    c->capacity = cap;
    c->obj = malloc(sizeof(struct obj_t) * cap);
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c) {
    if (c->obj != NULL) {
        free(c->obj);
        c->obj = NULL;
    }
    init_cluster(c, 0);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap) {
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t *) arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj) {
    if (c->capacity <= c->size) {
        c = resize_cluster(c, c->capacity + CLUSTER_CHUNK);
    }
    c->obj[c->size] = obj;
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2) {
    assert(c1 != NULL);
    assert(c2 != NULL);

    for (int i = 0; i < c2->size; ++i) {
        append_cluster(c1, c2->obj[i]);
    }
    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx) {
    assert(idx < narr);
    assert(narr > 0);

    // If idx is bigger than number of clusters nothing happens, because it's not possible to remove cluster that does not exist
    if (idx >= narr) {
        return narr;
    }

    clear_cluster(&carr[idx]);
    carr[idx] = carr[narr - 1];
    return narr - 1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2) {
    assert(o1 != NULL);
    assert(o2 != NULL);

    return sqrt(pow(o1->x - o2->x, 2) + pow(o1->y - o2->y, 2));
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2) {
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float smallestDistance = obj_distance(&c1->obj[0], &c2->obj[0]);
    for (int i = 0; i < c1->size; ++i) {
        for (int j = 0; j < c2->size; ++j) {
            float distance = obj_distance(&c1->obj[i], &c2->obj[j]);
            if (distance < smallestDistance) {
                smallestDistance = distance;
            }
        }
    }
    return smallestDistance;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2) {
    assert(narr > 0);

    *c1 = 0;
    *c2 = 1;
    float smallestDistance = cluster_distance(&carr[0], &carr[1]);
    for (int i = 0; i < narr; ++i) {
        for (int j = 0; j < narr; ++j) {
            if (i == j) {
                continue;
            }

            float distance = cluster_distance(&carr[i], &carr[j]);

            if (distance < smallestDistance) {
                smallestDistance = distance;
                *c1 = i;
                *c2 = j;
            }
        }
    }
    // Casting to void because otherwise throws error "parameter set but not used"
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b) {
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *) a;
    const struct obj_t *o2 = (const struct obj_t *) b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c) {
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c) {
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++) {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

// isNumber checks if string 'str' is a number. Returns true or false
int isNumber(char str[]) {
    int len = strlen(str);
    for (int i = 0; i < len; ++i) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr) {
    assert(arr != NULL);

    // Opens file and checks for error
    FILE* file;
    file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }

    // Reads from file line by line
    // Size of line is set to 256, which due to the nature of the data should be sufficient
    char line[256];
    memset(line, 0, 256);
    int lineNumber = 0;
    int cnt = 0;
    while (fgets(line, sizeof(line), file)) {
        struct obj_t o;
        lineNumber++;

        if(lineNumber > 1 && lineNumber > cnt + 1) {
            continue;
        }

        // Value from count in the first line is parsed and saved into numOfObjects
        // Cluster array is initialized
        if (lineNumber == 1) {
            char * count = strrchr(line, '=');
            // Removes first character (=) and last character (\n)
            memmove(count, count + 1, strlen(count));
            count[strlen(count) - 1] = '\0';
            if(!isNumber(count)) {
                return -2;
            }
            cnt = atoi(count);
            *arr = malloc(sizeof(struct cluster_t) * cnt);
            for (int i = 0; i < cnt; ++i) {
                init_cluster(&(*arr)[i], CLUSTER_CHUNK);
            }
            memset(line, 0, 256);
            continue;
        }

        // Sets attributes in object o after checking if they are in the correct format
        // Tried using scanf, didn't work, this does
        char * tokenID;
        char * tokenX;
        char * tokenY;

        tokenID = strtok(line, " ");
        tokenX = strtok(NULL, " ");
        tokenY = strtok(NULL, " ");

        // Removes last character (\n) from tokenY
        tokenY[strlen(tokenY) - 1] = '\0';
        if (!isNumber(tokenY) || !isNumber(tokenX) || !isNumber(tokenID)) {
            return -3;
        }

        o.id = atoi(tokenID);
        o.x = atoi(tokenX);
        o.y = atoi(tokenY);

        if (o.y > 1000 || o.x > 1000) {
            return -3;
        }

        append_cluster(&(*arr)[lineNumber - 2], o);
        memset(line, 0, 256);
    }
    return cnt;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr) {
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++) {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[]) {
    struct cluster_t *clusters;
    int targetClusters = 1;
    int narr;

    // Verifies correctness of arguments
    if (argc < 2) {
        fprintf(stderr, "Too few arguments\n");
        return EXIT_FAILURE;
    } else if (argc > 3) {
        fprintf(stderr, "Too many arguments\n");
        return EXIT_FAILURE;
    } else if (argc == 3 && !isNumber(argv[2])) {
        fprintf(stderr, "Last argument needs to be a number\n");
        return EXIT_FAILURE;
    }

    // If 3rd argument is set, targetClusters gets overwritten from default value 1 with the value in 3rd argument
    if (argc == 3) {
        targetClusters = atoi(argv[2]);
    }

    // Loads clusters and returns error
    // If error occurs, returns error message and exits
    narr = load_clusters(argv[1], &clusters);
    switch (narr) {
        case -1:
            fprintf(stderr, "Error when loading clusters from a file\n");
            fprintf(stderr, "Cannot open the file\n");
            return EXIT_FAILURE;
        case -2:
            fprintf(stderr, "Error when loading clusters from a file\n");
            fprintf(stderr, "First line is in wrong format \"count=[object count]\"\n");
            return EXIT_FAILURE;
        case -3:
            fprintf(stderr, "Error when loading clusters from a file\n");
            fprintf(stderr, "One or more objects are in wrong format\n");
            return EXIT_FAILURE;
    }

    while(narr > targetClusters){
        int c1;
        int c2;
        find_neighbours(clusters, narr, &c1, &c2);
        merge_clusters(&clusters[c1], &clusters[c2]);
        narr = remove_cluster(clusters, narr, c2);
    }

    print_clusters(clusters, targetClusters);

    for (int i = 0; i < narr; ++i) {
        clear_cluster(&clusters[i]);
    }
    free(clusters);

    return EXIT_SUCCESS;
}