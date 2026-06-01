#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <time.h>
#include "skiplist.h"

bool isEqualLevel0(skiplist *pList, char *expected)
{
    char *res = printToStringLevel0(pList);

    int cmp = strcmp(res, expected);

    if (cmp != 0)
    {
        printf("%s\n", res);
    }

    free(res);

    return cmp == 0;
}

bool isEqualAllLevels(skiplist *sl, char *str)
{
    char *res = printToStringFull(sl);

    int cmp = strcmp(res, str);

    if (cmp != 0)
    {
        printf("actual\n");
        printf("%s", res);

        printf("expected\n");
        printf("%s", str);
    }

    free(res);

    return cmp == 0;
}

void testOrder()
{
    skiplist *myList = createSkiplist();

    insertNode(myList, 100, "bbb");
    assert(isEqualLevel0(myList, "100 bbb"));

    insertNode(myList, 200, "bb");
    assert(isEqualLevel0(myList, "100 bbb,200 bb"));

    // TODO : ajouter des manipulations

    printf("TestOrder passed.\n");
}

skiplist *createSkiplistSample()
{
    skiplist *sl = createSkiplist();
    skiplistNode *n12 = createNode(2, 12, "bb");
    skiplistNode *n17 = createNode(5, 17, "");
    skiplistNode *n20 = createNode(1, 20, "");
    skiplistNode *n25 = createNode(4, 25, "");
    skiplistNode *n31 = createNode(3, 31, "");
    skiplistNode *n38 = createNode(2, 38, "");
    skiplistNode *n39 = createNode(1, 39, "");
    skiplistNode *n44 = createNode(4, 44, "");
    skiplistNode *n50 = createNode(1, 50, "");
    skiplistNode *n55 = createNode(4, 55, "");

    sl->level = 5;
    sl->length = 10;

    sl->header->level[0].forward = n12;
    sl->header->level[0].span = 1;
    sl->header->level[1].forward = n12;
    sl->header->level[1].span = 1;
    sl->header->level[2].forward = n17;
    sl->header->level[2].span = 2;
    sl->header->level[3].forward = n17;
    sl->header->level[3].span = 2;
    sl->header->level[4].forward = n17;
    sl->header->level[4].span = 2;

    n12->level[0].forward = n17;
    n12->level[0].span = 1;
    n12->level[1].forward = n17;
    n12->level[1].span = 1;

    n17->level[0].forward = n20;
    n17->level[0].span = 1;
    n17->level[1].forward = n25;
    n17->level[1].span = 2;
    n17->level[2].forward = n25;
    n17->level[2].span = 2;
    n17->level[3].forward = n25;
    n17->level[3].span = 2;
    n17->level[4].forward = NULL;
    n17->level[4].span = 9;

    n20->level[0].forward = n25;
    n20->level[0].span = 1;

    n25->level[0].forward = n31;
    n25->level[0].span = 1;
    n25->level[1].forward = n31;
    n25->level[1].span = 1;
    n25->level[2].forward = n31;
    n25->level[2].span = 1;
    n25->level[3].forward = n55;
    n25->level[3].span = 6;

    n31->level[0].forward = n38;
    n31->level[0].span = 1;
    n31->level[1].forward = n38;
    n31->level[1].span = 1;
    n31->level[2].forward = n55;
    n31->level[2].span = 5;

    n38->level[0].forward = n39;
    n38->level[0].span = 1;
    n38->level[1].forward = n44;
    n38->level[1].span = 2;

    n39->level[0].forward = n44;
    n39->level[0].span = 1;

    n44->level[0].forward = n50;
    n44->level[0].span = 1;
    n44->level[1].forward = n55;
    n44->level[1].span = 2;

    n50->level[0].forward = n55;
    n50->level[0].span = 1;

    n55->level[0].span = 1;
    n55->level[1].span = 1;
    n55->level[2].span = 1;
    n55->level[3].span = 1;

    return sl;
}

void testSkipSearch()
{
    skiplist *sl = createSkiplistSample();

    skiplistNode *n = skipSearch(sl, 42, "");
    assert(n && n->score == 39);

    n = skipSearch(sl, 10, "");
    assert(n == sl->header);

    n = skipSearch(sl, 12, "aa");
    assert(n == sl->header);

    n = skipSearch(sl, 12, "cc");
    assert(n == sl->header->level[0].forward);

    n = skipSearch(sl, 55, "");
    assert(n && n->score == 55);

    n = skipSearch(sl, 56, "");
    assert(n && n->score == 55);

    freeSkiplist(sl);

    printf("testSkipSearch passed\n");
}

void testInsert()
{
    skiplist *sl = createSkiplistSample();

    skiplistNode *n = insertNodeWithLevel(sl, 42, "test", 3);
    assert(n && n->score == 42 && strcmp(n->ele, "test") == 0);

    assert(isEqualAllLevels(sl,
                            "Level 4: (-inf,2,header) (17,10,) \n"
                            "Level 3: (-inf,2,header) (17,2,) (25,7,) (55,1,) \n"
                            "Level 2: (-inf,2,header) (17,2,) (25,1,) (31,3,) (42,3,test) (55,1,) \n"
                            "Level 1: (-inf,1,header) (12,1,bb) (17,2,) (25,1,) (31,1,) (38,2,) (42,1,test) (44,2,) (55,1,) \n"
                            "Level 0: (-inf,1,header) (12,1,bb) (17,1,) (20,1,) (25,1,) (31,1,) (38,1,) (39,1,) (42,1,test) (44,1,) (50,1,) (55,1,) \n"));

    n = insertNodeWithLevel(sl, 43, "test", 6);

    // TODO : assert quoi ?

    freeSkiplist(sl);

    printf("testInsert passed\n");
}

void testRemove()
{
    skiplist *sl = createSkiplistSample();

    removeValue(sl, 25, "");

    // TODO...
}

void assertRankEquals(unsigned long actual, unsigned long expected)
{
    if (actual != expected)
    {
        printf("actual: %lu\n", actual);
        printf("expected: %lu\n", expected);
    }

    assert(actual == expected);
}

void testGetRank()
{
    skiplist *sl = createSkipListSample();

    unsigned long rank = getRank(sl, 25, "");
    assert(rank == 25);

    printf("testGetRank passed\n");
}

void testPrintRangeByScore()
{
    skiplist *sl = createSkiplistSample();

    printRangeByScore(sl, 30, 60);

    freeSkiplist(sl);

    printf("testPrintRangeByScore passed\n");
}

int main()
{
    srand(time(NULL));

    // testSkipSearch();
    // testOrder();
    testInsert();
    // testRemove();
    // testGetRank();
    testPrintRangeByScore();

    return 0;
}