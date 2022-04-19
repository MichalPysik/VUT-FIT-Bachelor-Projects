/**********************************************************************/
/*   ____  ____                                                       */
/*  /   /\/   /                                                       */
/* /___/  \  /                                                        */
/* \   \   \/                                                       */
/*  \   \        Copyright (c) 2003-2009 Xilinx, Inc.                */
/*  /   /          All Right Reserved.                                 */
/* /---/   /\                                                         */
/* \   \  /  \                                                      */
/*  \___\/\___\                                                    */
/***********************************************************************/

/* This file is designed for use with ISim build 0x79f3f3a8 */

#define XSI_HIDE_SYMBOL_SPEC true
#include "xsi.h"
#include <memory.h>
#ifdef __GNUC__
#include <stdlib.h>
#else
#include <malloc.h>
#define alloca _alloca
#endif
extern char *STD_STANDARD;
static const char *ng1 = "C:/FitkitSVN/apps/ivh/Projekt/fpga/counter.vhd";
extern char *IEEE_P_3620187407;

unsigned char ieee_p_3620187407_sub_2546418145_3965413181(char *, char *, char *, int );
char *ieee_p_3620187407_sub_436279890_3965413181(char *, char *, char *, char *, int );


int work_a_3903431996_3212880686_sub_3508650715_3057020925(char *t1, int t2)
{
    char t3[144];
    char t4[8];
    char t8[8];
    char t14[8];
    int t0;
    char *t5;
    char *t6;
    char *t7;
    char *t9;
    char *t10;
    char *t11;
    char *t12;
    char *t13;
    char *t15;
    char *t16;
    char *t17;
    char *t18;
    char *t19;
    int t20;
    unsigned char t21;
    char *t22;
    int t23;
    int t24;
    char *t25;

LAB0:    t5 = (t3 + 4U);
    t6 = ((STD_STANDARD) + 240);
    t7 = (t5 + 52U);
    *((char **)t7) = t6;
    t9 = (t5 + 36U);
    *((char **)t9) = t8;
    *((int *)t8) = 0;
    t10 = (t5 + 48U);
    *((unsigned int *)t10) = 4U;
    t11 = (t3 + 72U);
    t12 = ((STD_STANDARD) + 240);
    t13 = (t11 + 52U);
    *((char **)t13) = t12;
    t15 = (t11 + 36U);
    *((char **)t15) = t14;
    *((int *)t14) = 1;
    t16 = (t11 + 48U);
    *((unsigned int *)t16) = 4U;
    t17 = (t4 + 4U);
    *((int *)t17) = t2;

LAB2:    t18 = (t11 + 36U);
    t19 = *((char **)t18);
    t20 = *((int *)t19);
    t21 = (t20 <= t2);
    if (t21 != 0)
        goto LAB3;

LAB5:    t6 = (t5 + 36U);
    t7 = *((char **)t6);
    t20 = *((int *)t7);
    t0 = t20;

LAB1:    return t0;
LAB3:    t18 = (t11 + 36U);
    t22 = *((char **)t18);
    t23 = *((int *)t22);
    t24 = (t23 * 2);
    t18 = (t11 + 36U);
    t25 = *((char **)t18);
    t18 = (t25 + 0);
    *((int *)t18) = t24;
    t6 = (t5 + 36U);
    t7 = *((char **)t6);
    t20 = *((int *)t7);
    t23 = (t20 + 1);
    t6 = (t5 + 36U);
    t9 = *((char **)t6);
    t6 = (t9 + 0);
    *((int *)t6) = t23;
    goto LAB2;

LAB4:;
LAB6:;
}

