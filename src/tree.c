
#include <assert.h> //assert()
#include <stdio.h> //for NULL
#include "qlib/qmemory.h"
#include "qlib/qtree.h"

#define NodeColor(x)  ((x)->m_color)
#define NodeParent(x) ((x)->m_parent)
#define Reverse(x)    ((~x)&0x1)

static QList* _TreeToListPrevOrder(QTreeNode *root, void* nullNode)
{
    QList     *list;
    QStack     stack;
    QTreeNode *node;

    list = (QList*)QMem_Malloc(sizeof(QList));
    assert(list);
    QList_init(list, NULL, NULL);
    QStack_init(&stack);

    if(root != nullNode)
        QStack_push(&stack, root);
    while(!QStack_empty(&stack))
    {
        node = QStack_pop(&stack);
        QList_addTail(list, node->m_data);
        if(node->m_link[RIGHT] != nullNode)
            QStack_push(&stack, node->m_link[RIGHT]);
        if(node->m_link[LEFT] != nullNode)
            QStack_push(&stack, node->m_link[LEFT]);
    }

    return list;
}

static QList* _TreeToListInOrder(QTreeNode *root, void *nullNode)
{
    QList  *list;
    QStack  stack;
    QTreeNode  *node;

    list = (QList*)QMem_Malloc(sizeof(QList));
    assert(list);
    QList_init(list, NULL, NULL);
    QStack_init(&stack);

    node = root;
    do{
        if(node != nullNode)
        {
            QStack_push(&stack, node);
            node = node->m_link[LEFT];
        }
        else if(!QStack_empty(&stack))
        {
            node = QStack_pop(&stack);
            QList_addTail(list, node->m_data);
            node = node->m_link[RIGHT];
        }
		else//node == NULL && stack is empty
            break;
    }while(1);

    return list;
}

static QList* _TreeToListPostOrder(QTreeNode *root, void *nullNode)
{
    QList     *list;
    QStack     stack;
    QTreeNode *node, *nr;

    list = (QList*)QMem_Malloc(sizeof(QList));
    assert(list);
    QList_init(list, NULL, NULL);
    QStack_init(&stack);

    if(root == nullNode)
        return list;//list has 0 elements

    node = root;
    do{
        while(node != nullNode)
        {
            if(node->m_link[RIGHT] != nullNode)
                QStack_push(&stack, node->m_link[RIGHT]);
            QStack_push(&stack, node);
            node = node->m_link[LEFT];
        }
        node = QStack_pop(&stack);
        nr = node->m_link[RIGHT];
        if(nr != nullNode && QStack_top(&stack) == nr)
        {
            QStack_pop(&stack);
            QStack_push(&stack, node);
            node = nr;
        }
        else
        {
            QList_addTail(list, node->m_data);
            node = nullNode;
        }
    }while(!QStack_empty(&stack));

    return list;
}

static QTreeNode* _NodeCreate(void *data)
{
    QTreeNode *node;

    node = (QTreeNode*)QMem_Malloc(sizeof(QTreeNode));
    assert(node);
    node->m_link[LEFT]  = NULL;
    node->m_link[RIGHT] = NULL;
    node->m_data = data;

    return node;
}

//Post order traverse, delete one by one
static void _TreeFastClear(QTreeNode *root, void *nullNode, DataDestroyFunc destroy)
{
    QStack     stack;
    QTreeNode *node, *nr;

    QStack_init(&stack);
    if(root == nullNode)
        return;

    node = root;
    do{
        while(node != nullNode)
        {
            if(node->m_link[RIGHT] != nullNode)
                QStack_push(&stack, node->m_link[RIGHT]);
            QStack_push(&stack, node);
            node = node->m_link[LEFT];
        }
        node = QStack_pop(&stack);
        nr = node->m_link[RIGHT];
        if(nr != nullNode && QStack_top(&stack) == nr)
        {
            QStack_pop(&stack);
            QStack_push(&stack, node);
            node = nr;
        }
        else
        {
            if(destroy != NULL)
                destroy(node->m_data);
            QMem_Free(node);
            node = nullNode;
        }
    }while(!QStack_empty(&stack));
}

void QBinTree_init(QBinTree *tree, DataCompFunc cmpFunc, DataDestroyFunc desFunc)
{
    assert(tree && cmpFunc);//desFunc can be NULL

    tree->m_head.m_link[LEFT] = NULL;
    tree->m_head.m_link[RIGHT] = NULL;
    tree->m_head.m_data = NULL;
    tree->m_counter = 0;
    tree->dataCompare = cmpFunc;
    tree->dataDestroy = desFunc;
}

