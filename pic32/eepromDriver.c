/*********************************************************************
 *
 *       Emulating Data EEPROM for PIC32 microcontrollers
 *
 *********************************************************************
 * FileName:        dee_emulation_pic32.c
 * Dependencies:
 * Processor:       PIC32
 *
 * Complier:        MPLAB C32
 *                  MPLAB IDE
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the �Company�) for its PIC32 Microcontroller is intended
 * and supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip PIC32 Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *$Id: $
 **********************************************************************
 * Description:
 *  See "dee_emulation_pic32.h" and AN1095.
 *
 * Change History:
 *  Author          Date        Comment
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Dave Bell       2013-11-12  address attribute for eedata_addr array.
 *	Arne Bergseth	2013-11-25	Maximum address 1023 with 4 Flash pages.
 *                              Removed misplaced Erase actions.
 *                              1 Flash page minimum configuration.
 *                              Modifications to improve performance.
 *	                            Error counter in dataEEFlags structure.
 **********************************************************************/
#include <source/dee_emulation_pic32.h>
#include <plib.h>

//
// DEE emulation pages allocated at a fixed address. 
// Together with Programmer settings, program may be relinked and the device program updated, 
// keeping the dee_emulation data intact. See description in dee_emulation_pic32.h
#ifdef EEDATA_ADDRESS
const unsigned int  eedata_addr[NUM_DATA_EE_PAGES][NUMBER_OF_INSTRUCTIONS_IN_PAGE] __attribute__((section("dee_data"), address(EEDATA_ADDRESS), space(prog)));  //0x9D03D000 to end of prog space
#else
// For the DEE emulation operation, 3 Pages should be allocated in the program memory.
const unsigned int  eedata_addr[NUM_DATA_EE_PAGES][NUMBER_OF_INSTRUCTIONS_IN_PAGE] __attribute__((aligned(4096), space(prog)));
#endif
		 int AddrIndex = 0;
unsigned int ActivePage = 0;
unsigned int CurrentPage = 0;
unsigned int LowerAddress = 0;     // to identify the read/write pointer address location
DATA_EE_FLAGS dataEEFlags;         //Flags for the error/warning condition. 
#ifdef __DEBUG
unsigned int GetPageStatusCount = 0;
unsigned int TestCount = 0;
volatile unsigned int TickP;
volatile unsigned int TickE;
#endif
/****************************************************************************
 * Function:        GetPageStatus
 *
 * PreCondition:    None
 *
 * Input:           page : Page number
 *                  field : Status field
 *
 * Output:          Right justified bit value representing selected Status
 *                  Field value
 *
 * Side Effects:    None
 *
 * Overview:        This routine returns the page status for the selected page, for the
 *                  selected field. The return value is right shifted into LSB position.
 *                  The definition of the field Macro's indicate bit position.
 * Note:            This is a private function.
 *****************************************************************************/

char inline GetPageStatus(unsigned char page, unsigned char field)
{
/*
    unsigned char status;
	unsigned int  statusLong;
    // read the status bits from the specified page.
    statusLong = eedata_addr[page-1][0];
	statusLong = statusLong >> field;
	status = statusLong & 0x00000001;
*/
#ifdef  __DEBUG
	GetPageStatusCount ++;
#endif
	// Or like this:
    // Read the status bits from the specified page.
	return ((eedata_addr[page-1][0] >> field) & 0x01);

/*
    unsigned char statusByte;
    unsigned char status;

    // read the status bits from the specified page.
    statusByte = (eedata_addr[page-1][0] >> 17) & 0xF;
    switch(field)
    {
        case STATUS_ACTIVE:
            status = ((statusByte & 1) >> 0);
            break;
        case STATUS_CURRENT:
            status = ((statusByte & 2) >> 1);
            break;
        case STATUS_AVAILABLE:
            status = ((statusByte & 4) >> 2);
            break;
        case STATUS_EXPIRED:
            status = ((statusByte & 8) >> 3);
            break;
        default:
            status = 0;
            break;
    }
    return(status);
 */
}

/****************************************************************************
 * Function:        ErasePage
 *
 * PreCondition:    None
 *
 * Input:           page : Page number
 *
 * Output:          Status from NVM operations.
 *             = 0  Success
 *             = 2  Page is Expired
 *             = 7  NVM WriteError
 *             = 8  LowVoltageError
 *                  dataEEFlags structure is updated if error ocurred.
 *
 * Side Effects:    Generates CPU stall during program/erase operations
 *                  
 * Overview:        This routine erases the selected page and update the status 
 *                  bits by incrementing the erase count.
 *
 * Note:            This is private function.
 *****************************************************************************/
unsigned int ErasePage(unsigned char page)
{
    unsigned int currentStatus;
    unsigned int retCode;

	// Increment the erase counter.
	currentStatus = (0x0000FFFF & eedata_addr[page-1][0]) +1;
//	currentStatus = (0xFFFF0000 | eedata_addr[page-1][0]) +1; 

    if (currentStatus > ERASE_WRITE_CYCLE_MAX)
	{												// Mark page as expired
        retCode = NVMWriteWord((void*)eedata_addr[page-1], 0xFFEFFFFF);
		SetPageExpiredPage(1);
		if (!retCode)
			return 2;
    }
    else
    {
        retCode = NVMErasePage((void*)eedata_addr[page-1]);
        if (!retCode)								// Write the Erase count 
		{	currentStatus = currentStatus | 0xFFFF0000;
			retCode = NVMWriteWord((void*)eedata_addr[page-1], currentStatus);
	}	}
    
    if(retCode & _NVMCON_LVDERR_MASK)
    {
        SetLowVoltageError(1);
        return (8);
    }
    else if(retCode & _NVMCON_WRERR_MASK)
    {
        SetPageWriteError(1);
        return (7);
    }
    return 0;
}

