#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <xil_printf.h>
#include <../include/myfifo.h>

// do not change this function
// but feel free to add your own fifo_print_debug that prints more information

void myfifo_print_status(myfifo_t volatile const * const fifo_admin)
{
  xil_printf("spaces=%u tokens=%u\n", myfifo_spaces(fifo_admin), myfifo_tokens(fifo_admin));
 // xil_printf("FIFO ADMIN OUTPUT\n");
  xil_printf("element_size =%d, capacity =%d, wr_idx =%d, rd_idx =%d, \n",fifo_admin->element_size,fifo_admin->capacity,fifo_admin->wr_idx,fifo_admin->rd_idx);
}

// your functions
//MYFIFO_INIT//
extern myfifo_t * myfifo_init(myfifo_t volatile * const fifo_admin, void volatile * const token_buffer, uint32_t const nr_tokens, uint32_t const token_size){

 	//token_buffer = (struct _token_t*) malloc(nr_tokens*token_size);
	
	if(token_buffer == NULL){
  		xil_printf("token_buffer is NULL\n");
		return NULL;
	}
	else 
	{
		fifo_admin->capacity = nr_tokens;
		fifo_admin->element_size = token_size;
		fifo_admin->avail_spaces = nr_tokens;
		fifo_admin->avail_tokens = 0;
		fifo_admin->base_ptr = token_buffer;

		fifo_admin->wr_cross = 0;
		fifo_admin->rd_cross = 0;
		fifo_admin->wr_idx = 0;
		fifo_admin->rd_idx = 0;
		fifo_admin->init = true;
		return fifo_admin;
	}
}

//MYFIFO_DESTROY//
extern void myfifo_destroy(myfifo_t volatile * const fifo_admin){

  	xil_printf("Destroy called!!\n");
	fifo_admin->init = false;
}


//MYFIFO_INITIALIZED//	
extern bool myfifo_initialized(myfifo_t volatile const * const fifo_admin){
	return fifo_admin->init;

}



//------------------------------- PRODUCER -------------------------//

//MYFIFO CLAIM SPACE//	
extern void volatile * myfifo_claim_space(myfifo_t volatile * const fifo_admin){
	
	while(fifo_admin->avail_spaces == 0); //Blocking
	uint32_t *base = (uint32_t*) fifo_admin->base_ptr;
	void volatile *space_ptr = base + (fifo_admin->wr_idx * fifo_admin->element_size);	
	
	//xil_printf("Space pointer: 0x%p\n", space_ptr);
	
	if(fifo_admin->wr_idx +1 == fifo_admin->capacity){ ~fifo_admin->wr_cross;}		
	fifo_admin->wr_idx = (fifo_admin->wr_idx+1)% fifo_admin->capacity;//next available space
	return space_ptr; 
	
}


//MYFIFO RELEASE TOKEN//	
extern void myfifo_release_token(myfifo_t volatile * const fifo_admin){
 	// No check needed as only if claim happens correctly, can you release.
	fifo_admin->avail_tokens = myfifo_tokens(fifo_admin);
	//xil_printf("Avail Tokens after release:%d\n", fifo_admin->avail_tokens);
}


//-------------------------------CONSUMER -------------------------//


//MYFIFO CLAIM TOKEN//	

extern void volatile * myfifo_claim_token(myfifo_t volatile * const fifo_admin){

	while (fifo_admin->avail_tokens==0); //Blocking
	uint32_t *base = (uint32_t*)fifo_admin->base_ptr;
	void volatile *token_ptr = base + (fifo_admin->rd_idx * fifo_admin->element_size);
	//xil_printf("Token pointer: 0x%p\n", token_ptr);
	
	if(fifo_admin->rd_idx + 1 == fifo_admin->capacity) ~fifo_admin->rd_cross;
	fifo_admin->rd_idx = (fifo_admin->rd_idx + 1)% fifo_admin->capacity;//next available space
	return token_ptr; 

}

extern void myfifo_release_space(myfifo_t volatile * const fifo_admin){
	fifo_admin->avail_spaces = myfifo_spaces(fifo_admin);
}


//------------------------------- GENERIC -------------------------//

// MYFIFO TOKENS//	

extern uint32_t myfifo_tokens(myfifo_t volatile const * const fifo_admin){
	uint32_t num_tokens=0;
	if(fifo_admin->wr_idx > fifo_admin->rd_idx){
		num_tokens = (fifo_admin->wr_idx - fifo_admin->rd_idx);
	}
	else if(fifo_admin->rd_idx > fifo_admin->wr_idx){
		num_tokens = fifo_admin->capacity - (fifo_admin->rd_idx - fifo_admin->wr_idx);
	}
	else {
		if(fifo_admin->rd_cross & fifo_admin->wr_cross){
			num_tokens =0;
		}
		else {// for both (rd_cross == wr_cross and rd_cross>wr_cross)
			num_tokens = fifo_admin->capacity;
		}
	
	}
	return num_tokens;
}

   
//MYFIFO SPACES//	
extern uint32_t myfifo_spaces(myfifo_t volatile const * const fifo_admin){
	uint32_t num_spaces= 0;
	if(fifo_admin->wr_idx > fifo_admin->rd_idx){
		num_spaces = (fifo_admin->capacity - (fifo_admin->wr_idx - fifo_admin->rd_idx));
	}
	else if(fifo_admin->rd_idx>fifo_admin->wr_idx){
		num_spaces = fifo_admin->rd_idx - fifo_admin->wr_idx;
	}
	else {
		if(fifo_admin->rd_cross & fifo_admin->wr_cross){
			num_spaces=0;
		}
		else {// for both (rd_cross == wr_cross and rd_cross>wr_cross)
			num_spaces = fifo_admin->capacity;
		}
	
	}
	return num_spaces;
}



