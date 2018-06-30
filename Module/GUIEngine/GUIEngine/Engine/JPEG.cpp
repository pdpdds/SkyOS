/*
 * JPEG decoding engine for DCT-baseline
 *
 *      JPEGLS - Copyright(c) 2004 by Hajime Uchimura(nikutama@gmail.com)
 * 
 * history::
 * 2003/04/28 | added OSASK-GUI ( by H.Kawai ) 
 * 2003/05/12 | optimized DCT ( 20-bits fixed point, etc...) -> line 407-464 ( by I.Tak. )
 * 2009/11/21 | optimized to RGB565 ( by kkamagui )
 */
#include "JPEG.h"

// 지그재그 테이블 
int zigzag_table[]={
     0, 1, 8, 16,9, 2, 3,10,
    17,24,32,25,18,11, 4, 5,
    12,19,26,33,40,48,41,34,
    27,20,13, 6, 7,14,21,28,
    35,42,49,56,57,50,43,36,
    29,22,15,23,30,37,44,51,
    58,59,52,45,38,31,39,46,
    53,60,61,54,47,55,62,63,
    0
};

// -------------------------- I/O ----------------------------

// 버퍼에서 데이터를 반환하는 함수들
unsigned char get_byte(JPEG *jpeg)
{
    unsigned char c;
    c = jpeg->data[ jpeg->data_index ];
    jpeg->data_index++;
    
    return c;
}
int get_word(JPEG *jpeg)
{
    unsigned char h,l;
    h = get_byte(jpeg);
    l = get_byte(jpeg);
    return (h<<8)|l;
}

unsigned short get_bits(JPEG *jpeg,int bit)
{
    unsigned char  c;
    unsigned short ret;
    unsigned long  buff;
    int remain;
    
    buff   = jpeg->bit_buff;
    remain = jpeg->bit_remain;

    while(remain <= 16){
        c = get_byte(jpeg);
        if(c == 0xFF) {  // 마커 오류를 방지를 위해 FF - FF 00 는 제거
            get_byte(jpeg);
        }
        buff = (buff << 8) | c;
        remain += 8;
    }
    ret = (unsigned short)(buff>>(remain - bit))&((1<<bit)-1);
    remain -= bit;

    jpeg->bit_remain = remain;
    jpeg->bit_buff   = buff;
    
    return ret;
}

// ------------------------ JPEG 세그먼트 구현 -----------------

// 지원하지 않는 것은 버퍼에서 제외
void jpeg_skip(JPEG *jpeg)
{
    unsigned w;
    w = get_word(jpeg) - 2;
    jpeg->data_index += w;
}

// start of frame
int jpeg_sof(JPEG *jpeg)
{
    unsigned char c,n;
    int i,h,v;
    
    c = get_word(jpeg);

    c = get_byte(jpeg); // bpp
    jpeg->height = get_word(jpeg);
    jpeg->width  = get_word(jpeg);

    n = get_byte(jpeg);    // Num of compo
    jpeg->compo_count = n; // nf
    
    for(i=0;i<n;i++){
        jpeg->compo_id[i] = get_byte(jpeg);

        c = get_byte(jpeg);
        jpeg->compo_sample[i] = c;
        h = (c>>4) & 0x0F;
        v =  c     & 0x0F;

        if(jpeg->max_h < h)
            jpeg->max_h = h;
        if(jpeg->max_v < v)
            jpeg->max_v = v;
        
        jpeg->compo_h[i] = (c>>4)&0x0F;
        jpeg->compo_v[i] = c & 0x0F;

        jpeg->compo_qt[i] = get_byte(jpeg);
    }
    return 0;
}

// data restart interval
void jpeg_dri(JPEG *jpeg)
{
    get_word(jpeg);
    jpeg->interval = get_word(jpeg);
}

// define quantize table
int jpeg_dqt(JPEG *jpeg)
{
    unsigned char c;
    int i,j,v,size;
    
    size = get_word(jpeg) - 2;
    
    while(size>0) {
        
        c = get_byte(jpeg);
        size--;
        j = c & 7;
        if(j > jpeg->n_dqt)
            jpeg->n_dqt = j;

        if((c>>3)){
            // 16 bit DQT
            for(i=0;i<64;i++){
                v = get_word(jpeg);
                size-=2;
                jpeg->dqt[j][ i ] = (v>>8);
            }
        }
        else{
            //  8 bit DQT
            for(i=0;i<64;i++){
                v = get_byte(jpeg);
                size--;
                jpeg->dqt[j][ i ] = v;
            }
        }
    }
    return 0;
}

