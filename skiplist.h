#ifndef SKIPLIST_H_
#define SKIPLIST_H_

#define SKIPLIST_MAXLEVEL 32
#define SKIPLIST_P 0.25

typedef struct skiplist
{
    struct skiplistNode *header;
    unsigned long length;
    int level;
} skiplist;

typedef struct skiplistNode
{
    char *ele;
    double score;
    struct skiplistLevel
    {
        struct skiplistNode *forward;
        unsigned long span;
    } level[];
} skiplistNode;

skiplistNode *createNode(int level, double score, char *ele);
void freeNode(skiplistNode *n);

char *printToStringLevel0(skiplist *pList);
char *printToStringFull(skiplist *pList);

skiplist *createSkiplist();
void freeSkiplist(skiplist *pList);

unsigned long size(skiplist *pList);
skiplistNode *skipSearch(skiplist *pList, double score, char *str);

skiplistNode *insertNode(skiplist *pList, double score, char *str);
skiplistNode *insertNodeWithLevel(skiplist *pList, double score, char *str, int level); // Do not use externally, for test purposes only.
int removeValue(skiplist *pList, double score, char *str);

/* Rank is 1 based. Returns 0 if not found. */
unsigned long getRank(skiplist *sl, double score, char *str);

void printRangeByScore(skiplist *sl, double min, double max);

#endif
