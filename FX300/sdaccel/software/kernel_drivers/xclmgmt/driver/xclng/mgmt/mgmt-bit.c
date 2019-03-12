/**
 *  Copyright (C) 2017 Xilinx, Inc. All rights reserved.
 *  Author: Sonal Santan
 *  Code copied verbatim from SDAccel xcldma kernel mode driver
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <linux/firmware.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include "mgmt-core.h"
#include "xclbin.h"

#define SWAP_ENDIAN_32(x)						\
	(unsigned)((((x) & 0xFF000000) >> 24) | (((x) & 0x00FF0000) >> 8) | \
		   (((x) & 0x0000FF00) << 8)  | (((x) & 0x000000FF) << 24))

/*
 * The bitstream is expected in big endian format
 */
const static unsigned fpga_boot_seq[] = {SWAP_ENDIAN_32(DUMMY_WORD),	\
		SWAP_ENDIAN_32(SYNC_WORD),				\
		SWAP_ENDIAN_32(TYPE1_NOOP),				\
		SWAP_ENDIAN_32(TYPE1_WRITE_CMD),			\
		SWAP_ENDIAN_32(IPROG_CMD),				\
		SWAP_ENDIAN_32(TYPE1_NOOP),				\
		SWAP_ENDIAN_32(TYPE1_NOOP)};

static int wait_for_done(struct xclmgmt_dev *lro)
{
	u32 w;
	int i = 0;

//	printk(KERN_INFO "IOCTL %s:%d\n", __FILE__, __LINE__);
	for (i = 0; i < 10; i++) {
		udelay(5);
		w = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + XHWICAP_SR);
		printk(KERN_INFO "XHWICAP_SR %x\n", w);
		if (w & 0x5)
			return 0;
	}
	printk(KERN_INFO "%d us timeout waiting for FPGA after bitstream download\n", 5 * 10);
	return -ETIMEDOUT;
}


static int hwicapWrite(struct xclmgmt_dev *lro, const u32 *word_buf, int size)
{
	u32 value = 0;
	int i = 0;

//	printk(KERN_INFO "IOCTL %s:%d\n", __FILE__, __LINE__);

	for (i = 0; i < size; i++) {
		value = be32_to_cpu(word_buf[i]);
		iowrite32(value, lro->bar[XCLMGMT_MAIN_BAR] + XHWICAP_WF);
	}
//	printk(KERN_INFO "IOCTL %s:%d\n", __FILE__, __LINE__);

	value = 0x1;
	iowrite32(value, lro->bar[XCLMGMT_MAIN_BAR] + XHWICAP_CR);
	for (i = 0; i < 20; i++) {
		value = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + XHWICAP_CR);
//		printk(KERN_INFO "XHWICAP_CR %x\n", value);
		if ((value & 0x1) == 0)
			return 0;
		ndelay(50);

	}
	printk(KERN_INFO "%d us timeout waiting for FPGA after writing %d dwords\n", 50 * 10, size);
	return -EIO;
}

