#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;

// 定义Cell结构体，存储数据的类型、大小和值
typedef struct {
    char type[10];
    int size;
    void *value;
} Cell;

// 定义Row结构体，存储一行的Cell数据
typedef struct {
    Cell *cells;
    int cell_count;
} Row;

// 定义RowSet结构体，存储一组Row数据
typedef struct {
    Row *rows;
    int row_count;
} RowSet;

// 定义索引项结构体
typedef struct {
    int key; // 索引键，这里是数字值
    int rowIndex; // 行号
    int cellIndex; // 列号
} IndexItem;

// 定义AVLNode结构体，包含IndexItem类型的value成员
typedef struct AVLNode {
    IndexItem value; 
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;


// 获取节点的高度
int getHeight(AVLNode *node) {
    if (node == NULL)
        return 0;
    return node->height;
}

// 计算平衡因子
int getBalanceFactor(AVLNode *node) {
    if (node == NULL)
        return 0;
    return getHeight(node->left) - getHeight(node->right);
}

// 更新节点的高度
void updateHeight(AVLNode *node) {
    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
}

// 右旋操作
AVLNode *rotateRight(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeight(y);
    updateHeight(x);

    return x;
}

// 左旋操作
AVLNode *rotateLeft(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y;
}

// 插入节点
AVLNode *insertNode(AVLNode *node, int key, int rowIndex, int cellIndex) {
    if (node == NULL) {
        AVLNode *newNode = new AVLNode;
        newNode->value.key = key;
        newNode->value.rowIndex = rowIndex;
        newNode->value.cellIndex = cellIndex;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->height = 1;
        return newNode;
    }

    if (key < node->value.key)
        node->left = insertNode(node->left, key, rowIndex, cellIndex);
    else if (key > node->value.key||key == node->value.key)
        node->right = insertNode(node->right, key, rowIndex, cellIndex);
 
    updateHeight(node);

    int balanceFactor = getBalanceFactor(node);

    // 左左情况
    if (balanceFactor > 1 && key < node->left->value.key)
        return rotateRight(node);

    // 右右情况
    if (balanceFactor < -1 && key > node->right->value.key)
        return rotateLeft(node);

    // 左右情况
    if (balanceFactor > 1 && key > node->left->value.key) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // 右左情况
    if (balanceFactor < -1 && key < node->right->value.key) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

// 查找节点
AVLNode *searchNode(AVLNode *node, int key) {
    if (node == NULL || node->value.key == key)
        return node;

    if (key < node->value.key)
        return searchNode(node->left, key);
    else
        return searchNode(node->right, key);
}

// 定义AVL树结构体
typedef struct {
    AVLNode *root;
} IndexTree;

// 初始化索引树
void initIndexTree(IndexTree *tree) {
    tree->root = NULL;
}

// 查找索引项
IndexItem *searchIndexItem(IndexTree *tree, int key) {
    AVLNode *node = searchNode(tree->root, key);
    return node ? &node->value : NULL;
}

// 插入索引项
void insertIndexItem(IndexTree *tree, int key, int rowIndex, int cellIndex) {
    tree->root = insertNode(tree->root, key, rowIndex, cellIndex);
}

// 从RowSet中提取指定列的数据并创建索引的函数实现
void createIndexFromRowSet(RowSet *rowSet, IndexTree *indexTree, int columnIndex) {
    for (int i = 1; i < rowSet->row_count; i++) {
        Row *row = &rowSet->rows[i];
        if (row->cell_count > columnIndex) {
            char *valueStr = (char *)row->cells[columnIndex].value;
            int value = atoi(valueStr);
            insertIndexItem(indexTree, value, i, columnIndex);
        }
    }
}

// 计算AVL树中节点的数量
int countNodes(AVLNode *node) {
    if (node == NULL)
        return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}


// 中序遍历AVL树并输出节点值、行数和列数
void inOrderTraversal(AVLNode *node) {
    if (node == NULL)
        return;

    inOrderTraversal(node->left);
    // 输出节点值、行数和列数
    printf("Key: %d, Row: %d, Column: %d\n", node->value.key, node->value.rowIndex, node->value.cellIndex);
    inOrderTraversal(node->right);
}
typedef struct {
    int *indexes;
    int count;
} IndexResult;

// 中序遍历AVL树并返回匹配key的行号数组
int *inOrderTraversal1(AVLNode *node, int a, int rowcount, int *indexes, int &indexCount) {
    if (node == NULL)
        return NULL;

    inOrderTraversal1(node->left, a, rowcount, indexes, indexCount);

    if (node->value.key == a) {
        indexes[indexCount++] = node->value.rowIndex; // 保存行号
    }

    inOrderTraversal1(node->right, a, rowcount, indexes, indexCount);

    if (indexCount == 0) {
        return NULL; // 如果没有找到匹配的key，返回NULL
    } else {
        indexes[indexCount] = -1; // 在最后添加一个-1作为结束标志，用于判断数组的结束位置
        return indexes; // 返回保存行号的数组
    }
}


// 插入索引项到AVL树
void insertIndexItemToAVL(IndexTree *tree, IndexItem item) {
    tree->root = insertNode(tree->root, item.key, item.rowIndex, item.cellIndex);
}

// 释放AVL树中的所有节点
void freeAVLTree(AVLNode *node) {
    if (node == NULL)
        return;

    freeAVLTree(node->left);
    freeAVLTree(node->right);

    // 释放当前节点的内存
    delete node;
}

// 释放IndexTree占用的内存
void freeIndexTree(IndexTree *tree) {
    if (tree == NULL)
        return;

    // 释放树的根节点
    freeAVLTree(tree->root);

    // 重置树的根节点为NULL
    tree->root = NULL;
}

