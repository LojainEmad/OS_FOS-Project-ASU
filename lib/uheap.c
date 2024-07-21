#include <inc/lib.h>

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//
struct pages_info
{
	void * intaialVa;
	uint32   numberofpages;
};
uint32 number =0;
struct pages_info  arr_pages_info[1048576];

int FirstTimeFlag = 1;
void InitializeUHeap()
{
	if(FirstTimeFlag)
	{
#if UHP_USE_BUDDY
		initialize_buddy();
		cprintf("BUDDY SYSTEM IS INITIALIZED\n");
#endif
		FirstTimeFlag = 0;
	}
}

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
void* sbrk(int increment)
{
	return (void*) sys_sbrk(increment);
}

//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================
void* malloc(uint32 size)
{

	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	if (size == 0)
		return NULL ;
	//==============================================================
	//TODO: [PROJECT'23.MS2 - #09] [2] USER HEAP - malloc() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");
	// uint32 sys_get_limit();
     uint32 limit =sys_get_limit();
     uint32 ptr = limit+PAGE_SIZE;
     uint32 sys_get_permission(uint32 ptr);
	   if (size <= DYN_ALLOC_MAX_BLOCK_SIZE) {
		  return alloc_block_FF(size);
	   }
	   else {
		   size = ROUNDUP(size, PAGE_SIZE);

		   uint32 allocFrames=(size/PAGE_SIZE);

		   uint32 count=0;
		     for (uint32 i = ptr; i < USER_HEAP_MAX; i += PAGE_SIZE)
		    {
		    	   uint32 x =sys_get_permission(i);
		    	 if(x==-1 || (x & PERM_MARKED_P) != PERM_MARKED_P)
		         {
		        	 count++;
		        	 if(count == allocFrames)
		        	 {
		        	 ptr=ptr-((allocFrames-1)*PAGE_SIZE);
		        	 sys_allocate_user_mem(ptr , size);
		        	 arr_pages_info[number].intaialVa =(void *)ptr;
		        	 arr_pages_info[number].numberofpages=allocFrames;
		        	 number++;
	                 return (void*) ptr;
		        	 }
		        	 ptr=ptr+PAGE_SIZE;
		         }
		         else{
		        	 count=0;
		        	 ptr=ptr+PAGE_SIZE;
		         }
	        }
		     return NULL;
		     if(count ==0)
		     {
		       return NULL;
		      }
		    else{
		    	 sys_allocate_user_mem(ptr , size);
                 return (void*) ptr;
		    	 }

		    }
}
//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address)
{
	//TODO: [PROJECT'23.MS2 - #11] [2] USER HEAP - free() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");
	   uint32 limit =sys_get_Break();
	   uint32 start = sys_get_start();
		int pages_to_be_unmaped ;
		uint32 x=(uint32)virtual_address;
	    int index =-1;
		  if ((uint32)virtual_address>= start&& (uint32)virtual_address<limit)
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

             uint32 Size = pages_to_be_unmaped*PAGE_SIZE;
			arr_pages_info[index].intaialVa=0;
			arr_pages_info[index].numberofpages=0;
             sys_free_user_mem(x,Size);
	     }

}


//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	if (size == 0) return NULL ;
	//==============================================================
	panic("smalloc() is not implemented yet...!!");
	return NULL;
}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	//==============================================================
	// Write your code here, remove the panic and write your code
	panic("sget() is not implemented yet...!!");
	return NULL;
}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.
void *realloc(void *virtual_address, uint32 new_size)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	InitializeUHeap();
	//==============================================================

	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	return NULL;

}


//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	// Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");
}


//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize)
{
	panic("Not Implemented");

}
void shrink(uint32 newSize)
{
	panic("Not Implemented");

}
void freeHeap(void* virtual_address)
{
	panic("Not Implemented");

}
