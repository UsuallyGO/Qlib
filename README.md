
The source code is a user level lib written by Maxwell(qinliok@126.com). I write it mainly for improving
my program skills and I don't think these code are good enough to be used in anyone's application.

All my work are only tested in windows(VS2010), MinGW(GCC) and linux(GCC) on X86 machine.

Now, it has implemented several dataStructs and algorithms:
1) Memory allocate management.
2) Basic struts as list, stack, hash(key is string, value can be type).
3) Binary search tree, AVL tree and RedBlack tree.
4) Compress and decompress algorithms, RLE and HUFFMAN.
5) Spinlock in both windows and linux, or more precisely, in VS compiler and GCC compiler.
6) Sort algorithm, include select/insert/buble/quick/shell sort algorithms, there should be more.
7) Graph algorithms, include DFS, BFS, Prim, Kruskal, Bell-man-Ford, Dijstra

There are still many works to do, perhaps they are LZ77 compress algorithm, splay tree, Map, Set....
If you'd like to do code review for me, please let me know, thanks a lot for you....

Supplement in 2016.1.17
Perhaps Qlib still needs vector.
Perhaps Qlib still needs QString
Perhaps need priority queue.
In C++ STL stack and queue are implemented on list.
Binary heap(maximum, minimum) can be implemented on vector.
Map and Set are should not implemented on list but on RB-Tree.
Perhaps more search and sort algorithms.
Perhaps some encrypted algorithms.
Perhaps some math algorithms ?

Just announce that the red-black is completely wrong in Qlib. So rewrite it in sometime.
This README.md is revised in 2016.11.14.