//When do we need this function??? Puzzling
void* QBinTree_find(QBinTree *tree, void *data)
{
    int res;
    QTreeNode *node;
    assert(tree && data);

    node = tree->m_head.m_link[RIGHT]; // head->right points to the root node
    while(node != NULL)
    {
        res = tree->dataCompare(data, node->m_data);
        if(res == 0)
            break;
        res = res < 0; //data < node->m_data, res = LEFT
        node = node->m_link[res];
    }

    return node;
}

int QBinTree_insert(QBinTree *tree, void *data)
{
    int res;
    QTreeNode *node, *prev;
    assert(tree && data);

    prev = &tree->m_head;
    node = tree->m_head.m_link[RIGHT];
    res = RIGHT;
    while(node != NULL)
    {
        prev = node;
        res = tree->dataCompare(data, node->m_data);
        if(res == 0)
            return QLIB_ERR_DUPLICATE;
        res = res < 0; //when data > node->m_data, res should be 0
        node = node->m_link[res];
    };

    //no duplicate node
    node = _NodeCreate(data);
    prev->m_link[res] = node;
    tree->m_counter++;

    return QLIB_SUCCEEDED;
}

//
/* There are three different cases when we want to delete a node from binary tree.
 * Suppose s is the node to be deleted, f is s's parent, f->right = s. Actually,
 * no matter s is f's left or right child, the operation we need to do are all the
 * same.
 * Case 1: s has no right child
 *   f->right = s->left;
 *   delete s;
 * Case 2: s has right child, but s->right->left is NULL
 *   s->right->left = s->left;
 *   f->right = s->right;
 *   delete s;
 * Case 3: s has right child and s->right->left is NOT NULL
 *   get s->right's most left child and its parent, each of them
 *   named s->right->most_left and s->right->most_left_parent
 *   (notice that, s->right->left_most has no left child now)
 *   s->right->most_left_parent->left = s->right->most_left->right
 *   s->right->most_left->right = s->right;
 *   s->right->most_left->left = s->left;
 *   f->right = s->right->most_left;
 *   delete s;
 */
int QBinTree_delete(QBinTree *tree, void *data)
{
    int res;
    QTreeNode *node, *prev, *nr;
    QTreeNode *lm, *lmp;
    assert(tree && data);

    prev = &tree->m_head;
    node = tree->m_head.m_link[RIGHT];
    res = RIGHT;
    while(node != NULL)
    {
        res = tree->dataCompare(data, node->m_data);
        if(res == 0)
        {
            if(prev != &tree->m_head)
            {
                res = tree->dataCompare(data, prev->m_data);
                res = res < 0;
            }
            break;
        }
        res = res < 0; //data < node->m_data, res = LEFT
        prev = node;
        node = node->m_link[res];
    };

    if(node == NULL)
        return QLIB_ERR_NOTFOUND;

    nr = node->m_link[RIGHT];
    if(nr == NULL)//Case 1, node has no right child
        prev->m_link[res] = node->m_link[LEFT];
    else if(nr->m_link[LEFT] == NULL)//Case 2
    {
        nr->m_link[LEFT] = node->m_link[LEFT];
        prev->m_link[res] = nr;
    }
    else// Case 3
    {
        lmp = nr;
        lm = nr->m_link[LEFT];
        while(lm->m_link[LEFT] != NULL)
            lmp = lm, lm = lm->m_link[LEFT];
        
        lmp->m_link[LEFT] = lm->m_link[RIGHT];//lm->left is NULL, lmp->left = lm
        lm->m_link[LEFT] = node->m_link[LEFT];
        lm->m_link[RIGHT] = node->m_link[RIGHT];
        prev->m_link[res] = lm;
    }

    tree->m_counter--;
    if(tree->dataDestroy != NULL)
        tree->dataDestroy(node->m_data);
    QMem_Free(node);
    return QLIB_SUCCEEDED;
}

void QBinTree_clearAll(QBinTree *tree)
{
    if(tree == NULL)
        return;
    _TreeFastClear(tree->m_head.m_link[RIGHT], NULL, tree->dataDestroy);
    tree->m_counter = 0;
    tree->m_head.m_link[RIGHT] = NULL;
}

