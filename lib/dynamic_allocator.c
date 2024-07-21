/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//=====================================================
// 1) GET BLOCK SIZE (including size of its meta data):
//=====================================================
uint32 get_block_size(void* va)
{
	struct BlockMetaData *curBlkMetaData = ((struct BlockMetaData *)va - 1) ;
	return curBlkMetaData->size ;
}

//===========================
// 2) GET BLOCK STATUS:
//===========================
int8 is_free_block(void* va)
{
	struct BlockMetaData *curBlkMetaData = ((struct BlockMetaData *)va - 1) ;
	return curBlkMetaData->is_free ;
}

//===========================================
// 3) ALLOCATE BLOCK BASED ON GIVEN STRATEGY:
//===========================================
void *alloc_block(uint32 size, int ALLOC_STRATEGY)
{
	void *va = NULL;
	switch (ALLOC_STRATEGY)
	{
	case DA_FF:
		va = alloc_block_FF(size);
		break;
	case DA_NF:
		va = alloc_block_NF(size);
		break;
	case DA_BF:
		va = alloc_block_BF(size);
		break;
	case DA_WF:
		va = alloc_block_WF(size);
		break;
	default:
		cprintf("Invalid allocation strategy\n");
		break;
	}
	return va;
}

//===========================
// 4) PRINT BLOCKS LIST:
//===========================

void print_blocks_list(struct MemBlock_LIST list)
{
	cprintf("=========================================\n");
	struct BlockMetaData* blk ;
	cprintf("\nDynAlloc Blocks List:\n");
	LIST_FOREACH(blk, &list)
	{
		cprintf("(size: %d, isFree: %d)\n", blk->size, blk->is_free) ;
	}
	cprintf("=========================================\n");

}
//
////********************************************************************************//
////********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//==================================
// [1] INITIALIZE DYNAMIC ALLOCATOR:
//==================================
struct MemBlock_LIST Avmemoryblocklist ;
bool is_initialized = 0;
void initialize_dynamic_allocator(uint32 daStart, uint32 initSizeOfAllocatedSpace)
{
	//=========================================
	//DON'T CHANGE THESE LINES=================
	if (initSizeOfAllocatedSpace == 0)
		return ;
	 is_initialized=1;
	//=========================================
	//=========================================
	//TODO: [PROJECT'23.MS1 - #5] [3] DYNAMIC ALLOCATOR - initialize_dynamic_allocator()
	//panic("initialize_dynamic_allocator is not implemented yet");
    LIST_INIT(& Avmemoryblocklist);
    struct BlockMetaData *blockMetaData =(struct BlockMetaData *)daStart;
    blockMetaData->is_free = 1;
    blockMetaData->size =initSizeOfAllocatedSpace;
    LIST_INSERT_HEAD(&Avmemoryblocklist,blockMetaData);
}

//=========================================
// [4] ALLOCATE BLOCK BY FIRST FIT:
//=========================================

