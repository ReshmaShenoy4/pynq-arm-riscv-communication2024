
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <timers.h>
#include <xil_printf.h>
#include <platform.h>
#include <vep_shared_memory_regions.h>
#include <myfifo.h>


#define BUFFER_MEM vep_memshared0_shared_region
//#define ADMIN_MEM  vep_tile0_partition1_shared_region 
#define ADMIN_MEM  BUFFER_MEM 

int main(void)
{
	//uint32_t nr_tokens = FIFO_DEPTH;
	//uint32_t token_size = sizeof(token_t);
	BUFFER_MEM->ptr = (void*) malloc(FIFO_DEPTH*sizeof(token_t));
	xil_printf("running on tile %d partition %d\n", TILE_ID, PARTITION_ID);

	if (myfifo_init(&ADMIN_MEM->fifo_admin, (void *) BUFFER_MEM->ptr, FIFO_DEPTH, sizeof(token_t)) == NULL) {
 		xil_printf("fifo_init failed\n");
 		return EXIT_FAILURE;
 	}
 	myfifo_print_status(&ADMIN_MEM->fifo_admin);
 	for (uint32_t cnt = 0;cnt<5; cnt++) {
 		token_t volatile * const space = myfifo_claim_space(&ADMIN_MEM->fifo_admin);
 		space->data = cnt;
 		space->timestamp = read_global_timer();
 		myfifo_release_token(&ADMIN_MEM->fifo_admin);
 		myfifo_print_status(&ADMIN_MEM->fifo_admin);
 	
	}

 	//myfifo_print_status(&ADMIN_MEM->fifo_admin);	
	myfifo_destroy(&ADMIN_MEM->fifo_admin);
 	myfifo_print_status(&ADMIN_MEM->fifo_admin);	
	return 0;
}
