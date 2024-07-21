/*
 * fault_handler.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include "trap.h"
#include <kern/proc/user_environment.h>
#include "../cpu/sched.h"
#include "../disk/pagefile_manager.h"
#include "../mem/memory_manager.h"
#include "kern/mem/kheap.h"

//2014 Test Free(): Set it to bypass the PAGE FAULT on an instruction with this length and continue executing the next one
// 0 means don't bypass the PAGE FAULT
uint8 bypassInstrLength = 0;

//===============================
// REPLACEMENT STRATEGIES
//===============================
//2020
void setPageReplacmentAlgorithmLRU(int LRU_TYPE)
{
	assert(LRU_TYPE == PG_REP_LRU_TIME_APPROX || LRU_TYPE == PG_REP_LRU_LISTS_APPROX);
	_PageRepAlgoType = LRU_TYPE ;
}
void setPageReplacmentAlgorithmCLOCK(){_PageRepAlgoType = PG_REP_CLOCK;}
void setPageReplacmentAlgorithmFIFO(){_PageRepAlgoType = PG_REP_FIFO;}
void setPageReplacmentAlgorithmModifiedCLOCK(){_PageRepAlgoType = PG_REP_MODIFIEDCLOCK;}
/*2018*/ void setPageReplacmentAlgorithmDynamicLocal(){_PageRepAlgoType = PG_REP_DYNAMIC_LOCAL;}
/*2021*/ void setPageReplacmentAlgorithmNchanceCLOCK(int PageWSMaxSweeps){_PageRepAlgoType = PG_REP_NchanceCLOCK;  page_WS_max_sweeps = PageWSMaxSweeps;}

//2020
uint32 isPageReplacmentAlgorithmLRU(int LRU_TYPE){return _PageRepAlgoType == LRU_TYPE ? 1 : 0;}
uint32 isPageReplacmentAlgorithmCLOCK(){if(_PageRepAlgoType == PG_REP_CLOCK) return 1; return 0;}
uint32 isPageReplacmentAlgorithmFIFO(){if(_PageRepAlgoType == PG_REP_FIFO) return 1; return 0;}
uint32 isPageReplacmentAlgorithmModifiedCLOCK(){if(_PageRepAlgoType == PG_REP_MODIFIEDCLOCK) return 1; return 0;}
/*2018*/ uint32 isPageReplacmentAlgorithmDynamicLocal(){if(_PageRepAlgoType == PG_REP_DYNAMIC_LOCAL) return 1; return 0;}
/*2021*/ uint32 isPageReplacmentAlgorithmNchanceCLOCK(){if(_PageRepAlgoType == PG_REP_NchanceCLOCK) return 1; return 0;}

//===============================
// PAGE BUFFERING
//===============================
void enableModifiedBuffer(uint32 enableIt){_EnableModifiedBuffer = enableIt;}
uint8 isModifiedBufferEnabled(){  return _EnableModifiedBuffer ; }

void enableBuffering(uint32 enableIt){_EnableBuffering = enableIt;}
uint8 isBufferingEnabled(){  return _EnableBuffering ; }

void setModifiedBufferLength(uint32 length) { _ModifiedBufferLength = length;}
uint32 getModifiedBufferLength() { return _ModifiedBufferLength;}

//===============================
// FAULT HANDLERS
//===============================

//Handle the table fault
void table_fault_handler(struct Env * curenv, uint32 fault_va)
{
	//panic("table_fault_handler() is not implemented yet...!!");
	//Check if it's a stack page
	uint32* ptr_table;
#if USE_KHEAP
	{
		ptr_table = create_page_table(curenv->env_page_directory, (uint32)fault_va);
	}
#else
	{
		__static_cpt(curenv->env_page_directory, (uint32)fault_va, &ptr_table);
	}
#endif
}

//Handle the page fault

