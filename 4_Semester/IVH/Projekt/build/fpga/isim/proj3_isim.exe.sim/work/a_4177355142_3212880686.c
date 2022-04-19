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
static const char *ng0 = "C:/FitkitSVN/apps/ivh/Projekt/fpga/cell.vhd";



static void work_a_4177355142_3212880686_p_0(char *t0)
{
    char *t1;
    char *t2;
    unsigned char t3;
    unsigned char t4;
    char *t5;
    unsigned char t6;
    char *t7;
    char *t8;
    char *t9;
    char *t10;
    unsigned char t11;
    unsigned char t12;
    unsigned char t13;
    unsigned char t14;
    unsigned char t15;
    unsigned char t16;
    char *t17;
    char *t18;
    char *t19;

LAB0:    xsi_set_current_line(36, ng0);
    t1 = (t0 + 684U);
    t2 = *((char **)t1);
    t3 = *((unsigned char *)t2);
    t4 = (t3 == (unsigned char)3);
    if (t4 != 0)
        goto LAB2;

LAB4:    t1 = (t0 + 592U);
    t2 = *((char **)t1);
    t4 = *((unsigned char *)t2);
    t6 = (t4 == (unsigned char)3);
    if (t6 == 1)
        goto LAB7;

LAB8:    t3 = (unsigned char)0;

LAB9:    if (t3 != 0)
        goto LAB5;

LAB6:
LAB3:    t1 = (t0 + 2328);
    *((int *)t1) = 1;

LAB1:    return;
LAB2:    xsi_set_current_line(37, ng0);
    t1 = (t0 + 776U);
    t5 = *((char **)t1);
    t6 = *((unsigned char *)t5);
    t1 = (t0 + 2380);
    t7 = (t1 + 32U);
    t8 = *((char **)t7);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = t6;
    xsi_driver_first_trans_fast(t1);
    goto LAB3;

LAB5:    xsi_set_current_line(39, ng0);
    t5 = (t0 + 1144U);
    t7 = *((char **)t5);
    t12 = *((unsigned char *)t7);
    t13 = (t12 == (unsigned char)3);
    if (t13 != 0)
        goto LAB10;

LAB12:
LAB11:    goto LAB3;

LAB7:    t1 = (t0 + 568U);
    t11 = xsi_signal_has_event(t1);
    t3 = t11;
    goto LAB9;

LAB10:    xsi_set_current_line(40, ng0);
    t5 = (t0 + 1052U);
    t8 = *((char **)t5);
    t14 = *((unsigned char *)t8);
    t15 = (t14 == (unsigned char)1);
    if (t15 != 0)
        goto LAB13;

LAB15:    t1 = (t0 + 1052U);
    t2 = *((char **)t1);
    t3 = *((unsigned char *)t2);
    t4 = (t3 == (unsigned char)0);
    if (t4 != 0)
        goto LAB16;

LAB17:
LAB14:    goto LAB11;

LAB13:    xsi_set_current_line(41, ng0);
    t5 = (t0 + 868U);
    t9 = *((char **)t5);
    t16 = *((unsigned char *)t9);
    t5 = (t0 + 2380);
    t10 = (t5 + 32U);
    t17 = *((char **)t10);
    t18 = (t17 + 40U);
    t19 = *((char **)t18);
    *((unsigned char *)t19) = t16;
    xsi_driver_first_trans_fast(t5);
    goto LAB14;

LAB16:    xsi_set_current_line(43, ng0);
    t1 = (t0 + 960U);
    t5 = *((char **)t1);
    t6 = *((unsigned char *)t5);
    t1 = (t0 + 2380);
    t7 = (t1 + 32U);
    t8 = *((char **)t7);
    t9 = (t8 + 40U);
    t10 = *((char **)t9);
    *((unsigned char *)t10) = t6;
    xsi_driver_first_trans_fast(t1);
    goto LAB14;

}

static void work_a_4177355142_3212880686_p_1(char *t0)
{
    char *t1;
    char *t2;
    unsigned char t3;
    char *t4;
    char *t5;
    char *t6;
    char *t7;
    char *t8;

LAB0:    xsi_set_current_line(50, ng0);

LAB3:    t1 = (t0 + 1328U);
    t2 = *((char **)t1);
    t3 = *((unsigned char *)t2);
    t1 = (t0 + 2416);
    t4 = (t1 + 32U);
    t5 = *((char **)t4);
    t6 = (t5 + 40U);
    t7 = *((char **)t6);
    *((unsigned char *)t7) = t3;
    xsi_driver_first_trans_fast_port(t1);

LAB2:    t8 = (t0 + 2336);
    *((int *)t8) = 1;

LAB1:    return;
LAB4:    goto LAB2;

}


extern void work_a_4177355142_3212880686_init()
{
	static char *pe[] = {(void *)work_a_4177355142_3212880686_p_0,(void *)work_a_4177355142_3212880686_p_1};
	xsi_register_didat("work_a_4177355142_3212880686", "isim/proj3_isim.exe.sim/work/a_4177355142_3212880686.didat");
	xsi_register_executes(pe);
}
