/*-
 *   BSD LICENSE
 *
 *   Copyright(c)  2019 Huawei Technologies Co., Ltd. All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Huawei Technologies Co., Ltd  nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fmtkapi.h"

/*******************************************************************************
Function     : ReadFileToRam
Description  : Read the file content to ram.
Input        : FileName
Output       : FileBuffer
Return       : None
*******************************************************************************/
void ReadFileToRam(char *FileName, SECURE_TLV_BUFFER_STRU *FileBuffer)
{
    unsigned Size = 0;
    int Length = 0;
    int Ret = 0;
    unsigned char *Buffer = NULL;
    FILE *File = NULL;

    if ((NULL == FileName) || (NULL == FileBuffer))
    {
        printf("input point is null!");

        return;
    }

    /* 打开要读取的文件 */
    File = fopen(FileName, "rb");
    if (NULL == File)
    {
        printf("Open file fail! (FileName:%s)", FileName);

        return;
    }

    /* fseek: 当调用成功时则返回0，若有错误则返回-1，errno会存放错误代码 */
    Ret = fseek(File, 0L, SEEK_END);
    if (0 != Ret)
    {
        printf("Fseek fail! (Ret:%d)", Ret);

        (void)fclose(File);
        return;
    }

    /* ftell:当调用成功时则返回目前的读写位置，若有错误则返回-1，errno会存放错误代码 */
    Length = ftell(File);
    if ((-1) == Length)
    {
        printf("Ftell fail!");

        (void)fclose(File);
        return;
    }

    Size = (unsigned)Length;

    /* 文件长度检查 */
    if (0 == Size)
    {
        printf("File is empty! (Size: %#x)", Size);
        return;
    }

    /* 为文件申请内存 */
    Buffer = (unsigned char *)malloc(Size);
    if (NULL == Buffer)
    {
        printf("malloc mem for file fail!(Size:0x%x)", Size);

        (void)fclose(File);
        return;
    }

    /* 读取文件到内存 */
    rewind(File);
    FileBuffer->usLength = fread(Buffer, 1, Size, File);
    if (FileBuffer->usLength != Size)
    {
        /* 释放为文件申请的内存 */
        free(Buffer);
        Buffer = NULL;
        (void)fclose(File);

        printf("Fread fail!(FileLength:0x%x, ReadSize:0x%x)", Size, FileBuffer->usLength);
        return;
    }

    (void)fclose(File);

    if (NULL == memcpy(FileBuffer->aucData, Buffer, Size))
    {
        /* 释放为文件申请的内存 */
        free(Buffer);
        Buffer = NULL;

        printf("data copy error!");
        return;
    }

    /* 释放为文件申请的内存 */
    free(Buffer);
    Buffer = NULL;

    return;
}

/*******************************************************************************
Function     : WriteDataToFile
Description  : Write data to a file.
Input        : strData 
               FileName
Output       : None
Return       : None
*******************************************************************************/
void WriteDataToFile(SECURE_TLV_BUFFER_STRU strData, char *FileName)
{
    unsigned i = 0;
    FILE *pFile = NULL;

    if (NULL == FileName)
    {
        printf("input file name is null\r\n");
        return;
    }

    /* 创建请求文件 */
    pFile = fopen(FileName, "wb+");
    if (NULL == pFile)
    {
        printf("Open file fail! (FileName:%s)\r\n", FileName);
        return;
    } 

    for (i = 0; i < strData.usLength; i++)
    {
        fputc(strData.aucData[i], pFile);
    }

    (void)fclose(pFile);

    return;
}

