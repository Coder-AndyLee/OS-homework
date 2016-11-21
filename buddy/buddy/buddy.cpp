#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <iostream>
#define MEMORY 2048			//设置总内存大小为2048
using namespace std;
int available[100], avaiNum = 0, TotalMemory = MEMORY;	//数组：各单元可用内存大小；int变量：数组内标号（对应各单元标号）
int fragment[100], fragNum = 0;							//数组：各单元碎片大小；int变量：数组内的标号（对应各单元标号）

typedef struct Tree
{
	struct Tree *left;//左子树
	struct Tree *right;//右子树
	struct Tree *father;//父亲结点
	int tab;			//该节点是否被分配过
	int this_memory;	//该节点总内存大小
	int already_used;	//该节点已使用内存大小
}TreeNode, *tree;

void init_tree(tree &temp);
int Allocate(int allocate , tree &root);
void Scan(tree root);
void Delete(int a[], int num);
void Release(int to_release, tree &root);


int main()
{
	int time_ , i , j , n , releNum=0;
	int size_of_allocated, released;
	int label;				//表明分配后的结果
	int Allocation[256];	//存放分配的结果
	tree init = NULL;
	
	init_tree(init);
	cout<<"内存大小为"<<MEMORY<<endl
		<<"请输入需要分配内存的次数：";
	cin>>time_;
	n=time_;

	srand(time(NULL));
	for(i = 0 ; i<time_ ; i++)
	{
		size_of_allocated = rand() % 600;
		cout<<"被分配的内存大小为："<<size_of_allocated<<endl;

		label = Allocate(size_of_allocated,init);
		Scan(init);
		TotalMemory -= size_of_allocated;
		fragment[fragNum] = '\0';
		available[avaiNum] = '\0';

		if(label == 1)
		{
			Allocation[releNum] = size_of_allocated;
			releNum++;
			cout<<"分配已完成"<<endl;

			j = 0;
			if(fragment[0]=='\0')
				cout<<"无碎片！"<<endl;
			else
			{
				cout<<"碎片为："<<endl;
				while (fragment[j] != '\0')
				{
					cout<<fragment[j]<<"  ";
					j++;
				}
				cout<<endl;
				fragNum = 0;//将数量清零
			}
		}
		else if(label == 0)
		{
			n--;
			cout<<"内存不足，无法分配！"<<endl;
		}
		else
		{
			n--;
			cout<<"碎片太多，无法分配！"<<endl;
		}
	}
	Allocation[i] = '\0';
	
	cout<<endl<<endl<<"现在开始释放内存"<<endl;
	time_ = n;
	for(i = 0 ; i<time_ ; i++)
	{
		releNum = rand() % n;
		n--;
		released = Allocation[releNum];
		Delete(Allocation,releNum);
		Release(released,init);
		Scan(init);
		TotalMemory += released;
		fragment[fragNum] = '\0';
		available[avaiNum] = '\0';

		cout<<"被释放的内存为："<<released<<endl
			<<"释放成功！"<<endl;

		j = 0;
		if(available[0] == '\0')
			cout<<"没有可释放的内存";
		else
		{
			cout<<"可释放内存为："<<endl;
			while(fragment[j] != '\0')
			{
				cout<<fragment[j]<<"  ";
				j++;
			}
		}
		cout<<endl;
		fragNum = 0;
	}
	system("pause");
	return 0;
}

void init_tree(tree &temp)
{
	temp = (tree)malloc(sizeof(TreeNode));
	temp->tab = 0;
	temp->this_memory = MEMORY;
	temp->already_used = 0;
	temp->left = NULL;
	temp->right = NULL;
	temp->father = NULL;
}

int Allocate(int allocate , tree &root)
{
	if(allocate > TotalMemory)				//需要分配的比总内存还大
		return 0;
	else if(allocate > root->this_memory)	//需要分配的内存大于该节点总内存
		return -1;
	
	else if (allocate > (root->this_memory)/2 )		//
	{
		if(root->left == NULL && root->tab == 0)
		{
			root->tab = 1;
			root->already_used = allocate;
			return 1;
		}
		else
			return -1;
	}
	else if(root->tab == 1)					//
		return -1;
	else
	{
		if(root->left ==NULL)
		{
			root->left = (tree)malloc(sizeof(TreeNode));
			root->left->tab = 0;
			root->left->this_memory = root->this_memory/2;
			root->left->already_used = 0;
			root->left->left = NULL;
			root->left->right = NULL;
			root->left->father = root;
			
			root->right = (tree)malloc(sizeof(TreeNode));
			root->right->tab = 0;
			root->right->this_memory = root->this_memory/2;
			root->right->already_used = 0;
			root->right->left = NULL;
			root->right->right = NULL;
			root->right->father = root;
		}
		if(Allocate(allocate,root->left) == 1)
			return 1;
		else
			return Allocate(allocate,root->right);
	}
}

void Scan(tree root)
{
	if(root->left == NULL)
	{
		if(root->tab == 0)
		{
			available[avaiNum] = root->this_memory;
			avaiNum++;
		}
		else
		{
			fragment[fragNum] = root->this_memory - root->already_used;
			fragNum++;
		}
	}
	else
	{
		Scan(root->left);
		Scan(root->right);
	}
}

void Delete(int a[], int num)
{
	int i = num;
	while(a[i] != '\0')
	{
		a[i] = a[i+1];
		i++;
	}
}

void Release(int to_release, tree &root)
{
	tree temp;
	if(root != NULL)
	{
		Release(to_release, root->left);
		Release(to_release, root->right);
		if(root->left == NULL)
		{
			if((root->tab == 0) && (root->father != NULL))
			{
				if((root->father->left->tab == 0) && (root->father->right->tab == 0) && (root->father->left->left == NULL) && (root->father->right->left == NULL))
				{
					temp = root->father;
					temp->left = NULL;
					temp->right = NULL;
				}
			}
			else if((root->tab == 1) && (root->already_used == to_release))
			{
				root->already_used = 0;
				root->tab = 0;
				if((root->father->left->tab == 0) && (root->father->right->tab == 0) && (root->father->left->left == NULL) && (root->father->right->left == NULL))
				{
					temp = root->father;
					temp->left = NULL;
					temp->right = NULL;
				}
			}
		}
	}
}