// define huffman table
int jpeg_dht(JPEG *jpeg)
{
    unsigned tc,th;
    unsigned code = 0;
    unsigned char val;
    int i,j,k,num,Li[17];
    int len;
    HUFF *table;
    
    len = get_word(jpeg) - 2;

    while(len > 0)
    {
        val = get_byte(jpeg);
        
        tc = (val>>4) & 0x0F; // 테이블 클래스(DC/AC 성분 선택)
        th =  val     & 0x0F; // 테이블 헤더(몇 번째 플레인인가?)

        table = (HUFF*)&(jpeg->huff[tc][th]);

        num = 0;
        for (i = 1; i <= 16; i++) {
            Li[i] = get_byte(jpeg);
            num += Li[i];
        }
        table->elem = num;

        // 코드 생성
        k=0;
        for(i=1;i<=16;i++) {
            for(j=0;j<Li[i];j++) {
                table->size[k++] = i;
            }
        }

        k=0;
        code=0;
        i = table->size[0];
        while(k<num) {
            while(table->size[k] == i){
                table->code[k++] = code++;
            }
            if(k>=num)
                break;
            do{
                code = code << 1;
                i++;
            }while(table->size[k] != i);
        }
        
        for(k=0;k<num;k++)
            table->value[k] = get_byte(jpeg);

        len = len - 18 - num;
    }
    return 0;
}

// start of scan
void jpeg_sos(JPEG *jpeg)
{
    int i;
    unsigned char c;
    get_word(jpeg);

    jpeg->scan_count = get_byte(jpeg);

    for(i=0;i<jpeg->scan_count;i++) {
        c = get_byte(jpeg);
        //printf(" id :%d\n",c);
        jpeg->scan_id[i] = c;

        c = get_byte(jpeg);
        jpeg->scan_dc[i] = c >> 4;   // DC Huffman Table
        jpeg->scan_ac[i] = c & 0x0F; // AC Huffman Table
    }
    //3 bytes skip
    get_byte(jpeg);
    get_byte(jpeg);
    get_byte(jpeg);
}

void jpeg_idct_init(void);

/**
 *  JPEG 이미지 파일의 전체가 담긴 파일 버퍼와 크기를 이용해서 JPEG 자료구조를 초기화
 *      파일 버퍼의 내용을 분석하여 이미지 전체의 크기와 기타 정보를 JPEG 자료구조에 삽입 
 */
bool kJPEGInit(JPEG *jpeg, BYTE* pbFileBuffer, DWORD dwFileSize)
{
    int i;
    unsigned char c;

    jpeg_idct_init();
    for(i=0;i<3;i++)
        jpeg->mcu_preDC[i]=0;
    jpeg->n_dqt = 0;

    jpeg->max_h = 0;
    jpeg->max_v = 0;
    jpeg->bit_remain = 0;
    jpeg->bit_buff   = 0;
    // DRI
    jpeg->interval = 0;
    
    // Data의 정보 설정
    jpeg->data_index = 0;
    jpeg->data_size = dwFileSize;
    jpeg->data = pbFileBuffer;
    
//    return 0;
//}
//
//int jpeg_header(JPEG *jpeg)
//{
//    unsigned char c;

    while(1)
    {
        if( jpeg->data_index > jpeg->data_size )
        {
            return FALSE;
        }
        c = get_byte(jpeg);

        if( jpeg->data_index > jpeg->data_size )
        {
            return FALSE;
        }
        c = get_byte(jpeg);

        switch(c)
        {
        case 0xD8:// printf("SOI\n");
            break;
        case 0xD9:// printf("EOI\n");
            return FALSE;
            break;
        case 0xC0:
            jpeg_sof(jpeg);
            break;
        case 0xC4:
            jpeg_dht(jpeg);
            break;
        case 0xDB:
            jpeg_dqt(jpeg);
            break;
        case 0xDD:
            jpeg_dri(jpeg);
            break;
        case 0xDA:
            jpeg_sos(jpeg);
            return TRUE;
        default:
            jpeg_skip(jpeg);
            break;
        }
    }
    return FALSE;
}

// MCU decode

// 디코딩
int jpeg_decode_init(JPEG *jpeg)
{
    int i,j;

    for(i=0;i<jpeg->scan_count;i++) {
        // i:scan
        for(j=0;j<jpeg->compo_count;j++) {
            // j:frame
            if(jpeg->scan_id[i] ==jpeg->compo_id[j]){
                //printf("scan %d is frame %d\n",i,j);
                jpeg->scan_h[i] = jpeg->compo_h[j];
                jpeg->scan_v[i] = jpeg->compo_v[j];
                jpeg->scan_qt[i]= jpeg->compo_qt[j];
                break;
            }
        }
        if(j>=jpeg->compo_count){
            return 1;
        }
    }
    
    jpeg->mcu_width  = jpeg->max_h * 8;
    jpeg->mcu_height = jpeg->max_v * 8;
    
    for(i=0;i<32*32*4;i++){
        jpeg->mcu_buf[i] = 0x80;
    }
    for(i=0;i<jpeg->scan_count;i++){
        jpeg->mcu_yuv[i] = jpeg->mcu_buf + i*32*32;
    }
    return 0;
}

