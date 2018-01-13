#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#include<stdlib.h>
#include"huffuman.h"
#include<algorithm>
#include<string>
#include<stdio.h>
#define __DEBUG__

struct CharInfo
{
	char _ch;//出现的字符
	long long _count;//字符出现的次数
	string _code; //huffuman code

	CharInfo(long long count = 0)
		:_count(count)
	{}

	CharInfo operator +(const CharInfo& info)
	{
		CharInfo ret;
		ret = _count + info._count;
		return ret;
	}

	//比较的是次数 次数不等于0则入堆
	bool operator != (const CharInfo& info)
	{
		return _count != info._count;
	}

	bool operator >(const CharInfo& info)
	{
		return _count > info._count;
	}

	bool operator <(const CharInfo& info)
	{
		return _count < info._count;
	}
};

class FileCpmoress
{
	typedef HuffumanTreeNode<CharInfo> Node;
public:
	FileCpmoress()
	{
		//依次初始化
		for (size_t i = 0; i < 256; ++i)
		{
			//汉字的编码是两个字节 char是0-127
			//会发生数组越界，解决方法是将char强转为unsigned char，可表示范围为0~255。
			_infos[(unsigned char)i]._ch = i;//i既是下标也是Ascall码
			_infos[(unsigned char)i]._count = 0;//出现的次数
		}
	}

	//内部类
	struct Config
	{	
        char _ch;
	    long long _count;
    };

	//压缩
	//input.txt->input.txt.huffuman
	void Compress(const char* file)
	{
		assert(file);
		//1,统计字符出现的次数
		//文本方式打开，会对‘\n’进行特殊处理，
		//那如果这个字符本身就是'\n'.这就会出现问题，
		//所以使用二进制打开，特点：不进行任何处理，是什么就是什么。
		FILE* fout = fopen(file, "rb");
		assert(fout);
		char ch = fgetc(fout);
		//feof是C语言标准库函数，其原型在stdio.h中，
		//其功能是检测流上的文件结束符，如果文件结束，则返回非0值，
		//否则返回0，文件结束符只能被clear()清除。
		while (!feof(fout))
		{
			_infos[(unsigned char)ch]._count++;
			ch = fgetc(fout);
		}
		//用charInfo构建的树里面既有权值又有次数（为了得到huffuman编码）
		//出现次数大于0的才比较大小，所以需要重载次数比较的运算符

		//2,构建huffuman tree
		CharInfo invalid;//非法值
		invalid._count = 0;//出现次数0次的为非法值
		HuffumanTree<CharInfo> tree(_infos, 256, invalid);

		//3.生成huffuman code（叶子节点）
		string code;
		_GetHuffumanCode(tree.GetRoot(), code);


		//4，压缩（8位）
		//fseek设置文件指针的位置，相对于当前位置
		string compressFile = file;
		compressFile += ".huffuman";
		FILE* fin = fopen(compressFile.c_str(), "wb");
		assert(fin);

		////配置---解压缩是重建huffuman tree
		Config info;
		for (size_t i = 0; i < 256; ++i)
		{
			if (_infos[i]._count > 0)
			{

				info._ch = _infos[i]._ch;
				info._count = _infos[i]._count;
				fwrite(&info, sizeof(Config), 1, fin);
			}
		}
		////分隔  多写8个字节
		info._count = 0;
		fwrite(&info, sizeof(Config), 1, fin);

		//文件指针 偏移量 基准位置
		char value = 0;//8个位
		long long count = 0;
		fseek(fout, 0, SEEK_SET);
		ch = fgetc(fout);
		/*EOF的16进制为0xFF（十进制为 - 1），特用在文本文件中，
		因为在文本文件中数据是以ASCⅡ码值的形式存放，普通字符的ASCⅡ码的范围是32到127（十进制）
		与EOF不冲突，因此可以直接使用；但是在二进制文件中，数据有可能出现 - 1
		因此不能用EOF来作为二进制文件的结束标志，可以通过feof函数来判断。
		也就是说feof这个函数可用于判断文件是否结束(包括文本文件和二进制文件)。*/
		while (!feof(fout))//
		{
			string& code = _infos[(unsigned char)ch]._code;
			//用位运算 左移是往高位移与方向无关
			for (size_t i = 0; i < code.size(); ++i)
			{
				value <<= 1;
				if (code[i] == '0')
				{
					value |= 0;
				}
				else if (code[i] == '1')
				{
					value |= 1;
				}
				++count;
				if (count == 8)
				{
					fputc(value, fin);
					value = 0;
					count = 0;
				}
			}
			ch = fgetc(fout);
		}
		if (count != 0)
		{
			value <<= (8 - count);
			fputc(value, fin);
		}
		fclose(fin);
		fclose(fout);
	}
				

