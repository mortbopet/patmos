//#include <string.h>
#include <machine/spm.h>
#include "include/bootable.h"
#include "include/patio.h"
#include "libnoc/noc.h"

#define __IO_CALC_ADDRESS_DYNAMIC(BASE, BANK, OFFSET) \
  ((void _SPM *)(((unsigned char*)BASE) + (BANK) + (OFFSET)))

#define ARGO_CONFIG_RD32(BANK, OFFSET) \
  *((volatile unsigned long int _SPM *)(__IO_CALC_ADDRESS_DYNAMIC ((NOC_DMA_BASE), (BANK), (OFFSET))))

#define ARGO_CONFIG_RD16(BANK, OFFSET) \
  *((volatile unsigned short int _SPM *)(__IO_CALC_ADDRESS_DYNAMIC ((NOC_DMA_BASE), (BANK), (OFFSET))))

#define ARGO_CONFIG_RD8(BANK, OFFSET) \
  *((volatile unsigned char _SPM *)(__IO_CALC_ADDRESS_DYNAMIC ((NOC_DMA_BASE), (BANK), (OFFSET))))



#define ARGO_CONFIG_WR32(BANK, OFFSET, DATA) \
  *((volatile unsigned long int _SPM *)(__IO_CALC_ADDRESS_DYNAMIC ((NOC_DMA_BASE), (BANK), (OFFSET)))) = (DATA)

#define ARGO_CONFIG_WR16(BANK, OFFSET, DATA) \
  *((volatile unsigned short int _SPM *)(__IO_CALC_ADDRESS_DYNAMIC ((NOC_DMA_BASE), (BANK), (OFFSET)))) = (DATA)

#define ARGO_CONFIG_WR8(BANK, OFFSET, DATA) \
  *((volatile unsigned char _SPM *)(__IO_CALC_ADDRESS_DYNAMIC ((NOC_DMA_BASE), (BANK), (OFFSET)))) = (DATA)


#define COM_SPM_RD32(OFFSET) \
  *((volatile unsigned long int _SPM *)(__IO_CALC_ADDRESS_DYNAMIC ((NOC_SPM_BASE), (0), (OFFSET))))

#define COM_SPM_WR32(OFFSET, DATA) \
  *((volatile unsigned long int _SPM *)(__IO_CALC_ADDRESS_DYNAMIC ((NOC_SPM_BASE), (0), (OFFSET)))) = (DATA)


#define OFFSET_WIDTH  (11+2)
#define BANK(ID)      (ID<<OFFSET_WIDTH)

#define DMA_BANK      BANK(0)
#define SCHED_BANK    BANK(1)
#define TDM_BANK      BANK(2)
#define MC_BANK       BANK(3)
#define IRQ_BANK      BANK(4)
#define DATA_BANK     BANK(5)
#define ERROR_BANK    BANK(6)
#define PERF_BANK     BANK(7)

int main(void) __attribute__((noreturn));

int main() {
  
  int tmp1 = 0;
  int tmp2 = 0;
  // Reading the 64 bit counter
  tmp1 = ARGO_CONFIG_RD32(TDM_BANK,3<<2);
  tmp2 = ARGO_CONFIG_RD32(TDM_BANK,2<<2);


  if (tmp1 == 0) {
    WRITE("ERROR0\n",7); 
  }
  if (tmp2 == tmp1) {
    WRITE("ERROR1\n",7); 
  } else {
    WRITE("OK1\n",4);
  }

  // Setting the first entry of the schedule table
  //        Route        | DMA_num    | Pktlen| t2n
  tmp1 = 0x77 << (8+3+5) | 0 << (3+5) | 1 << 5| 5;
  ARGO_CONFIG_WR32(SCHED_BANK,0<<2,tmp1);
  tmp2 = ARGO_CONFIG_RD32(SCHED_BANK,0<<2);

  if (tmp2 == tmp1) {
    WRITE("OK2\n",4); 
  }

  // Write to the COM_SPM
  COM_SPM_WR32(0,0x11223344);
  // Setting the first entry of the DMA table
  //      Read_ptr| Header field
  //              | Pkt format | Write_ptr
  tmp1 =  0 << 16 | 0 << 14    | 0;
  //    Active bit | count
  tmp2 =  1 << 31  | 1;
  ARGO_CONFIG_WR32(DMA_BANK,0<<2,tmp1);
  ARGO_CONFIG_WR32(DMA_BANK,1<<2,tmp2);

  tmp2 = ARGO_CONFIG_RD32(DMA_BANK,0<<2);

  if (tmp2 == tmp1) {
    WRITE("OK3\n",4); 
  } else {
    WRITE("ERROR3\n",7); 
  }

  // Setting stbl_maxp1 to 2 and stbl_min to 0
  tmp1 = 4 << 16 | 0;
  ARGO_CONFIG_WR32(MC_BANK,8<<2,tmp1);
  tmp2 = ARGO_CONFIG_RD32(MC_BANK,8<<2);

  if (tmp2 == tmp1) {
    WRITE("OK4\n",4); 
  }

  // Setting run to 1
  tmp1 = 1;
  ARGO_CONFIG_WR32(TDM_BANK,128<<2,tmp1);
  asm volatile ("nop");
  tmp2 = ARGO_CONFIG_RD32(TDM_BANK,128<<2);

  if (tmp2 == tmp1) {
    WRITE("OK5\n",4); 
  }

}
