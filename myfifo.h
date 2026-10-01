#ifndef _MYFIFOH_
#define _MYFIFOH_

#include <stdint.h>
#include <stdbool.h>

typedef volatile struct _myfifo_t {
  bool init;
  
  uint32_t element_size; // each element that will be put inside a FIFO
  uint32_t capacity;
  uint32_t  avail_spaces;
  uint32_t  avail_tokens;

  //uint32_t wr_cross;
  //uint32_t rd_cross;
  bool wr_cross;
  bool rd_cross;
  void volatile * base_ptr;
  uint32_t wr_idx; // next available space to write
  uint32_t rd_idx; // next available space to write
} myfifo_t;

// token_buffer must point to an array containing nr_tokens that are aligned
// function returns NULL on failure and fifo_admin on success
extern myfifo_t * myfifo_init(myfifo_t volatile * const fifo_admin, void volatile * const token_buffer, uint32_t const nr_tokens, uint32_t const token_size);

extern void myfifo_destroy(myfifo_t volatile * const fifo_admin);

// myfifo_initialized must return false
// - before myfifo_init has been called successfully, or
// - after myfifo_destroy has been called
// otherwise it returns true
extern bool myfifo_initialized(myfifo_t volatile const * const fifo_admin);

// do not change myfifo_print_status
extern void myfifo_print_status(myfifo_t volatile const * const fifo_admin);

// use myfifo_spaces/tokens to poll, for non-blocking behaviour
// myfifo_spaces/tokens returns the number of spaces/tokens that can still be claimed
extern uint32_t myfifo_spaces(myfifo_t volatile const * const fifo_admin);
extern uint32_t myfifo_tokens(myfifo_t volatile const * const fifo_admin);

// the claim/release functions are blocking,
// i.e. return when space/token has been claimed/released
// the claim functions return a pointer to the space/token
extern void volatile * myfifo_claim_space(myfifo_t volatile * const fifo_admin);
extern void myfifo_release_space(myfifo_t volatile * const fifo_admin);

extern void myfifo_release_token(myfifo_t volatile * const fifo_admin);
extern void volatile * myfifo_claim_token(myfifo_t volatile * const fifo_admin);


// blocking functions; write/read = claim + memcpy + release
// can only write/read when no spaces/tokens have been claimed
extern void myfifo_write_token(myfifo_t volatile * const fifo_admin, void const * const token_to_write);
extern void myfifo_read_token(myfifo_t volatile * const fifo_admin, void * const new_token);

#endif