void page_fault_handler(struct Env * curenv, uint32 fault_va)
{
#if USE_KHEAP
		struct WorkingSetElement *victimWSElement = NULL;
		uint32 wsSize = LIST_SIZE(&(curenv->page_WS_list));
#else
		int iWS =curenv->page_last_WS_index;
		uint32 wsSize = env_page_ws_get_size(curenv);
#endif

		if(isPageReplacmentAlgorithmFIFO())
				{
					if(wsSize < (curenv->page_WS_max_size))
						{

						struct FrameInfo *ptr_frame_info;
						allocate_frame(&ptr_frame_info);
						map_frame(curenv->env_page_directory,ptr_frame_info,fault_va,PERM_WRITEABLE|PERM_USER|PERM_MARKED_P);
						struct WorkingSetElement *new_e =env_page_ws_list_create_element(curenv, fault_va) ;
							int info = pf_read_env_page(curenv,(void*) fault_va);
							if(/*ptr_frame_info==NULL*/   info == E_PAGE_NOT_EXIST_IN_PF   /*cheking page not exist*/){
								if((fault_va>=USER_HEAP_START &&fault_va < USER_HEAP_MAX)|| (fault_va>=USTACKBOTTOM && fault_va<USTACKTOP)){

								}else{
								sched_kill_env(curenv->env_id);
							} }

//
							LIST_INSERT_TAIL(&(curenv->page_WS_list),new_e);
							wsSize = LIST_SIZE(&(curenv->page_WS_list));
							if(wsSize == (curenv->page_WS_max_size)){
								curenv->page_last_WS_element = LIST_FIRST(&(curenv->page_WS_list)) ;
							}else {

							}
						}

					else{


					struct WorkingSetElement *em_to_be_removed=curenv->page_last_WS_element ;
					uint32 x = pt_get_page_permissions(curenv->env_page_directory,em_to_be_removed->virtual_address);
					uint32  *ptr_Page_table;
				    struct FrameInfo *ptr_frame_info= get_frame_info(curenv->env_page_directory,em_to_be_removed->virtual_address,&ptr_Page_table );
					 if(x==-1||(x & PERM_MODIFIED) == PERM_MODIFIED )
					 {

						 pf_update_env_page(curenv,em_to_be_removed->virtual_address,ptr_frame_info);
					 }
					 struct FrameInfo *ptr_frame_info2;
					 allocate_frame(&ptr_frame_info2);
					 map_frame(curenv->env_page_directory,ptr_frame_info2,fault_va,PERM_WRITEABLE|PERM_USER|PERM_MARKED_P);
					 struct WorkingSetElement * l_element =LIST_LAST(&curenv->page_WS_list);
					  int info2 = pf_read_env_page(curenv,(void*) fault_va);
					  if( info2 == E_PAGE_NOT_EXIST_IN_PF){
					  if((fault_va>=USER_HEAP_START &&fault_va < USER_HEAP_MAX)|| (fault_va>=USTACKBOTTOM && fault_va<USTACKTOP)){

		               }else{
							 sched_kill_env(curenv->env_id);
						    }
						}
		             unmap_frame(curenv->env_page_directory,em_to_be_removed->virtual_address);
		              curenv->page_last_WS_element->virtual_address=fault_va;
		              struct WorkingSetElement * next_element =  LIST_NEXT(curenv->page_last_WS_element);
		              struct WorkingSetElement * checkelment  =LIST_FIRST(&curenv->page_WS_list);
		              struct WorkingSetElement * new_element;
		             		if(curenv->page_last_WS_element==l_element)
		             		{
		             		 curenv->page_last_WS_element=checkelment;
		             		}
		             		else
		             		{
		             		 curenv->page_last_WS_element = next_element;
		             		 }
				  }
				}
		if(isPageReplacmentAlgorithmLRU(PG_REP_LRU_LISTS_APPROX))
						{
					//env_page_ws_print(curenv);
							if( ((curenv->ActiveList.size) + (curenv->SecondList.size)) <curenv->page_WS_max_size){
							//TODO: [PROJECT'23.MS3 - #2] [1] PAGE FAULT HANDLER - LRU Replacement
							// Write your code here, remove the panic and write your code
							//panic("page_fault_handler() LRU Replacement is not implemented yet...!!");


								uint32 x = pt_get_page_permissions(curenv->env_page_directory,fault_va);
								if(x == (x & (~PERM_PRESENT))&&x == (x | (PERM_MARKED_P))){

								}else{
								int info = pf_read_env_page(curenv,(void*) fault_va);

								if(  info == E_PAGE_NOT_EXIST_IN_PF ){
									if((fault_va>=USER_HEAP_START &&fault_va < USER_HEAP_MAX)|| (fault_va>=USTACKBOTTOM && fault_va<USTACKTOP)){

									}else{

									sched_kill_env(curenv->env_id);
								} }
								}
								struct FrameInfo *ptr_frame_info;
								uint32 Asize = 0 ;
								struct WorkingSetElement *new_e;
								struct WorkingSetElement * tovictim ;
								tovictim = curenv->page_last_WS_element ;
								if(curenv->ActiveList.size < curenv->ActiveListSize){

								allocate_frame(&ptr_frame_info);
								map_frame(curenv->env_page_directory,ptr_frame_info,fault_va,PERM_WRITEABLE|PERM_USER|PERM_MARKED_P);
								new_e =env_page_ws_list_create_element(curenv, fault_va) ;
								LIST_INSERT_HEAD(&(curenv->ActiveList),new_e);
																 Asize = LIST_SIZE(&(curenv->ActiveList));
																if(Asize == (curenv->ActiveListSize)){
																	curenv->page_last_WS_element = LIST_LAST(&(curenv->ActiveList)) ;
																}else {
																	curenv->page_last_WS_element = NULL ;
																}
								}else{


									if(x == (x & (~PERM_PRESENT))&&x == (x | (PERM_MARKED_P))){

										pt_set_page_permissions(curenv->env_page_directory,fault_va,PERM_MARKED_P|PERM_PRESENT,0);

										struct WorkingSetElement *s ;
										LIST_FOREACH(s,&(curenv->SecondList)){

											if(s->virtual_address == fault_va){

											new_e = s ;
											break ;
											}
										}

										LIST_REMOVE(&(curenv->SecondList),new_e);
										LIST_REMOVE(&(curenv->ActiveList),curenv->page_last_WS_element);
										LIST_INSERT_HEAD(&(curenv->SecondList),tovictim);
										LIST_INSERT_HEAD(&(curenv->ActiveList),new_e);
										pt_set_page_permissions(curenv->env_page_directory,tovictim->virtual_address,PERM_MARKED_P,PERM_PRESENT);
										curenv->page_last_WS_element = LIST_LAST(&(curenv->ActiveList)) ;

									}else{
										int info = pf_read_env_page(curenv,(void*) fault_va);
																		if(  info == E_PAGE_NOT_EXIST_IN_PF ){
																			if((fault_va>=USER_HEAP_START &&fault_va < USER_HEAP_MAX)|| (fault_va>=USTACKBOTTOM && fault_va<USTACKTOP)){

																			}else{
																			sched_kill_env(curenv->env_id);
																		} }

										tovictim = curenv->page_last_WS_element ;
										LIST_REMOVE(&(curenv->ActiveList),tovictim);

										allocate_frame(&ptr_frame_info);
										map_frame(curenv->env_page_directory,ptr_frame_info,fault_va,PERM_WRITEABLE|PERM_USER|PERM_MARKED_P);
										new_e =env_page_ws_list_create_element(curenv, fault_va) ;
										LIST_INSERT_HEAD(&(curenv->SecondList),tovictim);
										LIST_INSERT_HEAD(&(curenv->ActiveList),new_e);
										pt_set_page_permissions(curenv->env_page_directory,tovictim->virtual_address,PERM_MARKED_P,PERM_PRESENT);

										curenv->page_last_WS_element = LIST_LAST(&(curenv->ActiveList)) ;


									}
								}
							}
					else{
						//replacement

						//panic("page_fault_handler() LRU Replacement is not implemented yet...!!");
						bool flag=0;

						 struct WorkingSetElement *new_element;
						 struct WorkingSetElement *term;
						 LIST_FOREACH(term,&(curenv->SecondList)){

						   uint32 rounded_va = ROUNDDOWN(term->virtual_address, PAGE_SIZE);
						 	uint32 rounded_vartual =ROUNDDOWN(fault_va,PAGE_SIZE);
						 	if(rounded_va == rounded_vartual){
                                flag =1;
						 		new_element=term ;
						 		break ;
						 			}
						 	}
						if(flag){

							    struct WorkingSetElement *removingElementACTIVE = LIST_LAST(&(curenv->ActiveList));
							    LIST_REMOVE(&(curenv->ActiveList),removingElementACTIVE);
							    LIST_INSERT_HEAD(&(curenv->SecondList), removingElementACTIVE);
							    uint32 vaRemoving =removingElementACTIVE->virtual_address;
							    uint32 presentRemoving = pt_get_page_permissions(curenv->env_page_directory, vaRemoving);

							     pt_set_page_permissions(curenv->env_page_directory,vaRemoving,0,PERM_PRESENT);
							    LIST_REMOVE(&(curenv->SecondList),term);
						     	LIST_INSERT_HEAD(&(curenv->ActiveList),term);
							    uint32 vaVictim =term->virtual_address;
						     	uint32 presentVic = pt_get_page_permissions(curenv->env_page_directory,vaVictim);

								   pt_set_page_permissions(curenv->env_page_directory,vaVictim,PERM_PRESENT, 0);
				          }

						else {
							struct WorkingSetElement *removeFROM_LRU = LIST_LAST(&(curenv->SecondList));
							LIST_REMOVE(&(curenv->SecondList), removeFROM_LRU);
							uint32 x = pt_get_page_permissions(curenv->env_page_directory,removeFROM_LRU->virtual_address);
							uint32  *ptr_Page_table;
							struct FrameInfo *ptr_frame_info= get_frame_info(curenv->env_page_directory,removeFROM_LRU->virtual_address,&ptr_Page_table );
							if(x==-1||(x & PERM_MODIFIED) == PERM_MODIFIED )
								{

									pf_update_env_page(curenv,removeFROM_LRU->virtual_address,ptr_frame_info);
								 }
						    unmap_frame(curenv->env_page_directory,removeFROM_LRU->virtual_address);
						        struct WorkingSetElement *moveLASTFROM_ACTIVE= LIST_LAST(&(curenv->ActiveList));
						        LIST_REMOVE(&(curenv->ActiveList), moveLASTFROM_ACTIVE);
						        LIST_INSERT_HEAD(&(curenv->SecondList), moveLASTFROM_ACTIVE);
						        uint32 va_moveLASTFROM_ACTIVE=moveLASTFROM_ACTIVE->virtual_address;
						        pt_set_page_permissions(curenv->env_page_directory,va_moveLASTFROM_ACTIVE,0,PERM_PRESENT);
						    	 struct FrameInfo *ptr_frame;
						    	 allocate_frame(&ptr_frame);
						    	 map_frame(curenv->env_page_directory,ptr_frame,fault_va,PERM_WRITEABLE|PERM_USER|PERM_MARKED_P);
						    	 int inf = pf_read_env_page(curenv,(void*) fault_va);
						    	 if( inf == E_PAGE_NOT_EXIST_IN_PF){
						    	 if((fault_va>=USER_HEAP_START &&fault_va < USER_HEAP_MAX) || (fault_va>=USTACKBOTTOM && fault_va<USTACKTOP)){
						    	 	}else{
						    	 	sched_kill_env(curenv->env_id);
						    	 }
						    	 }
						    	 new_element=env_page_ws_list_create_element(curenv, fault_va) ;
						    	 LIST_INSERT_HEAD(&(curenv->ActiveList), new_element);
						    	 pt_set_page_permissions(curenv->env_page_directory,fault_va,PERM_PRESENT,0);
						    }


						}

				 }


			 }
					//TODO: [PROJECT'23.MS3 - BONUS] [1] PAGE FAULT HANDLER - O(1) implementation of LRU replacement
     // }
//  }


void __page_fault_handler_with_buffering(struct Env * curenv, uint32 fault_va)
{
	panic("this function is not required...!!");
}
