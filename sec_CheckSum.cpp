/*
 * sec_CheckSum.c
 *
 *  Created on: 24/08/2015
 *      Author: Lanhsin
 */
#include <string.h>
#include <stdlib.h>

#define CAL_CHECK_SUM  1

#if CAL_CHECK_SUM
#include <stdio.h>
#include "pattern_fpga/sec_tp_types.h"
#include "pattern_fpga/sec_random_S_000.h"
#include "pattern_fpga/sec_mix_MS_001.h"
#else
#include "pattern_fpga/sum_random_S_000.h"
#include "pattern_fgpa/sum_mix_MS_001.h"
#endif


#if CAL_CHECK_SUM
typedef unsigned char tBoolean;
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#define DUMP_FILE_NAME  "checksum"
FILE * Sum_File_p = NULL;

void 
secTest_dump_initial(const char* sum_file_name)
{
    const char *file_p = NULL;
    char file_name[50] = {0};

    if(sum_file_name != NULL)
        file_p = sum_file_name;
    else
        file_p = DUMP_FILE_NAME;

    memcpy(file_name, file_p, strlen(sum_file_name));
    
    memcpy(file_name+strlen(file_name), ".h", 3);
    
    Sum_File_p = fopen(file_name, "w");

    fprintf(Sum_File_p, "#ifndef __%s__", file_p);
    fprintf(Sum_File_p, "\n#define __%s__", file_p);

    fprintf(Sum_File_p, "\n\nconst unsigned int %s[]=\n{", file_p);
}


void 
secTest_dump_deinitial(void)
{
    fclose(Sum_File_p);
}

#endif


unsigned int
secTest_CalcSum(const unsigned char  *Data_ptr, const unsigned int size)
{
    unsigned int i, sum = 0;
    for(i = 0; i < size; i++)
        sum += Data_ptr[i];

    return sum;
}


tBoolean
secTest_CheckSum(const unsigned int *sumSet_p, const struct_sec_test_case_macro *test_macro_p, unsigned int sizeOfdvtest)
{
    unsigned int i, sum = 0;
    for(i = 0; i < sizeOfdvtest; i++)
    {
        sum = secTest_CalcSum(test_macro_p[i].Descriptor_ptr, test_macro_p[i].Descriptor_size);
        sum += secTest_CalcSum(test_macro_p[i].Data_In_ptr, test_macro_p[i].Data_In_size);
        sum += secTest_CalcSum(test_macro_p[i].Data_Out_ptr, test_macro_p[i].Data_Out_size);
        if(sumSet_p[i] != sum)
            return false;
    }

    return true;
}


#if CAL_CHECK_SUM

void
secTest_WriteSum(const struct_sec_test_case_macro *test_macro_p, unsigned int sizeOfdvtest)
{
    unsigned int i, sum = 0;
    for(i = 0; i < sizeOfdvtest; i++)
    {
        sum = secTest_CalcSum(test_macro_p[i].Descriptor_ptr, test_macro_p[i].Descriptor_size);
        sum += secTest_CalcSum(test_macro_p[i].Data_In_ptr, test_macro_p[i].Data_In_size);
        sum += secTest_CalcSum(test_macro_p[i].Data_Out_ptr, test_macro_p[i].Data_Out_size);
        /* Write sum to file */
        if(i % 4 == 0)
            fprintf(Sum_File_p, "\n");

        fprintf(Sum_File_p, " 0x%08x,", sum);
    }

    fprintf(Sum_File_p, "\n}; \n\n#endif\n");
}



int 
main(void)
{
    secTest_dump_initial("sum_mix_MS_001");
    secTest_WriteSum(sec_mix_MS_macro_001, sizeof(sec_mix_MS_macro_001)/sizeof(sec_mix_MS_macro_001[0]));
    secTest_dump_deinitial();

    secTest_dump_initial("sum_random_S_000");
    secTest_WriteSum(sec_random_S_macro_000, sizeof(sec_random_S_macro_000)/sizeof(sec_random_S_macro_000[0]));
    secTest_dump_deinitial();
    return EXIT_SUCCESS;
}
#endif

