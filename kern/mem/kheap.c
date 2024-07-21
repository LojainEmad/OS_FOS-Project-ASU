#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"
uint32 number =0;
struct pages_info
{
	void * intaialVa;
	uint32   numberofpages;
};

struct pages_info  arr_pages_info[1048576];

int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
{
	//TODO: [PROJECT'23.MS2 - #01] [1] KERNEL HEAP - initialize_kheap_dynamic_allocator()
		//Initialize the dynamic allocator of kernel heap with the given start address, size & limit
		//All pages in the given range should be allocated
		//Remember: call the initialize_dynamic_a llocator(..) to complete the initialization
		//Return:
		//	On success: 0
		//	Otherwise (if no memory OR initial size exceed the given limit): E_NO_MEM
		//Comment the following line(s) before start coding...
		//panic("not implemented yet");
		START = daStart ;
		HARD_LIMIT = daLimit ;
		BREAK = (daStart +initSizeToAllocate);
		if (daStart >=HARD_LIMIT)
			return E_NO_MEM ;
		if(BREAK>HARD_LIMIT)
			return E_NO_MEM ;
		int ptr=daStart;
		while (ptr<BREAK)
		{
		struct FrameInfo *ptr_frame_info;
		allocate_frame(&ptr_frame_info);
		ptr_frame_info->va=ptr;
		int ref=map_frame(ptr_page_directory,ptr_frame_info,ptr, PERM_WRITEABLE);
		if( ref == E_NO_MEM )
		{
			  free_frame (ptr_frame_info);
			  return E_NO_MEM;
		}
		ptr=ptr+PAGE_SIZE;
		}
		initialize_dynamic_allocator(daStart ,initSizeToAllocate);
		return 0;
}

void* sbrk(int increment)
{
	//TODO: [PROJECT'23.MS2 - #02] [1] KERNEL HEAP - sbrk()
	/* increment > 0: move the segment break of the kernel to increase the size of its heap,
	 * 				you should allocate pages and map them into the kernel virtual address space as necessary,
	 * 				and returns the address of the previous break (i.e. the beginning of newly mapped memory).
	 * increment = 0: just return the current position of the segment break
	 * increment < 0: move the segment break of the kernel to decrease the size of its heap,
	 * 				you should deallocate pages that no longer contain part of the heap as necessary.
	 * 				and returns the address of the new break (i.e. the end of the current heap space).
	 *
	 * NOTES:
	 * 	1) You should only have to allocate or deallocate pages if the segment break crosses a page boundary
	 * 	2) New segment break should be aligned on page-boundary to avoid "No Man's Land" problem
	 * 	3) Allocating additional pages for a kernel dynamic allocator will fail if the free frames are exhausted
	 * 		or the break exceed the limit of the dynamic allocator. If sbrk fails, kernel should panic(...)
	 */
	//MS2: COMMENT THIS LINE BEFORE START CODING====
	uint32 orginal_break=BREAK;
	uint32 new_break ;
	if (increment==0)
	{
		return (void *)BREAK;
	}
	if(increment>0)
	{
		int increment1=ROUNDUP(increment,PAGE_SIZE);// increment 1.5 ->>4
	    new_break = BREAK+increment1;
	    if(new_break == KERNEL_HEAP_START + DYN_ALLOC_MAX_SIZE )
	    {

	    	return (void*)-1;
	    }

	    else if( new_break <HARD_LIMIT)
	    {

	      struct FrameInfo *ptr_frame_info;
	      allocate_frame(&ptr_frame_info);
	      ptr_frame_info->va=BREAK;
	      int ref=map_frame(ptr_page_directory,ptr_frame_info,BREAK, PERM_WRITEABLE);
	      if(ref ==E_NO_MEM)
	      {

	        return (void *)-1;
	      }
	      BREAK=new_break;
	      return (void *)orginal_break;
	    }


        return (void*)-1;
	}
	//-1.5*-1-->1.5 after rowndup -->4
	if (increment < 0) {
	    int va=-increment;
	    int pagesDealloc=va/PAGE_SIZE;
	    if(pagesDealloc>0){
	    	 int tempBreak=BREAK;
	    	 for (int i=0;i<pagesDealloc;i++) {
	    		        int tempBreak=tempBreak-PAGE_SIZE;
	    		        unmap_frame(ptr_page_directory,tempBreak);
	    		        free_frame((void *)tempBreak);
	    		        //tempBreak=tempBreak-PAGE_SIZE;
	    		    }
	    }
	    BREAK=BREAK-va;
	    new_break=BREAK;
	    return (void *)new_break;
	}
		return (void*)-1 ;
		//panic("not implemented yet");
}

