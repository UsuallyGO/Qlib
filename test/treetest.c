
#include <stdio.h> //printf
#include <stdlib.h> //random
#include <time.h> //time(0)
#include "qlib/qtree.h"
#include "qlib/qlist.h"
#include "qlib/qmemory.h"

//build a binary search tree with 20 nodes, which are randomly selected from
// 1 to 100.
int dataCmp(const void *a1, const void *a2)
{
    int m, n;

    m = *(int*)a1;
    n = *(int*)a2;

    if(m == n)
        return 0;
    else if(m > n)
        return 1;
    else
        return -1;
}

void bintreetest()
{
    int number[100], num2[40];
    int index, counter, tmp;
    int *data;
    QBinTree btree;
    QList *list;
    QListIterator iter;

    QBinTree_init(&btree, dataCmp, NULL);
    for(index = 0; index <100; index++)
        number[index] = index+1;

    printf("Binary tree test\n");
    printf("Raw data:\n");
    for(counter = 0; counter < 20; counter++){
        index = counter + rand()%(100-counter);
        tmp = number[index];
        number[index] = number[counter];
        number[counter] = tmp;
        printf("%d  ", number[counter]);
        QBinTree_insert(&btree, &number[counter]);
    }

    list = QBinTree_prevOrder(&btree);
    printf("\nPrev Order:\n");
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
        printf("%d  ", *data);
    QList_clearAll(list);
    QMem_Free(list);

    list = QBinTree_inOrder(&btree);
    printf("\nIn Order:\n");
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
        printf("%d  ", *data);
    QList_clearAll(list);
    QMem_Free(list);

    list = QBinTree_postOrder(&btree);
    printf("\nPost Order:\n");
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
	    printf("%d  ", *data);
    QList_clearAll(list);
    QMem_Free(list);

    printf("\n\nDelete 5 nodes:\n");
    for(index = 0; index < 20; index++)
        num2[index] = number[index];
    for(counter = 0; counter < 5; counter++){
        index = counter + rand()%(20-counter);
        tmp = num2[index];
        num2[index] = num2[counter];
        num2[counter] = tmp;
        printf("%d  \n", num2[counter]);
        QBinTree_delete(&btree, &num2[counter]);
    }

    list = QBinTree_prevOrder(&btree);
    printf("\nPrev Order:\n");
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
        printf("%d  ", *data);
    QList_clearAll(list);
    QMem_Free(list);

    list = QBinTree_inOrder(&btree);
    printf("\nIn Order:\n");
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
        printf("%d  ", *data);
    QList_clearAll(list);
    QMem_Free(list);

    list = QBinTree_postOrder(&btree);
    printf("\nPost Order:\n");
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
	    printf("%d  ", *data);
    QList_clearAll(list);
    QMem_Free(list);

    QBinTree_clearAll(&btree);
}

void avltreetest()
{
    int number[100], num2[40];
    int index, counter, tmp;
    int *data;
    QAVLTree avltree;
    QList *list;
    QListIterator iter;

    QAVLTree_init(&avltree, dataCmp, NULL);
    for(index = 0; index <100; index++)
        number[index] = index+1;

    printf("\n\nAVL tree test\n");
    printf("Raw data:\n");
    for(counter = 0; counter < 20; counter++){//guarantee no same value node
        index = counter + rand()%(100-counter);
        tmp = number[index];
        number[index] = number[counter];
        number[counter] = tmp;
        printf("%d  ", number[counter]);
        QAVLTree_insert(&avltree, &number[counter]);
    }

    list = QAVLTree_prevOrder(&avltree);
    printf("\nPrev Order:\n");
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
        printf("%d  ", *data);
    QList_clearAll(list);
    QMem_Free(list);

    list = QAVLTree_inOrder(&avltree);
    printf("\nIn Order:\n");
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
        printf("%d  ", *data);
    QList_clearAll(list);
    QMem_Free(list);

    list = QAVLTree_postOrder(&avltree);
    printf("\nPost Order:\n");
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
	    printf("%d  ", *data);
    QList_clearAll(list);
    QMem_Free(list);

    //then we delete 5 nodes
    printf("\n\nDelete 5 nodes:\n");
    for(index = 0; index < 20; index++)
        num2[index] = number[index];
    for(counter = 0; counter < 5; counter++){//guarantee no same value
        index = counter + rand()%(20-counter);
        tmp = num2[index];
        num2[index] = num2[counter];
        num2[counter] = tmp;
        printf("%d  ", num2[counter]);
        QAVLTree_delete(&avltree, &num2[counter]);
    }
    list = QAVLTree_prevOrder(&avltree);
    printf("\nPrev Order:\n");
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
        printf("%d  ", *data);
    QList_clearAll(list);
    QMem_Free(list);

    list = QAVLTree_inOrder(&avltree);
    printf("\nIn Order:\n");
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
        printf("%d  ", *data);
    QList_clearAll(list);
    QMem_Free(list);

    list = QAVLTree_postOrder(&avltree);
    printf("\nPost Order:\n");
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
        printf("%d  ", *data);
    QList_clearAll(list);
    QMem_Free(list);

    QAVLTree_clearAll(&avltree);
}

void rbtreetest()
{
    int number2[20] = {12, 1, 9, 2, 0, 11, 7, 19, 4, 15,
                      18, 5, 14, 13, 10, 16, 6, 3, 8, 17};
    int number[100];
    int index, counter, tmp;
    QRBTree rbtree;
    QList *list;
    QListIterator iter;
    void *data;

    QRBTree_init(&rbtree, dataCmp, NULL);
    for(index = 0; index <100; index++)
        number[index] = index+1;

//    for(index = 0; index < 20; index++)
 //       QRBTree_insert(&rbtree, &number2[index]);
    printf("\n\nRedBlack tree test\n");
    printf("Raw data:\n");
    for(counter = 0; counter < 20; counter++){//guarantee no same value node
        index = counter + rand()%(100-counter);
        tmp = number[index];
        number[index] = number[counter];
        number[counter] = tmp;
        printf("%2d  ", number[counter]);
        QRBTree_insert(&rbtree, &number[counter]);
    }

    printf("\nPrev order:\n");
    list = QRBTree_prevOrder(&rbtree);
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
        printf("%2d  ", *(int*)data);
    QList_clearAll(list);
    QMem_Free(list);
    printf("\n");

    printf("\nIn order:\n");
    list = QRBTree_inOrder(&rbtree);
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
        printf("%2d  ", *(int*)data);
    QList_clearAll(list);
    QMem_Free(list);

    printf("\nPost order:\n");
    list = QRBTree_postOrder(&rbtree);
    QListIterReset(&iter, list);
    while((data = (int*)QListIterNext(&iter)) != NULL)
        printf("%2d  ", *(int*)data);
    QList_clearAll(list);
    QMem_Free(list);

    printf("\nDelete node from RBTree.\n");
    for(index = 0; index < 20; index++){
        QRBTree_delete(&rbtree, &number[index]);
        list = QRBTree_prevOrder(&rbtree);
        QListIterReset(&iter, list);
        while((data = (int*)QListIterNext(&iter)) != NULL)
            printf("%2d  ", *(int*)data);
       printf("\n");
       QList_clearAll(list);
       QMem_Free(list);
    }
    QRBTree_clearAll(&rbtree);
}

int main()
{
    //srand(time(0));//for test
    bintreetest();
    avltreetest();
    rbtreetest();
    QMemDumpInfo(0);
    return 0;
}