static void work_a_3903431996_3212880686_p_0(char *t0)
{
    char t24[16];
    char *t1;
    char *t2;
    unsigned char t3;
    unsigned char t4;
    char *t5;
    char *t6;
    char *t7;
    char *t8;
    unsigned char t9;
    unsigned char t10;
    unsigned char t11;
    unsigned char t12;
    char *t13;
    char *t14;
    int t15;
    unsigned char t16;
    char *t17;
    char *t18;
    char *t19;
    char *t20;
    unsigned int t21;
    unsigned int t22;
    unsigned int t23;

LAB0:    xsi_set_current_line(55, ng1);
    t1 = (t0 + 684U);
    t2 = *((char **)t1);
    t3 = *((unsigned char *)t2);
    t4 = (t3 == (unsigned char)3);
    if (t4 != 0)
        goto LAB2;

LAB4:    t1 = (t0 + 568U);
    t4 = xsi_signal_has_event(t1);
    if (t4 == 1)
        goto LAB7;

LAB8:    t3 = (unsigned char)0;

LAB9:    if (t3 != 0)
        goto LAB5;

LAB6:
LAB3:    t1 = (t0 + 1972);
    *((int *)t1) = 1;

LAB1:    return;
LAB2:    xsi_set_current_line(56, ng1);
    t1 = (t0 + 2016);
    t5 = (t1 + 32U);
    t6 = *((char **)t5);
    t7 = (t6 + 40U);
    t8 = *((char **)t7);
    *((unsigned char *)t8) = (unsigned char)2;
    xsi_driver_first_trans_fast_port(t1);
    xsi_set_current_line(57, ng1);
    t1 = xsi_get_transient_memory(22U);
    memset(t1, 0, 22U);
    t2 = t1;
    memset(t2, (unsigned char)2, 22U);
    t5 = (t0 + 1220U);
    t6 = *((char **)t5);
    t5 = (t6 + 0);
    memcpy(t5, t1, 22U);
    goto LAB3;

LAB5:    xsi_set_current_line(59, ng1);
    t2 = (t0 + 1220U);
    t6 = *((char **)t2);
    t2 = (t0 + 3460U);
    t12 = ieee_p_3620187407_sub_2546418145_3965413181(IEEE_P_3620187407, t6, t2, 0);
    if (t12 == 1)
        goto LAB13;

LAB14:    t7 = (t0 + 1220U);
    t8 = *((char **)t7);
    t7 = (t0 + 3460U);
    t13 = (t0 + 1084U);
    t14 = *((char **)t13);
    t15 = *((int *)t14);
    t16 = ieee_p_3620187407_sub_2546418145_3965413181(IEEE_P_3620187407, t8, t7, t15);
    t11 = t16;

LAB15:    if (t11 != 0)
        goto LAB10;

LAB12:    xsi_set_current_line(64, ng1);
    t1 = (t0 + 2016);
    t2 = (t1 + 32U);
    t5 = *((char **)t2);
    t6 = (t5 + 40U);
    t7 = *((char **)t6);
    *((unsigned char *)t7) = (unsigned char)2;
    xsi_driver_first_trans_fast_port(t1);
    xsi_set_current_line(65, ng1);
    t1 = (t0 + 1220U);
    t2 = *((char **)t1);
    t1 = (t0 + 3460U);
    t5 = ieee_p_3620187407_sub_436279890_3965413181(IEEE_P_3620187407, t24, t2, t1, 1);
    t6 = (t0 + 1220U);
    t7 = *((char **)t6);
    t6 = (t7 + 0);
    t8 = (t24 + 12U);
    t21 = *((unsigned int *)t8);
    t22 = (1U * t21);
    memcpy(t6, t5, t22);

LAB11:    goto LAB3;

LAB7:    t2 = (t0 + 592U);
    t5 = *((char **)t2);
    t9 = *((unsigned char *)t5);
    t10 = (t9 == (unsigned char)3);
    t3 = t10;
    goto LAB9;

LAB10:    xsi_set_current_line(60, ng1);
    t13 = (t0 + 2016);
    t17 = (t13 + 32U);
    t18 = *((char **)t17);
    t19 = (t18 + 40U);
    t20 = *((char **)t19);
    *((unsigned char *)t20) = (unsigned char)3;
    xsi_driver_first_trans_fast_port(t13);
    xsi_set_current_line(61, ng1);
    t1 = xsi_get_transient_memory(22U);
    memset(t1, 0, 22U);
    t2 = t1;
    memset(t2, (unsigned char)2, 22U);
    t5 = (t0 + 1220U);
    t6 = *((char **)t5);
    t5 = (t6 + 0);
    memcpy(t5, t1, 22U);
    xsi_set_current_line(62, ng1);
    t1 = (t0 + 1220U);
    t2 = *((char **)t1);
    t15 = (0 - 21);
    t21 = (t15 * -1);
    t22 = (1U * t21);
    t23 = (0 + t22);
    t1 = (t2 + t23);
    *((unsigned char *)t1) = (unsigned char)3;
    goto LAB11;

LAB13:    t11 = (unsigned char)1;
    goto LAB15;

}


extern void work_a_3903431996_3212880686_init()
{
	static char *pe[] = {(void *)work_a_3903431996_3212880686_p_0};
	static char *se[] = {(void *)work_a_3903431996_3212880686_sub_3508650715_3057020925};
	xsi_register_didat("work_a_3903431996_3212880686", "isim/proj3_isim.exe.sim/work/a_3903431996_3212880686.didat");
	xsi_register_executes(pe);
	xsi_register_subprogram_executes(se);
}
