#ifdef __cplusplus
extern "C" {
#endif

/*

NOTICE:
This document contains information that is proprietary to RADVision LTD.
No part of this publication may be reproduced in any form whatsoever without 
written prior approval by RADVision LTD.

RADVision LTD reserves the right to revise this publication and make changes 
without obligation to notify any person of such revisions or changes.

*/ 

/****************************************************************************
  
  netutils.c  --  Network utilities

  Module Author: Oz Solomonovich
  This Comment:  26-Dec-1996

  Abstract:      IP and network byte order conversion utilities.  
  
  Platforms:     All platforms supported.  The network byte order conversion
                 utilities determine the host byte order at run-time.

  Known Bugs:    None.

****************************************************************************/

#include <stdio.h>
#include <netutils.h>

#define CHAR(i) ((char *)(&i))

BOOL isMulticastIP(UINT32 ip)
{
#define IN_CLASSD(i) (((i) & 0xf0000000ul) == 0xe0000000ul)
    ip=rv_ntohl(ip);
    return IN_CLASSD(ip);
}

UINT32 rv_htonl(UINT32 host)
{
    UINT32 order = 0x00010203, net=0;
    int i;
    
    for (i = 0; i < 4; i++)
        CHAR(net)[i] = CHAR(host)[CHAR(order)[i]];
    
    return net;
}

UINT32 rv_ntohl(UINT32 net)
{
    UINT32 order = 0x00010203, host=0;
    int i;
    
    for (i = 0; i < 4; i++)
        CHAR(host)[CHAR(order)[i]] = CHAR(net)[i];
    
    return host;
}

UINT16 rv_htons(UINT16 host)
{
    UINT16 order = 0x0001, net=0;
    int i;
    
    for (i = 0; i < 2; i++)
        CHAR(net)[i] = CHAR(host)[CHAR(order)[i]];
    
    return net;
}

UINT16 rv_ntohs(UINT16 net)
{
    UINT16 order = 0x0001, host=0;
    int i;
    
    for (i = 0; i < 2; i++)
        CHAR(host)[CHAR(order)[i]] = CHAR(net)[i];
    
    return host;
}


UINT32 ip_to_uint32(const char *cp)
{
    const char *p = cp;
    UINT32 addr[4], maxvals[4];
    UINT32 net_addr = 0, last_val;
    int n = 0, i = 0;
    
    memset(maxvals, 0xFF, sizeof maxvals);
    
    for (;;)
    {
        if (*p == 0 || *p == '.' || n >= 4)
            return CONVERSION_ERROR;
        
        addr[n]  = 0;
        last_val = 0;
        
        if (n)
        {
            maxvals[n] = maxvals[n - 1] >> 8;
            maxvals[n - 1] = 255;
        }
        
        do 
        {
            if (!((*p >= '0')  &&  (*p <= '9')))
                return CONVERSION_ERROR;
            
            addr[n] *= 10;
            addr[n] += (*p - '0');
            if (addr[n] < last_val)  /* overflow check  */
                return CONVERSION_ERROR;
            last_val = addr[n];
            
            p++;
            
            if (*p == 0) goto stop;
        } while (*p != '.');
        
        p++;
        n++;
    } 
    
stop:
    
    for (i = 0; i <= n; i++)
        if (addr[n] > maxvals[n])
            return CONVERSION_ERROR;
        
        net_addr = addr[n];
        if (n >= 1) net_addr += (addr[0] << 24);
        if (n >= 2) net_addr += (addr[1] << 16);
        if (n >= 3) net_addr += (addr[2] <<  8);
        
        return rv_htonl(net_addr);
}


void uint32_to_ip(UINT32 addr, char *ip)
{
    char ip_buffer[16];
    int curr_num, curr_dig;
    UINT32 num;
    char *p = &(ip_buffer[14]);
    
    addr = rv_ntohl(addr);
    strcpy(ip_buffer, "xxx.xxx.xxx.xxx");    
    for (curr_num = 0; curr_num <= 3; curr_num++)
    {
        num = addr & 0xFF; 
        addr >>= 8;
        for (curr_dig = 0; curr_dig <= 2; curr_dig++)
        {
            *p = (char)(((char)(num % 10) + '0'));
            num /= 10;
            p--;
            if (num == 0)
                break;
        }
        if (curr_num < 3)
            *p-- = '.';
    }
    strcpy(ip,(p+1));
}

#ifndef VXD
void ATMaddr_to_string(BYTE * atmaddr, int length, char outstring[50])
{
    int    i;
    int    points[10] ;
    int  * point = points;
    
    points[0] = 1 ;
    points[1] = 2;
    points[2] = 1;
    points[3] = 3;
    points[4] = 2;
    points[5] = 2;
    points[6] = 2;
    points[7] = 6;
    points[8] = 1;
    points[9] = 0;

    for (i=0; i<length; i++, outstring+=2 ) 
    {
        if (*point && i==*point) {
            *outstring = '.';
            outstring++;
            point++;
            *point +=i;
        }
        sprintf (outstring,"%02x",atmaddr[i]);
    }
}

int string_to_ATMaddr(char * string, BYTE * atmaddr)
{
    int  i, j, x;
    char str[5] ;
    
    strcpy(str, "0xZZ");

    
    for (j=i=0; string[i] && string[i+1] ; i++)
    {
        if (string[i]!='.' &&  string[i]!=' ') {
            str[2] = string[i];
            str[3] = string[i+1];
            /* atmaddr[j] = atoi(str); */
            sscanf(str,"%4x",&x);
            atmaddr[j] = (unsigned char)x;
            j++;
            i++;
        }
    }
    return j;
}
#endif

#ifdef __cplusplus
}
#endif