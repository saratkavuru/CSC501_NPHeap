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

struct list{
    __u64 offset;
    void* addr;
    unsigned long size;
   // struct mutex *lock;
    struct list *next;
};
struct list *head=NULL;


int npheap_mmap(struct file *filp, struct vm_area_struct *vma)
{
    	struct list *temp=head;
   	while(temp!=NULL)
    	{
        	if(temp->offset==(vma->vm_pgoff<<PAGE_SHIFT))
        	{    
           		return temp->addr;
        	}
        temp=temp->next;
    	}
	void* kernel_memory = kmalloc(vma->vm_end - vma->vm_start, GFP_KERNEL);
    	if(!remap_pfn_range(vma,virt_to_phys(kernel_memory), vma->vm_pgoff,vma->vm_end-vma->vm_start, vma->vm_page_prot))
		printk(KERN_CONT "successfuly allocated kernel memory\n");
	struct list *new_mapping=kmalloc(sizeof(struct list),GFP_KERNEL);
        new_mapping->addr=kernel_memory;
       	new_mapping->offset = vma->vm_pgoff<<PAGE_SHIFT;
   	new_mapping->size = vma->vm_end - vma->vm_start;
       	new_mapping->next=NULL;
    	if(temp==NULL)
   	     head=new_mapping;
    	else
        {
    		while(temp->next!=NULL)
        	temp=temp->next;
        	temp->next=new_mapping;
        }
    return new_mapping->addr;
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

