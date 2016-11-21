#include<stdio.h> 
#include<stdlib.h> 
#include<time.h>
#define INVALID -1 
#define VALID -2
#define total_instruction 2048   //指令序列
#define total_vp 32              //虚存页数
#define MAX  3000

int page_fault;                  //页面失效次数
int a[total_instruction];        //指令序列
int page[total_instruction];    //页地址流
int full_;                      //full_=-1时，表示内存已满

typedef struct frame_struct   //内存结构
{      
	int pn;         //对应虚存的页号
	int count;      //访问次数
	int time;       //进入内存时间
	int exe_time;   //下一次执行时间
}frame_type; 

typedef struct      //虚存结构
{         
	int sig;        //是否在内存的标志
	int pfn;       //如果在内存中，所在内存页的页号

}p_type; 

p_type page_v[total_vp];  //虚存页结构数组
frame_type page_m[total_vp];    //内存页结构数组

void initialize(int total_pf)   //内存,外存页的初始化
{ 
	int i; 
	page_fault=0; 
	for(i=0;i<total_vp;i++) 
	{ 
		page_v[i].sig=INVALID;   
		page_v[i].pfn=-1;
	}
	for(i=0;i<total_pf;i++) 
	{
		page_m[i].pn=-1;
		page_m[i].count=0;
		page_m[i].time=-1;
		page_m[i].exe_time=MAX;
	} 
}

int full(int total_pf)           //判断内存是否已装满，如果是返回-1，否则返回空页的页号
{
	for(int i=0;i<total_pf;i++)
	{
		if(page_m[i].pn==-1)    
			return i;
	}
	return -1;
}

int find_old(int total_pf)       //查找最早进入内存的页，返回页号
{
	int old = MAX;
	int k = 0;
	for(int i=0;i<total_pf;i++)
	{
		if(page_m[i].time<old && page_m[i].time!=-1)
		{
			old=page_m[i].time;
			k=i;
		}
	}
	return k;
}

int find_exetime(int k)    //查找该内存页指令下一次被执行的时间
{
	for(int i=k+1;i<total_instruction;i++)
	{
		if(page[i] == page[k])
			return i;
	}
	return MAX;   
}		

int find_most(int total_pf)   //查找内存中最不经常被使用的页，返回页号
{
	int max_=0;
	int k=0;
	for(int i=0;i<total_pf;i++)
	{
		if(page_m[i].count>max_)
		{
			max_=page_m[i].count;
			k=i;
		}		
	}
	return k;
}

int find_max(int total_pf)   //查找所有内存页中距下一次执行时间最远的页
{
	int max=0;
	int k=0;
	for(int i=0;i<total_pf;i++)
	{
		if(page_m[i].exe_time>max)
		{
			max=page_m[i].exe_time;
			k=i;
		}
	}
	return k;
}

int find_least(int total_pf)   //查找内存中访问次数最少的页，返回页号
{
	int min_=MAX;
	int k=0;
	for(int i=0;i<total_pf;i++)
	{
		if(page_m[i].count<min_&&page_m[i].count!=0)
		{
			min_=page_m[i].count;
			k=i;
		}
	}
	return k;
}

void FIFO(int total_pf) 
{ 
	int i,j=0; 
	int old,m;

	initialize(total_pf);
	for(i=0;i<total_instruction;i++) 
	{ 
		if(page_v[page[i]].sig==INVALID) //内存页失效时
		{ 
			page_fault++;   
			full_=full(total_pf);
			if(full_!=-1)                //内存有空页时，放入内存中
			{
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=full_;
				page_m[full_].pn=page[i];//记录到来内存时间
				page_m[full_].time=j;
				j++;
			}
			else
			{
				old=find_old(total_pf);      //内存已满，寻找最先到内存的页，置换
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=old;
				page_v[page_m[old].pn].sig=INVALID;
				page_v[page_m[old].pn].pfn=-1;
				page_m[old].pn=page[i];
				page_m[old].time=j;
				j++;
			}
		}
	}
	printf("\t\t%.2f",1-(float)page_fault/total_instruction);  //计算命中率
}

