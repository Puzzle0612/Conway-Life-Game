// MyConwayGameSIMD.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include<iostream>
#include<fstream>
#include<string>
#include<time.h>
#include<emmintrin.h>

using namespace std;
//定义两个全局数组变量存放上一代和下一代的信息
#define ROW 50
#define COL 100
//增加两行两列空行方便处理
_MM_ALIGN16 int thisGen[ROW+2][COL+2];
_MM_ALIGN16 int nextGen[ROW+2][COL+2];


//读原始状态文件，存入数组
int readData()
{
	int i, j;
	ifstream in;
	in.open("input_50x100");
	for (i = 0; i <= 51; i++)
		for (j = 0; j <= 101; j++)
		{
			if (i == 0 || i == 51 || j == 0 || j == 101)
				thisGen[i][j] = 0;
			else
				in >> thisGen[i][j];
		}
	return 0;
}

//写结果文件，存入数组
int writeData()
{
	int i, j;
	ofstream out("output_50x100_simd");
	for (i = 1; i <=50; i++)
	{
		for (j = 1; j <= 100; j++)
			out << thisGen[i][j] << " ";
		out << endl;
	}
	return 0;
}
//只在串行中需要，并行中不需要，并行的加速比来源之一
/*//对于周围元素不满8个的元素做边界检查
bool checkBound(int i, int j)
{
	if (i >= 0 && i <= 49 && j >= 0 && j <= 99)
		return 1;
	else
		return 0;
}

//统计八字格中的存活元素个数
int countLives(int i, int j)
{
	int count = 0;
	int m, n;
	for (m = i - 1; m <= i + 1; m++)
		for (n = j - 1; n <= j + 1; n++)
			if (checkBound(i, j))
				if (thisGen[m][n] == 1)
					count++;
	if (thisGen[i][j] == 1)
		count--;
	return count;
}*/

//更新一代
int updateNext()
{
	//每次处理4个数据，并行的加速比来源之二
	for(int i=1;i<=50;i++)
		for (int j = 1; j <= 97; j = j + 4)
		{
			//中心元素
			const __m128i *center;
			__m128i _center;
			center = (const __m128i *)&thisGen[i][j];
			_center = _mm_loadu_si128(center);
			//八个方向的元素，lu:left-up rd:right-down ...依次类推
			const __m128i *lu, *u, *ru, *r, *l, *ld, *rd, *d;
			__m128i _lu, _u, _ru, _r, _l, _ld, _rd, _d;
			lu = (const __m128i *)&thisGen[i - 1][j - 1];
			u = (const __m128i *)&thisGen[i - 1][j];
			ru = (const __m128i *)&thisGen[i - 1][j + 1];
			l = (const __m128i *)&thisGen[i][j - 1];
			r = (const __m128i *)&thisGen[i][j + 1];
			ld = (const __m128i *)&thisGen[i + 1][j - 1];
			d = (const __m128i *)&thisGen[i + 1][j];
			rd = (const __m128i *)&thisGen[i + 1][j + 1];
			_lu = _mm_loadu_si128(lu);
			_u = _mm_loadu_si128(u);
			_ru = _mm_loadu_si128(ru);
			_l = _mm_loadu_si128(l);
			_r = _mm_loadu_si128(r);
			_ld = _mm_loadu_si128(ld);
			_d = _mm_loadu_si128(d);
			_rd = _mm_loadu_si128(rd);
			__m128i sum;
			//将八个方向元素进行统计
			sum = _mm_add_epi32(_mm_add_epi32(_mm_add_epi32(_lu, _u), _mm_add_epi32(_ru, _l)), _mm_add_epi32(_mm_add_epi32(_r, _ld), _mm_add_epi32(_d, _rd)));
			int * result;
			result = (int *)&sum;
			//根据得到的结果更新下一代
			for (int count = j; count <= j + 3; count++)
				if (thisGen[i][count] == 1)
				{
					if (result[count - j] < 2 || result[count - j]>3)
						nextGen[i][count] = 0;
					else
						nextGen[i][count] = 1;
				}
				else
				{
					if (result[count - j] == 3)
						nextGen[i][count] = 1;
					else
						nextGen[i][count] = 0;
				}
		}
	return 0;

}

//主函数入口
int main(int argc,char* argv[])
{
	//设定计时器
	clock_t start, finish;
	double time_use;
	int i, j;
	readData();
	start = clock();
	//多次迭代
	for (int time = 0; time <= atoi(argv[1]); time++)
	{
		updateNext();
		for (i = 1; i <=50 ; i++)
			for (j = 1; j <= 100; j++)
				thisGen[i][j] = nextGen[i][j];
	}
	finish = clock();
	time_use = (double)(finish - start) / CLOCKS_PER_SEC;
	//打印时间
	cout << "\nThe program takes about " << time_use << " seconds!" << endl;
	writeData();
	return 0;
}


