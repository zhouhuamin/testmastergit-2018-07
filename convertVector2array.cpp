vector<vector<int> > array(1028); //这个m一定不能少//vector<int> array[m];
//初始化一个m*n的二维数组
for(int i=0;i<1028;i++) {
    array[i].resize(1028);
}

//遍历赋值
for(i = 0; i < array.size(); i++)
{
	for (j = 0; j < array[i].size();j++)
	{
		array[i][j] = (i+1)*(j+1);
	}
}

int a[6] = {0, 1 ,2, 3, 4, 5}; // 一维数组
int b[2][3]; // 二维数组
int m = 2, n = 3;
for (int i = 0; i < m; i++)
{
	for (int j = 0; j < n; j++)
	{
		b[i][j] = a[i*n + j];
	}
}

#include <iostream>
using namespace std;
void main()
{
    int array1[3][4]={{1,2,3,4},
                      {5,6,7,8},
                      {9,10,11,12}};
    int array2[12]={0};
    int row,col,i;
    cout << "array old" <<endl;
    for(row=0;row<3;row++)
    {
        for(col=0;col<4;col++)
        {
            cout << array1[row][col];
        }
        cout << endl;
    }
    cout << "array new" << endl;
    for(row=0;row<3;row++)
    {
        for(col=0;col<4;col++)
        {
            i=col+row*4;
            array2[i]=array1[row][col];
        }
    }
    for(i=0;i<12;i++)
        cout << array2[i] << endl;
}