	//解压缩
	//input.txt.huffuman.txt->input.txt
	void Uncompress(const char* file)
	{
		//1.读取配置文件信息--字符出现的次数
		assert(file);
		string uncompressFile = file;
		size_t position = uncompressFile.rfind(".");//倒着找到.的位置
		assert(position != string::npos);//不存在

		uncompressFile = uncompressFile.substr(0, position);
		uncompressFile += ".unhuffuman";

		FILE* fout = fopen(file, "rb");//可以考虑用智能指针
		assert(fout);
		FILE* fin = fopen(uncompressFile.c_str(), "wb");
		assert(fin);

		Config info;
		while (1)
		{
			fread(&info, sizeof(Config), 1, fout);
			if (info._count == 0)
			{
				break;
			}
			else
			{
				_infos[(unsigned char)info._ch]._ch = info._ch;
				_infos[(unsigned char)info._ch]._count = info._count;
			}
		}

		//重建huffuman tree
		CharInfo invalid;
		invalid._count = 0;
		HuffumanTree<CharInfo> tree(_infos, 256, invalid);
		Node* root = tree.GetRoot();
		long long count = root->_w._count;


		//哈夫曼树的头结点的权值内的count存的是原来文件的字符个数

		//解压缩
		char ch = fgetc(fout);
		Node* cur = root;
		while (!feof(fout))
		{
			for (int pos = 7; pos >= 0; --pos)
			{
				if (ch &(1 << pos))
				{
					cur = cur->_right;
				}
				else
				{
					cur = cur->_left;
				}
				if (cur->_left == NULL && cur->_right == NULL)
				{
					fputc(cur->_w._ch, fin);
					cur = root;
					--count;
					if (count == 0)
					{
						break;
					}
				}
			}
			ch = fgetc(fout);
		}
		uncompressFile.erase(position, uncompressFile.size() - position + 1);//删除.之后的字符串
		fclose(fin);
		fclose(fout);
	}
protected:
	//void _GetHuffumanCode(Node* root)
	//{
	//	if (root == NULL)
	//		return;
	//	//叶子节点
	//	if (root->_left == NULL && root->_right == NULL)
	//	{
	//		//root是叶子节点，节点的权值_w是CharInfo类型，这个字符就是CharInfo中的ch
	//		//直接用引用，减少系统开销

	//		string& code = _infos[root->_w._ch]._code;
	//		Node*  cur = root;//叶子节点
	//		Node* parent = cur->_parent;
	//		//循环到parent为NULL就结束
	//		while (parent)
	//		{
	//			if (cur == parent->_left)
	//			{
	//				code.push_back('0');
	//			}
	//			else if (cur == parent->_right)
	//			{
	//				code.push_back('1');
	//			}
	//			cur = parent;
	//			parent = cur->_parent;
	//		}
	//		//此时为逆置的编码
	//		reverse(code.begin(), code.end());
	//		return;
	//	}
	//	_GetHuffumanCode(root->_left);
	//	_GetHuffumanCode(root->_right);
	//}

	bool ReadLine(FILE *fOut, string& line)
	{
		char ch = fgetc(fOut);
		if (feof(fOut))
			return false;
		while (!feof(fOut) && ch != '\n')
		{
			line += ch;
			ch = fgetc(fOut);
		}
		return true;
	}

	void _GetHuffumanCode(Node* root, string& code)
	{
		//遍历
		if (root == NULL)
			return;
		if (root->_left == NULL  && root->_right == NULL)
		{
			_infos[(unsigned char)root->_w._ch]._code = code;
			return;
		}
		_GetHuffumanCode(root->_left, code+'0');
		_GetHuffumanCode(root->_right, code+'1');
	}
private:
	CharInfo _infos[256];//根据Ascall码统计字符出现的次数
};

void TestFileCompess()
{
	/*FileCpmoress fc;
	fc.Compress("111.png");*/
	FileCpmoress f1;
	f1.Uncompress("111.png.huffuman");
}