/****************************************************************************
 * Function:        PrevPage
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          return the previous page
 *
 * Side Effects:    None
 *
 * Overview:        This routine gives you the page before the current page
 *
 * Note:            This is a private function.
 *****************************************************************************/

unsigned int PrevPage(unsigned int currentPage)
{
   unsigned int prevPage;

   prevPage = currentPage - 1;
   if(currentPage == 1)
   prevPage = NUM_DATA_EE_PAGES;
   return prevPage;
}

/****************************************************************************
 * Function:        GetNextAvailCount
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Page offset to next available location
 *
 * Side Effects:    None
 *
 * Overview:        This routine finds the current page and performs a backward search to find
 *                  the first available location. The available location is determined by
 *                  reading a 0xFFFF in the address region. The returned value can be added
 *                  to the first address in page to compute the available address. A return
 *                  value of 0xFFFF means the entire page was filled which is an error condition.
 *                  This routine can be called by the user to determine how full the current
 *                  page is prior to a pack.
 *
 * Note:            This is a public function.
 *****************************************************************************/
unsigned int GetNextAvailCount(void)
{
    int i = 0;
    unsigned int addrEEval;
    unsigned int nextAvailCount = 0;

    if (CurrentPage == 0)
    {
       SetPageCorruptStatus(1);
    } 
    else	// A Current page is located.
    {	i = AddrIndex >> 2;
		nextAvailCount = AddrIndex;
       do
       {
          addrEEval = eedata_addr[CurrentPage-1][4 + i];
          if((addrEEval >> 16) == 0xFFFF)
          {
             LowerAddress = 0;
             break;
          }
          else if((addrEEval & 0xFFFF) == 0xFFFF)
          {
             LowerAddress = 1;
             break;
          }
          i++;
          nextAvailCount +=4;
       }while(nextAvailCount < DATA_OFFSET);
    }
         
    if ((nextAvailCount == DATA_OFFSET) && (ActivePage > 1))
    {
       nextAvailCount = 0xFFFF;  // Error - No available locations
    }
 
    return(nextAvailCount);
}

/****************************************************************************
 * Function:        EmulationCheckSum
 *
 * PreCondition:    None
 *
 * Input:           data
 *
 * Output:          return the checksum
 *
 * Side Effects:    None
 *
 * Overview:        This routine gives you the checksum for the data. Checksum will be
 *                  stored in upper 6 bits of 16 bit address value. This is mainly used
 *                  for the data integrity purpose. 
 *
 * Note:            This is a private function.
 * Note AB:			This checksum routine gives checksum = 0 for data == 0, 
 *                  this may be difficult to distuinguish from nothing set.
 *                  The maximum checksum from a 32 bit word is 0x30. 
 *                  If starting with a seed of 3 or 4, the checksum value of 0x0
 *                  may be distinguished as invalid.
 *****************************************************************************/

unsigned char EmulationCheckSum(unsigned int data)
{
   unsigned char sum;
   
   sum = 4 + (0x3 & data);	// Start with seed 4, AB
// sum = 0x3 & data;        // Original.

   while((data = data >> 2))
      sum = sum + (0x3 & data);
   return sum;
}

/*****************************************************************************
 *	Function:	DataEEVerifyPage
 *
 *	Overview:	Verification of the whole EEdata page.
 * 				There are several possible outcomes:
 *
 *	Precondition:	None.
 *
 * 	Input:          page : Page number in eedata_addr.
 *
 *	Output: 
 *			11.	The memory contain all 0xFFFFFFFF 
 *	    		    as when erased during  programming. 
 *					The page may be initialized 
 *					for DataEE operation without erase.
 *			12.	The memory contain all 0x00000000 
 *	        		as when initialized as an array by the compiler. 
 *	        		A NVM erase will be needed.
 *			 3.	The memory contain random data not beeing a DataEE page.
 *			 2.	The memory contain a DataEE page that is expired.
 *			 6.	The memory contain a DataEE page with corrupted contents.
 *			 9.	One or more entries has failed the Checksum test,
 *          		number of failed words in the high end of dataEEFlags
 *		    		page is not expired and has a valid Erase counter.
 *			 0.	Page contain a valid DataEE datastructure.
 *			13. Page contain DataEE data, but is marked as Not Active. A Page Erase may be used.
 *	Arne Bergseth	17. November 2013 
 ***************************************************************************/