void *alloc_block_FF(uint32 size)
{
		//panic("alloc_block_FF is not implemented yet");
			 struct BlockMetaData * myBlock;
			 if(size ==0)
			 {
				 return NULL;
			 }

			 if (!is_initialized)
			 				 {

			 				 uint32 required_size = size + sizeOfMetaData();
			 				 uint32 da_start = (uint32)sbrk(required_size);
			 				 //get new break since it's page aligned! thus, the size can be more than the required one
			 				 uint32 da_break = (uint32)sbrk(0);
			 				 initialize_dynamic_allocator(da_start, da_break - da_start);
			 				 }

			LIST_FOREACH(myBlock ,&(Avmemoryblocklist))
			{
//				if(myBlock== (struct BlockMetaData*)HARD_LIMIT)
//				{
//					return NULL;
//				}
				 if (myBlock->is_free ==1 && (myBlock->size) >=(size+sizeOfMetaData()))
					 {
					            if(myBlock->is_free ==1 && myBlock->size == (size+sizeOfMetaData()))
					            {
					            	 myBlock->is_free=0;
					                 return myBlock+1;
					            }

					            else if(myBlock->is_free ==1 && myBlock->size >=(size+sizeOfMetaData()))
					 			{

					            	if( myBlock->size-size-sizeOfMetaData()>= sizeOfMetaData())
					            	{
					            	struct BlockMetaData * newBlock = (struct BlockMetaData*)((char*)(myBlock+1) + size);
					 				newBlock->size = myBlock->size-size-sizeOfMetaData();
					 				newBlock->is_free=1;
					 				LIST_INSERT_AFTER(&Avmemoryblocklist,myBlock,newBlock);
					 				myBlock->is_free=0;
					 				myBlock->size=size+sizeOfMetaData();
			                         return myBlock+1;
					            	}

					            	else if( myBlock->size-size-sizeOfMetaData()<sizeOfMetaData())
					            	{
					            		myBlock->is_free=0;
					            		myBlock->size=size+sizeOfMetaData()+(myBlock->size-size-sizeOfMetaData());
					            		return myBlock+1;
					            	}
					 			}

				     		 }
			    }
			  //cprintf("iam here 164");
			  struct BlockMetaData * brk=sbrk(size+sizeOfMetaData());
                              if (brk ==(void *)-1)
							  {
                            	  cprintf("i am here 168");
                            	  return NULL;
                              }
                              else
                              {
                            	//  cprintf("iam here 173");
                            	  if (PAGE_SIZE-size-sizeOfMetaData()>sizeOfMetaData())
                            	  {
                            	  struct BlockMetaData * newBlock1 = (struct BlockMetaData*)(brk);
                            	  newBlock1->size=size+sizeOfMetaData();
                            	  newBlock1->is_free=0;
                            	  LIST_INSERT_TAIL(&Avmemoryblocklist,newBlock1);
                            	  struct BlockMetaData * newBlock2 = (struct BlockMetaData*)((char*)((char *)brk+sizeOfMetaData()) + size);
                            	  newBlock2->size = PAGE_SIZE-size-sizeOfMetaData();
                            	  newBlock2->is_free=1;
                            	  LIST_INSERT_AFTER(&Avmemoryblocklist,newBlock1 ,newBlock2);
                            	  return   newBlock1+1;
                            	  }
                            	  else
                            	  {
                            		  struct BlockMetaData * newBlock1 = (struct BlockMetaData*)(brk);
                            		   newBlock1->size=size+sizeOfMetaData()+(PAGE_SIZE-size-sizeOfMetaData());
                            		   newBlock1->is_free=0;
                            		   LIST_INSERT_TAIL(&Avmemoryblocklist,newBlock1);
                            		   return   newBlock1+1;
                            	  }
                              }
			return NULL;
}


//=========================================
// [5] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size)
{	//TODO: [PROJECT'23.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF()
	 panic("alloc_block_BF is not implemented yet");
	    return NULL;
}
//=========================================
// [6] ALLOCATE BLOCK BY WORST FIT:
//=========================================
void *alloc_block_WF(uint32 size)
{
	panic("alloc_block_WF is not implemented yet");
	return NULL;
}

//=========================================
// [7] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size)
{
	panic("alloc_block_NF is not implemented yet");
	return NULL;
}

//===================================================
// [8] FREE BLOCK WITH COALESCING:
//===================================================

	//TODO: [PROJECT'23.MS1 - #7] [3] DYNAMIC ALLOCATOR - free_block()
	//panic("free_block is not implemented yet");