int QBinTree_getCount(QBinTree *tree)
{
    int count;
    assert(tree);

    count = tree->m_counter;
    return count;
}

QList* QBinTree_prevOrder(QBinTree *tree)
{
    QList *list;
    assert(tree);

    list = _TreeToListPrevOrder(tree->m_head.m_link[RIGHT], NULL);
    return list;
}

QList* QBinTree_inOrder(QBinTree *tree)
{
    QList *list;
    assert(tree);

    list = _TreeToListInOrder(tree->m_head.m_link[RIGHT], NULL);
    return list;
}

QList* QBinTree_postOrder(QBinTree *tree)
{
    QList *list;
    assert(tree);

    list = _TreeToListPostOrder(tree->m_head.m_link[RIGHT], NULL);
    return list;
}

static int _AVLHeight(QTreeNode *root)
{
    int leftH, rightH, maxH;
    if(root == NULL)
        return 0;

    leftH = _AVLHeight(root->m_link[LEFT]);
    rightH = _AVLHeight(root->m_link[RIGHT]);
    if(leftH > rightH)
        maxH = leftH;
    else
        maxH = rightH;

    return maxH + 1;
}

//The height of left child minus the height of right child
static int _AVLHeightMargin(QTreeNode *root)
{
    int leftH, rightH;

    leftH = _AVLHeight(root->m_link[LEFT]);
    rightH = _AVLHeight(root->m_link[RIGHT]);

    return leftH - rightH;
}

static QTreeNode* _AVLRotateLL(QTreeNode *node)
{
    QTreeNode *left;

    left = node->m_link[LEFT];
    node->m_link[LEFT] = left->m_link[RIGHT];
    left->m_link[RIGHT] = node;

    return left;
}

static QTreeNode* _AVLRotateLR(QTreeNode *node)
{
    QTreeNode *left, *leftRight;

    left = node->m_link[LEFT];
    leftRight = left->m_link[RIGHT];
    node->m_link[LEFT] = leftRight->m_link[RIGHT];
    left->m_link[RIGHT] = leftRight->m_link[LEFT];
    leftRight->m_link[RIGHT] = node;
    leftRight->m_link[LEFT] = left;

    return leftRight;
}

static QTreeNode* _AVLRotateRR(QTreeNode *node)
{
    QTreeNode *right;

    right = node->m_link[RIGHT];
    node->m_link[RIGHT] = right->m_link[LEFT];
    right->m_link[LEFT] = node;
    return right;
}

static QTreeNode* _AVLRotateRL(QTreeNode *node)
{
    QTreeNode *right, *rightLeft;

    right = node->m_link[RIGHT];
    rightLeft = right->m_link[LEFT];
    node->m_link[RIGHT] = rightLeft->m_link[LEFT];
    right->m_link[LEFT] = rightLeft->m_link[RIGHT];
    rightLeft->m_link[LEFT] = node;
    rightLeft->m_link[RIGHT] = right;
    return rightLeft;
}

static QTreeNode* _AVLBalance(QTreeNode *root)
{
    int margin;
    QTreeNode *node;

    if(root == NULL)
        return NULL;

    if(root->m_link[LEFT] != NULL)//Pay attention this: recursive balance
        root->m_link[LEFT] = _AVLBalance(root->m_link[LEFT]);
    if(root->m_link[RIGHT] != NULL)
        root->m_link[RIGHT] = _AVLBalance(root->m_link[RIGHT]);

    margin = _AVLHeightMargin(root);
    if(margin >= 2)//unbalance-left
    {
        if(_AVLHeightMargin(root->m_link[LEFT]) <= -1)//left-right
            node = _AVLRotateLR(root);
        else//left-left
            node = _AVLRotateLL(root);
    }
    else if(margin <= -2)//right child is heigher
    {
        if(_AVLHeightMargin(root->m_link[RIGHT]) >= 1)//right-left
            node= _AVLRotateRL(root);
        else//right-right
            node = _AVLRotateRR(root);
    }
    else //balanced state
        node = root;

    return node;
}

//Actually QAVLTree_init() is the same as QBinTree_init()
void QAVLTree_init(QAVLTree *tree, DataCompFunc compare, DataDestroyFunc destroy)
{
    QBinTree_init(tree, compare, destroy);
}

