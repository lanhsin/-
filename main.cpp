#include "security.h"
#include "log.h"

#include <fstream>
#include <memory>
#include <iostream>


bool sec_profile_read (unsigned char* dataOut, unsigned char* dataIn, cipherPara_S *sec_Para)
{
    std::ifstream i("input.txt");
    if(false == i.is_open())
        return false;

    uint32_t u32buf;
    /* read Mode */
    i >> std::hex >> u32buf;
    sec_Para->u32Mode = u32buf;
    
    /* read count */
    i >> u32buf;
    sec_Para->u32Count = u32buf;

    /* read bearer */
    i >> u32buf;   
    sec_Para->u32Bearer = u32buf;
	
    /* read DIR */
    i >> u32buf;
    sec_Para->u32Dir = u32buf;

    /* read key */
    unsigned char* key;
    if(sec_Para->u32Mode & 0xF)
        key = sec_Para->aEiaKey;
    else
        key = sec_Para->aEeaKey;
    for (auto j = 0; j < 4; j++)
    {
        i >> u32buf;
        key[j*4] = (u32buf >> 24) & 0xff;
        key[j*4 + 1] = (u32buf >> 16) & 0xff;
        key[j*4 + 2] = (u32buf >> 8) & 0xff;
        key[j*4 + 3] = (u32buf) & 0xff;
    }

    /* read length bits */
    uint32_t length;
    i >> length;
    sec_Para->u32Len = length;
    
    length = (length + 64 + 31) / 32;
    /* read plaintext 4 bytes */
    sec_Para->aData = dataIn;
    for (uint32_t j = 0; j < length; j++)
    {
        i >> u32buf;
        dataIn[j*4] = (u32buf >> 24) & 0xff;
        dataIn[j*4 + 1] = (u32buf >> 16) & 0xff;
        dataIn[j*4 + 2] = (u32buf >> 8) & 0xff;
        dataIn[j*4 + 3] = (u32buf) & 0xff;
    }

    sec_Para->dataOut = dataOut;

    return true;
}

int main(void)
{
    std::unique_ptr<unsigned char[]> dataIn = std::make_unique<unsigned char[]>(MAX_DATA_LEN);
    std::unique_ptr<unsigned char[]> dataOut = std::make_unique<unsigned char[]>(MAX_DATA_LEN);

    cipherPara_S sec_Para;
    std::cout << "\nCHIPER-IP : Log level is " << LOG_LEVEL << "\n\n";

    sec_log_setlevel(LOG_LEVEL);
    if(sec_profile_read(dataOut.get(), dataIn.get(), &sec_Para) == false)
        return EXIT_FAILURE;

    sec_cipherSdu(&sec_Para);

    return EXIT_SUCCESS;
}