static int bitstream_parse_header(const unsigned char *Data, unsigned int Size, XHwIcap_Bit_Header *Header)
{
	unsigned int I;
	unsigned int Len;
	unsigned int Tmp;
	unsigned int Index;

	/* Start Index at start of bitstream */
	Index = 0;

	/* Initialize HeaderLength.  If header returned early inidicates
	 * failure.
	 */
	Header->HeaderLength = XHI_BIT_HEADER_FAILURE;

	/* Get "Magic" length */
	Header->MagicLength = Data[Index++];
	Header->MagicLength = (Header->MagicLength << 8) | Data[Index++];

	/* Read in "magic" */
	for (I = 0; I < Header->MagicLength - 1; I++) {
		Tmp = Data[Index++];
		if (I%2 == 0 && Tmp != XHI_EVEN_MAGIC_BYTE)
			return -1;   /* INVALID_FILE_HEADER_ERROR */

		if (I%2 == 1 && Tmp != XHI_ODD_MAGIC_BYTE)
			return -1;   /* INVALID_FILE_HEADER_ERROR */

	}

	/* Read null end of magic data. */
	Tmp = Data[Index++];

	/* Read 0x01 (short) */
	Tmp = Data[Index++];
	Tmp = (Tmp << 8) | Data[Index++];

	/* Check the "0x01" half word */
	if (Tmp != 0x01)
		return -1;	 /* INVALID_FILE_HEADER_ERROR */



	/* Read 'a' */
	Tmp = Data[Index++];
	if (Tmp != 'a')
		return -1;	  /* INVALID_FILE_HEADER_ERROR	*/


	/* Get Design Name length */
	Len = Data[Index++];
	Len = (Len << 8) | Data[Index++];

	/* allocate space for design name and final null character. */
	Header->DesignName = kmalloc(Len, GFP_KERNEL);
    if (!Header->DesignName) {    
		return -1;
    }

	/* Read in Design Name */
	for (I = 0; I < Len; I++)
		Header->DesignName[I] = Data[Index++];


	if (Header->DesignName[Len-1] != '\0')
		return -1;

	/* Read 'b' */
	Tmp = Data[Index++];
	if (Tmp != 'b')
		return -1;	/* INVALID_FILE_HEADER_ERROR */


	/* Get Part Name length */
	Len = Data[Index++];
	Len = (Len << 8) | Data[Index++];

	/* allocate space for part name and final null character. */
	Header->PartName = kmalloc(Len, GFP_KERNEL);
    if (!Header->PartName) {    
		return -1;
    }

	/* Read in part name */
	for (I = 0; I < Len; I++)
		Header->PartName[I] = Data[Index++];

	if (Header->PartName[Len-1] != '\0')
		return -1;

	/* Read 'c' */
	Tmp = Data[Index++];
	if (Tmp != 'c')
		return -1;	/* INVALID_FILE_HEADER_ERROR */


	/* Get date length */
	Len = Data[Index++];
	Len = (Len << 8) | Data[Index++];

	/* allocate space for date and final null character. */
	Header->Date = kmalloc(Len, GFP_KERNEL);
    if (!Header->Date) {    
		return -1;
    }
    
	/* Read in date name */
	for (I = 0; I < Len; I++)
		Header->Date[I] = Data[Index++];

	if (Header->Date[Len - 1] != '\0')
		return -1;

	/* Read 'd' */
	Tmp = Data[Index++];
	if (Tmp != 'd')
		return -1;	/* INVALID_FILE_HEADER_ERROR  */

	/* Get time length */
	Len = Data[Index++];
	Len = (Len << 8) | Data[Index++];

	/* allocate space for time and final null character. */
	Header->Time = kmalloc(Len, GFP_KERNEL);
    if (!Header->Time) {    
		return -1;
    }

	/* Read in time name */
	for (I = 0; I < Len; I++)
		Header->Time[I] = Data[Index++];

	if (Header->Time[Len - 1] != '\0')
		return -1;

	/* Read 'e' */
	Tmp = Data[Index++];
	if (Tmp != 'e')
		return -1;	/* INVALID_FILE_HEADER_ERROR */

	/* Get byte length of bitstream */
	Header->BitstreamLength = Data[Index++];
	Header->BitstreamLength = (Header->BitstreamLength << 8) | Data[Index++];
	Header->BitstreamLength = (Header->BitstreamLength << 8) | Data[Index++];
	Header->BitstreamLength = (Header->BitstreamLength << 8) | Data[Index++];
	Header->HeaderLength = Index;

	printk(KERN_INFO "%s: Design \"%s\"\n%s: Part \"%s\"\n%s: Timestamp \"%s %s\"\n%s: Raw data size 0x%x\n",
	       DRV_NAME, Header->DesignName, DRV_NAME, Header->PartName, DRV_NAME, Header->Time,
	       Header->Date, DRV_NAME, Header->BitstreamLength);

	return 0;
}

static int bitstream_icap_helper(struct xclmgmt_dev *lro, const u32 *word_buffer, unsigned word_count)
{
	unsigned remain_word = word_count;
	unsigned word_written = 0;
	int wr_fifo_vacancy = 0;
	int err = 0;

	for (remain_word = word_count; remain_word > 0; remain_word -= word_written) {
		wr_fifo_vacancy = ioread32(lro->bar[XCLMGMT_MAIN_BAR] + XHWICAP_WFV);
		if (wr_fifo_vacancy <= 0) {
			printk(KERN_INFO "IOCTL %s:%d vacancy: %d\n", __FILE__, __LINE__, wr_fifo_vacancy);
			err = -EIO;
			break;
		}
		word_written = (wr_fifo_vacancy < remain_word) ? wr_fifo_vacancy : remain_word;
		if (hwicapWrite(lro, word_buffer, word_written) != 0) {
			printk(KERN_INFO "IOCTL %s:%d\n", __FILE__, __LINE__);
			err = -EIO;
			break;
		}
		word_buffer += word_written;
	}
	return err;
}