unsigned int DataEEVerifyPage(unsigned char Page) 
{
	unsigned int data, addr;
    unsigned int addLoc;
    unsigned int dataLoc;
    int addrIndex, ErrCnt = 0;
    unsigned int addrRead;

	int i,	p = Page -1;
	if (eedata_addr[p][0] == 0x0)	// Zero
	{	for (i = 1; i < NUMBER_OF_INSTRUCTIONS_IN_PAGE; i++)
		{	if (eedata_addr[p][i] != 0x0)
			{	SetPageCorruptStatus(1);	// Page contain garbage data 
				return 6;
		}	}
		return 12;
	}
	if (eedata_addr[p][0] == 0xFFFFFFFF)	// Erased
	{	for (i = 1; i < NUMBER_OF_INSTRUCTIONS_IN_PAGE; i++)
		{	if (eedata_addr[p][i] != 0xFFFFFFFF)
			{	SetPageCorruptStatus(1);	// Page contain garbage data 
				return 6;
		}	}
		return 11;
	}
	// Check for other signs of corruption
		p = Page -1;
		if ((eedata_addr[p][0] & 0xFFE10000) != 0xFFE10000)
		{	SetPageCorruptStatus(1); 
			return 6;
		}
		if ((eedata_addr[p][0] & 0x0000FFFF) > ERASE_WRITE_CYCLE_MAX
		&&  (eedata_addr[p][0] & 0x0000FFFF) != 0xFFFF)
		{	SetPageExpiredPage(1);
			return 2;
		} 
		if ((eedata_addr[p][0] & 0x00100000) == 0)	// Check expired flag 
		{	SetPageExpiredPage(1); 
			return 2;
		}
		// Possible check for invalid combinations of Status bits.

	addLoc = (unsigned int)eedata_addr[Page-1] + 16;
	addrIndex = DATA_OFFSET -4;
    do // Scan the page
    {
		addrRead = (*(int*)(addrIndex+addLoc));
		dataLoc = (addrIndex*2) + DATA_OFFSET + 4 + addLoc;
		if ((addrRead & 0xFFFF) == 0xFFFF)	// Erased
		{	data = (*(int*)(dataLoc));
			if (data != 0xFFFFFFFF)			// Check data word is erased.
			{	SetPageCorruptStatus(1);
				ErrCnt ++; } 
		}
		else
		{
			addr = addrRead & 0x3FF;	// Lower half word 
			data = (*(int*)(dataLoc));
			if(((addrRead & 0xFC00)>>0xA) == EmulationCheckSum(data))
			{	if (GetPageStatus(Page, STATUS_ACTIVE) == PAGE_NOT_ACTIVE) return 13; //   return (0); //Success
			}
			else
			{	SetCheckSumError(1);
				ErrCnt ++;
			} 
		}

		// Check the higher halfword of Address 
		dataLoc = (addrIndex*2) + DATA_OFFSET + addLoc;
		if ((addrRead & 0xFFFF0000) == 0xFFFF0000)	// Erased
		{
			data = (*(int*)(dataLoc));
			if (data != 0xFFFFFFFF)				// Check Data word
			{	SetPageCorruptStatus(1); 
				ErrCnt ++; 
		}	}
		else
		{
			addr = (addrRead>>16) & 0x3FF;
			data = (*(int*)(dataLoc));
			if(((addrRead & 0xFC000000)>>0x1A) == EmulationCheckSum(data))
			{ if (GetPageStatus(Page, STATUS_ACTIVE) == PAGE_NOT_ACTIVE) return 13; // return 0; //Success
			}
			else
			{	SetCheckSumError(1);
				ErrCnt ++;
			}
		}
		addrIndex -=4;
	} while(addrIndex >= 0);
	if (ErrCnt) 
	{	dataEEFlags.errorCount += ErrCnt;	// Accumulate
		if (GetPageCorruptStatus())
			return 6;
		if (GetCheckSumError())
			return 9;
	}
	return 0;
}
/****************************************************************************
 * Function:        DataEEInit
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Check the dataEEFlags for the error status.
 *                  value 0 for success.
 *                  Value 2 for expired page.
 *                  Value 6 for page corrupt status.
 *                  Value 7 for write error.
 *                  Value 8 for Low voltage operation.
 *                  Value 9 for Checksum failure.
 *
 * Side Effects:    Data EE flags may be updated.
 *
 * Overview:        This routine finds an unexpired page to become an active page.
 *                  It then counts the number of active pages.
 *                  If no active pages are found, the first page
 *                  not expired is initialized for emulation. 
 *                  If one or two active pages found, it assumes
 *                  a reset occurred and the function does nothing. 
 *                  If three active pages are found,
 *                  it assumes a reset occurred during a pack. The page
 *                  after current is erased and a pack is called.
 *                  This function must be called prior to any other operation.
 *
 * Note:            This is a public function.
 *
 * Change History:
 * Arne Bergseth    Ensure that Expired page is Not counted as Active. 
 *****************************************************************************/
