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


// If exist, return the data.
 //extern struct list;
struct list{
    __u64 offset;
    void* addr;
    unsigned long size;
    struct mutex lock;
    struct list *next;
};

DEFINE_MUTEX(global_lock);

 extern struct list *head;
long npheap_lock(struct npheap_cmd __user *user_cmd)
{
    //struct npheap_cmd = temp;
    //if(!copy_from_user(&temp,(void __user *) user_cmd,sizeof(struct npheap_cmd)))

     struct list *temp = head;
     while(temp!=NULL)
            {
	 if(temp->offset==(user_cmd->offset/PAGE_SIZE))
        {
             mutex_lock(&global_lock);
             //printk(KERN_CONT "GLOBAL LOCKED\n");
		mutex_lock(&temp->lock);
        //printk(KERN_CONT "OBJECT LOCKED\n");
	mutex_unlock(&global_lock);
              //printk(KERN_CONT "GLOBAL UNLOCKED\n");
             return 0;
         }
         temp=temp->next;
     }
     return 0;
}     

long npheap_unlock(struct npheap_cmd __user *user_cmd)
{
    struct list *temp=head;
    //printk(KERN_CONT "IN UNLOCK\n");
    while(temp!=NULL)
    {
        //printk(KERN_CONT "The offsets are %lu %lu\n",temp->offset,user_cmd->offset/PAGE_SIZE);
        if(temp->offset==(user_cmd->offset/PAGE_SIZE))
        {//printk(KERN_CONT "IN IF\n");
            mutex_unlock(&global_lock);
		mutex_unlock(&temp->lock);
        //printk(KERN_CONT "OBJECT UNLOCKED\n");
            return 0;
        }
        temp=temp->next;
    }
    return 0;
}

long npheap_getsize(struct npheap_cmd __user *user_cmd)
{
	struct list *temp=head;
    while(temp!=NULL)
    {
        if(temp->offset==(user_cmd->offset/PAGE_SIZE))
        {
		return temp->size;
	}
	temp=temp->next;
     }
   return 0;
}

long npheap_delete(struct npheap_cmd __user *user_cmd)
{
    struct list *temp=head;
    while(temp!=NULL)
    {
        if(temp->offset==(user_cmd->offset/PAGE_SIZE))
        {
          //   printk(KERN_CONT "inside delete %lu\n",temp->offset);
            
            
                kfree(temp->addr);
                //temp->addr=NULL;
                //temp->offset=-1;
               // printk(KERN_CONT "OBJECT DELETED\n");
                return 0;
            
        }
        temp=temp->next;
    }
    return 0;
}

long npheap_ioctl(struct file *filp, unsigned int cmd,
                                unsigned long arg)
{
    switch (cmd) {
    case NPHEAP_IOCTL_LOCK:
        return npheap_lock((void __user *) arg);
    case NPHEAP_IOCTL_UNLOCK:
        return npheap_unlock((void __user *) arg);
    case NPHEAP_IOCTL_GETSIZE:
        return npheap_getsize((void __user *) arg);
    case NPHEAP_IOCTL_DELETE:
        return npheap_delete((void __user *) arg);
    default:
        return -ENOTTY;
    }
}