static long bitstream_icap(struct xclmgmt_dev *lro, const char *buffer, unsigned length)
{
	long err = 0;
	XHwIcap_Bit_Header bit_header;
	unsigned numCharsRead = DMA_HWICAP_BITFILE_BUFFER_SIZE;
	unsigned byte_read;

	printk(KERN_DEBUG "IOCTL %s:%s:%d\n", __func__, __FILE__, __LINE__);

	if (!buffer || !length)
		return 0;

	memset(&bit_header, 0, sizeof(bit_header));

	if (bitstream_parse_header(buffer, DMA_HWICAP_BITFILE_BUFFER_SIZE, &bit_header)) {
		err = -EINVAL;
		goto free_buffers;
	}

	if ((bit_header.HeaderLength + bit_header.BitstreamLength) > length) {
		err = -EINVAL;
		goto free_buffers;
	}

	buffer += bit_header.HeaderLength;

	for (byte_read = 0; byte_read < bit_header.BitstreamLength; byte_read += numCharsRead) {
		numCharsRead = bit_header.BitstreamLength - byte_read;
		if (numCharsRead > DMA_HWICAP_BITFILE_BUFFER_SIZE)
			numCharsRead = DMA_HWICAP_BITFILE_BUFFER_SIZE;

		err = bitstream_icap_helper(lro, (u32 *)buffer, numCharsRead / 4);
		if (err) {
			printk(KERN_INFO "err value AFTER bitstream_icap_helper call from bitstream_icap: %ld", err);
			goto free_buffers;
		}
		buffer += numCharsRead;
	}
	err = wait_for_done(lro);
	if (err != 0) {
		goto free_buffers;
	}

free_buffers:
    if (bit_header.DesignName) {    
	    kfree(bit_header.DesignName);
    }
    
    if (bit_header.PartName) {    
	    kfree(bit_header.PartName);
    }
    
    if (bit_header.Date) {    
	    kfree(bit_header.Date);
    }
    
    if (bit_header.Time) {    
	    kfree(bit_header.Time);
    }
	printk(KERN_DEBUG "IOCTL %s:%d\n", __FILE__, __LINE__);
	return err;
}


static const struct axlf_section_header* get_axlf_section(
	struct xclmgmt_dev *lro, const struct axlf* top,
	enum axlf_section_kind kind)
{
	int i = 0;
	mgmt_info(lro, "Trying to find section header for axlf section %d",
		kind);
	for(i = 0; i < top->m_header.m_numSections; i++)
	{
		mgmt_info(lro, "Section is %d",
			top->m_sections[i].m_sectionKind);
		if(top->m_sections[i].m_sectionKind == kind) {
			mgmt_info(lro, "Found section header for axlf");
			return &top->m_sections[i];
		}
	}
	mgmt_info(lro, "Did NOT find section header for axlf section %d", kind);
	return NULL;
}

