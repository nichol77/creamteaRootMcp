#ifndef DEFS
#define DEFS

#define MAX_TARGET_MODULES    4        // Number of 64 channel target modules
#define NUM_TARGETS           4        // Number of target chips
#define NUM_CHANNELS          16       // Number of channels
#define NUM_TOTAL_CHANNELS    NUM_TARGETS*NUM_CHANNELS //64   
#define NUM_PIXELS            64 //64   
#define NUM_ROWS              8        // Rows per channel
#define NUM_COLS              32       // Columns per row
#define SAMPLES_PER_COL        512   //Amount of values per signal //was 64
//#define SAMPLES_PER_COL        64   //Amount of values per signal //was 64
//efine SAMPLES_PER_ROW       NUM_COLS*NUM_ROWS
#define NUM_SAMPLES SAMPLES_PER_COL
#define DATASIZE       NUM_CHANNELS * NUM_SAMPLES
//#define BUFFERSIZE    4140  //Size of buffer, need 5 more values due to headers and stuff.
//#ifdef READOUT_MCP_CPCI
//#define BUFFERSIZE    32850  //Size of buffer, need 5 more values due to headers and stuff, +16 extra things for fun
//#else
#define BUFFERSIZE    32810  //Size of buffer, need 5 more values due to headers and stuff.
//#endif
//#define VOLTS_PER_COUNT      0.001 * 2.7782   // old value: 0.001 * 0.6184
#define VOLTS_PER_COUNT      0.001 * 0.6184


#define DEFAULT_PEDESTAL_VALUE    256

#define CLEAR_EVENT_CPCI_BOARD_MASK    0x00000001
#define DISABLE_SYNC_USB_MASK          0x00000001
#define ENABLE_SYNC_USB_MASK           0x00010001
#define SOFT_TRIG_MASK                 0x00000002
#define TRIG_THRESH_MASK               0x00000003
#define ENABLE_PED_MASK                0x00010004
#define DISABLE_PED_MASK               0x00000004
#define PED_ROW_COL_MASK               0x00000005
#define WBIAS_MASK                     0x00000006
#define TRIG_POLARITY_POS              0x00000007
#define TRIG_POLARITY_NEG              0x00010007
#define TERM_MASK                      0x000f0008
#define TERM_BASE                      0x00000008


#define TERM_100_OHMS_SHIFT 18
#define TERM_1K_OHMS_SHIFT  17
#define TERM_10K_OHMS_SHIFT 16
#define TRIG_THRESH_SHIFT   16
#define WBIAS_SHIFT         16
#define PED_COL_SHIFT       20
#define PED_ROW_SHIFT       25


#define SAMPLING_SPEED_MASK 0x0006

#endif