// 허프만 1 코드 디코딩
int jpeg_huff_decode(JPEG *jpeg,int tc,int th)
{
    HUFF *h = &(jpeg->huff[tc][th]);
    int code,size,k,v;
    code = 0;
    size = 0;
    k = 0;

    while( size < 16 ){
        size++;
        v = get_bits(jpeg,1);
        if(v < 0){
            return v;
        }
        code = (code << 1) | v;

        while(h->size[k] == size){
            if(h->code[k] == code){
                return h->value[k];
            }
            k++;
        }
    }

    return -1;
}

// 역 DCT
int base_img[64][64];

void jpeg_idct_init(void)
{
    int u, v, m, n;
    int tmpm[8], tmpn[8];
    int cost[32];
    cost[ 0] =  32768; cost[ 1] =  32138; cost[ 2] =  30274; cost[ 3] =  27246; cost[ 4] =  23170; cost[ 5] =  18205; cost[ 6] =  12540; cost[ 7] =   6393;
    cost[ 8] =      0; cost[ 9] =  -6393; cost[10] = -12540; cost[11] = -18205; cost[12] = -23170; cost[13] = -27246; cost[14] = -30274; cost[15] = -32138;
    for (u = 0; u < 16; u++)
        cost[u + 16] = - cost[u];

    for (u = 0; u < 8; u++) {
        {
            int i=u, d=u*2;
            if (d == 0)
                i = 4;
            for (m = 0; m < 8; m++){
                tmpm[m] = cost[i]; // 컬럼 Cos 값
                i=(i+d)&31;
            }
        }
        for (v = 0; v < 8; v++) {
            {
                int i=v,d=v*2;
                if (d == 0)
                    i=4;
                for (n = 0; n < 8; n++){
                    tmpn[n] = cost[i]; // Row Cos 값
                    i=(i+d)&31;
                }
            }
            // 확장하여 기본 이미지 생성
            for (m = 0; m < 8; m++) {
                for (n = 0; n < 8; n++) {
                    base_img[u * 8 + v][m * 8 + n] = (tmpm[m] * tmpn[n])>>15;
                }
            }
        }
    }
    return;
}

void jpeg_idct(int *block, int *dest)
{
    int i, j ,k;

    for (i = 0; i < 64; i++)
        dest[i] = 0;

    for (i = 0; i < 64; i++) {
        k = block[i];
        if(k) { //k가 0인 경우는 제외
            for (j = 0; j < 64; j++) {
                dest[j] += k * base_img[i][j];
            }
        }
    }
    // 고정 소수점을 정수로 변환
    for (i = 0; i < 64; i++)
        dest[i] >>= 17;
    return;
}

// 디코딩된 숫자를 반환
int jpeg_get_value(JPEG *jpeg,int size) {
    int val = 0;
    if(size == 0)
        val = 0;
    else
    {
        val = get_bits(jpeg,size);
        if (!(val & (1<<(size-1))))
            val = val - (1 << size) + 1;
    }
    return val;
}

// ---- 블록 디코딩 ---
// 허프만 디코딩 + 역양자화 + 역 지그재그 
int jpeg_decode_huff(JPEG *jpeg,int scan,int *block)
{
    int size, val, run, index;
    int *pQt = (int *)(jpeg->dqt[jpeg->scan_qt[scan]]);
    
    // DC
    size = jpeg_huff_decode(jpeg,0,jpeg->scan_dc[scan]);
    if(size < 0)
        return 0;
    val = jpeg_get_value(jpeg,size);
    jpeg->mcu_preDC[scan] += val;
    block[0] = jpeg->mcu_preDC[scan] * pQt[0];

    //AC 디코딩
    index = 1;
    while(index<64)
    {
        size = jpeg_huff_decode(jpeg,1,jpeg->scan_ac[scan]);
        if(size < 0)
            break;
        // EOB
        if(size == 0)
            break;
        
        // RLE
        run  = (size>>4)&0xF;
        size = size & 0x0F;
        
        val = jpeg_get_value(jpeg,size);
        if(val>=0x10000) {
            // 마커 발견
            return val;
        }

        // ZRL
        while (run-- > 0)
            block[ zigzag_table[index++] ] = 0;
        
        block[ zigzag_table[index] ] = val * pQt[index];
        index++;
    }
    while(index<64)
        block[zigzag_table[index++]]=0;
    return 0;
}

// 블록 복원
// 리셈플링
void jpeg_mcu_bitblt(int *src,int *dest,int width,
                     int x0,int y0,int x1,int y1)
{
    int w,h;
    int x,y,x2,y2;
    w = x1 - x0;
    h = y1 - y0;
    
    for(y=y0;y<y1;y++) {
        y2 = (y-y0) * 8 / h;
        
        for(x=x0;x<x1;x++) {
            x2 = (x-x0) * 8 / w;
            
            dest[(y*width) + x] = src[(y2*8) + x2];
        }
    }
}