void* kmalloc(unsigned int size)
{
	//TODO: [PROJECT'23.MS2 - #03] [1] KERNEL HEAP - kmalloc()
	//refer to the project presentation and documentation for details
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy

	//change this "return" according to your answer
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");
	uint32 ptr =HARD_LIMIT +PAGE_SIZE;
	  if (size >DYN_ALLOC_MAX_SIZE )
	   	    {
	   		 return NULL;
	   	    }

	   if (size <= DYN_ALLOC_MAX_BLOCK_SIZE)
    	{
    		return alloc_block_FF(size);
    	}
// check page allocator

    else
	{
     size =ROUNDUP(size,PAGE_SIZE);
     uint32 num_of_pages =(size/PAGE_SIZE);
     uint32 counter =0;
	for(uint32 i =ptr;i<KERNEL_HEAP_MAX;i+=PAGE_SIZE)
	{
		uint32  *ptr_Page_table;
	  struct FrameInfo *ptr_frame_info= get_frame_info(ptr_page_directory,ptr,&ptr_Page_table );
		if(ptr_frame_info ==NULL)
		{
			counter++;
			if (counter ==num_of_pages)
			{
				ptr=ptr-((num_of_pages-1)*PAGE_SIZE);
				uint32 var =ptr;
					for(int j=0;j<num_of_pages;j++)
					  {
						 struct FrameInfo *ptr_frame_info;
						 allocate_frame(&ptr_frame_info);
						 ptr_frame_info->va=var;
						 int ref =map_frame(ptr_page_directory,ptr_frame_info,var,PERM_WRITEABLE);
                         if(ref == E_NO_MEM)
                         {

                        	 return NULL;
                         }
						 var=var+PAGE_SIZE;
					  }

					arr_pages_info[number].intaialVa =(void *)ptr;
					arr_pages_info[number].numberofpages=num_of_pages;
					number++;
                   break ;
			}
			ptr=ptr+PAGE_SIZE;
		}
		else if (ptr_frame_info !=NULL)
				{
					counter=0;
					ptr=ptr+PAGE_SIZE;
				}
	 }
	if (counter == 0)
	{
		return NULL;
	}
	else
      return (void *)ptr;
	}
}


void kfree(void* virtual_address)
{
	//TODO: [PROJECT'23.MS2 - #04] [1] KERNEL HEAP - kfree()
	//refer to the project presentation and documentation for details
	// Write your code here, remove the panic and write your code
    //panic("kfree() is not implemented yet...!!");
	int pages_to_be_unmaped ;
	uint32 x=(uint32)virtual_address;
    int index =-1;
	  if ((uint32)virtual_address>=START&& (uint32)virtual_address<BREAK)
	   {
		free_block(virtual_address);
	   }
	  else
	  {
		for(int i=0;i<1048576;i++)
		{
			if(arr_pages_info[i].intaialVa==virtual_address)
			{
				index=i;
				pages_to_be_unmaped=arr_pages_info[i].numberofpages;
				break;
			}
		}
		     for(int i=0;i<pages_to_be_unmaped;i++)
		     {
               uint32 *ptr_Page_table;
		      struct FrameInfo *ptr_Frame_info =get_frame_info(ptr_page_directory,x,&ptr_Page_table);

			  unmap_frame(ptr_page_directory,x);
			  //ptr_Frame_info->va=0;
			    x=x+PAGE_SIZE;
		     }
		     arr_pages_info[index].intaialVa=0;
		     arr_pages_info[index].numberofpages=0;
		    //number--;
     }
}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//TODO: [PROJECT'23.MS2 - #05] [1] KERNEL HEAP - kheap_virtual_address()
	//refer to the project presentation and documentation for details
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");
	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
	//change this "return" according to your answer
	     uint32 offset=(physical_address)% PAGE_SIZE;
	    struct FrameInfo *ptr_Frame_info =to_frame_info(physical_address);
	    if(ptr_Frame_info ==NULL||ptr_Frame_info->references==0)
	    {
	    	  return 0;
	    }

	    return (ptr_Frame_info->va+offset);
}


unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//TODO: [PROJECT'23.MS2 - #06] [1] KERNEL HEAP - kheap_physical_address()
	//refer to the project presentation and documentation for details
	// Write your code here, remove the panic and write your code
	 //panic("kheap_physical_address() is not implemented yet...!!");
	uint32  *ptr_Page_table;
    struct FrameInfo *ptr_frame_info= get_frame_info(ptr_page_directory,virtual_address,&ptr_Page_table );
    if (ptr_frame_info!= NULL)
    {
    	uint32 pageindex =PTX(virtual_address);
    	uint32 *ptr_page_table =NULL;
    	get_page_table(ptr_page_directory,virtual_address,&ptr_page_table);
    	uint32  entry= ptr_page_table[pageindex];
    	uint32 frame_number = (entry >> 12);
    	uint32 offset = PGOFF(virtual_address);
        unsigned int pa = (frame_number << 12) | offset;
    	return pa;
    }
    else
    {
    	return 0;
    }
}


void kfreeall()
{
	panic("Not implemented!");
}

void kshrink(uint32 newSize)
{
	panic("Not implemented!");
}

void kexpand(uint32 newSize)
{
	panic("Not implemented!");
}




//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT'23.MS2 - BONUS#1] [1] KERNEL HEAP - krealloc()
	// Write your code here, remove the panic and write your code
	return NULL;
	panic("krealloc() is not implemented yet...!!");
}