/*************************************************************************************
Function     : SecureTest
Description  : secure chip test
    1. reset test.
    2. echo test.
    3. query the chip life cycle.
    4. Obtain chip product data.
    5. read chip zone0 information.
    6. Read the Huawei certificate from zone0.
    7. Read the public key.
    8. Search for Huawei certificates using vendor id.
    9. Write the customer certificate.
    10. Search certificates written by the customer using vendor id and certificate id.
    11. signature
    12. Verifying the Signature Result
        a. Convert the certificate format from der to pem.
        b. Extract the public key from the certificate.
        c. Signature verification.
Input        : SessionHdl 
               FpgaSlot 
               ChipIndex
Output       : None
Return       : None
*************************************************************************************/
void SecureTest(FmSession* SessionHdl, unsigned FpgaSlot, ENUM_SECURE_CHIP ChipIndex)
{
    fmerrno Result = FM_API_ERR_CODE_EINVAL;
    int i = 0;
    unsigned randdata = 0;
    unsigned zone = 0;
    unsigned length = 0;
    unsigned addr = 0;
    unsigned short VendorID = 0;
    unsigned short CertID = 0;
    SECURE_TLV_BUFFER_STRU TLVIn = {0};
    SECURE_TLV_BUFFER_STRU TLVOut = {0};
    SECURE_PUBLICKKEY_BUFFER_STRU PublicKey = {0};
    SECURE_CERT_KEY_INFO_STRU CertInfo = {0}; 

    /* 1.reset test: Check whether the chip functions are available. */
    Result = FmAPI_SecureReset(SessionHdl, FpgaSlot, ChipIndex);
    if (FM_API_SUCCESS != Result)
    {
        printf("\r\n[FmAPI_SecureReset]: slot[%u] chip[%u] call FmAPI_SecureReset failed! errcode=0x%x\n", FpgaSlot, ChipIndex, Result);
    }
    else
    {
        printf("\r\n[FmAPI_SecureReset]: slot[%u] chip[%u] FmAPI_SecureReset test success!\n", FpgaSlot, ChipIndex);
    }

    /* 2.echo test: Check whether the chip functions are available. */
    /* Generate a random number for echo test. */
    srand((unsigned)time(NULL));
    randdata = (unsigned)rand();
    TLVIn.usLength = 4;
    for (i = 0; i < TLVIn.usLength; i++)
    {
       TLVIn.aucData[i] = (unsigned char)(randdata >> ((4 - i - 1) * 8));
    }

    Result = FmAPI_SecureEcho(SessionHdl, FpgaSlot, ChipIndex, TLVIn, &TLVOut);
    if (FM_API_SUCCESS != Result)
    {
        printf("\r\n[FmAPI_SecureEcho]: slot[%u] chip[%u] call FmAPI_SecureEcho failed! errcode=0x%x\n", FpgaSlot, ChipIndex, Result);
    }
    else
    {
        printf("\r\n[FmAPI_SecureEcho]: slot[%u] chip[%u] FmAPI_SecureEcho test success!\n", FpgaSlot, ChipIndex);
    }

    printf("TLVIn data is: ");
    for (i = 0; i < TLVIn.usLength; i++)
    {
        printf("%02x", TLVIn.aucData[i]);
    }
    printf("\r\n");
    printf("TLVOut data is: ");
    for (i = 0; i < TLVOut.usLength; i++)
    {
        printf("%02x", TLVOut.aucData[i]);
    }
    printf("\r\n");


    /* 3.query lifestate: Check whether the chip life cycle is 3(operational). */
    memset((void *)&TLVOut, 0, sizeof(SECURE_TLV_BUFFER_STRU));
    Result = FmAPI_SecureQuery(SessionHdl, FpgaSlot, ChipIndex, 0x8, &TLVOut);
    if (FM_API_SUCCESS != Result)
    {
        printf("\r\n[FmAPI_SecureQuery]: slot[%u] chip[%u] call FmAPI_SecureQuery failed! errcode=0x%x\n", FpgaSlot, ChipIndex, Result);
    }
    else
    {
        printf("\r\n[FmAPI_SecureQuery]: slot[%u] chip[%u] lifestate: 0x%02x (0x01:Born 0x03:Operational 0x04:Terminated 0x81:Born and Locked 0x83:Operational and Locked)\n",
            FpgaSlot, ChipIndex, TLVOut.aucData[0]);
    }

    /* 4.get chip product data */
    memset((void *)&TLVOut, 0, sizeof(SECURE_TLV_BUFFER_STRU));
    Result = FmAPI_SecureGetChipInfo(SessionHdl, FpgaSlot, ChipIndex, &TLVOut);
    if (FM_API_SUCCESS != Result)
    {
        printf("\r\n[FmAPI_SecureGetChipInfo]: slot[%u] chip[%u] call FmAPI_SecureGetChipInfo failed! errcode=0x%x\n", FpgaSlot, ChipIndex, Result);
    }
    else
    {
        printf("\r\n[FmAPI_SecureGetChipInfo]: slot[%u] chip[%u] product data: ", FpgaSlot, ChipIndex);
        for (i = 0; i < TLVOut.usLength; i++)
        {
            printf("%02x", TLVOut.aucData[i]);
        }
        printf("\r\n");
    }

    /* 5.read zone0 information */
    memset((void *)&TLVOut, 0, sizeof(SECURE_TLV_BUFFER_STRU));
    zone = 0;
    addr = 0x0;
    length = 30;
    Result = FmAPI_SecureRead(SessionHdl, FpgaSlot, ChipIndex, zone, addr, length, &TLVOut);
    if (FM_API_SUCCESS != Result)
    {
        printf("\r\n[FmAPI_SecureRead]: slot[%u] chip[%u] call FmAPI_SecureRead failed! errcode=0x%x\n", FpgaSlot, ChipIndex, Result);
    }
    else
    {
        printf("\r\n[FmAPI_SecureRead]: slot[%u] chip[%u] zone[%d] addr[0x%x] length[%d] data: ", FpgaSlot, ChipIndex, zone, addr, length);
        for (i = 0; i < TLVOut.usLength; i++)
        {
            printf("%02x", TLVOut.aucData[i]);
        }
        printf("\r\n");
    }

    /* 6.read huawei certificate from zone0 */
    memset((void *)&TLVOut, 0, sizeof(SECURE_TLV_BUFFER_STRU));
    zone = 0;
    Result = FmAPI_SecureReadCert(SessionHdl, FpgaSlot, ChipIndex, zone, &TLVOut);
    if (FM_API_SUCCESS != Result)
    {
        printf("\r\n[FmAPI_SecureReadCert]: slot[%u] chip[%u] call FmAPI_SecureReadCert failed! errcode=0x%x\n", FpgaSlot, ChipIndex, Result);
    }
    else
    {
        printf("\r\n[FmAPI_SecureReadCert]: slot[%u] chip[%u] zone[0] certificate is: ", FpgaSlot, ChipIndex);
        for (i = 0; i < TLVOut.usLength; i++)
        {
            printf("%02x", TLVOut.aucData[i]);
        }
        printf("\r\n");

        WriteDataToFile(TLVOut, "cert.der");
    }

    /* 7.get public key */
    memset((void *)&PublicKey, 0, sizeof(SECURE_PUBLICKKEY_BUFFER_STRU));
    Result = FmAPI_SecureGetPublicKey(SessionHdl, FpgaSlot, ChipIndex, &PublicKey);
    if (FM_API_SUCCESS != Result)
    {
        printf("\r\n[FmAPI_SecureGetPublicKey]: slot[%u] chip[%u] call FmAPI_SecureGetPublicKey failed! errcode=0x%x\n", FpgaSlot, ChipIndex, Result);
    }
    else
    {
        printf("\r\n[FmAPI_SecureGetPublicKey]: slot[%u] chip[%u] public key data: ", FpgaSlot, ChipIndex);
        for (i = 0; i < PublicKey.usPublicKeyLen; i++)
        {
            printf("%02x", PublicKey.aucPublicKey[i]);
        }

        printf("\r\nslot[%u] chip[%u] curve ID: ", FpgaSlot, ChipIndex);
        for (i = 0; i < PublicKey.usCurveIDLen; i++)
        {
            printf("%02x", PublicKey.aucCurveID[i]);
        }
        printf("\r\n");
    }

    /* 8.search huaewi certificate by vendorid(0x19e5) */
    memset((void *)&CertInfo, 0, sizeof(SECURE_CERT_KEY_INFO_STRU));
    VendorID = 0x19e5;
    Result = FmAPI_SecureSearchCertByVID(SessionHdl, FpgaSlot, ChipIndex, VendorID, &CertInfo);
    if (FM_API_SUCCESS != Result)
    {
        printf("\r\n[FmAPI_SecureSearchCertByVID]: slot[%u] chip[%u] call FmAPI_SecureSearchCertByVID failed! errcode=0x%x\n", FpgaSlot, ChipIndex, Result);
    }
    else
    {
        printf("\r\n[FmAPI_SecureSearchCertByVID]: slot[%u] chip[%u] read certificate by vendor id(0x%x): \r\n", FpgaSlot, ChipIndex, VendorID);

        printf("+--------------------+--------------------+--------------------+--------------------+\r\n");
        printf("| Zone               | Length             | Vendor ID          | Certificate ID     |\r\n");
        printf("+--------------------+--------------------+--------------------+--------------------+\r\n");

        /* no certificate was found */
        if (0 == CertInfo.ucCertNum)
        {
            printf("|                    |                    |                    |                    |\r\n");
            printf("+--------------------+--------------------+--------------------+--------------------+\r\n");

            printf("No certificate was found matching the vendor id(0x%x).\n", VendorID);
        }
        else
        {
            for (i = 0; i < CertInfo.ucCertNum; i++)
            {
                printf("| %d                  | 0x%04x             | 0x%04x             | 0x%04x             |\r\n",
                    CertInfo.astrOneCertKeyInfo[i].ucZone, CertInfo.astrOneCertKeyInfo[i].usLength,
                    CertInfo.astrOneCertKeyInfo[i].usVenderId, CertInfo.astrOneCertKeyInfo[i].usCertId);
            }
            printf("+--------------------+--------------------+--------------------+--------------------+\r\n");
        }
    }

    /* 9.write certificate to zone1: first, convert the certificate content to the hexadecimal format. 
       input parameter[CertTLVIn]: input the hexadecimal content of the certificate */
    zone = 1;
    VendorID = 0x1010;
    CertID = 0x5a5a;
    ReadFileToRam("cert.der", &TLVIn);
    Result = FmAPI_SecureWriteCert(SessionHdl, FpgaSlot, ChipIndex, zone, VendorID, CertID, TLVIn);
    if (FM_API_SUCCESS != Result)
    {
        printf("\r\n[FmAPI_SecureWriteCert]: slot[%u] chip[%u] write certificate to zone[1] failed! errcode=0x%x\n", FpgaSlot, ChipIndex, Result);
    }
    else
    {
        printf("\r\n[FmAPI_SecureWriteCert]: slot[%u] chip[%u] zone[1] write certificate success\n", FpgaSlot, ChipIndex);
    }

    /* 10.search certificate by vendorid(0x19e5) an certificate id(0x5a5a) */
    memset((void *)&CertInfo, 0, sizeof(SECURE_CERT_KEY_INFO_STRU));
    VendorID = 0x1010;
    CertID = 0x5a5a;
    Result = FmAPI_SecureSearchCertByVIDandCID(SessionHdl, FpgaSlot, ChipIndex, VendorID, CertID, &CertInfo);
    if (FM_API_SUCCESS != Result)
    {
        printf("\r\n[FmAPI_SecureSearchCertByVIDandCID]: slot[%u] chip[%u] call FmAPI_SecureSearchCertByVIDandCID failed! errcode=0x%x\n", FpgaSlot, ChipIndex, Result);
    }
    else
    {
        printf("\r\n[FmAPI_SecureSearchCertByVIDandCID]: slot[%u] chip[%u] read certificate by vendor id(0x%x) and certificate id(0x%x): \r\n", FpgaSlot, ChipIndex, VendorID, CertID);

        printf("+--------------------+--------------------+--------------------+--------------------+\r\n");
        printf("| Zone               | Length             | Vendor ID          | Certificate ID     |\r\n");
        printf("+--------------------+--------------------+--------------------+--------------------+\r\n");

        /* no certificate was found */
        if (0 == CertInfo.ucCertNum)
        {
            printf("|                    |                    |                    |                    |\r\n");
            printf("+--------------------+--------------------+--------------------+--------------------+\r\n");

            printf("No certificate was found matching the vendor id(0x%x) and certificate id(0x%x).\n", VendorID, CertID);
        }
        else
        {
            for (i = 0; i < CertInfo.ucCertNum; i++)
            {
                printf("| %d                  | 0x%04x             | 0x%04x             | 0x%04x             |\r\n",
                    CertInfo.astrOneCertKeyInfo[i].ucZone, CertInfo.astrOneCertKeyInfo[i].usLength,
                    CertInfo.astrOneCertKeyInfo[i].usVenderId, CertInfo.astrOneCertKeyInfo[i].usCertId);
            }
            printf("+--------------------+--------------------+--------------------+--------------------+\r\n");
        }
    }

    /* 11.signature test: the input parameter[TLVIn] is a random number */
    /* The Openssl version cannot be earlier than OpenSSL 1.0.2j-fips  26 Sep 2016 */
    /* Generate a random number for signature. */
    system("openssl rand 8 > signin");
    ReadFileToRam("signin", &TLVIn);

    memset((void *)&TLVOut, 0, sizeof(SECURE_TLV_BUFFER_STRU));
    Result = FmAPI_SecureSignature(SessionHdl, FpgaSlot, ChipIndex, TLVIn, &TLVOut);
    if (FM_API_SUCCESS != Result)
    {
        printf("\r\[FmAPI_SecureSignature]: nslot[%u] chip[%u] call FmAPI_SecureSignature failed! errcode=0x%x\n", FpgaSlot, ChipIndex, Result);
    }
    else
    {
        printf("\r\n[FmAPI_SecureSignature]: slot[%u] chip[%u] signature success\n", FpgaSlot, ChipIndex);

        WriteDataToFile(TLVOut, "signout");
    }

    /* Convert the certificate format from der to pem. */
    system("openssl x509 -in cert.der -inform der -outform pem -out cert.pem");
    /* Extract the public key from the certificate. */
    system("openssl x509 -in cert.pem -pubkey -noout > pub.pem");
    /* Signature verification */
    system("openssl dgst -verify pub.pem -signature signout signin");

    /* Delete process files */
    system("rm cert.der");
    system("rm cert.pem");
    system("rm pub.pem");
    system("rm signin");
    system("rm signout");

    return;
}

