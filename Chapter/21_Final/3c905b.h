#ifndef BELLONA_3COM905B_NIC_HEADER_jj
#define BELLONA_3COM905B_NIC_HEADER_jj

extern int get_3c905b_mac_address( unsigned short *pMac );

extern int find_3c905b_nic();

extern int init_3c905b( UINT16 *pMacWord );

// dump upds
extern int dump_upds();

#endif