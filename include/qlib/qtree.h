//
/* A Red-Black tree is a binary search tree with one extra bit of storage per node:
 * its color, which can be either RED or BLACK. By constraining the node colors on 
 * any simple path from the root to a leaf, red-black trees ensure that no such path
 * is more than twice as long as any other, so that the tree is approximately balanced.
 * Each node of the tree now contains the attributes [color, key, left, right and p(parent).] 
 * If a child or the parent of a node does not exist, the corresponding pointer attribute
 * of the node contains the value NIL(a certain node stands for null). We shall regard
 * these NILS as being pointers to leaves(external nodes) of the binary search tree and
 * the normal, key-bearing nodes as being internal nodes of the tree.
 * A red-black tree is a binary tree that satisfies the following red-black properties:
 * 1. Every node is either red or black.
 * 2. The root is black.
 * 3. Every leaf(NIL) is black.
 * 4. If a node is red, then both its children are black.
 * 5. For each node, all simple paths from the node to descendant leaves contain the same
 *    number of black nodes.
 *
 * To make RBTree preserve the red-black properties, we change the pointer structure through
 * rotation. There are two kinds of rotations, actually they are almost the same when you
 * exchange the right and left pointer. Rotation doesn't change the height of the tree or
 * the local tree, but it change the position of red nodes and black nodes. This is very 
 * important to red black tree.
 *
 * Red-Black tree insert
 * When we insert a node, the new node(named z here) always colored with RED. Then we have to
 * do something to make sure the new tree is still red-black tree. We have to deal with 3
 * different cases to fix up the tree. We just talk about when p[z] is the left child of p[p[z]].
 *
 * RB-INSERT-FIXUP(T, z)
 * while color[p[z]] = RED
 *   do if p[z] = left[p[p[z]]]
 *        y <- right[p[p[z]]] //y is z's uncle, p[z]'s brother
 *        if color[y] = RED //Case 1
 *           color[p[z]] <- BLACK
 *           color[y] <- BLACK
 *           color[p[p[z]]] <- RED
 *           z <- p[p[z]] //iterative here
 *        else
 *           if z = right[p[z]] //Case 2
 *              z <- p[z]
 *              LEFT-ROTATE(T, z)
 *           color[p[z]] <- BLACK //Case 3
 *           color[p[p[z]]] <- RED
 *           RIGHT-ROTATE(T, p[p[z]])
 *      else(same as then clause with "right" and "left" exchange)
 * color[T->root] <- BLACK // the end of fix up
 *
 * Case 1: z's uncle is RED
 * Case 2: z's uncle is BLACK, but z is the right child of its parent
 * Case 3: z's uncle is BLACK, but z is the left child of its parent
 * In Case 1 and Case 3, we should change the color of some nodes.
 * In Case 2, z, p[z] and p[p[z]] are in the different direction
 * In Case 3, z, p[z] and p[p[z]] are in the same direction
 */

#ifndef _QLIB_TREE_H_
#define _QLIB_TREE_H_

#include "qgeneral.h"
#include "qlist.h"
#include "qstack.h"

//RIGHT is 0, LEFT is 1,  we can benefit from it when we do insert and delete
#define RIGHT  0
#define LEFT   1
#define BINARY 2

#define BLACK  0
#define RED    1

typedef struct QBinTree_T  QBinTree;
typedef struct QBinTree_T  QAVLTree; //AVL tree is also a kind of binary tree
typedef struct QTreeNode_T QTreeNode;
typedef struct QRBNode_T   QRBNode;
typedef struct QRBTree_T   QRBTree;
typedef void*  TreeData;

struct QTreeNode_T{
    QTreeNode *m_link[BINARY];
    TreeData   m_data;
};

struct QRBNode_T{ //QRBNode is just QTreeNode with parent and color
    QRBNode   *m_link[BINARY];
    TreeData   m_data;
    QRBNode   *m_parent;
    int        m_color;
};

struct QBinTree_T{
    QTreeNode       m_head;
    unsigned int    m_counter;
    DataCompFunc    dataCompare;
    DataDestroyFunc dataDestroy;
};

struct QRBTree_T{
    QRBNode         m_head;
    QRBNode         m_nullNode;
    unsigned int    m_counter;
    DataCompFunc    dataCompare;
    DataDestroyFunc dataDestroy;
};

void   QBinTree_init(QBinTree *tree, DataCompFunc cmpFunc,
                           DataDestroyFunc desFunc);
void*  QBinTree_find(QBinTree *tree, void *data);
int    QBinTree_insert(QBinTree *tree, void *data);
int    QBinTree_delete(QBinTree *tree, void *data);
int    QBinTree_getCount(QBinTree *tree);
void   QBinTree_clearAll(QBinTree *tree);
QList* QBinTree_prevOrder(QBinTree *tree);
QList* QBinTree_inOrder(QBinTree *tree);
QList* QBinTree_postOrder(QBinTree *tree);

void   QAVLTree_init(QAVLTree *tree, DataCompFunc cmpFunc,
                     DataDestroyFunc descFunc);
int    QAVLTree_insert(QAVLTree *tree, void *data);
void   QAVLTree_clearAll(QAVLTree *tree);
int    QAVLTree_delete(QAVLTree *tree, void *data);
QList* QAVLTree_prevOrder(QAVLTree *tree);
QList* QAVLTree_inOrder(QAVLTree *tree);
QList* QAVLTree_postOrder(QAVLTree *tree);

void   QRBTree_init(QRBTree *tree, DataCompFunc compare,
                    DataDestroyFunc destroy);
int    QRBTree_insert(QRBTree *tree, void *data);
int    QRBTree_delete(QRBTree *tree, void *data);
void   QRBTree_clearAll(QRBTree * tree);
QList* QRBTree_prevOrder(QRBTree *tree);
QList* QRBTree_inOrder(QRBTree *tree);
QList* QRBTree_postOrder(QRBTree *tree);


#endif //!_QLIB_TREE_H_