void LRU (int total_pf) 
{ 
	int max,i; 
	initialize(total_pf); 

	for(i=0;i<total_instruction;i++) 
	{ 
		if(page_v[page[i]].sig==INVALID)   //内存页失效
		{ 
			page_fault++; 
			full_=full(total_pf);
			if(full_!=-1)                //内存未满时
			{
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=full_;
				page_m[full_].pn=page[i];//放入内存页中，count置0
				page_m[full_].count=0;   
			}
			else
			{
				max=find_most(total_pf);  //内存装满时，查找到最大的count值，置换
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=max;
				page_v[page_m[max].pn].sig=INVALID;
				page_v[page_m[max].pn].pfn=-1;
				page_m[max].pn=page[i];
				page_m[max].count=0;          //置换后内存页count置0

				for(int j=0;j<total_pf;j++)   //其他未被访问的内存页count+1
				{
					if(page_m[j].pn!=page[i])
						page_m[j].count++;
				}
			}
		}
		else   //页面在内存中
		{
			for(int j=0;j<total_pf;j++)     //其他未被访问的内存页count+1
			{
				if(page_m[j].pn!=page[i])
					page_m[j].count++;
			}
			page_m[page_v[page[i]].pfn].count=0;  //被访问的内存页置0
		}
	}
	printf("\t\t%.2f",1-(float)page_fault/total_instruction); 
}

void OPT(int total_pf) 
{ 
	int i,j,max;  
	int exe_t;
	initialize(total_pf); 
	for(i=0;i<total_instruction;i++) 
	{ 
		if(page_v[page[i]].sig==INVALID) //如果页面失效
		{ 
			page_fault++; 
			full_=full(total_pf);
			if(full_!=-1)    //如果内存不满
			{
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=full_;
				//将该页放入内存中并记录下一次执行时间
				exe_t=find_exetime(i);
				page_m[full_].exe_time=exe_t; 
				page_m[full_].pn=page[i];
			}
			else
			{
				max=find_max(total_pf);  //找到下一次执行时间最晚的页，置换
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=max;
				page_v[page_m[max].pn].sig=INVALID;
				page_v[page_m[max].pn].pfn=-1;
				page_m[max].pn=page[i];    //新的页面放入内存
				exe_t=find_exetime(i);     //更新下一次执行时间
				page_m[max].exe_time=exe_t;
			}
		}
		else  //内存页未失效，更新下一次执行时间
		{
			exe_t=find_exetime(i);        
			page_m[page_v[page[i]].pfn].exe_time=exe_t;	
		}
	}
	printf("\t\t%.2f",1-(float)page_fault/total_instruction); 
}

void LFU(int total_pf) 
{ 
	int i,j,min,minpage,m; 
	int full_;

	initialize(total_pf); 
	for(i=0;i<total_instruction;i++)  
	{ 
		if(page_v[page[i]].sig==INVALID)  //页面失效
		{
			page_fault++;
			full_=full(total_pf);
			if(full_!=-1)                 //内存不满时，将页放入，count置1
			{
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=full_;
				page_m[full_].pn=page[i];
				page_m[full_].count=1;
			}
			else                       //内存装满时，置换访问次数最少的页，即count值最小的页
			{
				min=find_least(total_pf);
				page_v[page[i]].sig=VALID;
				page_v[page[i]].pfn=min;
				page_v[page_m[min].pn].sig=INVALID;
				page_v[page_m[min].pn].pfn=-1;
				page_m[min].pn=page[i];
				page_m[min].count=1;

			}
		}
		else            
			page_m[page_v[page[i]].pfn].count++;    //命中，内存页count++
	} 
	printf("\t\t%.2f",1-(float)page_fault/total_instruction); 
} 

int main(int) 
{   
	int i,m,m1,m2; 
	srand(time(NULL)); 
	for(i=0;i<total_instruction;i++)     //产生指令序列
	{
		m=rand()%(total_instruction/2); 
		a[i++]=m;        
		a[i++]=m+1; 

		m1=rand()%(m+2);  
		a[i++]=m1;
		a[i++]=m1+1;
		m2 = (rand() % (total_instruction - (m1 + 2))) + (m1 + 2);
		a[i++]=m2;
	}
	for(i=0;i<total_instruction;i++)    //产生页地址流
	{
		page[i]=a[i]/64; 
	} 
	printf("\t\t\t对不同帧数的各种算法命中率比较\n"); 
	printf("\n"); 
	printf("Frame\t\tFIFO\t\tLRU\t\tOPT\t\tLFU\n"); 
	for(i=4;i<=32;i++)
	{
		printf("%d",i);//内存页数 
		FIFO(i);
		LRU(i); 
		OPT(i); 
		LFU(i); 

		printf("\n");
	} 
	system("pause"); 
	return 0; 
} 