int QAVLTree_insert(QAVLTree *tree, void *data)
{
    int res;
    QTreeNode *node, *pnode;
    assert(tree && data);

    res = RIGHT;
    pnode = &tree->m_head;
    node = tree->m_head.m_link[res];
    while(node != NULL)
    {
        res = tree->dataCompare(data, node->m_data);
        if(res == 0)
            return QLIB_ERR_DUPLICATE;
        res = res < 0;
        pnode = node;
        node = node->m_link[res];
    }
    node = _NodeCreate(data);
    pnode->m_link[res] = node;
    tree->m_counter++;
    tree->m_head.m_link[RIGHT] = _AVLBalance(tree->m_head.m_link[RIGHT]);

    return QLIB_SUCCEEDED;
}

//
/* Qlib divide delete operation into 2 steps. Step 1, delete the node from the AVL
 * tree, Step 2, balance AVL tree again. So, we have to deal with 3 different cases
 * when we delete a node. Suppose node is which we want to delete, prev is the parent
 * of node.
 * Case 1:
 *   node has only left child. Delete node and set prev->m_link[res] = node->left;
 * Case 2:
 *   node has only right child. Delete node and set prev->m_link[res] = node->right;
 * Case 3:
 *   node has both left and right child. Suppose lm is the left most right child of node,
 *   also say the prev node of "node" in inorder travese, plmr is the parent of lmr.
 *   Now, if plmr == node(which means lm == node->left), we set node->left = lmr->left,
 *   then, node->data = lmr->data; if plmr != node, we set plmr->right = lmr->left,
 *   node->data = lmr->data;
 * After all of this, we balance the tree again.
 */
int QAVLTree_delete(QAVLTree *tree, void *data)
{
    int res;
    QTreeNode *node, *prev;
    QTreeNode *lmr, *plmr;//left most right, parent left most right
    assert(tree && data);

    res = RIGHT;
    prev = &tree->m_head;
    node = prev->m_link[res];

    while(node != NULL)
    {
        res = tree->dataCompare(data, node->m_data);
        if(res == 0)
        {
            if(prev != &tree->m_head)
            {
                res = tree->dataCompare(data, prev->m_data);
                res = res < 0;
            }
            break;
        }
        res = res < 0;
        prev = node;
        node = node->m_link[res];
    }

    if(node == NULL)
        return QLIB_ERR_NOTFOUND;

    if(tree->dataDestroy != NULL)
        tree->dataDestroy(node->m_data);
    if(node->m_link[RIGHT] == NULL)//only has left child
        prev->m_link[res] = node->m_link[LEFT];
    else if(node->m_link[LEFT] == NULL)//only has right child
        prev->m_link[res] = node->m_link[RIGHT];
    else//has both left and right child
    {
        plmr = node;
        lmr = node->m_link[LEFT];
        while(lmr->m_link[RIGHT] != NULL)
        {
            plmr  = lmr;
            lmr = lmr->m_link[RIGHT];
        }
        if(plmr  == node)
            node->m_link[LEFT] = lmr->m_link[LEFT];
        else
            plmr ->m_link[RIGHT] = lmr->m_link[LEFT];
        node->m_data = lmr->m_data;
        node = lmr;
    }

    QMem_Free(node);
    tree->m_counter--;
    tree->m_head.m_link[RIGHT] = _AVLBalance(tree->m_head.m_link[RIGHT]);
    return QLIB_SUCCEEDED;
}

void QAVLTree_clearAll(QAVLTree *tree)
{
    if(tree == NULL)
        return;
    _TreeFastClear(tree->m_head.m_link[RIGHT], NULL, tree->dataDestroy);
    tree->m_counter = 0;
    tree->m_head.m_link[RIGHT] = NULL;
}

QList* QAVLTree_inOrder(QAVLTree *tree)
{
    QList *list;
    assert(tree);

    list = _TreeToListInOrder(tree->m_head.m_link[RIGHT], NULL);
    return list;
}

QList* QAVLTree_prevOrder(QAVLTree *tree)
{
    QList *list;
    assert(tree);

    list = _TreeToListPrevOrder(tree->m_head.m_link[RIGHT], NULL);
    return list;
}

QList* QAVLTree_postOrder(QAVLTree *tree)
{
    QList *list;
    assert(tree);

    list = _TreeToListPostOrder(tree->m_head.m_link[RIGHT], NULL);
    return list;
}