unsigned int DataEEInit(void)
{
    unsigned int pageCount;
	unsigned int expiredPage = 0;
	unsigned int firstPage = 0;
    unsigned int retCode;
	unsigned int CheckSumError = 0;
	unsigned int status;
    
    dataEEFlags.val = 0;
	AddrIndex = 0;

    // Check all pages.
    for (pageCount = 1; pageCount <= NUM_DATA_EE_PAGES; pageCount++)
    {
		// Verification of the whole eedata page.
		status = DataEEVerifyPage(pageCount);
		if (status == 6)
		{	SetPageCorruptStatus(1); 
			continue; 
		}
	 	if (status == 11)	// Page is erased.
		{	// Initialize page, Not Active, Erase count 0
            retCode = NVMWriteWord((void*)eedata_addr[pageCount -1], 0xFFFF0000);
	        if (retCode & _NVMCON_LVDERR_MASK)
	        {   SetLowVoltageError(1);
	            return (8);
	        }
	        else if(retCode & _NVMCON_WRERR_MASK)
	        {   SetPageWriteError(1);
    	        return (7);
	        }
		}
		if (status == 12)	// Page is all zero.
		{					// ErasePage will increment Erase counter.
			retCode = ErasePage(pageCount);	
			if (retCode) 
				return retCode;
		}
		if (status == 13)	// Page contain data but is not active.
		{					// ErasePage will increment Erase counter.
			retCode = ErasePage(pageCount);	
			if (retCode) 
				return retCode;
		}
		if (status == 2)
		{	expiredPage += 1;
//			SetPageExpiredPage(1);
			continue;
		}
		if (status == 9)	// Page contain data with Checksum error(s).
		{	CheckSumError += 1;
			SetCheckSumError(1);
			continue;
		}
		if (firstPage == 0)
			firstPage = pageCount;
    }
	if (GetPageCorruptStatus() & (firstPage == 0))
		return (6);
	if (expiredPage == NUM_DATA_EE_PAGES) 
		return (2);
	if (CheckSumError == NUM_DATA_EE_PAGES)
		return (9);	// Data verification failed in all pages, 
					// number of failed words in high halfword 
					// of dataEEFlags.

    // Count Active pages and identify Current page.
	ActivePage = 0;	CurrentPage = 0;
    for (pageCount = 1; pageCount <= NUM_DATA_EE_PAGES; pageCount++)
	{ // Ensure that Expired page is Not counted as Active. 
      if(GetPageStatus(pageCount, STATUS_EXPIRED) == PAGE_NOT_EXPIRED)
      { 
		if(GetPageStatus(pageCount, STATUS_ACTIVE) == PAGE_ACTIVE)
        {
          ActivePage++;	// Count Active pages
          if(GetPageStatus(pageCount, STATUS_CURRENT) == PAGE_CURRENT)
          {
            CurrentPage = pageCount;
          }
        }
      }
    }
	if (CheckSumError)
		status = 9;	// Checksum verification failed in some pages,
					// number of failed words in high halfword of 
	else			// dataEEFlags 
		status = 0;

    //If no active pages found, initialize page 1
    if(ActivePage == 0)
    {
		// Initialize first page, Active & Current, keep Erase count.
        retCode = NVMWriteWord((void*)(eedata_addr[firstPage -1]), 0xFFFD0FFF);
        if(retCode & _NVMCON_LVDERR_MASK)
        {
            SetLowVoltageError(1);
            return (8);
        }
        else if(retCode & _NVMCON_WRERR_MASK)
        {
            SetPageWriteError(1);
            return (7);
        }
		CurrentPage = firstPage;
		ActivePage = 1;
		AddrIndex = 0;
        return(status);
    }
    //If Full active pages, erase the page after the current page
    else if(ActivePage == NUM_DATA_EE_PAGES)
    {
        ErasePage((CurrentPage % NUM_DATA_EE_PAGES)+1); // Erase the page after the current page
		AddrIndex = 0;
		AddrIndex = GetNextAvailCount();
        if (AddrIndex == 0xFFFF)	//Page full
        {
            PackEE();
        }
        return(status);
    }
    //If some active pages, do nothing
    else if(ActivePage > 0)
    {								// Find index to free area in CurrentPage.
		AddrIndex = 0;
		AddrIndex = GetNextAvailCount();
		if (AddrIndex == 0xFFFF)	// Check if CurrentPage is full.
			status = PackEE();
        return(status);
    }
    else
    {
        SetPageCorruptStatus(1);
        return(6);
    }
}

/****************************************************************************
 * Function:        DataEEWrite
 *
 * PreCondition:    None
 *
 * Input:           Data EE address and data
 *
 * Output:          Check the dataEEFlags for the error status.
 *                  value 0 for success.
 *                  Value 2 for expired page.
 *                  Value 4 for pack skipped.
 *                  Value 5 for Illegal address.
 *                  Value 6 for page corrupt status.
 *                  Value 7 for write error.
 *                  Value 8 for Low voltage operation.
 *
 * Side Effects:    Data EE flags may be updated. CPU stall occurs for flash
 *                  programming. Pack may be generated.
 *
 * Overview:        This routine verifies the address is valid. If not, the Illegal Address
 *                  flag is set and an error code is returned. It then finds the active page.
 *                  If an active page can not be found, the Page Corrupt status bit is set
 *                  and an error code is returned. A read is performed, if the data was not
 *                  changed, the function exits. If the last location is programmed, the Pack
 *                  Skipped error flag is set (one location should always be available). The
 *                  data EE information address and data is programmed and verified. The data
 *                  checksum is written along with the address. 10 LSBits are allocated for 
 *                  address and 6 bits are allotted for checksum. If the verify fails, 
 *                  the Write Error flag is set. If the write went into the last location 
 *                  of the page, pack is called. This function can be called by the user.
 *
 * Note:            This is a public function.
 *
 * Revision:
 * Arne Bergseth    Active page and Current page is not searched here, 
 *                  use module static values, 
 *                  maintained when a page status change occur.
 *****************************************************************************/