long load_boot_firmware(struct xclmgmt_dev *lro)
{
	const struct xclBin *bin_obj;
	const struct axlf *bin_obj_axlf;
	const struct firmware *fw;
	char fw_name[64];
	XHwIcap_Bit_Header bit_header;
	long err = 0;
	uint64_t length = 0;
	uint64_t primaryFirmwareOffset = 0;
	uint64_t primaryFirmwareLength = 0;
	uint64_t secondaryFirmwareOffset = 0;
	uint64_t secondaryFirmwareLength = 0;
	uint64_t mbBinaryOffset = 0;
	uint64_t mbBinaryLength = 0;
	const struct axlf_section_header* primaryHeader = 0;
	const struct axlf_section_header* secondaryHeader = 0;
	const struct axlf_section_header* mbHeader = 0;

	printk(KERN_DEBUG "%s:%s:%d\n", __func__, __FILE__, __LINE__);
	memset(&bit_header, 0, sizeof(bit_header));

	snprintf(fw_name, sizeof(fw_name), "xilinx/%04x-%04x-%04x-%016llx.dsabin",
		 le16_to_cpu(lro->pci_dev->vendor),
		 lro->user_pci_dev ? le16_to_cpu(lro->user_pci_dev->device) :
			le16_to_cpu(lro->pci_dev->device + 1),
		 le16_to_cpu(lro->pci_dev->subsystem_device),
		 le64_to_cpu(lro->feature_id));

	printk(KERN_INFO "dsa name is %s\n", fw_name);

	err = request_firmware(&fw, fw_name, &lro->pci_dev->dev);

	if(!err && (lro->header.FeatureBitMap & MB_SCHEDULER)) {
		//Try locating the microblaze binary.
		bin_obj_axlf = (const struct axlf*)fw->data;
		mbHeader = get_axlf_section(lro, bin_obj_axlf,SCHED_FIRMWARE);
		if(mbHeader) {
			mbBinaryOffset = mbHeader->m_sectionOffset;
			mbBinaryLength = mbHeader->m_sectionSize;
			length = bin_obj_axlf->m_header.m_length;
			if (length > fw->size) {
				err = -EINVAL;
				goto done;
			}
			if ((mbBinaryOffset + mbBinaryLength) > length) {
				err = -EINVAL;
				goto done;
			}
			vfree(lro->mb_stash.sche_binary);
			lro->mb_stash.sche_binary = vmalloc(mbBinaryLength);
			if (!lro->mb_stash.sche_binary) {
				lro->mb_stash.sche_binary_length = 0;
				err = -ENOMEM;
				goto done;
			}
			lro->mb_stash.sche_binary_length = mbBinaryLength;
			memcpy(lro->mb_stash.sche_binary, fw->data + mbBinaryOffset, lro->mb_stash.sche_binary_length);
			mgmt_info(lro, "Stashed mb sche binary\n");
		}
	}

	if(!err && (lro->header.FeatureBitMap & BOARD_MGMT_ENBLD)) {
		//Try locating the board mgmt binary.
		bin_obj_axlf = (const struct axlf*)fw->data;
		mbHeader = get_axlf_section(lro, bin_obj_axlf,FIRMWARE);
		if(mbHeader) {
			mbBinaryOffset = mbHeader->m_sectionOffset;
			mbBinaryLength = mbHeader->m_sectionSize;
			length = bin_obj_axlf->m_header.m_length;
			if (length > fw->size) {
				err = -EINVAL;
				goto done;
			}
			if ((mbBinaryOffset + mbBinaryLength) > length) {
				err = -EINVAL;
				goto done;
			}
			vfree(lro->mb_stash.mgmt_binary);
			lro->mb_stash.mgmt_binary = vmalloc(mbBinaryLength);
			if (!lro->mb_stash.mgmt_binary) {
				lro->mb_stash.mgmt_binary_length = 0;
				err = -ENOMEM;
				goto done;
			}
			lro->mb_stash.mgmt_binary_length = mbBinaryLength;
			memcpy(lro->mb_stash.mgmt_binary, fw->data + mbBinaryOffset, lro->mb_stash.mgmt_binary_length);
			mgmt_info(lro, "Stashed mb mgmt binary\n");
		}
	}

	//TODO: remove this check after due diligence.
	//if((is_ultrascale_plus(lro) == true)) {
	//	goto done;
	//}

	//retry with the legacy dsabin
	if(err) {
		printk(KERN_WARNING "Unable to find firmware %s, try legacy dsabin.\n", fw_name);
		snprintf(fw_name, sizeof(fw_name), "xilinx/%04x-%04x-%04x-%016llx.dsabin",
			 le16_to_cpu(lro->pci_dev->vendor),
			 le16_to_cpu(lro->pci_dev->device + 1),
			 le16_to_cpu(lro->pci_dev->subsystem_device),
			 le64_to_cpu(0x0000000000000000));
		err = request_firmware(&fw, fw_name, &lro->pci_dev->dev);
	}
	if (err) {
		printk(KERN_WARNING "Unable to find firmware %s\n", fw_name);
		return err;
	}

	if (!memcmp(fw->data, "xclbin2", 8)) {
		printk(KERN_INFO "In axlf load_boot_firmware");
		bin_obj_axlf = (const struct axlf*)fw->data;
		length = bin_obj_axlf->m_header.m_length;
		//Match the xclbin with the hardware.
		printk(KERN_INFO "xclbin: TimeStamp:%llx VBNV:%s\n", bin_obj_axlf->m_header.m_featureRomTimeStamp, bin_obj_axlf->m_header.m_platformVBNV);
		printk(KERN_INFO "ROM: TimeStamp:%llx VBNV:%s\n", lro->header.TimeSinceEpoch, lro->header.VBNVName);
//		if(strncmp(bin_obj_axlf->m_header.m_platformVBNV, lro->header.VBNVName, 64)) {
//			printk(KERN_ERR "VBNV of ROM did not match Xclbin\n");
//			return -EINVAL;
//		}
		if(bin_obj_axlf->m_header.m_featureRomTimeStamp != lro->header.TimeSinceEpoch) {
			printk(KERN_ERR "TimeStamp of ROM did not match Xclbin\n");
			return -EINVAL;
		}
		printk(KERN_INFO "VBNV and TimeStamps matched\n");

		primaryHeader = get_axlf_section(lro, bin_obj_axlf,BITSTREAM);
		secondaryHeader = get_axlf_section(lro, bin_obj_axlf,
			CLEARING_BITSTREAM);
		if(primaryHeader) {
			primaryFirmwareOffset = primaryHeader->m_sectionOffset;
			primaryFirmwareLength = primaryHeader->m_sectionSize;
		}
		if(secondaryHeader) {
			secondaryFirmwareOffset = secondaryHeader->m_sectionOffset;
			secondaryFirmwareLength = secondaryHeader->m_sectionSize;
		}

	} else {
		//legacy xclbin0 flow
		printk(KERN_INFO "In legacy load_boot_firmware");
		bin_obj = (const struct xclBin *)fw->data;
		length = bin_obj->m_length;
		primaryFirmwareOffset = bin_obj->m_primaryFirmwareOffset;
		primaryFirmwareLength = bin_obj->m_primaryFirmwareLength;
		secondaryFirmwareOffset = bin_obj->m_secondaryFirmwareOffset;
		secondaryFirmwareLength =  bin_obj->m_secondaryFirmwareLength;
	}

	if (length > fw->size) {
		err = -EINVAL;
		goto done;
	}

	if ((primaryFirmwareOffset + primaryFirmwareLength) > length) {
		err = -EINVAL;
		goto done;
	}

	if ((secondaryFirmwareOffset + secondaryFirmwareLength) > length) {
		err = -EINVAL;
		goto done;
	}

	if (primaryFirmwareLength) {
		printk(KERN_INFO "%s: Found second stage bitstream of size 0x%llx in %s\n", DRV_NAME, primaryFirmwareLength, fw_name);
		err = bitstream_icap(lro, fw->data + primaryFirmwareOffset, primaryFirmwareLength);
		/* If we loaded a new second stage, we do not need the previously stashed clearing bitstream if any */
		vfree(lro->stash.clear_bitstream);
		lro->stash.clear_bitstream = 0;
		lro->stash.clear_bitstream_length = 0;
		if (err) {
			printk(KERN_ERR "%s: Failed to download second stage bitstream\n", DRV_NAME);
			goto done;
		}
		printk(KERN_INFO "%s: Downloaded second stage bitstream\n", DRV_NAME);
	}

	/*
	 * If both primary and secondary bitstreams have been provided then ignore the previously stashed bitstream
	 * if any. If only secondary bitstream was provided  but we found a previously stashed bitstream we should
	 * use the latter since it is more appropriate for the current state of the device
	 */
	if (secondaryFirmwareLength && (primaryFirmwareLength || !lro->stash.clear_bitstream)) {
		vfree(lro->stash.clear_bitstream);
		lro->stash.clear_bitstream = vmalloc(secondaryFirmwareLength);
		if (!lro->stash.clear_bitstream) {
			lro->stash.clear_bitstream_length = 0;
			err = -ENOMEM;
			goto done;
		}
		lro->stash.clear_bitstream_length = secondaryFirmwareLength;
		memcpy(lro->stash.clear_bitstream, fw->data + secondaryFirmwareOffset,
		       lro->stash.clear_bitstream_length);
		printk(KERN_INFO "%s: Found clearing bitstream of size 0x%x in %s\n", DRV_NAME,
		       lro->stash.clear_bitstream_length, fw_name);
	}
	else if (lro->stash.clear_bitstream) {
		printk(KERN_INFO "%s: Using previously stashed clearing bitstream of size 0x%x\n",
		       DRV_NAME, lro->stash.clear_bitstream_length);
	}

	if (lro->stash.clear_bitstream && bitstream_parse_header(lro->stash.clear_bitstream,
								 DMA_HWICAP_BITFILE_BUFFER_SIZE, &bit_header)) {
		err = -EINVAL;
		vfree(lro->stash.clear_bitstream);
		lro->stash.clear_bitstream = NULL;
		lro->stash.clear_bitstream_length = 0;
		goto done;
	}

done:
	release_firmware(fw);
    if (bit_header.DesignName) {    
	    kfree(bit_header.DesignName);
    }

    if (bit_header.PartName) {    
	    kfree(bit_header.PartName);
    }

    if (bit_header.Date) {    
	    kfree(bit_header.Date);
    }

    if (bit_header.Time) {    
	    kfree(bit_header.Time);
    }
    
	return err;
}