static QRBNode* _RBNodeCreate(void *data, QRBNode *nullNode)
{
    QRBNode *node;

    node = (QRBNode*)QMem_Malloc(sizeof(QRBNode));
    assert(node);
    node->m_link[LEFT] = nullNode;
    node->m_link[RIGHT] = nullNode;
    node->m_parent = nullNode;
    node->m_color = RED;
    node->m_data = data;
    return node;
}

static int _GetDirection(QRBNode *node, QRBNode *parent)
{
    if(parent->m_link[LEFT] == node)
        return LEFT;
    else if(parent->m_link[RIGHT] == node)
        return RIGHT;
    else
        assert(0);
}

static void _RBRotate(QRBTree *tree, QRBNode *node, int direc)
{
    int left, right, res;
    QRBNode *child;

    //actually, left rotate is the same as right rotate
    left = (direc == LEFT ? LEFT : RIGHT);
    right = Reverse(left);

    child = node->m_link[right];
    node->m_link[right] = child->m_link[left];
    if(child->m_link[left] != &tree->m_nullNode)
        NodeParent(child->m_link[left]) = node;
    NodeParent(child) = NodeParent(node);
    if(NodeParent(node) == &tree->m_nullNode)
        tree->m_head.m_link[RIGHT] = child;
    else
    {
        res = _GetDirection(node, NodeParent(node));
        NodeParent(node)->m_link[res] = child;
    }
    child->m_link[left] = node;
    NodeParent(node) = child;
}

static void _RBBalance(QRBTree *tree, QRBNode *node)
{
    int res;
    QRBNode *uncle;

    while(NodeColor(node->m_parent) == RED)
    {
        res = _GetDirection(NodeParent(node), NodeParent(node->m_parent));
        res = Reverse(res); //we need to operate different child
        uncle = NodeParent(node->m_parent)->m_link[res];
        if(uncle->m_color == RED)
        {
            NodeParent(node)->m_color = BLACK;
            uncle->m_color = BLACK;
            NodeParent(node->m_parent)->m_color = RED;
            node = NodeParent(node->m_parent);
        }
        else
        {
            if(res == _GetDirection(node, NodeParent(node)))
            {
                node = NodeParent(node);
	            if(res == RIGHT)
                    _RBRotate(tree, node, LEFT);
	            else //res must be LEFT here
	                _RBRotate(tree, node, RIGHT);
            }
            NodeParent(node)->m_color = BLACK;
            NodeParent(node->m_parent)->m_color = RED;
            _RBRotate(tree, NodeParent(node->m_parent), res);
        }
    }
    tree->m_head.m_link[RIGHT]->m_color = BLACK;
}

void QRBTree_init(QRBTree *tree, DataCompFunc compare, DataDestroyFunc destroy)
{
    assert(tree && compare);

    tree->m_nullNode.m_link[LEFT] = NULL; //all null node points to nullNode
    tree->m_nullNode.m_link[RIGHT] = NULL;
    tree->m_nullNode.m_color = BLACK;
    tree->m_nullNode.m_parent = NULL;
    tree->m_nullNode.m_data = NULL;
    tree->m_head.m_link[LEFT] = NULL;
    tree->m_head.m_link[RIGHT] = &tree->m_nullNode;
    tree->m_head.m_parent = &tree->m_nullNode;
    tree->m_head.m_color = RED; //non sense here
    tree->m_head.m_data = NULL;
    tree->m_counter = 0;
    tree->dataCompare = compare;
    tree->dataDestroy = destroy;
}

int QRBTree_insert(QRBTree *tree, void *data)
{
    int res;
    QRBNode *node, *prev;
    assert(tree && data);

    res = RIGHT;
    prev = &tree->m_head;
    node = prev->m_link[RIGHT];
    while(node != &tree->m_nullNode)
    {
        res = tree->dataCompare(data, node->m_data);
        if(res == 0)
            return QLIB_ERR_DUPLICATE;
        prev = node;
        res = res < 0;
        node = node->m_link[res];
    }

    node = _RBNodeCreate(data, &tree->m_nullNode);
    prev->m_link[res] = node;
    if(prev != &tree->m_head)//or, node->m_parent is nullNode
        node->m_parent = prev;
    tree->m_counter++;
    _RBBalance(tree, node);
    return QLIB_SUCCEEDED;
}