unsigned int DataEEWrite(unsigned int data, unsigned int addr)
{
    unsigned int nextAddLoc;
    unsigned int addLoc;
    unsigned int nextDataLoc;
    unsigned int addCheckSum;
             int dataRead;
    unsigned int retCode;

	// Check addr argument.
    if (addr >= DATA_EE_SIZE)
    {
        SetPageIllegalAddress(1);
        return(5);
    }

    // Do not write data if it did not change
    retCode = DataEERead(&dataRead,addr);
    if ((retCode == 0) && (dataRead == data))	// && (dataEEFlags.addrNotFound == 0))
    {
        return(0);		// Data is Found and Equal.
    }
    else if ((retCode > 1))	//  || (dataEEFlags.val >1))
    {   return (retCode); 	//error condition
    }

    // Data is not found or not equal. 
	// Use next available index.
    AddrIndex = GetNextAvailCount();
    if (AddrIndex == 0xFFFF)	// Page Full
    {
        SetPagePackSkipped(1);
        return(4);  //Error - Number of writes exceeds page size
    }
    addLoc = (unsigned int)eedata_addr[CurrentPage-1]+16;
    nextAddLoc = addLoc + AddrIndex;
    addCheckSum = (unsigned int)EmulationCheckSum(data);
    addCheckSum = addCheckSum << 0xA;
	dataEEFlags.addrNotFound = 0; // Address has been determined.
    if (LowerAddress == 0)
    {
        addr = ((addCheckSum | addr)<<16)|0xFFFF;
        retCode = NVMWriteWord((void*)(nextAddLoc), addr); //Writing address to the location
        nextDataLoc = (AddrIndex*2) + DATA_OFFSET + addLoc;
        if(!retCode)
            retCode = NVMWriteWord((void*)(nextDataLoc), data); //Writing data to the location
        if(retCode & _NVMCON_LVDERR_MASK)
        {
            SetLowVoltageError(1);
            return (8);
        }
        else if(retCode & _NVMCON_WRERR_MASK)
        {
            SetPageWriteError(1);
            return (7);
        }
        //Check whether data and address are written correctly.
        if((addr != (*(int *)(nextAddLoc))) || (data != (*(int *)(nextDataLoc))))
        {
            SetPageWriteError(1);
            return(7);  //Error - RAM does not match PM
        }
    }
    else if (LowerAddress == 1)
    {
        addr = addCheckSum | addr | 0xFFFF0000;
        retCode = NVMWriteWord((void*)(nextAddLoc), addr); //Writing address to the location
        nextDataLoc = (AddrIndex*2) + DATA_OFFSET + 4 + addLoc;
        if(!retCode)
            retCode = NVMWriteWord((void*)(nextDataLoc), data); //Writing data to the location
        if(retCode & _NVMCON_LVDERR_MASK)
        {
            SetLowVoltageError(1);
            return (8);
        }
        else if(retCode & _NVMCON_WRERR_MASK)
        {
            SetPageWriteError(1);
            return (7);
        }
        //Check whether data and address are written correctly.
        if(((addr<<16) != ((*(int *)(nextAddLoc))<<16)) || (data != (*(int *)(nextDataLoc))))
        {
            SetPageWriteError(1);
            return(7);  //Error - RAM does not match PM
        }
    }
    //Pack if page is full
    if (LowerAddress == 1)	// If only one page, 
							// the contents cannot be packed, 
							// and no more data can be stored.
    {	if (((AddrIndex + 4) == DATA_OFFSET) && (NUM_DATA_EE_PAGES == 1))
		{	SetPageCorruptStatus(1);
			return (6);
		}					// With only 2 pages, the contents must be packed.
   else if (((AddrIndex + 4) == DATA_OFFSET) && (NUM_DATA_EE_PAGES == 2))
		{	retCode = PackEE();
			if (retCode)
				return (retCode);
		}
   else if (((AddrIndex + 4) == DATA_OFFSET) && (ActivePage == 1))
        {
            // Mark the page as not_current and active
            retCode = NVMWriteWord((void*)(addLoc-16), 0xFFF5FFFF);
            if(!retCode)
            {	ActivePage ++;

	            // Mark the next page as current and active.
				retCode = NVMWriteWord((void*)eedata_addr[CurrentPage % NUM_DATA_EE_PAGES], 0xFFFDFFFF); 
                CurrentPage = (CurrentPage % NUM_DATA_EE_PAGES) +1;
				AddrIndex = 0;
			}
            if(retCode & _NVMCON_LVDERR_MASK)
            {
                SetLowVoltageError(1);
                return (8);
            }
            else if(retCode & _NVMCON_WRERR_MASK)
            {
                SetPageWriteError(1);
                return (7);
            }
        }
        else if (((AddrIndex + 4) == DATA_OFFSET) && (ActivePage == 2))	// Both active pages are full then pack the page.
        {
            retCode = PackEE();
			if (retCode)
				return (retCode);
        }	else
		{	AddrIndex += 4;		// Next address.
		LowerAddress = 0; }
    }
	else
		LowerAddress = 1;

    return(0);
}

/****************************************************************************
 * Function:        DataEERead
 *
 * PreCondition:    None
 *
 * Input:           Read pointer and Data EE address
 *
 * Output:          Check the dataEEFlags for the error status.
 *                  value 0 for success.
 *                  Value 1 for address not found.
 *                  Value 5 for Illegal address.
 *                  Value 6 for page corrupt status.
 *
 * Side Effects:    Data EE flags may be updated.
 *
 * Overview:        This routine verifies whether the address is valid. If not, the Illegal Address
 *                  flag is set and 0 is returned. It then finds the active page. If an
 *                  active page can not be found, the Page Corrupt status bit is set and
 *                  0 is returned. A reverse search of the active page attempts to find
 *                  the matching address in the program memory. If a match is found,
 *                  the corresponding data EEPROM data is returned, otherwise 0
 *                  is returned. This function can be called by the user.
 *
 * Note:            This is a public function.
 *****************************************************************************/