void free_block(void *va)
	{

		struct BlockMetaData *current_MetaData ;
		LIST_FOREACH(current_MetaData, &Avmemoryblocklist)
		{
			if ((current_MetaData+1) == va)
			{

					current_MetaData->is_free = 1;
					struct BlockMetaData *next_MetaData = LIST_NEXT(current_MetaData);
					struct BlockMetaData *previous_MetaData = LIST_PREV(current_MetaData);
					if(next_MetaData != NULL && previous_MetaData == NULL && next_MetaData->is_free == 1)
					{
						current_MetaData->size = current_MetaData->size + next_MetaData->size;
						next_MetaData->size = 0;
						next_MetaData-> is_free= 0;
						LIST_REMOVE(&Avmemoryblocklist,next_MetaData);

					}
					else if(next_MetaData == NULL && previous_MetaData != NULL && previous_MetaData->is_free == 1)
					{
						previous_MetaData->size = current_MetaData->size + previous_MetaData->size ;
						current_MetaData->size = 0;
						current_MetaData->is_free=0;
						LIST_REMOVE(&Avmemoryblocklist,current_MetaData);

					}
					else if (next_MetaData != NULL && previous_MetaData != NULL && next_MetaData->is_free == 1 && previous_MetaData->is_free == 1)
					{
						previous_MetaData->size = previous_MetaData->size + current_MetaData->size + next_MetaData->size;
						current_MetaData->size = 0;
						current_MetaData->is_free=0;
						LIST_REMOVE(&Avmemoryblocklist,current_MetaData);
						next_MetaData->size = 0;
						next_MetaData->is_free=0;
						LIST_REMOVE(&Avmemoryblocklist,next_MetaData);
						current_MetaData = previous_MetaData;
					}

					else if (next_MetaData != NULL && next_MetaData->is_free == 1 && previous_MetaData!= NULL &&previous_MetaData->is_free == 0)
					{
						current_MetaData->size = current_MetaData->size + next_MetaData->size;
						next_MetaData->size = 0;
						next_MetaData-> is_free= 0;
						LIST_REMOVE(&Avmemoryblocklist,next_MetaData);

					}
					else if (previous_MetaData != NULL && previous_MetaData->is_free == 1&& next_MetaData != NULL && next_MetaData->is_free == 0)
					{
						previous_MetaData->size = current_MetaData->size + previous_MetaData->size ;
						current_MetaData->size = 0;
						current_MetaData->is_free=0;
						LIST_REMOVE(&Avmemoryblocklist,current_MetaData);
						current_MetaData = previous_MetaData;
					}

			}
		}
}


//=========================================
// [4] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
	void *realloc_block_FF(void* va, uint32 new_size)
	{
		//TODO: [PROJECT'23.MS1 - #8] [3] DYNAMIC ALLOCATOR - realloc_block_FF()
		//panic("realloc_block_FF is not implemented yet");
		if((void*) va == NULL && new_size == 0 )
			return NULL ;

		struct BlockMetaData *current_MetaData;
				LIST_FOREACH(current_MetaData, &Avmemoryblocklist)
				{
					if ((current_MetaData + 1) == va)
						break;
				}

		int case_n;
		struct BlockMetaData * newBlock ;
		uint32 frag = 0 ;
		if((void*) va != NULL && new_size == 0){
			case_n = 2 ;
		}else if ((void*) va == NULL && new_size > 0){
			return  alloc_block_FF(new_size);
		}else if (new_size > (current_MetaData->size - sizeOfMetaData()) ){
			case_n = 0 ;
	   }else if (new_size < (current_MetaData->size - sizeOfMetaData()) ){
			case_n = 1 ;
	   }else{
		   case_n = 3;
	   }

		struct BlockMetaData * Block = current_MetaData ;
		//struct BlockMetaData * Block = va ;
		struct BlockMetaData * nxtBlock = LIST_NEXT(Block);
		uint32 s = nxtBlock->size;
		switch(case_n){

		case 0:
			 if ( nxtBlock->is_free==1 &&(nxtBlock->size + Block->size - (sizeOfMetaData()+sizeOfMetaData()) )>= new_size){
				 Block->size = new_size+sizeOfMetaData();
				 nxtBlock->is_free = 0;
				 nxtBlock->size =0;
				 LIST_REMOVE(&Avmemoryblocklist, nxtBlock);
			return Block+1 ;
			}else{
				free_block(va);
				return  alloc_block_FF(new_size);
			}
			break;

		case 1 :
			frag = Block->size - new_size - sizeOfMetaData() ;
			Block->size = new_size + sizeOfMetaData() ;
			if (frag >= sizeOfMetaData()){
			newBlock = va + new_size ;
			newBlock->size = frag ;
			newBlock->is_free = 1 ;
			LIST_INSERT_AFTER(&Avmemoryblocklist, Block, newBlock);
			}else{
				Block->size = Block->size+ frag;
			}

			return Block+1 ;
		break;

		case 2 :
			free_block(va);
			return NULL ;
			break ;

		case 3 :
			return  Block+1 ;
			break;
		}
		return NULL;
	}