// MCU 한 개를 변환
int jpeg_decode_mcu(JPEG *jpeg)
{
    int scan,val;
    int h,v;
    int *p,hh,vv;
    int block[64],dest[64];

    // mcu_width x mcu_height 크기의 블록을 변환
    for(scan=0;scan<jpeg->scan_count;scan++)
    {
        hh = jpeg->scan_h[scan];
        vv = jpeg->scan_v[scan];
        for(v=0;v<vv;v++){
            for(h=0;h<hh;h++){

                // block (8x8) 디코딩 
                val = jpeg_decode_huff(jpeg , scan , block);
                
                // 역 DCT
                jpeg_idct(block,dest);

                // 리셈플링                
                // 쓰기 버퍼
                p = jpeg->mcu_buf + (scan * 32 * 32);
                
                // 확대 전송
                jpeg_mcu_bitblt(dest , p ,
                                jpeg->mcu_width,
                                jpeg->mcu_width * h / hh,
                                jpeg->mcu_height* v / vv,
                                jpeg->mcu_width * (h+1) / hh,
                                jpeg->mcu_height* (v+1) / vv);
            }
        }
    }
    return 0;
}

// YCrCb => RGB 변환
int jpeg_decode_yuv(JPEG *jpeg,int h,int v,COLOR *rgb)
{
    int x0,y0,x,y,x1,y1;
    int *py,*pu,*pv;
    int Y,U,V,k;
    int R,G,B;
    int mw,mh,w;
    
    mw = jpeg->mcu_width;
    mh = jpeg->mcu_height;
    w = jpeg->width;
    
    x0 = h * jpeg->max_h * 8;
    y0 = v * jpeg->max_v * 8;

    x1 = jpeg->width - x0;
    if(x1 > mw)
        x1 = mw;
    y1 = jpeg->height - y0;
    if(y1 > mh)
        y1 = mh;
    
    py = jpeg->mcu_buf;
    pu = jpeg->mcu_buf + 1024;
    pv = jpeg->mcu_buf + 2048;
    

    for(y=0;y<y1;y++){
        for(x=0;x<x1;x++){
            k = y*mw+x;
            
            Y = py[ k ];
            U = pu[ k ];
            V = pv[ k ];
            
            R = 128 + ((Y*0x1000 + V*0x166E) / 4096) * 1300 / 1000;
            R = (R & 0xffffff00) ? (R >> 24) ^ 0xff : R;

            G = 128 + ((Y*0x1000 - V*0x0B6C) / 4096) * 1300 / 1000;
            G = (G & 0xffffff00) ? (G >> 24) ^ 0xff : G;
            
            B = 128 + ((Y*0x1000 - V*4 + U*0x1C59) / 4096) * 1300 / 1000;
            B = (B & 0xffffff00) ? (B >> 24) ^ 0xff : B;
            
            // RGB888 -> RGB565로 변환
            rgb[(y0+y)*w + (x0+x)] = RGB( R, G, B);
        }
    }
    return 0;
}

/**
 *  JPEG 자료구조에 저장된 정보를 이용하여 디코딩한 결과를 출력 버퍼에 저장
 */
bool kJPEGDecode(JPEG *jpeg,COLOR* pstOutputBuffer)
{
    int h_unit,v_unit;
    int mcu_count,h,v;
    
    // MCU 크기 계산
    if(jpeg_decode_init(jpeg))
        return FALSE; // 오류
    
    h_unit = jpeg->width / jpeg->mcu_width;
    v_unit = jpeg->height/ jpeg->mcu_height;
    if((jpeg->width  % jpeg->mcu_width) > 0){
        h_unit++;
    }
    if((jpeg->height % jpeg->mcu_height) > 0){
        v_unit++;
    }
    
    // 1 블록 변환
    mcu_count = 0;
    for(v=0;v<v_unit;v++){
        for(h=0;h<h_unit;h++){
            mcu_count++;
            jpeg_decode_mcu(jpeg);
            jpeg_decode_yuv(jpeg,h,v,pstOutputBuffer);
            
            if(jpeg->interval > 0 && mcu_count >= jpeg->interval){
                
                // RST 마커를 제외(FF hoge)
                // hoge 뒤에 붙은 FF도 제외
                jpeg->bit_remain -= (jpeg->bit_remain & 7);
                jpeg->bit_remain -= 8;
                
                jpeg->mcu_preDC[0] = 0;
                jpeg->mcu_preDC[1] = 0;
                jpeg->mcu_preDC[2] = 0;
                mcu_count = 0;
                
            }
        }
    }
    
    return TRUE;
}