unsigned int DataEERead( int *data, unsigned int addr)
{
    unsigned int addLoc;
    unsigned int dataLoc;
    int addrIndex;
    unsigned int addrRead;

    if(addr >= DATA_EE_SIZE)
    {
        SetPageIllegalAddress(1);
        return(5);
    }

    if (ActivePage == 0)
    {
       SetPageCorruptStatus(1);
       return(6);
    }

    addLoc = (unsigned int)eedata_addr[CurrentPage-1] + 16;
	if (AddrIndex == 0xFFFF)
	    addrIndex = DATA_OFFSET -4;	// Start at top of Page.
	else
		addrIndex = AddrIndex;		// Start at Current page stack pointer.

    do // scan the current page
    {	addrRead = (*(int*)(addrIndex+addLoc));

		if ((addrRead & 0x03FF) == addr)
		  if ((addrRead & 0xFFFF) != 0xFFFF)
		  {
			dataLoc = (addrIndex*2) + DATA_OFFSET + 4 + addLoc;
			*data = (*(int*)(dataLoc));
			if(((addrRead & 0xFC00)>>0xA) == EmulationCheckSum(*data))
			{	SetaddrNotFound(0);
				return (0); //Success
			}
			else
			{
				SetPageCorruptStatus(1);
				return(6);
			}
		  }
		if ((addrRead >> 16 & 0x3FF) == addr)
		  if ((addrRead >> 16 & 0xFFFF)!= 0xFFFF)
		  {
			dataLoc = (addrIndex*2) + DATA_OFFSET + addLoc;
		   *data = (*(int*)(dataLoc));
			if(((addrRead & 0xFC000000)>>0x1A) == EmulationCheckSum(*data))
			{
				return 0; //Success
			}
			else
			{
				SetPageCorruptStatus(1);
				return(6);
			}
		  }
		addrIndex -=4;
	} while(addrIndex >= 0);

    if ((addrIndex < 0) && (ActivePage == 2))
    {
		addLoc = (unsigned int)eedata_addr[PrevPage(CurrentPage)-1] +16; //go to the previous page starting
    }

	if (ActivePage == 2)
	{
	  addrIndex = DATA_OFFSET -4; // go to the end of the page where address is stored.
	  do // scan the second active page.
	  {
		addrRead = (*(int*)(addrIndex+addLoc));
		if((addrRead & 0x3FF)== addr)
		{
			dataLoc = (addrIndex*2) + DATA_OFFSET + 4 + addLoc;
		   *data = (*(int*)(dataLoc));
			if(((addrRead & 0xFCFF)>>0xA) == EmulationCheckSum(*data))
			{
				return 0; //Success
			}
			else
			{
				SetPageCorruptStatus(1);
				return(6);
			}
		}
		else if(((addrRead>>16)&0x3FF) == addr)
		{
			dataLoc = (addrIndex*2) + DATA_OFFSET + addLoc;
		   *data = (*(int*)(dataLoc));
			if(((addrRead & 0xFCFFFFFF)>>0x1A) == EmulationCheckSum(*data))
			{
				return 0; //Success
			}
			else
			{
				SetPageCorruptStatus(1);
				return(6);
			}
		}
		addrIndex -=4;
	  } while(addrIndex >= 0);
	}
	if(addrIndex<0)
	{
		SetaddrNotFound(1);
		return(1);
	}
	return 0;
}

/****************************************************************************
 * Function:        PackEE
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Check the dataEEFlags for the error status.
 *                  value 0 for success.
 *                  Value 6 for page corrupt status.
 *                  Value 7 for write error.
 *                  Value 8 for Low voltage operation.
 *
 * Side Effects:    Generates CPU stall during program/erase operations
 *                  Data EE flags may be updated
 *
 * Overview:        This routine finds the active page and an unexpired packed page. The most
 *                  recent data EEPROM values are located for each address will be read and 
 *                  written into pack page. Page status is read from active
 *                  page and erase/write count is incremented if page 0 is packed. After all
 *                  information is programmed and verified, the current page is erased. The
 *                  packed page becomes the current page. This function can be called at any-
 *                  time by the user to schedule the CPU stall.
 *
 * Note:            This is a public function.
 *****************************************************************************/
