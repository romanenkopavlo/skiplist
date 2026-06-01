#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <stdarg.h>
#include "skiplist.h"

skiplistNode *createNode(int level, double score, char *ele)
{
    skiplistNode *n = malloc(sizeof(*n) + level * sizeof(struct skiplistLevel));
    n->score = score;
    n->ele = ele == NULL ? NULL : strdup(ele);
    for (int i = 0; i < level; i++)
    {
        n->level[i].forward = NULL;
        n->level[i].span = 0;
    }
    return n;
}

void freeNode(skiplistNode *n)
{
    if (n == NULL)
        return;

    free(n->ele);

    free(n);
}

skiplist *createSkiplist()
{
    skiplist *myList = malloc(sizeof(skiplist));
    myList->header = createNode(SKIPLIST_MAXLEVEL, -DBL_MAX, NULL);
    myList->length = 0;
    myList->level = 1;
    return myList;
}

void freeSkiplist(skiplist *pList)
{
    skiplistNode *cur = pList->header;

    while (cur)
    {
        skiplistNode *c = cur;
        cur = cur->level[0].forward;
        freeNode(c);
    }

    free(pList);
}

unsigned long size(skiplist *pList)
{
    return pList->length;
}

int cmpNodes(skiplistNode *a, double score, char *str)
{
    if (a->score < score)
        return -1;
    if (a->score > score)
        return 1;

    if (a->ele == str)
        return 0;
    if (a->ele == NULL)
        return -1;
    if (str == NULL)
        return 1;
    return strcmp(a->ele, str);
}

skiplistNode *skipSearch(skiplist *pList, double score, char *str)
{
    skiplistNode *x = pList->header;

    for (int i = pList->level - 1; i >= 0; i--) {
        skiplistNode *next = NULL;
        while (1) {
            next = x->level[i].forward;
            if (next && cmpNodes(next, score, str) <= 0) {
                x = next;
            } else {
                break;
            }
        }
    }

    return x;
}

int computeRandomLevel(void)
{
    static const int threshold = SKIPLIST_P * RAND_MAX;
    int level = 1;
    while (rand() < threshold)
        level += 1;
    return (level < SKIPLIST_MAXLEVEL) ? level : SKIPLIST_MAXLEVEL;
}

skiplistNode *insertNode(skiplist *pList, double score, char *str)
{
    int level = computeRandomLevel();
    return insertNodeWithLevel(pList, score, str, level);
}

skiplistNode *insertNodeWithLevel(skiplist *pList, double score, char *str, int level)
{
    skiplistNode *update[SKIPLIST_MAXLEVEL];
    unsigned long rank[SKIPLIST_MAXLEVEL];

    for (int i = 0; i < SKIPLIST_MAXLEVEL; i++)
    {
        update[i] = pList->header;
    }

    skiplistNode *x = pList->header;
    unsigned long xRank = 0;

    for (int i = pList->level - 1; i >= 0; i--) {
        skiplistNode *next = NULL;
        while (1) {
            next = x->level[i].forward;
            if (next && cmpNodes(next, score, str) < 0) {
                xRank += x->level[i].span;
                x = next;
            } else {
                break;
            }
        }

        update[i] = x;
        rank[i] = xRank;
    }

    if (level > pList->level)
    {
        pList->level = level;
    }

    skiplistNode *newNode = createNode(level, score, str);

    for (int i = 0; i < level; i++) {
        newNode->level[i].forward = update[i]->level[i].forward;
        update[i]->level[i].forward = newNode;

        newNode->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
        update[i]->level[i].span = (rank[0] - rank[i]) + 1;
    }

    for (int i = level; i < pList->level; i++) {
        update[i]->level[i].span++;
    }

    pList->length++;

    return newNode;
}

int removeValue(skiplist *pList, double score, char *str)
{
    skiplistNode *update[SKIPLIST_MAXLEVEL];

    for (int i = 0; i < SKIPLIST_MAXLEVEL; i++)
    {
        update[i] = pList->header;
    }

    // TODO: parcours depuis le top niveau pour trouver le nœud et remplir «update»

    // TODO : supprimer le nœud

    // TODO : mettre à jour pList->length et pList->level

    return 1;
}

char *printToStringLevel0(skiplist *pList)
{
    int total_size = 0;

    for (skiplistNode *cur = pList->header->level[0].forward; cur != NULL; cur = cur->level[0].forward)
    {
        total_size += snprintf(NULL, 0, "%.0f %s", cur->score, cur->ele);
        if (cur->level[0].forward != NULL)
        {
            total_size += 1; // +1 for comma
        }
    }

    char *result = malloc(total_size + 1); // +1 for null terminator

    if (result == NULL)
    {
        perror("malloc failed");
        return NULL;
    }

    result[total_size] = '\0';

    char *ptr = result;

    for (skiplistNode *cur = pList->header->level[0].forward; cur != NULL; cur = cur->level[0].forward)
    {
        ptr += sprintf(ptr, "%.0f %s", cur->score, cur->ele);
        if (cur->level[0].forward != NULL)
        {
            ptr += sprintf(ptr, ",");
        }
    }

    return result;
}

int appendStringToBuffer(char **buffer, size_t *allocated_size, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    // Calculate the required size for the new formatted string
    size_t required_size = vsnprintf(NULL, 0, format, args) + 1; // +1 for the null terminator
    va_end(args);

    // Ensure that there's enough space in the buffer
    while (required_size + strlen(*buffer) >= *allocated_size)
    {
        *allocated_size *= 2; // Double the allocated size
        *buffer = (char *)realloc(*buffer, *allocated_size);
        if (*buffer == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            return -1;
        }
    }

    // Append the new formatted string to the buffer
    va_start(args, format);
    vsprintf(*buffer + strlen(*buffer), format, args);
    va_end(args);

    return 0;
}

char *printToStringFull(skiplist *pList)
{
    size_t buffer_size = 1024;
    char *buffer = malloc(buffer_size * sizeof(char));
    buffer[0] = '\0'; // Initialize as empty string for strlen

    for (int i = pList->level - 1; i >= 0; i--)
    {
        if (appendStringToBuffer(&buffer, &buffer_size, "Level %d: ", i) != 0)
        {
            return NULL;
        }

        appendStringToBuffer(&buffer, &buffer_size, "(-inf,%lu,header) ", pList->header->level[i].span);

        for (skiplistNode *node = pList->header->level[i].forward; node != NULL; node = node->level[i].forward)
        {
            if (appendStringToBuffer(&buffer, &buffer_size, "(%.0f,%lu,%s) ", node->score, node->level[i].span, node->ele) != 0)
            {
                return NULL;
            }
        }

        if (appendStringToBuffer(&buffer, &buffer_size, "\n") != 0)
        {
            return NULL;
        }
    }

    return buffer;
}

unsigned long getRank(skiplist *pList, double score, char *str)
{
    skipListNode *x = pList->header;
    unsinged long rank = 0;

    for (int i = pList->level - 1; i >= 0; i--) {
        skipListNode *next = NULL;
        while (1) {
            next = x->level[i].forward;
            if (next && cmpNodes(next, score, str) <= 0) {
                rank += x->level[i].span;
                x = next;
            } else {
                break;
            }
        }
    }

    return rank;
}

void printRangeByScore(skiplist *sl, double min, double max)
{
    // ...
}

void printRangeByScoreLog(skiplist *sl, double min, double max)
{
    // ...
}