long bitstream_clear_icap(struct xclmgmt_dev *lro)
{
	long err = 0;
	const char *buffer;
	unsigned length;

//  printk(KERN_DEBUG "IOCTL %s:%d\n", __FILE__, __LINE__);
	buffer = lro->stash.clear_bitstream;
	if (!buffer)
		return 0;

	length = lro->stash.clear_bitstream_length;
	printk(KERN_INFO "%s: Downloading clearing bitstream of length 0x%x\n", DRV_NAME, length);
	err = bitstream_icap(lro, buffer, length);

	vfree(lro->stash.clear_bitstream);
	lro->stash.clear_bitstream = 0;
	lro->stash.clear_bitstream_length = 0;
	printk(KERN_DEBUG "IOCTL %s:%d\n", __FILE__, __LINE__);
	return err;
}

static int bitstream_ioctl_icap(struct xclmgmt_dev *lro, const char __user *bit_buf, unsigned long length)
{
	long err = 0;
	XHwIcap_Bit_Header bit_header;
	char *buffer = NULL;
	unsigned numCharsRead = DMA_HWICAP_BITFILE_BUFFER_SIZE;
	unsigned byte_read;

	printk(KERN_INFO "%s: Using kernel mode ICAP bitstream download framework\n", DRV_NAME);
	memset(&bit_header, 0, sizeof(bit_header));

	freezeAXIGate(lro);
	err = bitstream_clear_icap(lro);
	if (err)
		goto free_buffers;

	buffer = kmalloc(DMA_HWICAP_BITFILE_BUFFER_SIZE, GFP_KERNEL);

	if (!buffer) {
		err = -ENOMEM;
		goto free_buffers;
	}
	printk(KERN_INFO "bitstream pointer %p length %lu\n", bit_buf, length);

	if (copy_from_user(buffer, bit_buf, DMA_HWICAP_BITFILE_BUFFER_SIZE)) {
		err = -EFAULT;
		goto free_buffers;
	}

	if (bitstream_parse_header(buffer, DMA_HWICAP_BITFILE_BUFFER_SIZE, &bit_header)) {
		err = -EINVAL;
		goto free_buffers;
	}

	if ((bit_header.HeaderLength + bit_header.BitstreamLength) > length) {
		err = -EINVAL;
		goto free_buffers;
	}

	bit_buf += bit_header.HeaderLength;

	for (byte_read = 0; byte_read < bit_header.BitstreamLength; byte_read += numCharsRead) {
		numCharsRead = bit_header.BitstreamLength - byte_read;
		if (numCharsRead > DMA_HWICAP_BITFILE_BUFFER_SIZE)
			numCharsRead = DMA_HWICAP_BITFILE_BUFFER_SIZE;
		if (copy_from_user(buffer, bit_buf, numCharsRead)) {
			err = -EFAULT;
			goto free_buffers;
		}

		bit_buf += numCharsRead;
//		printk(KERN_INFO "err value BERFORE bitstream_icap_helper call from bitstream_ioctl_icap: %ld", err);
		err = bitstream_icap_helper(lro, (u32 *)buffer, numCharsRead / 4);
		if (err) {
			printk(KERN_INFO "err value AFTER bitstream_icap_helper call from bitstream_ioctl_icap: %ld", err);
			goto free_buffers;
		}
	}

	if (wait_for_done(lro)) {
		err = -ETIMEDOUT;
		printk(KERN_INFO "err value wait for done failure: %ld", err);
		goto free_buffers;
	}

	/**
	 * Perform frequency scaling since PR download can silenty overwrite MMCM settings
	 * in static region changing the clock frequencies although ClockWiz CONFIG registers
	 * will misleading report the older configuration from before bitstream download as
	 * if nothing has changed.
	 */
	if (!err)
		err = ocl_freqscaling(lro, true);


free_buffers:
	freeAXIGate(lro);

    if (buffer) {    
	    kfree(buffer);
    }
    
    if (bit_header.DesignName) {    
	    kfree(bit_header.DesignName);
    }

    if (bit_header.PartName) {    
	    kfree(bit_header.PartName);
    }

    if (bit_header.Date) {    
	    kfree(bit_header.Date);
    }

    if (bit_header.Time) {    
	    kfree(bit_header.Time);
    }
	printk(KERN_INFO "IOCTL %s:%d\n", __FILE__, __LINE__);
	return err;
}