unsigned int PackEE(void)
{
             int data;
    unsigned int addr=0;
	unsigned int addrWrite;
    unsigned int addrLoc;
	unsigned int dataLoc;
    unsigned int addrIndex=0;
    unsigned int lowerAddr=0;
    unsigned int addCheckSum;
    unsigned int retCode;
	unsigned int nextPage = 0;

#ifdef __DEBUG
    int pageCount;
    int activePage = 0;
    int currentPage = 0;

    // Check the active page.
    for (pageCount = 1; pageCount <= NUM_DATA_EE_PAGES; pageCount++)
	 // Ensure that Expired page is Not counted as Active. 
    { if(GetPageStatus(pageCount, STATUS_EXPIRED) == PAGE_NOT_EXPIRED)
      { if(GetPageStatus(pageCount, STATUS_ACTIVE) == PAGE_ACTIVE)
        { activePage++;
          if(GetPageStatus(pageCount, STATUS_CURRENT) == PAGE_CURRENT)
            currentPage = pageCount;
    } } }
	if ((activePage != ActivePage) || (currentPage != CurrentPage))
		TestCount ++;
	TickP = _CP0_GET_COUNT();;
#endif

	if (AddrIndex == 0xFFFF) 
		AddrIndex = DATA_OFFSET -4;

	if (ActivePage == 1 && NUM_DATA_EE_PAGES > 2)
	{ ; }		// No action needed
	else if ((ActivePage == 2 && NUM_DATA_EE_PAGES > 2) 
		 ||  (ActivePage == 1 && NUM_DATA_EE_PAGES == 2))
	{	int status = 0;
		unsigned int packPage = (CurrentPage % NUM_DATA_EE_PAGES) +1;
		addrLoc = (unsigned int)eedata_addr[packPage -1] + 0x10;
		dataLoc = addrLoc + DATA_OFFSET;
        do
        {
    //        addr = addCount;            
			status = DataEERead(&data,addr);
            if (status == 1)
            {	addr++;	//	addCount++;
                continue;
            }
            else if(status > 1)
            {
                SetPageCorruptStatus(1);
                return (6);
            }
            addCheckSum = (unsigned int)EmulationCheckSum(data);
            addrWrite = addCheckSum << 0xA | addr;
            if(lowerAddr == 0)
            {
                addrWrite = addrWrite << 16 | 0xFFFF;
                retCode = NVMWriteWord((void*)(addrLoc), addrWrite);
                if(!retCode)
                    retCode = NVMWriteWord((void*)(dataLoc), data);
                if(retCode & _NVMCON_LVDERR_MASK)
                {
                    SetLowVoltageError(1);
                    return (8);
                }
                else if(retCode & _NVMCON_WRERR_MASK)
                {
                    SetPageWriteError(1);
                    return (7);
                }
                //Check whether data and address are written correctly.
                if((addrWrite != (*(int *)(addrLoc))) || (data != (*(int *)(dataLoc))))
                {
                    SetPageWriteError(1);
                    return(7);  //Error - RAM does not match PM
                }
                lowerAddr = 1;
				dataLoc += 4;
            }
            else if(lowerAddr == 1)
            {
                addrWrite = addrWrite | 0xFFFF0000;
                retCode = NVMWriteWord((void*)(addrLoc), addrWrite);
                if(!retCode)
                   retCode = NVMWriteWord((void*)(dataLoc), data);
                if(retCode & _NVMCON_LVDERR_MASK)
                {
                    SetLowVoltageError(1);
                    return (8);
                }
                else if(retCode & _NVMCON_WRERR_MASK)
                {
                    SetPageWriteError(1);
                    return (7);
                }

                //Check whether data and address are written correctly.
                if(((addrWrite<<16) != ((*(int *)(addrLoc))<<16)) || (data != (*(int *)(dataLoc))))
                {
                    SetPageWriteError(1);
                    return(7);  //Error - RAM does not match PM
                }
                lowerAddr = 0;
                addrIndex +=4;
				addrLoc += 4;
				dataLoc += 4;
            }
            addr ++;
/*
 *			If DATA_EE_SIZE is larger than 680,
 *			Two pages may be needed to pack the 
 *			data before the Current page and the Previous pages
 *			are released, and the pointers used for reading can be reused.
 *			Also 4 Pages or more of flash memory will be needed.
 */ 
#if (NUM_DATA_EE_PAGES > 3) && (DATA_EE_SIZE > (DATA_OFFSET / 2))
			//   Change to writing the next Page.
		    if ((addrIndex == DATA_OFFSET) && (lowerAddr == 0))	
			{	nextPage = (packPage % NUM_DATA_EE_PAGES) +1;       
				addrLoc = (unsigned int)eedata_addr[nextPage -1] + 0x10;	
				dataLoc = addrLoc + DATA_OFFSET;
			}
#elif (NUM_DATA_EE_PAGES < 4) && (DATA_EE_SIZE > (DATA_OFFSET / 2))
	#error "Address size >= 680 need 4 Flash pages minimum."
#endif
		} while (addr < DATA_EE_SIZE);	// (addCount < DATA_EE_SIZE);

#ifdef __DEBUG
	TickP = _CP0_GET_COUNT() - TickP;
	TickE = _CP0_GET_COUNT();
#endif

        if (addrIndex < DATA_OFFSET)
        {						// Space still available in packPage
								// Mark the packed page as Active and Current.
            retCode = NVMWriteWord((void*)eedata_addr[packPage-1], 0xFFFDFFFF);
            status = ErasePage(CurrentPage);	// Erase the old Current page

			if (NUM_DATA_EE_PAGES > 2 && ActivePage == 2)	// Erase the Previous page
            {	status = ErasePage(PrevPage(CurrentPage));
				ActivePage --;
			}
			CurrentPage = packPage;				// Packed page is now Current.
        }
		else if (nextPage && NUM_DATA_EE_PAGES > 3)
        {	// Pack data use 2 pages. Mark as Active and Not Current.
            retCode = NVMWriteWord((void*)eedata_addr[packPage-1], 0xFFF5FFFF); //mark the packed page as active and not current.
           	retCode = NVMWriteWord((void*)eedata_addr[nextPage-1], 0xFFFDFFFF);
            status = ErasePage(CurrentPage);	// Erase the CurrentPage
												// Erase also Previous page if active.
			if (NUM_DATA_EE_PAGES > 2 && ActivePage == 2) 
            {	status = ErasePage(PrevPage(CurrentPage));
				ActivePage --;
			}
			ActivePage ++;
			CurrentPage = nextPage;			// Next page is now Current.		
		}
        else
        {	// Pack page is filled. Mark as Active and Not Current.
            retCode = NVMWriteWord((void*)eedata_addr[packPage-1], 0xFFF5FFFF); //mark the packed page as active and not current.
            status = ErasePage(CurrentPage);	// Erase the CurrentPage
												// Erase also Previous page if active.
			if (NUM_DATA_EE_PAGES > 2 && ActivePage == 2) 
            {	status = ErasePage(PrevPage(CurrentPage));
				ActivePage --;
			}
			// Mark the next page as Current and Active.
			nextPage = (packPage % NUM_DATA_EE_PAGES) +1;
    	   	retCode = NVMWriteWord((void*)eedata_addr[nextPage-1], 0xFFFDFFFF);
			if (!retCode)
			{	ActivePage ++;
				CurrentPage = nextPage;		// Next page is now Current.		
			}
			else
			{	ActivePage = 1;
				CurrentPage = packPage;		// Packed page is now Current.
	        }
		}
		AddrIndex = 0;						// Reset index.
		AddrIndex = GetNextAvailCount();	// Search for erased area.
        if (retCode & _NVMCON_LVDERR_MASK)
        {
            SetLowVoltageError(1);
            return (8);
        }
        else if(retCode & _NVMCON_WRERR_MASK)
        {
            SetPageWriteError(1);
            return (7);
        }
//		if (status)			// Check status from ErasePage()
//			return (status);
	}
	else
	{	SetPagePackBeforeInit(1);	// Something wrong, should not be here.
    }

#ifdef __DEBUG
	// Find the current page and number of active pages again.
	activePage = 0;	currentPage = 0;
	for (pageCount = 1; pageCount <= NUM_DATA_EE_PAGES; pageCount++)
	 // Ensure that Expired page is Not counted as Active. 
    { if(GetPageStatus(pageCount, STATUS_EXPIRED) == PAGE_NOT_EXPIRED)
	  { if (GetPageStatus(pageCount, STATUS_ACTIVE) == PAGE_ACTIVE)
		{	activePage++;
		  if (GetPageStatus(pageCount, STATUS_CURRENT) == PAGE_CURRENT)
        	currentPage = pageCount;
	} } }
	if ((activePage != ActivePage) || (currentPage != CurrentPage))
		TestCount ++;

	AddrIndex = 0;						// Reset stack index.
	AddrIndex = GetNextAvailCount();	// Locate erased area.
	TickE = _CP0_GET_COUNT() - TickE;
#endif

    return(0);
}

