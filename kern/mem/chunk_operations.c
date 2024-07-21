/*
 * chunk_operations.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include <kern/trap/fault_handler.h>
#include <kern/disk/pagefile_manager.h>
#include "kheap.h"
#include "memory_manager.h"
#include <inc/queue.h>
#include <kern/tests/utilities.h>

//extern void inctst();

/******************************/
/*[1] RAM CHUNKS MANIPULATION */
/******************************/

//===============================
// 1) CUT-PASTE PAGES IN RAM:
//===============================
//This function should cut-paste the given number of pages from source_va to dest_va on the given page_directory
//	If the page table at any destination page in the range is not exist, it should create it
//	If ANY of the destination pages exists, deny the entire process and return -1. Otherwise, cut-paste the number of pages and return 0
//	ALL 12 permission bits of the destination should be TYPICAL to those of the source
//	The given addresses may be not aligned on 4 KB
int cut_paste_pages(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 num_of_pages)
{
	panic("cut_paste_pages() is not implemented yet...!!");

	return 0 ;
}

//===============================
// 2) COPY-PASTE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va on the given page_directory
//	Ranges DO NOT overlapped.
//	If ANY of the destination pages exists with READ ONLY permission, deny the entire process and return -1.
//	If the page table at any destination page in the range is not exist, it should create it
//	If ANY of the destination pages doesn't exist, create it with the following permissions then copy.
//	Otherwise, just copy!
//		1. WRITABLE permission
//		2. USER/SUPERVISOR permission must be SAME as the one of the source
//	The given range(s) may be not aligned on 4 KB
int copy_paste_chunk(uint32* page_directory, uint32 source_va, uint32 dest_va, uint32 size)
{
	panic("copy_paste_chunk() is not implemented yet...!!");
	return 0;
}

//===============================
// 3) SHARE RANGE IN RAM:
//===============================
//This function should copy-paste the given size from source_va to dest_va on the given page_directory
//	Ranges DO NOT overlapped.
//	It should set the permissions of the second range by the given perms
//	If ANY of the destination pages exists, deny the entire process and return -1. Otherwise, share the required range and return 0
//	If the page table at any destination page in the range is not exist, it should create it
//	The given range(s) may be not aligned on 4 KB
int share_chunk(uint32* page_directory, uint32 source_va,uint32 dest_va, uint32 size, uint32 perms)
{
	panic("share_chunk() is not implemented yet...!!");
	return 0;
}

//===============================
// 4) ALLOCATE CHUNK IN RAM:
//===============================
//This function should allocate the given virtual range [<va>, <va> + <size>) in the given address space  <page_directory> with the given permissions <perms>.
//	If ANY of the destination pages exists, deny the entire process and return -1. Otherwise, allocate the required range and return 0
//	If the page table at any destination page in the range is not exist, it should create it
//	Allocation should be aligned on page boundary. However, the given range may be not aligned.
int allocate_chunk(uint32* page_directory, uint32 va, uint32 size, uint32 perms)
{
	panic("allocate_chunk() is not implemented yet...!!");
	return 0;
}

//=====================================
// 5) CALCULATE ALLOCATED SPACE IN RAM:
//=====================================
void calculate_allocated_space(uint32* page_directory, uint32 sva, uint32 eva, uint32 *num_tables, uint32 *num_pages)
{
	panic("calculate_allocated_space() is not implemented yet...!!");
}


//=====================================
// 6) CALCULATE REQUIRED FRAMES IN RAM:
//=====================================
//This function should calculate the required number of pages for allocating and mapping the given range [start va, start va + size) (either for the pages themselves or for the page tables required for mapping)
//	Pages and/or page tables that are already exist in the range SHOULD NOT be counted.
//	The given range(s) may be not aligned on 4 KB
uint32 calculate_required_frames(uint32* page_directory, uint32 sva, uint32 size)
{
	panic("calculate_required_frames() is not implemented yet...!!");
	return 0;
}

//=================================================================================//
//===========================END RAM CHUNKS MANIPULATION ==========================//
//=================================================================================//

/*******************************/
/*[2] USER CHUNKS MANIPULATION */
/*******************************/

//======================================================
/// functions used for USER HEAP (malloc, free, ...)
//======================================================