int bitstream_ioctl(struct xclmgmt_dev *lro, const void __user *arg)
{
	struct xclmgmt_ioc_bitstream bitstream_obj;
	struct xclBin bin_obj;
	char __user *buffer;
	struct xclmgmt_ioc_info obj;
	long err = 0;
	unsigned i;

	printk(KERN_INFO "%s: %s \n", DRV_NAME, __FUNCTION__);
	if (copy_from_user((void *)&bitstream_obj, arg, sizeof(struct xclmgmt_ioc_bitstream)))
		return -EFAULT;
	if (copy_from_user((void *)&bin_obj, (void *)bitstream_obj.xclbin, sizeof(struct xclBin)))
		return -EFAULT;
	if (memcmp(bin_obj.m_magic, "xclbin0", 8))
		return -EINVAL;

	if ((bin_obj.m_primaryFirmwareOffset + bin_obj.m_primaryFirmwareLength) > bin_obj.m_length)
		return -EINVAL;

	if ((bin_obj.m_secondaryFirmwareOffset + bin_obj.m_secondaryFirmwareLength) > bin_obj.m_length)
		return -EINVAL;

	if ((lro->pci_dev->device == 0x7138) && bin_obj.m_secondaryFirmwareLength)
		return -EINVAL;

	buffer = (char __user *)bitstream_obj.xclbin;
	err = !access_ok(VERIFY_READ, buffer, bin_obj.m_length);
	if (err)
		return -EFAULT;

	lro->unique_id_last_bitstream = 0;

	buffer += bin_obj.m_primaryFirmwareOffset;
	err = bitstream_ioctl_icap(lro, buffer, bin_obj.m_primaryFirmwareLength);

	if (!err && bin_obj.m_secondaryFirmwareLength) {
		vfree(lro->stash.clear_bitstream);
		lro->stash.clear_bitstream = vmalloc(bin_obj.m_secondaryFirmwareLength);
		if (!lro->stash.clear_bitstream) {
			lro->stash.clear_bitstream_length = 0;
			return -ENOMEM;
		}
		buffer = (char __user *)bitstream_obj.xclbin;
		buffer += bin_obj.m_secondaryFirmwareOffset;
		err = copy_from_user(lro->stash.clear_bitstream, buffer, bin_obj.m_secondaryFirmwareLength);
		if (err) {
		        printk(KERN_INFO "err value after copy_from_user call : %ld", err);
			err = -EFAULT;
			vfree(lro->stash.clear_bitstream);
			lro->stash.clear_bitstream = NULL;
			lro->stash.clear_bitstream_length = 0;
		} else {
			lro->stash.clear_bitstream_length = bin_obj.m_secondaryFirmwareLength;
		}
	}

	if (!err && is_XPR(lro)) {
		printk(KERN_INFO "IOCTL 6 %s:%d, err: %ld\n", __FILE__, __LINE__, err);

		err = -ETIMEDOUT;
		// Check for MIG calibration
		for(i = 0; i < 10; ++i) {
			msleep(500);
			device_info(lro, &obj);
			if(obj.mig_calibration[0] == true) {
				err = 0;
				break;
			}
		}
		if(err != 0) {
			printk(KERN_ERR "MIG Calibration failed after bitstream download %s:%d, err: %ld\n", __FILE__, __LINE__, err);
			return -ETIMEDOUT;
		}
	}
	printk(KERN_INFO "%s err: %ld\n", __FUNCTION__, err);
	return err;
}

