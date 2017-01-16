#ifndef __UTILS_H__
#define __UTILS_H__





void utils_get_sys_time_s(char s_time[13]);
void utils_set_sys_time_s(char s_time[13]);
void utils_notify_printf(const unsigned int notify);
void utils_string_to_bcd(unsigned char *string, unsigned char *bcd);
void utils_time_to_bcd_yymmddhhMMss(unsigned int *time, unsigned char tm[6]);
void utils_set_systime_bcd(unsigned char tm[7]);
void utils_tm_to_bcd_time(struct tm *ptm, unsigned char s_time[7]);


#endif


