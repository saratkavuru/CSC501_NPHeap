// Project 1: Rachit Thirani, rthiran; Sarat Kavuru, skavuru;
//////////////////////////////////////////////////////////////////////
//                             North Carolina State University
//
//
//
//                             Copyright 2016
//
////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify it
// under the terms and conditions of the GNU General Public License,
// version 2, as published by the Free Software Foundation.
//
// This program is distributed in the hope it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
//
////////////////////////////////////////////////////////////////////////
//
//   Author:  Hung-Wei Tseng
//
//   Description:
//     Skeleton of NPHeap Pseudo Device
//
////////////////////////////////////////////////////////////////////////

#include "npheap.h"

#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/poll.h>
#include <linux/mutex.h>

extern struct miscdevice npheap_dev;
//list ot keep a track of the objects
struct list{
    __u64 offset;
    void* addr;
    unsigned long size;
    struct mutex lock;
    struct list *next;
};
struct list *head=NULL;


int npheap_mmap(struct file *filp, struct vm_area_struct *vma)
{
    	struct list *temp=head;
        int flag=0;
    //printk(KERN_CONT "hi %lu %lu\n",vma->vm_pgoff>>PAGE_SHIFT, temp->offset);
	//check if the offset exists
   	while(temp!=NULL)
    	{
            //printk(KERN_CONT "mmap in %lu %lu\n",vma->vm_pgoff, temp->offset);
            // printk(KERN_CONT "hi %lu \n",temp->offset);
        	if(temp->offset==(vma->vm_pgoff))
        	{   //printk(KERN_CONT "offset found\n");
                if(temp->addr!=NULL)
                   { if(!remap_pfn_range(vma,vma->vm_start,virt_to_phys(temp->addr)>>PAGE_SHIFT,vma->vm_end-vma->vm_start, vma->vm_page_prot))
                    {
                         //printk(KERN_CONT "successfuly allocated kernel memory %lu at %lu \n",kernel_memory,virt_to_phys(kernel_memory)>> PAGE_SHIFT);
           if(!copy_from_user(temp->addr,vma->vm_start,vma->vm_end-vma->vm_start))
    {
        //printk(KERN_CONT "Data Copied\n");
    }
                    }
         // printk(KERN_CONT "successfuly allocated kernel memory %lu at %lu \n",temp->addr,virt_to_phys(temp->addr)>>PAGE_SHIFT); 
           		 return 0;}
                 else{
                    flag=1;
                 }
        	}
            if(flag==1)
                break;
         temp=temp->next;
        
    	}
//if the offset is not present allocate kernel memory
        printk(KERN_CONT "line 72\n");
	void* kernel_memory = kzalloc(vma->vm_end - vma->vm_start, GFP_KERNEL);
        //printk(KERN_CONT "line 73\n");
        phys_addr_t kernel_phys_addr = __pa(kernel_memory);
    	//if(!remap_pfn_range(vma,vma->vm_start,kernel_phys_addr >> PAGE_SHIFT,vma->vm_end-vma->vm_start, vma->vm_page_prot))
      // if(!remap_pfn_range(vma,vma->vm_start,virt_to_phys(kernel_memory),vma->vm_end-vma->vm_start, vma->vm_page_prot))
        if(!remap_pfn_range(vma,vma->vm_start,virt_to_phys(kernel_memory)>> PAGE_SHIFT,vma->vm_end-vma->vm_start, vma->vm_page_prot))
           {
            printk(KERN_CONT "successfuly allocated kernel memory %lu at %lu \n",kernel_memory,virt_to_phys(kernel_memory)>> PAGE_SHIFT);
           if(!copy_from_user(kernel_memory,vma->vm_start,vma->vm_end-vma->vm_start))
    {
        //printk(KERN_CONT "Data Copied\n");
    }
           }
		//printk(KERN_CONT "successfuly allocated kernel memory %lu at %lu \n",kernel_memory,kernel_phys_addr >> PAGE_SHIFT);
	struct list *new_mapping=kmalloc(sizeof(struct list),GFP_KERNEL);
        new_mapping->addr=kernel_memory;
       	new_mapping->offset = vma->vm_pgoff;
   	new_mapping->size = vma->vm_end - vma->vm_start;
	//mutex_unlock(&new_mapping->lock);
       	new_mapping->next=NULL;
        temp = head;
        if(flag==1)
        {
            temp->addr=kernel_memory;
            temp->size=vma->vm_end - vma->vm_start;
        }
        else
    	{if(temp==NULL)
   	     head=new_mapping;
    	else
        {
    		while(temp->next!=NULL)
        	temp=temp->next;
        	temp->next=new_mapping;
        }}
    return 0;
}

int npheap_init(void)
{
    int ret;
    if ((ret = misc_register(&npheap_dev)))
        printk(KERN_ERR "Unable to register \"npheap\" misc device\n");
    else
        printk(KERN_ERR "\"npheap\" misc device installed\n");
    return ret;
}

void npheap_exit(void)
{
    misc_deregister(&npheap_dev);
}