/*******************************************************************************
Function     : DemoSecureTest
Description  : test secure chip of slot0 fpga device
Input        : None
Output       : None
Return       : None
*******************************************************************************/
void DemoSecureTest()
{
    fmerrno Result = FM_API_ERR_CODE_EINVAL;
    FmSession* SessionHdl = NULL;
    int chip = SECURE_STATIC_CHIP;

    Result = FmAPI_Initialize(SELECTED_API_VERSION, SELECTED_API_SUB_VER);
    if (FM_API_SUCCESS != Result)
    {
        return;
    }

    printf("FMTK api Initialized.\n");

    FmAPI_StartSession(&SessionHdl);

    if ( 0 == ( SessionHdl->PfNum ) )
    {
        printf ( "No fpga devices!\n" );
        FmAPI_StopSession(SessionHdl);
        FmAPI_Unload();
        return;
    }
    else
    {
        printf ( "%u fpga devices founded!\n", ( SessionHdl->PfNum ));
    }

    /* You can also use another slot to perform this operation */
    printf ( "Start to operate the Security Chip from the slot 0.\n" );

    SecureTest(SessionHdl, 0, chip);

    /* Other API call can be added here */

    /* stop sesssions; make sure all device mutex lock released */
    FmAPI_StopSession(SessionHdl);

    /* check sessions and release all resources */
    FmAPI_Unload();

    return;    
}

/*******************************************************************************
Function     : main
Description  : Main function entry
Input        : None
Output       : None
Return       : 0:sucess other:fail
*******************************************************************************/
int main()
{
    DemoSecureTest();

    return 0;
}

