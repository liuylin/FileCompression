#pragma once
#include"heap.h"
#include<functional>

template<class W>//Ȩֵ
struct HuffumanTreeNode
{
	HuffumanTreeNode<W>* _left;
	HuffumanTreeNode<W>* _right;
	HuffumanTreeNode<W>* _parent;

	W _w;//Ȩֵ

	//W��һ�������ͣ��������Զ������ͣ�����Ҫ������
	HuffumanTreeNode(const W& w)
		:_w(w)
		, _left(NULL)
		, _right(NULL)
		, _parent(NULL)//����������
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

	HuffumanTree(W* a, size_t n,const W& invalid)//����Ȩֵ������
	{
		//̰���㷨ѡ��С�ģ���С�ѣ�//Ҳ����ʹ�ÿ���������ȼ�����
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
				minHeap.Push(new Node(a[i]));//�������Ԫ�ض���Ѳ���С��
		}
		//��size >1 ʱ�����е�Ԫ�ض�Ҫѡ����С��������Ӻ������
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

			minHeap.Push(parent);//��ӵĽ��Ҫ�ٷŻ�ȥ
		}
		_root = minHeap.Top();
	}

	//�ͷŽڵ�
	~HuffumanTree()
	{
		Destroy(_root);
		_root = NULL;
	}

	//���������ͷ�
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
	HuffumanTree(const HuffumanTree<W>&);//������
	HuffumanTree<W>& operator=(const HuffumanTree<W>&);
	Node* _root;
};

void TestHuffumanTree()
{
	int a[] = { 1, 2, 3, 4};
	HuffumanTree<int> t(a, sizeof(a) / sizeof(a[0]),0);
}