//=====================================
// 1) ALLOCATE USER MEMORY:
//=====================================
void allocate_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	/*=============================================================================*/
	//TODO: [PROJECT'23.MS2 - #10] [2] USER HEAP - allocate_user_mem() [Kernel Side]
	/*REMOVE THESE LINES BEFORE START CODING */
	//inctst();
	//return;
	/*=============================================================================*/
	// Write your code here, remove the panic and write your code
	//panic("allocate_user_mem() is not implemented yet...!!");
	uint32 * ptr_page_table;
	 size = ROUNDUP(size, PAGE_SIZE);
	 uint32 allocFrames=size/PAGE_SIZE;
	 uint32 ptr =virtual_address;
	 for(int i =0 ; i < allocFrames ; i++){
		 int reg= get_page_table(e->env_page_directory,ptr,&ptr_page_table);
		 if(reg==TABLE_NOT_EXIST){
		 		create_page_table(e->env_page_directory,ptr);
		 	}
		 pt_set_page_permissions( (uint32*)e->env_page_directory,ptr, PERM_MARKED_P,0);
		 ptr=ptr+PAGE_SIZE;
	 }

}



//=====================================
// 2) FREE USER MEMORY:
//=====================================
void free_user_mem(struct Env* e, uint32 virtual_address, uint32 size)
{
	/*==========================================================================*/
	//TODO: [PROJECT'23.MS2 - #12] [2] USER HEAP - free_user_mem() [Kernel Side]
	/*REMOVE THESE LINES BEFORE START CODING */
	//inctst();
    int num =ROUNDUP(size,PAGE_SIZE);
    int num2=num/PAGE_SIZE;
	uint32 va = virtual_address ;
	//uint32 *ptr_page_table ;
	//ROUNDDOWN(va , PAGE_SIZE);
	//get_page_table(e->env_page_directory,virtual_address,&ptr_page_table);
	for(uint32 itr = 0 ; itr< size ; itr=itr + PAGE_SIZE){
		/*unmark*/
	/*==========================================================================*/
		 //int reg= get_page_table(e->env_page_directory,va,&ptr_page_table);
				 //if(reg!=TABLE_NOT_EXIST){
		uint32 x = pt_get_page_permissions(e->env_page_directory,va);
		if(x==  ( x | (PERM_MARKED_P))){
		pt_set_page_permissions( e->env_page_directory, va,0 ,PERM_PRESENT|PERM_MARKED_P|PERM_WRITEABLE);

		/*ptr_page_table[PTX(va )] =(ptr_page_table[PTX(va )] | (PERM_PRESENT)) ;
		ptr_page_table[PTX(va )] =(ptr_page_table[PTX(va )] & ~(PERM_AVAILABLE)) ;*/

		/*free all pages */
	/*==========================================================================*/
		pf_remove_env_page(e,va);

		/*romove from working set*/
	/*==========================================================================*/
		env_page_ws_invalidate(e, va);
		unmap_frame(e->env_page_directory,va);
		}
		va = va + PAGE_SIZE ;
	}
//
	struct WorkingSetElement * elemiter;
	struct WorkingSetElement * elem2;
   LIST_FOREACH( elemiter,&(e->page_WS_list)){
//
		if(elemiter == e->page_last_WS_element)
		{
			break;
		}
		else
		{
			    elem2=elemiter;
      			LIST_REMOVE(&(e->page_WS_list),elemiter);
     		    LIST_INSERT_TAIL(&(e->page_WS_list),elem2);
		}
   }




	return;
	// Write your code here, remove the panic and write your code
	//panic("free_user_mem() is not implemented yet...!!");

	//TODO: [PROJECT'23.MS2 - BONUS#2] [2] USER HEAP - free_user_mem() IN O(1): removing page from WS List instead of searching the entire list

}

//=====================================
// 2) FREE USER MEMORY (BUFFERING):
//=====================================
void __free_user_mem_with_buffering(struct Env* e, uint32 virtual_address, uint32 size)
{
	// your code is here, remove the panic and write your code
	panic("__free_user_mem_with_buffering() is not implemented yet...!!");
}

//=====================================
// 3) MOVE USER MEMORY:
//=====================================
void move_user_mem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
{
	//your code is here, remove the panic and write your code
	panic("move_user_mem() is not implemented yet...!!");

	// This function should move all pages from "src_virtual_address" to "dst_virtual_address"
	// with the given size
	// After finished, the src_virtual_address must no longer be accessed/exist in either page file
	// or main memory
}

//=================================================================================//
//========================== END USER CHUNKS MANIPULATION =========================//
//=================================================================================//
