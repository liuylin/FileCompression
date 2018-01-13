#pragma once
#include<vector>  

template<class T>
struct Greater
{
	bool operator()(const T& a, const T& b)
	{
		return a > b;
	}
};

template<class T>
struct Less
{
	bool operator()(const T& s, const T& b)
	{
		return a < b;
	}
};

template<class T, class Compare>
class Heap
{
public:
	Heap()
	{}

	Heap(T* a, int n)
	{
		_a.reserve(n);//增容（可以直接拷贝数据）  
		for (int i = 0; i < n; i++)
		{
			_a.push_back(a[i]);
		}
		//调整成堆  
		for (int j = (_a.size() - 2) / 2; j >= 0; --j)
		{
			//向下调整  
			_AjustDown(j);
		}
	}

	void Push(const T& x)
	{
		_a.push_back(x);
		_AjustUp(_a.size() - 1);
	}

	void Pop()
	{
		assert(!_a.empty());
		swap(_a[0], _a[_a.size() - 1]);
		_a.pop_back();
		_AjustDown(0);
	}


	T& Top()
	{
		assert(!_a.empty());
		return _a[0];
	}

	size_t Size()
	{
		return _a.size();
	}

	bool Empty()
	{
		return _a.empty();
	}
protected:
	//push_heap算法：向上调整
	void _AjustUp(int child)
	{
		assert(!_a.empty());
		int parent = (child - 1) >> 1;

		while (child>0)
		{
			//如果孩子节点的值大于父节点的值  
			Compare com;
			if (com(_a[child], _a[parent]))
			{
				swap(_a[child], _a[parent]);
				child = parent;
				parent = (child - 1) >> 1;
			}
			else
			{
				break;
			}
		}
	}
protected:
	//pop_heap:向下调整算法
	void _AjustDown(int root)
	{
		int parent = root;
		int child = parent * 2 + 1;
		while (child < _a.size())
		{
			Compare com;
			if ((child + 1 < _a.size()) && (_a[child + 1] > _a[child]))
			{
				++child;
			}
			if (com(_a[child], _a[parent]))
			{
				swap(_a[child], _a[parent]);
				parent = child;
				child = parent * 2 + 1;
			}
			else
			{
				break;
			}
		}
	}
private:
	vector<T> _a;
};

//void Test()
//{
//	int a[] = { 1, 6, 8, 2, 3, 5, 7, 4, 9 };
//	int len = sizeof(a) / sizeof(a[0]);
//	Heap<int, Greater<int>>  h(a, len);
//	h.Top();
//	h.Push(25);
//	h.Pop();
//	while (!h.Empty())
//	{
//		cout << h.Top() << " ";
//		h.Pop();
//	}
//	cout << endl;
//}