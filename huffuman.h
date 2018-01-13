#pragma once
#include"heap.h"
#include<functional>

template<class W>//权值
struct HuffumanTreeNode
{
	HuffumanTreeNode<W>* _left;
	HuffumanTreeNode<W>* _right;
	HuffumanTreeNode<W>* _parent;

	W _w;//权值

	//W不一定是整型，可能是自定义类型，所以要给引用
	HuffumanTreeNode(const W& w)
		:_w(w)
		, _left(NULL)
		, _right(NULL)
		, _parent(NULL)//构造三叉连
	{}
};

template<class W>
class HuffumanTree
{
public:
	typedef HuffumanTreeNode<W> Node;

	HuffumanTree()
		:_root(NULL)
	{}

	HuffumanTree(W* a, size_t n,const W& invalid)//根据权值构建树
	{
		//贪心算法选最小的（建小堆）//也可以使用库里面的优先级队列
		struct NodeCompare
		{
			bool operator()(Node* l, Node* r)
			{
				return l->_w < r->_w;
			}
		};
		Heap<Node*,NodeCompare> minHeap;
		for (size_t i = 0; i < n; ++i)
		{
			if (a[i] != invalid)
				minHeap.Push(new Node(a[i]));//将数组的元素都入堆并建小堆
		}
		//当size >1 时，堆中的元素都要选出最小的两个相加后再入堆
		while (minHeap.Size() > 1)
		{
			Node* left = minHeap.Top();
			minHeap.Pop();
			Node* right = minHeap.Top();
			minHeap.Pop();

			Node* parent = new Node(left->_w + right->_w);
			parent->_left = left;
			parent->_right = right;
			left->_parent = parent;
			right->_parent = parent;

			minHeap.Push(parent);//相加的结果要再放回去
		}
		_root = minHeap.Top();
	}

	//释放节点
	~HuffumanTree()
	{
		Destroy(_root);
		_root = NULL;
	}

	//二叉树的释放
	void Destroy(Node* root)
	{
		if (root == NULL)
			return;
		Destroy(root->_left);
		Destroy(root->_right);
		delete root;
	}

	Node* GetRoot()
	{
		return _root;
	}

protected:
	HuffumanTree(const HuffumanTree<W>&);//防拷贝
	HuffumanTree<W>& operator=(const HuffumanTree<W>&);
	Node* _root;
};

void TestHuffumanTree()
{
	int a[] = { 1, 2, 3, 4};
	HuffumanTree<int> t(a, sizeof(a) / sizeof(a[0]),0);
}