/****************************************************************************
 * Function:        DataEEWriteArray
 *
 * PreCondition:    None
 *
 * Input:           char array pointer, address to be started and the size of array
 *
 * Output:          Check the dataEEFlags for the error status.
 *                  value 0 for success.
 *                  Value 2 for expired page.
 *                  Value 4 for pack skipped.
 *                  Value 5 for Illegal address.
 *                  Value 6 for page corrupt status.
 *                  Value 7 for write error.
 *                  Value 8 for Low voltage operation.
 *
 * Side Effects:    Data EE flags may be updated. CPU stall occurs for flash
 *                  programming. Pack may be generated.
 *
 * Overview:        This routine will write a char array of data with a given 
 *                  starting address upto the array size specified by the user.
 *                  Use DataEEWriteArray function to read the data written using this function
 *                  This is solely designed to write char array.
 *
 * Note:            This is a public function.
 *****************************************************************************/
unsigned int DataEEWriteArray(unsigned char *data, unsigned int addr, unsigned int size)
{
   int i;
   int numberOfWords;
   unsigned int status=0;
   unsigned int writeData;
   unsigned char *tempData;

   if((size%4))
   {
      numberOfWords = (size/4)+1;
   }
   else
   {
      numberOfWords = (size/4);
   }
   tempData = data;
   
   for(i=0; i<numberOfWords; i++)
   {  writeData = (((unsigned int)* tempData)
				| (((unsigned int)*(tempData+1))<<8)
				| (((unsigned int)*(tempData+2)) <<16)
				| (((unsigned int)*(tempData+3)) << 24));
      tempData +=4;
      if((status = DataEEWrite(writeData, addr))>0)
      {
         return status;
      }
      addr++;
   }
   return (0);
}

/****************************************************************************
 * Function:        DataEEReadArray
 *
 * PreCondition:    None
 *
 * Input:           data pointer, address to be started and the size of array
 *
 * Output:          Check the dataEEFlags for the error status.
 *                  value 0 for success.
 *                  Value 1 for address not found.
 *                  Value 5 for Illegal address.
 *                  Value 6 for page corrupt status.
 *
 * Side Effects:    Data EE flags may be updated. Reading the data in between the 
 *                  array address will cause incorrect data return.
 *
 * Overview:        This function will read a byte array of values starting from addr 
 *                  and will copy to the array pointer "data". User should read the
 *                  data from the starting address of the array. 
 *                  
 * Note:            This is a public function.
 * Revision:
 *	Arne Bergseth   Modified to avoid output buffer overflow
 *****************************************************************************/
unsigned int DataEEReadArray(unsigned char *data, unsigned int addr, unsigned int size)
{
   int i;
   unsigned int status=0;
   unsigned int numberOfWords, numberOfBytes;
            int readData;
   unsigned int count=0;

//   if((size%4))
//   {
//      numberOfWords = (size/4)+1;
//   }
//   else
//   {
	numberOfWords = (size/4);
//   }
   for(i=0; i<numberOfWords; i++)	// Transfer whole 32bit words
   {
      if((status = DataEERead(&readData,addr))>0)
      {
         return status;
      }
//      *(data+count++) = (readData & 0xFF000000)>>24;
//      *(data+count++) = (readData & 0xFF0000)>>16;
//      *(data+count++) = (readData & 0xFF00)>>8;
//      *(data+count++) = (readData & 0xFF);
      
      *(data+count++) = (readData & 0xFF);
      *(data+count++) = (readData & 0xFF00)>>8;
      *(data+count++) = (readData & 0xFF0000)>>16;
      *(data+count++) = (readData & 0xFF000000)>>24;
      addr++;
   }
	if((numberOfBytes = size%4))	// Transfer remaining bytes.
	{
		if((status = DataEERead(&readData,addr))>0)
			return status;

			*(data+count++) = (readData & 0xFF);
		if (numberOfBytes > 1)
			*(data+count++) = (readData & 0xFF00)>>8;
		if (numberOfBytes > 2)
			*(data+count++) = (readData & 0xFF0000)>>16;
		if (numberOfBytes > 3)
			*(data+count++) = (readData & 0xFF000000)>>24;
	}

   return (0);
}