static void _RBFixUp(QRBTree *tree, QRBNode *check, QRBNode *parent)
{
    QRBNode *root, *node, *bro;
    QRBNode *p, *pb;//parent, parent of brother
    int res;

    node = check;
    root = tree->m_head.m_link[RIGHT];
	p = parent;
    while(node != root && node->m_color == BLACK)
    {
        res = _GetDirection(node, p);
        bro = p->m_link[Reverse(res)];//get node's brother
        pb = p;
        if(bro->m_color == RED)
        {
            bro->m_color = BLACK;
            p->m_color = RED;
            _RBRotate(tree, p, res);
            bro = p->m_link[Reverse(res)];//after rotate, get bro again, pb is still p
        }
        if(bro->m_link[LEFT]->m_color == BLACK
           && bro->m_link[RIGHT]->m_color == BLACK)
        {
           bro->m_color = RED;
		   node = p;
           p = node->m_parent;
        }
        else{
            if(bro->m_link[Reverse(res)]->m_color == BLACK)
            {
                bro->m_link[res]->m_color = BLACK;
                bro->m_color = RED;
                _RBRotate(tree, bro, Reverse(res));
                bro = p->m_link[Reverse(res)];
            }
            bro->m_color = p->m_color;
			p->m_color = BLACK;
            bro->m_link[Reverse(res)]->m_color = BLACK;
            _RBRotate(tree, p, res);
            node = root;
            p = &tree->m_head;
        }
    }
    node->m_color = BLACK;
}

int QRBTree_delete(QRBTree *tree, void *data)
{
    int res;
    QRBNode *node, *parent, *check;
    assert(tree && data);

    node = tree->m_head.m_link[RIGHT];
    while(node != &tree->m_nullNode)
    {
        res = tree->dataCompare(data, node->m_data);
        if(res == 0)
            break;
        res = res < 0;
        node = node->m_link[res];
    }

    if(node == &tree->m_nullNode)
        return QLIB_ERR_NOTFOUND;

    if(tree->dataDestroy != NULL)
        tree->dataDestroy(node->m_data);
    parent = NodeParent(node);
    if(node == tree->m_head.m_link[RIGHT])
        parent = &tree->m_head;
    res = _GetDirection(node, parent);
    if(node->m_link[LEFT] == &tree->m_nullNode)
    {
        check = node->m_link[RIGHT];
        parent->m_link[res] = check;
    }
    else if(node->m_link[RIGHT] == &tree->m_nullNode)
    {
        check = node->m_link[LEFT];
        parent->m_link[res] = check;
    }
    else//in this case, node has two childre, here we choose left most right
    {
        check = node->m_link[LEFT];//can't be nullNode here
        while(check->m_link[RIGHT] != &tree->m_nullNode)
            check = check->m_link[RIGHT];

        /*when check is node, check->left is the left son of node, or 
          check->left is the right son of check->parent*/
        res = check == node->m_link[LEFT] ? LEFT : RIGHT ;
        NodeParent(check)->m_link[res] = check->m_link[LEFT];		
        parent = NodeParent(check); //prev parent is node's parent
        node->m_data = check->m_data;
        node = check;
    }

    QMem_Free(node);
    tree->m_counter--;

    check = parent->m_link[res];
    if(check != &tree->m_nullNode)
        NodeParent(check) = parent;
    if(node->m_color == BLACK)
        _RBFixUp(tree, check, parent);

    return QLIB_SUCCEEDED;
}

void QRBTree_clearAll(QRBTree *tree)
{
    if(tree == NULL)
        return;
    _TreeFastClear((QTreeNode*)tree->m_head.m_link[RIGHT], &tree->m_nullNode,
                   tree->dataDestroy);
    tree->m_counter = 0;
    tree->m_head.m_link[RIGHT] = NULL;
}

QList* QRBTree_prevOrder(QRBTree *tree)
{
    QList *list;
    assert(tree);
    list = _TreeToListPrevOrder((QTreeNode*)tree->m_head.m_link[RIGHT],
                                &tree->m_nullNode);
    return list;
}

QList* QRBTree_inOrder(QRBTree *tree)
{
    QList *list;
    assert(tree);
    list = _TreeToListInOrder((QTreeNode*)tree->m_head.m_link[RIGHT],
                              &tree->m_nullNode);
    return list;
}

QList* QRBTree_postOrder(QRBTree *tree)
{
    QList *list;
    assert(tree);
    list = _TreeToListPostOrder((QTreeNode*)tree->m_head.m_link[RIGHT],
                            &tree->m_nullNode);
    return list;
}