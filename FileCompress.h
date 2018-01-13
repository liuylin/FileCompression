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
	char _ch;//���ֵ��ַ�
	long long _count;//�ַ����ֵĴ���
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

	//�Ƚϵ��Ǵ��� ����������0�����
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
		//���γ�ʼ��
		for (size_t i = 0; i < 256; ++i)
		{
			//���ֵı����������ֽ� char��0-127
			//�ᷢ������Խ�磬��������ǽ�charǿתΪunsigned char���ɱ�ʾ��ΧΪ0~255��
			_infos[(unsigned char)i]._ch = i;//i�����±�Ҳ��Ascall��
			_infos[(unsigned char)i]._count = 0;//���ֵĴ���
		}
	}

	//�ڲ���
	struct Config
	{	
        char _ch;
	    long long _count;
    };

	//ѹ��
	//input.txt->input.txt.huffuman
	void Compress(const char* file)
	{
		assert(file);
		//1,ͳ���ַ����ֵĴ���
		//�ı���ʽ�򿪣���ԡ�\n���������⴦��
		//���������ַ��������'\n'.��ͻ�������⣬
		//����ʹ�ö����ƴ򿪣��ص㣺�������κδ�����ʲô����ʲô��
		FILE* fout = fopen(file, "rb");
		assert(fout);
		char ch = fgetc(fout);
		//feof��C���Ա�׼�⺯������ԭ����stdio.h�У�
		//�书���Ǽ�����ϵ��ļ�������������ļ��������򷵻ط�0ֵ��
		//���򷵻�0���ļ�������ֻ�ܱ�clear()�����
		while (!feof(fout))
		{
			_infos[(unsigned char)ch]._count++;
			ch = fgetc(fout);
		}
		//��charInfo���������������Ȩֵ���д�����Ϊ�˵õ�huffuman���룩
		//���ִ�������0�ĲűȽϴ�С��������Ҫ���ش����Ƚϵ������

		//2,����huffuman tree
		CharInfo invalid;//�Ƿ�ֵ
		invalid._count = 0;//���ִ���0�ε�Ϊ�Ƿ�ֵ
		HuffumanTree<CharInfo> tree(_infos, 256, invalid);

		//3.����huffuman code��Ҷ�ӽڵ㣩
		string code;
		_GetHuffumanCode(tree.GetRoot(), code);


		//4��ѹ����8λ��
		//fseek�����ļ�ָ���λ�ã�����ڵ�ǰλ��
		string compressFile = file;
		compressFile += ".huffuman";
		FILE* fin = fopen(compressFile.c_str(), "wb");
		assert(fin);

		////����---��ѹ�����ؽ�huffuman tree
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
		////�ָ�  ��д8���ֽ�
		info._count = 0;
		fwrite(&info, sizeof(Config), 1, fin);

		//�ļ�ָ�� ƫ���� ��׼λ��
		char value = 0;//8��λ
		long long count = 0;
		fseek(fout, 0, SEEK_SET);
		ch = fgetc(fout);
		/*EOF��16����Ϊ0xFF��ʮ����Ϊ - 1�����������ı��ļ��У�
		��Ϊ���ı��ļ�����������ASC����ֵ����ʽ��ţ���ͨ�ַ���ASC����ķ�Χ��32��127��ʮ���ƣ�
		��EOF����ͻ����˿���ֱ��ʹ�ã������ڶ������ļ��У������п��ܳ��� - 1
		��˲�����EOF����Ϊ�������ļ��Ľ�����־������ͨ��feof�������жϡ�
		Ҳ����˵feof��������������ж��ļ��Ƿ����(�����ı��ļ��Ͷ������ļ�)��*/
		while (!feof(fout))//
		{
			string& code = _infos[(unsigned char)ch]._code;
			//��λ���� ����������λ���뷽���޹�
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
				

	//��ѹ��
	//input.txt.huffuman.txt->input.txt
	void Uncompress(const char* file)
	{
		//1.��ȡ�����ļ���Ϣ--�ַ����ֵĴ���
		assert(file);
		string uncompressFile = file;
		size_t position = uncompressFile.rfind(".");//�����ҵ�.��λ��
		assert(position != string::npos);//������

		uncompressFile = uncompressFile.substr(0, position);
		uncompressFile += ".unhuffuman";

		FILE* fout = fopen(file, "rb");//���Կ���������ָ��
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

		//�ؽ�huffuman tree
		CharInfo invalid;
		invalid._count = 0;
		HuffumanTree<CharInfo> tree(_infos, 256, invalid);
		Node* root = tree.GetRoot();
		long long count = root->_w._count;


		//����������ͷ����Ȩֵ�ڵ�count�����ԭ���ļ����ַ�����

		//��ѹ��
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
		uncompressFile.erase(position, uncompressFile.size() - position + 1);//ɾ��.֮����ַ���
		fclose(fin);
		fclose(fout);
	}
protected:
	//void _GetHuffumanCode(Node* root)
	//{
	//	if (root == NULL)
	//		return;
	//	//Ҷ�ӽڵ�
	//	if (root->_left == NULL && root->_right == NULL)
	//	{
	//		//root��Ҷ�ӽڵ㣬�ڵ��Ȩֵ_w��CharInfo���ͣ�����ַ�����CharInfo�е�ch
	//		//ֱ�������ã�����ϵͳ����

	//		string& code = _infos[root->_w._ch]._code;
	//		Node*  cur = root;//Ҷ�ӽڵ�
	//		Node* parent = cur->_parent;
	//		//ѭ����parentΪNULL�ͽ���
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
	//		//��ʱΪ���õı���
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
		//����
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
	CharInfo _infos[256];//����Ascall��ͳ���ַ����ֵĴ���
};

void TestFileCompess()
{
	/*FileCpmoress fc;
	fc.Compress("111.png");*/
	FileCpmoress f1;
	f1.Uncompress("111.png.huffuman");
}
