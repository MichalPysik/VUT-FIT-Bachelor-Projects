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
static const char *ng0 = "C:/FitkitSVN/apps/ivh/Projekt/fpga/display.vhd";
extern char *IEEE_P_3620187407;

char *ieee_p_3620187407_sub_436279890_3965413181(char *, char *, char *, char *, int );


static void work_a_1474961982_1142133938_p_0(char *t0)
{
    char t20[16];
    char *t1;
    char *t2;
    unsigned char t3;
    unsigned char t4;
    char *t5;
    char *t6;
    char *t7;
    char *t8;
    char *t9;
    unsigned int t10;
    unsigned int t11;
    unsigned int t12;
    unsigned char t13;
    unsigned char t14;
    unsigned char t15;
    unsigned char t16;
    int t17;
    unsigned char t18;
    int t19;
    char *t21;
    int t22;
    char *t23;

LAB0:    xsi_set_current_line(57, ng0);
    t1 = (t0 + 684U);
    t2 = *((char **)t1);
    t3 = *((unsigned char *)t2);
    t4 = (t3 == (unsigned char)3);
    if (t4 != 0)
        goto LAB2;

LAB4:    t1 = (t0 + 776U);
    t2 = *((char **)t1);
    t4 = *((unsigned char *)t2);
    t13 = (t4 == (unsigned char)3);
    if (t13 == 1)
        goto LAB7;

LAB8:    t3 = (unsigned char)0;

LAB9:    if (t3 != 0)
        goto LAB5;

LAB6:
LAB3:    t1 = (t0 + 2700);
    *((int *)t1) = 1;

LAB1:    return;
LAB2:    xsi_set_current_line(58, ng0);
    t1 = (t0 + 1660U);
    t5 = *((char **)t1);
    t1 = (t5 + 0);
    *((int *)t1) = 0;
    xsi_set_current_line(59, ng0);
    t1 = (t0 + 5088);
    t5 = (t0 + 2760);
    t6 = (t5 + 32U);
    t7 = *((char **)t6);
    t8 = (t7 + 40U);
    t9 = *((char **)t8);
    memcpy(t9, t1, 4U);
    xsi_driver_first_trans_fast(t5);
    xsi_set_current_line(60, ng0);
    t1 = (t0 + 592U);
    t2 = *((char **)t1);
    t10 = (127 - 7);
    t11 = (t10 * 1U);
    t12 = (0 + t11);
    t1 = (t2 + t12);
    t5 = (t0 + 2796);
    t6 = (t5 + 32U);
    t7 = *((char **)t6);
    t8 = (t7 + 40U);
    t9 = *((char **)t8);
    memcpy(t9, t1, 8U);
    xsi_driver_first_trans_fast(t5);
    goto LAB3;

LAB5:    xsi_set_current_line(62, ng0);
    t5 = (t0 + 1420U);
    t6 = *((char **)t5);
    t15 = *((unsigned char *)t6);
    t16 = (t15 == (unsigned char)3);
    if (t16 != 0)
        goto LAB10;

LAB12:
LAB11:    goto LAB3;

LAB7:    t1 = (t0 + 752U);
    t14 = xsi_signal_has_event(t1);
    t3 = t14;
    goto LAB9;

LAB10:    xsi_set_current_line(63, ng0);
    t5 = (t0 + 1660U);
    t7 = *((char **)t5);
    t17 = *((int *)t7);
    t18 = (t17 == 120);
    if (t18 != 0)
        goto LAB13;

LAB15:    xsi_set_current_line(66, ng0);
    t1 = (t0 + 1660U);
    t2 = *((char **)t1);
    t17 = *((int *)t2);
    t19 = (t17 + 8);
    t1 = (t0 + 1660U);
    t5 = *((char **)t1);
    t1 = (t5 + 0);
    *((int *)t1) = t19;

LAB14:    xsi_set_current_line(68, ng0);
    t1 = (t0 + 1236U);
    t2 = *((char **)t1);
    t1 = (t0 + 4852U);
    t5 = ieee_p_3620187407_sub_436279890_3965413181(IEEE_P_3620187407, t20, t2, t1, 1);
    t6 = (t0 + 2760);
    t7 = (t6 + 32U);
    t8 = *((char **)t7);
    t9 = (t8 + 40U);
    t21 = *((char **)t9);
    memcpy(t21, t5, 4U);
    xsi_driver_first_trans_fast(t6);
    xsi_set_current_line(69, ng0);
    t1 = (t0 + 592U);
    t2 = *((char **)t1);
    t1 = (t0 + 1660U);
    t5 = *((char **)t1);
    t17 = *((int *)t5);
    t19 = (t17 + 7);
    t10 = (127 - t19);
    t1 = (t0 + 1660U);
    t6 = *((char **)t1);
    t22 = *((int *)t6);
    xsi_vhdl_check_range_of_slice(127, 0, -1, t19, t22, -1);
    t11 = (t10 * 1U);
    t12 = (0 + t11);
    t1 = (t2 + t12);
    t7 = (t0 + 2796);
    t8 = (t7 + 32U);
    t9 = *((char **)t8);
    t21 = (t9 + 40U);
    t23 = *((char **)t21);
    memcpy(t23, t1, 8U);
    xsi_driver_first_trans_fast(t7);
    goto LAB11;

LAB13:    xsi_set_current_line(64, ng0);
    t5 = (t0 + 1660U);
    t8 = *((char **)t5);
    t5 = (t8 + 0);
    *((int *)t5) = 0;
    goto LAB14;

}

static void work_a_1474961982_1142133938_p_1(char *t0)
{
    char *t1;
    char *t2;
    char *t3;
    char *t4;
    char *t5;
    char *t6;
    char *t7;

LAB0:    xsi_set_current_line(75, ng0);

LAB3:    t1 = (t0 + 1236U);
    t2 = *((char **)t1);
    t1 = (t0 + 2832);
    t3 = (t1 + 32U);
    t4 = *((char **)t3);
    t5 = (t4 + 40U);
    t6 = *((char **)t5);
    memcpy(t6, t2, 4U);
    xsi_driver_first_trans_fast_port(t1);

LAB2:    t7 = (t0 + 2708);
    *((int *)t7) = 1;

LAB1:    return;
LAB4:    goto LAB2;

}

static void work_a_1474961982_1142133938_p_2(char *t0)
{
    char *t1;
    char *t2;
    char *t3;
    char *t4;
    char *t5;
    char *t6;
    char *t7;

LAB0:    xsi_set_current_line(76, ng0);

LAB3:    t1 = (t0 + 1328U);
    t2 = *((char **)t1);
    t1 = (t0 + 2868);
    t3 = (t1 + 32U);
    t4 = *((char **)t3);
    t5 = (t4 + 40U);
    t6 = *((char **)t5);
    memcpy(t6, t2, 8U);
    xsi_driver_first_trans_fast_port(t1);

LAB2:    t7 = (t0 + 2716);
    *((int *)t7) = 1;

LAB1:    return;
LAB4:    goto LAB2;

}


extern void work_a_1474961982_1142133938_init()
{
	static char *pe[] = {(void *)work_a_1474961982_1142133938_p_0,(void *)work_a_1474961982_1142133938_p_1,(void *)work_a_1474961982_1142133938_p_2};
	xsi_register_didat("work_a_1474961982_1142133938", "isim/proj3_isim.exe.sim/work/a_1474961982_1142133938.didat");
	xsi_register_executes(pe);
}
