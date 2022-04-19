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



int work_p_0821586110_sub_2256325812_4087394869(char *t1, int t2, int t3, int t4, int t5)
{
    char t6[208];
    char t7[24];
    char t11[8];
    char t17[8];
    char t23[8];
    int t0;
    char *t8;
    char *t9;
    char *t10;
    char *t12;
    char *t13;
    char *t14;
    char *t15;
    char *t16;
    char *t18;
    char *t19;
    char *t20;
    char *t21;
    char *t22;
    char *t24;
    char *t25;
    char *t26;
    char *t27;
    char *t28;
    char *t29;
    int t30;
    char *t31;
    char *t32;
    int t33;
    int t34;
    int t35;

LAB0:    t8 = (t6 + 4U);
    t9 = ((STD_STANDARD) + 240);
    t10 = (t8 + 52U);
    *((char **)t10) = t9;
    t12 = (t8 + 36U);
    *((char **)t12) = t11;
    xsi_type_set_default_value(t9, t11, 0);
    t13 = (t8 + 48U);
    *((unsigned int *)t13) = 4U;
    t14 = (t6 + 72U);
    t15 = ((STD_STANDARD) + 240);
    t16 = (t14 + 52U);
    *((char **)t16) = t15;
    t18 = (t14 + 36U);
    *((char **)t18) = t17;
    xsi_type_set_default_value(t15, t17, 0);
    t19 = (t14 + 48U);
    *((unsigned int *)t19) = 4U;
    t20 = (t6 + 140U);
    t21 = ((STD_STANDARD) + 240);
    t22 = (t20 + 52U);
    *((char **)t22) = t21;
    t24 = (t20 + 36U);
    *((char **)t24) = t23;
    xsi_type_set_default_value(t21, t23, 0);
    t25 = (t20 + 48U);
    *((unsigned int *)t25) = 4U;
    t26 = (t7 + 4U);
    *((int *)t26) = t2;
    t27 = (t7 + 8U);
    *((int *)t27) = t3;
    t28 = (t7 + 12U);
    *((int *)t28) = t4;
    t29 = (t7 + 16U);
    *((int *)t29) = t5;
    t30 = xsi_vhdl_mod(t2, t5);
    t31 = (t14 + 36U);
    t32 = *((char **)t31);
    t31 = (t32 + 0);
    *((int *)t31) = t30;
    t30 = xsi_vhdl_mod(t3, t4);
    t9 = (t20 + 36U);
    t10 = *((char **)t9);
    t9 = (t10 + 0);
    *((int *)t9) = t30;
    t9 = (t14 + 36U);
    t10 = *((char **)t9);
    t30 = *((int *)t10);
    t33 = (t30 * t4);
    t9 = (t20 + 36U);
    t12 = *((char **)t9);
    t34 = *((int *)t12);
    t35 = (t33 + t34);
    t9 = (t8 + 36U);
    t13 = *((char **)t9);
    t9 = (t13 + 0);
    *((int *)t9) = t35;
    t9 = (t8 + 36U);
    t10 = *((char **)t9);
    t30 = *((int *)t10);
    t0 = t30;

LAB1:    return t0;
LAB2:;
}


extern void work_p_0821586110_init()
{
	static char *se[] = {(void *)work_p_0821586110_sub_2256325812_4087394869};
	xsi_register_didat("work_p_0821586110", "isim/proj3_isim.exe.sim/work/p_0821586110.didat");
	xsi_register_subprogram_executes(se);
}
