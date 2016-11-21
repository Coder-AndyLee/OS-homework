#include <stdlib.h>
#include <linux/init.h>   
#include <linux/module.h>   
#include <linux/module.h>   
#include <linux/unistd.h>  
#include <sys/type.h>  
#include <linux/slab.h>  
  
MODULE_LICENSE("GPL");    
  
#define first 1000     /* 第一次尝试分配1000个对象 */  
  
struct sample_struct 
{  
    int id;  
    char name[20];  
    char address[50];  
};  
static struct kmem_cache *sample_struct_cachep;  
static struct sample_struct *sample1[first];  

static int sample_mod_init(void)		//定义一个 kmem_cache 对象，然后对其进行初始化
{  
    int i;  
  
    sample_struct_cachep = kmem_cache_create(  
            "sample_struct_cachep",         /* Name */  
            sizeof(struct sample_struct),   /* Object Size */  
            0,                              /* Alignment */  
            SLAB_HWCACHE_ALIGN,             /* Flags */  
            NULL);                          /* Constructor */  
    /* 确保创建成功：有可能失败 */  
    if (NULL == sample_struct_cachep)  
        return 1;  
    printk(KERN_INFO "Cache name is %s\n",kmem_cache_name(sample_struct_cachep));  
     
    /* 首次分配 */  
    for (i = 0; i < first; i++)  
    {  
        sample1[i] = kmem_cache_alloc(sample_struct_cachep, GFP_KERNEL);  
        if (NULL == sample1[i])  
        {  
            int ii;  
            printk("First alloc ERR: %d/n", i);  
            for (ii = 0; ii < i; ii++)  
            {  
                kmem_cache_free(sample_struct_cachep, sample1[ii]);  
                sample1[ii] = NULL;  
            }  
        }  
    }
    return 0;  
}  

int slab_test(void)		//使用所分配的 slab 缓存对象，或释放所分配的 slab 缓存对象
{  
	struct sample_struct *object;  
	printk( "Cache name is %s/n", kmem_cache_name( sample_struct_cachep ) );  
	printk( "Cache object size is %d/n", kmem_cache_size( sample_struct_cachep ) );  
	object = kmem_cache_alloc(sample_struct_cachep, GFP_KERNEL);  
	if (object)
	{  
		kmem_cache_free(sample_struct_cachep, object);  
	}
	return 0;  
}  

static void sample_mod_exit(void) //slab 缓存的销毁 
{  
    int i;  
    if (sample1[0])  
    {  
        for (i = 0; i < first; i++)  
            {  
                kmem_cache_free(sample_struct_cachep, sample1[i]);  
                sample1[i] = NULL;  
            }  
    }  
  
    if (sample_struct_cachep)   
    {  
        kmem_cache_destroy( sample_struct_cachep );  
        printk(KERN_INFO "Destroy sample_struct_cachep!\n");  
    }  
  
    return ;  
}   

int main()
{
	sample_mod_init();
	slab_test();
	sample_mod_exit();

	system("pause");
	return 0;
}