int bitstream_ioctl_axlf(struct xclmgmt_dev *lro, const void __user *arg)
{
	struct xclmgmt_ioc_bitstream_axlf bitstream_obj;
	struct axlf bin_obj;
	char __user *buffer;
	struct xclmgmt_ioc_info obj;
	long err = 0;
	uint64_t primaryFirmwareOffset = 0;
	uint64_t primaryFirmwareLength = 0;
	uint64_t secondaryFirmwareOffset = 0;
	uint64_t secondaryFirmwareLength = 0;
	const struct axlf_section_header* primaryHeader = 0;
	const struct axlf_section_header* secondaryHeader = 0;
	unsigned i;
	uint64_t copy_buffer_size = 0;
	struct axlf* copy_buffer;

	printk(KERN_INFO "%s: %s \n", DRV_NAME, __FUNCTION__);
	if (copy_from_user((void *)&bitstream_obj, arg, sizeof(struct xclmgmt_ioc_bitstream_axlf)))
		return -EFAULT;
	if (copy_from_user((void *)&bin_obj, (void *)bitstream_obj.xclbin, sizeof(struct axlf)))
		return -EFAULT;
	if (memcmp(bin_obj.m_magic, "xclbin2", 8))
		return -EINVAL;

	//Match the xclbin with the hardware.
	printk(KERN_INFO "xclbin: TimeStamp:%llx VBNV:%s\n", bin_obj.m_header.m_featureRomTimeStamp, bin_obj.m_header.m_platformVBNV);
	printk(KERN_INFO "ROM: TimeStamp:%llx VBNV:%s\n", lro->header.TimeSinceEpoch, lro->header.VBNVName);
//	if(strncmp(bin_obj.m_header.m_platformVBNV, lro->header.VBNVName, 64)) {
//		printk(KERN_ERR "VBNV of ROM did not match Xclbin\n");
//		return -EINVAL;
//	}
	if(bin_obj.m_header.m_featureRomTimeStamp != lro->header.TimeSinceEpoch) {
		printk(KERN_ERR "TimeStamp of ROM did not match Xclbin\n");
		return -EINVAL;
	}
	printk(KERN_INFO "VBNV and TimeStamps matched\n");

	//skip bitstream re-download
	printk(KERN_INFO "uniqueId axlf: %016llx, xdma_dev:%016llx", bin_obj.m_uniqueId, lro->unique_id_last_bitstream);
	if(lro->unique_id_last_bitstream ==  bin_obj.m_uniqueId) {
		printk(KERN_INFO "Freeze/Free AXI Gate before skipping bitstream download");
		freezeAXIGate(lro);
		msleep(50);
		freeAXIGate(lro);
		msleep(50);
		printk(KERN_INFO "Skipping bistream re-download");
		return err;
	}

	copy_buffer_size = (bin_obj.m_header.m_numSections)*sizeof(struct axlf_section_header) + sizeof(struct axlf);
	printk(KERN_INFO "Marker , numSections : %d, user_buf_size %llu, axlf struct size %lu\n", bin_obj.m_header.m_numSections, copy_buffer_size, sizeof(struct axlf));

	copy_buffer = (struct axlf*)vmalloc(copy_buffer_size);
	if(!copy_buffer) {
		printk(KERN_ERR "Unable to create copy_buffer");
		return -EFAULT;
	}

	if (copy_from_user((void *)copy_buffer, (void *)bitstream_obj.xclbin, copy_buffer_size)) {
		err = -EFAULT;
		goto done;
	}

	printk(KERN_INFO "Finding BITSTREAM section\n");
	primaryHeader = get_axlf_section(lro, copy_buffer,BITSTREAM);
	if (primaryHeader == 0) {
		printk(KERN_INFO "Did not find BITSTREAM section\n");
		err = -EINVAL;
		goto done;
	}
	if((primaryHeader->m_sectionOffset + primaryHeader->m_sectionSize) > bin_obj.m_header.m_length) {
		err = -EINVAL;
		goto done;
	}

	primaryFirmwareOffset = primaryHeader->m_sectionOffset;
	primaryFirmwareLength = primaryHeader->m_sectionSize;

	secondaryHeader = get_axlf_section(lro, copy_buffer,CLEARING_BITSTREAM);
	if(secondaryHeader) {
		secondaryFirmwareOffset = secondaryHeader->m_sectionOffset;
		secondaryFirmwareLength = secondaryHeader->m_sectionSize;
		printk(KERN_INFO "secondaryOffset: %llu\n",secondaryFirmwareOffset);
		printk(KERN_INFO "secondaryLength: %llu\n",secondaryFirmwareLength);
		if((secondaryHeader->m_sectionOffset + secondaryHeader->m_sectionSize) > bin_obj.m_header.m_length) {
			err = -EINVAL;
			goto done;
		}
	}

	if (lro->pci_dev->device == 0x7138) {
		if (secondaryHeader != 0) {
			err = -EINVAL;
			goto done;
		}
		secondaryFirmwareOffset = 0;
		secondaryFirmwareLength = 0;
	}

	//TODO: Once xlnx_bitstream is used in xclbin.h , need to add to this offset.
	buffer = (char __user *)bitstream_obj.xclbin;
	err = !access_ok(VERIFY_READ, buffer, bin_obj.m_header.m_length);
	if (err) {
		err = -EFAULT;
		goto done;
	}

	buffer += primaryFirmwareOffset;
	err = bitstream_ioctl_icap(lro, buffer, primaryFirmwareLength);

	if (!err && secondaryFirmwareLength) {
		vfree(lro->stash.clear_bitstream);
		lro->stash.clear_bitstream = vmalloc(secondaryFirmwareLength);
		if (!lro->stash.clear_bitstream) {
			lro->stash.clear_bitstream_length = 0;
			err = -ENOMEM;
			goto done;
		}

		buffer = (char __user *)bitstream_obj.xclbin;
		buffer += secondaryFirmwareOffset;
		err = copy_from_user(lro->stash.clear_bitstream, buffer, secondaryFirmwareLength);
		if (err) {
			//printk(KERN_INFO "err value after copy_from_user call : %ld", err);
			err = -EFAULT;
			vfree(lro->stash.clear_bitstream);
			lro->stash.clear_bitstream = NULL;
			lro->stash.clear_bitstream_length = 0;
		} else {
			lro->stash.clear_bitstream_length = secondaryFirmwareLength;
		}
	}

	if (!err && is_XPR(lro)) {
		printk(KERN_INFO "IOCTL 6 %s:%d, err: %ld\n", __FILE__, __LINE__, err);

		err = -ETIMEDOUT;
		// Check for MIG calibration
		for(i = 0; i < 10; ++i) {
			msleep(500);
			device_info(lro, &obj);
			if(obj.mig_calibration[0] == true) {
				err = 0;
				break;
			}
		}
		if(err != 0) {
			printk(KERN_ERR "MIG Calibration failed after bitstream download %s:%d, err: %ld\n", __FILE__, __LINE__, err);
			err = -ETIMEDOUT;
			goto done;
		}

	}

	//Populate with "this" bitstream, so avoid redownload the next time
	lro->unique_id_last_bitstream = bin_obj.m_uniqueId;

done:
	printk(KERN_INFO "%s err: %ld\n", __FUNCTION__, err);
	vfree(copy_buffer);
	return err;
}

static int icap_reset_ministream(struct xclmgmt_dev *lro)
{
	u32 value;
	int i;

	for (i=0; i < (sizeof(fpga_boot_seq) / 4); i++) {
		value = be32_to_cpu(fpga_boot_seq[i]);
		iowrite32(value, lro->bar[XCLMGMT_MAIN_BAR] + XHWICAP_WF);
	}
	value = 0x1;
	iowrite32(value, lro->bar[XCLMGMT_MAIN_BAR] + XHWICAP_CR);
	printk(KERN_INFO "%s: Downloaded reset ministream\n", DRV_NAME);
	msleep(4000);
	return 0;
}

int mgmt_load_reset_mini_bitstream(struct xclmgmt_dev *lro)
{
	return icap_reset_ministream(lro);
}

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
