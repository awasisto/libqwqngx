/* QWQNG_Linux Library

  Version 1.4

  Copyright (c) 2019, The Quantum World Corporation.
  All rights reserved.
  
  New BSD License

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.
      * Neither the name of the <organization> nor the
	names of its contributors may be used to endorse or promote products
	derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "qwqng.hpp"

/*************************************************** QWQNG Class *******************************************/

QngStatus qngStatus;


/* Public Factory Method (former Singleton) */
QWQNG* QWQNG::Instance() 
{
	return new QWQNG();
};


/* Public Constructor */
QWQNG::QWQNG() : pDevice(NULL), devId_(NULL), statusString_(NULL) 
{
	pDevice = (IDevice*)new FtdiDevice(&qngStatus);

	devId_ = new char[18];
	statusString_ =  new char [64];

	pDevice->Open();
};
  
  
QWQNG::~QWQNG() 
{
	if (pDevice != NULL) 
	{	
		delete pDevice;			// release instance
		pDevice = NULL;
	}
	if (devId_ != NULL) 
	{	
		delete [] devId_;	  	// delallocate memory
		devId_ = NULL;
	}
	if (statusString_ != NULL) 
	{	
		delete [] statusString_; 	// deallocate memory
		statusString_ = NULL;
	}
};  


/* STATUS HANDLING: Property to return Status String */
char* QWQNG::StatusString() 
{
	// read status string
	std::string statusString = qngStatus.GetStatusString();
	strcpy(statusString_, statusString.c_str());

	return statusString_; 
};


/* Method to attempt to restart the hardware */
char* QWQNG::DeviceID() 
{ 
	// read from device
	std::string deviceId = pDevice->GetDeviceId();
	strcpy( devId_, deviceId.c_str() );

	return devId_;
};


/* Property to return a 32 bit random number */
int QWQNG::RandInt32(long* pVal) 
{  
	// read from device
	pDevice->GetInt32((int32_t*)pVal);  

	// Check status message
	if (qngStatus.GetStatus() == QNG_S_OK) 
		return S_OK;
	else 
		return qngStatus.GetStatus(); 
};


/* Property to return a uniform number [0,1) */
int QWQNG::RandUniform(double* pVal) 
{ 
	// read from device
	pDevice->GetUniform(pVal);

	// Check status message
	if (qngStatus.GetStatus() == QNG_S_OK) 
		return S_OK;
	else 
		return qngStatus.GetStatus(); 
};


/* Property to return a normal number */
int QWQNG::RandNormal(double *pVal) 
{ 
	// read from device
	pDevice->GetNormal(pVal);

	// Check status message
	if (qngStatus.GetStatus() == QNG_S_OK) 
		return S_OK;
	else 
		return qngStatus.GetStatus();  
};


/* Property to read byte */
int QWQNG::RandBytes(char* pval, long length) 
{ 
	if (pDevice->deviceType_ >= FTDIDEVICE_QNG_GEN4_) {
		// Check if valid length <= 65536
		if(length>65536) 
		{
			qngStatus.SetStatus(QNG_E_IO_ARRAY_OVERSIZED);
			return QNG_E_IO_ARRAY_OVERSIZED;   
		}
		if(length==0)
			return S_OK;
	}
	else {
		// Check if valid length <= 8192
		if(length>8192) 
		{
			qngStatus.SetStatus(QNG_E_IO_ARRAY_OVERSIZED);
			return QNG_E_IO_ARRAY_OVERSIZED;   
		}
		if(length==0)
			return S_OK;
	}

	// read from device
	pDevice->GetBytes(pval, length);

	// Check status message
	if (qngStatus.GetStatus() == QNG_S_OK) 
		return S_OK;
	else 
		return qngStatus.GetStatus();
};


/* Method to clear all buffers */
int QWQNG::Clear() 
{ 
	// read from device
	pDevice->Clear();

	// Check status message
	if (qngStatus.GetStatus() == QNG_S_OK) 
		return S_OK;
	else 
		return qngStatus.GetStatus();
};  


/* Method to attempt to restart the hardware */
int QWQNG::Reset() 
{ 
	DEBUGTOUCH();

	// get a real device
	if (pDevice != NULL) 
		pDevice->Reset();
	else 
	{
		pDevice = (IDevice*)new FtdiDevice(&qngStatus);
		pDevice->Open();
	}

	// Check status message
	if (qngStatus.GetStatus() == QNG_S_OK) 
		return S_OK;
	else 
		return qngStatus.GetStatus();
};


/* Property to return internal stats */
int QWQNG::RuntimeInfo(float* pVal) 
{ 
	DEBUGTOUCH();

	if (pDevice->GetDeviceType() < FTDIDEVICE_QNG_GEN2_)
	{
		qngStatus.SetStatus(QNG_E_STATS_UNSUPPORTED);
		return QNG_E_STATS_UNSUPPORTED;   
	}

	// read from device
	pDevice->GetRuntimeInfo(pVal);

	// this is not allowed to fail since it gets called on failure
	return S_OK;
};


/* Property to return Diagnostics data */
int QWQNG::Diagnostics(char dxCode, char* dxInfo)
{
	DEBUGTOUCH();

	if (pDevice->GetDeviceType() < FTDIDEVICE_QNG_GEN3_)
	{
		qngStatus.SetStatus(QNG_E_DIAGX_UNSUPPORTED);
		return QNG_E_DIAGX_UNSUPPORTED;   
	}
	
	if (dxCode == 0x19 && pDevice->GetDeviceType() < FTDIDEVICE_QNG_GEN5_)
	{
		qngStatus.SetStatus(QNG_E_DIAGX_UNSUPPORTED);
		return QNG_E_DIAGX_UNSUPPORTED;   
	}

	// fill array with data
	pDevice->Diagnostics((uint8_t)dxCode, (uint8_t*)dxInfo);

	// Check status message
	if (qngStatus.GetStatus() == QNG_S_OK) 
		return S_OK;
	else 
		return qngStatus.GetStatus();
};   


/*************************************************** FtdiDevice Class *******************************************/

Mutex atomicMutex_;

FtdiDevice::FtdiDevice(QngStatus* qngStatus) : IDevice(qngStatus), prevDxCode_(0)
{
	qngStatus->SetStatus(QNG_E_DEVICE_NOT_OPENED);
}


FtdiDevice::~FtdiDevice() 
{ 
	Close(); 
}


// opens the next FTDI QNG device, if available
void FtdiDevice::Open() 
{  
	DEBUGTOUCH();
	MutexSync mutexSync(atomicMutex_);

	FindOpen(); // Finds QNG device, opens handle
	  
	Initialize();  // Check internal stats, start RNG

	if (qngStatus->GetStatus() != QNG_S_OK) 
	{
		DEBUGOUT("BAD OPEN");
		FtdiClose(); // close libftdi context
	}
}


void FtdiDevice::Close() 
{   
	DEBUGTOUCH();
	MutexSync mutexSync(atomicMutex_);

	// Stop Device
	if (StopStreaming() != true)
		DEBUGOUT("COULD NOT STOP STREAMING!");
		
	FtdiClose(); // close libftdi context
}


void FtdiDevice::Clear() 
{
	DEBUGTOUCH();
	MutexSync mutexSync(atomicMutex_);

	if (StopStreaming() != true)
		DEBUGOUT("COULD NOT STOP STREAMING!");
}


void FtdiDevice::Reset() 
{ 
	DEBUGTOUCH();
	MutexSync mutexSync(atomicMutex_);

	// Stop Device
	if (StopStreaming() != true)
		DEBUGOUT("COULD NOT STOP STREAMING!");

	qngStatus->SetStatus(QNG_E_IO_ERROR);

	// close libftdi USB device
	if( (ftdiStatus = ftdi_usb_close(ftdic)) < 0 ) 
	{
		DEBUGOUT("ftdi_usb_close failed");
		qngStatus->SetStatus(QNG_E_IO_ERROR);
		ftdi_free(ftdic);
		return;
	}

	// Open Device
	if ((ftdiStatus = ftdi_usb_open_desc(ftdic, devVID, devPID, descInfo, serialInfo)) < 0 ) 
	{
		DEBUGOUT("BAD RESET");
		qngStatus->SetStatus(QNG_E_IO_ERROR);
		ftdi_free(ftdic);
		return;
	}
	// device reopened successfully 
	qngStatus->SetStatus(QNG_S_OK);    

	// Initialize device
	Initialize();
	if (qngStatus->GetStatus() != QNG_S_OK) 
	{
		DEBUGOUT("BAD RESET");
		FtdiClose();
		return;
	}   
}


void FtdiDevice::GetBytes(char* bytes, int bytesRequested) 
{
	MutexSync mutexSync(atomicMutex_);

	if (isStreaming_ == false)
	{
		if (StartStreaming() != true)
			DEBUGOUT("COULD NOT START STREAMING!");
	}

	if (deviceType_ >= FTDIDEVICE_QNG_GEN4_) {
		if (bytesRequested>FTDIDEVICE_FAST_MAX_ARRAY_SIZE_ || bytesRequested<0) 
		{
			qngStatus->SetStatus(QNG_E_IO_ARRAY_OVERSIZED);
			return;
		}
	}
	else {
		if (bytesRequested>FTDIDEVICE_MAX_ARRAY_SIZE_ || bytesRequested<0) 
		{
			qngStatus->SetStatus(QNG_E_IO_ARRAY_OVERSIZED);
			return;
		}
	}
	FtdiRead(bytes, bytesRequested, false);
} 


void FtdiDevice::GetInt32(int32_t* int32) 
{
	MutexSync mutexSync(atomicMutex_);

	if (isStreaming_ == false)
	{
		if (StartStreaming() != true)
			DEBUGOUT("COULD NOT START STREAMING!");
	}

	FtdiRead(int32, 4, false);
}


void FtdiDevice::GetUniform(double* uniform) 
{
	MutexSync mutexSync(atomicMutex_);

	if (isStreaming_ == false)
	{
		if (StartStreaming() != true)
			DEBUGOUT("COULD NOT START STREAMING!");
	}

	uint64_t uint48 = 0;
	FtdiRead(&uint48, 6, false);

	*uniform = Uint48ToUniform(uint48);
}


void FtdiDevice::GetNormal(double* normal) 
{
	MutexSync mutexSync(atomicMutex_);

	if (isStreaming_ == false)
	{
		if (StartStreaming() != true)
			DEBUGOUT("COULD NOT START STREAMING!");
	}

	if (haveNormU1_ == true) 
	{
		// create normU2
		uint64_t uint48 = 0;
		FtdiRead(&uint48, 6, false);
		double normU2 = Uint48ToUniform(uint48);
		normU2 += FTDIDEVICE_HALF_OF_UNIFORM_LSB_;

		// n1 = cos(2PI * u2) * sqrt(-2 * ln(u1)) 
		// n2 = sin(2PI * u2) * sqrt(-2 * ln(u1))
		double sqrtTerm = sqrt(-2.0 * log(normU1_));
		*normal = cos(FTDIDEVICE_2_PI_ * normU2) * sqrtTerm;
		normalConjugate_ = sin(FTDIDEVICE_2_PI_ * normU2) * sqrtTerm;

		haveNormU1_ = false;
	}
	else 
	{   // do not have uniform 1, fill conjugate;
		// create normU1
		uint64_t uint48 = 0;
		FtdiRead(&uint48, 6, false);
		normU1_ = Uint48ToUniform(uint48);
		normU1_ += FTDIDEVICE_HALF_OF_UNIFORM_LSB_;  

		haveNormU1_ = true;

		*normal = normalConjugate_;
	}
}


void FtdiDevice::GetRuntimeInfo(float* runtimeInfo) 
{
    DEBUGTOUCH();
    MutexSync mutexSync(atomicMutex_);
    
    // initialize to failure condition
    runtimeInfo[0] = 1.0;
    for (int i=0; i<=3; ++i) 
    {
		runtimeInfo[4*i+1] = 0.0;
		runtimeInfo[4*i+2] = 0.0;
		runtimeInfo[4*i+3] = 0.0;
		runtimeInfo[4*i+4] = 1.0;
    }
    
    // check if this chip supports runtime info
    if (deviceType_ < FTDIDEVICE_QNG_GEN2_)
    {
    	qngStatus->SetStatus(QNG_E_STATS_UNSUPPORTED);
    	return;
    }

    // Stop Device
    if (StopStreaming() != true)
    {
		DEBUGOUT("COULD NOT STOP STREAMING!");
		return;
    }

    // get test stats status
    uint32_t rngStatus = 0;
    FtdiCommandResponse(FTDIDEVICE_TEST_STATUS_COMMAND_, (uint8_t*)&rngStatus);
    if (rngStatus & FTDIDEVICE_TEST_BAD_STATS_MASK_)
        runtimeInfo[0] = -1.0;
    else
        runtimeInfo[0] = 0.0;

    // request final out stats
    FtdiCommandResponse(FTDIDEVICE_TEST_FINAL_BASE_+1, (uint8_t*)&runtimeInfo[3]);
    FtdiCommandResponse(FTDIDEVICE_TEST_FINAL_BASE_+2, (uint8_t*)&runtimeInfo[4]);
    FtdiCommandResponse(FTDIDEVICE_TEST_FINAL_BASE_+3, (uint8_t*)&runtimeInfo[2]);
    runtimeInfo[1] = CalcEntropy(runtimeInfo[2]);

    // request channel stats
    for (int j=0; j<3; j++)
    {
        FtdiCommandResponse(FTDIDEVICE_TEST_CHANNELS_BASE_ + j*0x10 + 1, (uint8_t*)&runtimeInfo[j*4 + 4 + 3]);
        FtdiCommandResponse(FTDIDEVICE_TEST_CHANNELS_BASE_ + j*0x10 + 2, (uint8_t*)&runtimeInfo[j*4 + 4 + 4]);
        FtdiCommandResponse(FTDIDEVICE_TEST_CHANNELS_BASE_ + j*0x10 + 3, (uint8_t*)&runtimeInfo[j*4 + 4 + 2]);
        runtimeInfo[j*4 + 4 + 1] = CalcEntropy(runtimeInfo[j*4 + 4 + 2]);
    }
}


void FtdiDevice::Diagnostics(uint8_t dxCode, uint8_t* dxData)
{
    DEBUGTOUCH();
    MutexSync mutexSync(atomicMutex_);

    // check if this device supports diagnostics
    if (deviceType_ < FTDIDEVICE_QNG_GEN3_)
    {
        qngStatus->SetStatus(QNG_E_DIAGX_UNSUPPORTED);
        return;
    }
	
	if (dxCode == 0x19 && deviceType_ < FTDIDEVICE_QNG_GEN5_)
    {
        qngStatus->SetStatus(QNG_E_DIAGX_UNSUPPORTED);
        return;
    }

    // clear buffer if previous data different
    if (dxCode!=prevDxCode_ && commandResponseLength_[prevDxCode_] >= 128)
    {
		isStreaming_ = true;
		if (StopStreaming() != true)
    	{
	    	DEBUGOUT("COULD NOT STOP STREAMING!");
	    	return;
    	}
    }

    FtdiCommandResponse((uint8_t)dxCode, dxData);

    prevDxCode_ = dxCode;
}


std::string& FtdiDevice::GetDeviceId()
{
    return deviceId_;
}


void FtdiDevice::SetDeviceId(std::string deviceId)
{
    deviceId_ = deviceId;
}


int FtdiDevice::GetDeviceType() 
{
    return deviceType_;
}


void FtdiDevice::FtdiClose() 
{
	DEBUGTOUCH();
  
    // close libftdi USB device
    if( (ftdiStatus = ftdi_usb_close(ftdic)) < 0 ) 
    {
		DEBUGOUT("ftdi_usb_close failed");
	  	qngStatus->SetStatus(QNG_E_IO_ERROR);
        ftdi_free(ftdic);
	  	return;
    }	 
     
    // deinitialize ftdi_context
    if (ftdic != NULL)
    {
        ftdi_free(ftdic);
        ftdic = NULL;
    }

    qngStatus->SetStatus(QNG_E_DEVICE_NOT_OPENED);   
}


void FtdiDevice::FindOpen() 
{
  
    qngStatus->SetStatus(QNG_S_OK);	// Set Status to OK
    
    int ret=0; 		// # of devices returned status
    int num_devs=0;	// number of FTDI devices found 
    int i=0;		// iterator for number of devices
    
    ftdiStatus = 0; // libftdi return value status
    
    // Set VID/PID Parameters for search
    Buffer[0].VID = 0x0403; // QNG Model R2000KU & PQ4000KU VendorID  (VID)
    Buffer[0].PID = 0x6001; // QNG Model R2000KU & PQ4000KU ProductID (PID)
    Buffer[1].VID = 0x0403; // QNG Model R32MU & PQ32MU VendorID  (VID)    
    Buffer[1].PID = 0x6014; // QNG Model R32MU & PQ32MU ProductID (PID)
    
    // Initialize Main context structure for all libftdi functions
    ftdic = ftdi_new();
    if (ftdic == NULL) 
    {
		DEBUGOUT("ftdi_init failed.");
		qngStatus->SetStatus(QNG_E_IO_ERROR);
		return;
    }

    // Find all connected QNG devices with PIDs 6001 and 6014
    for (int j=0; j<2; j++) 
    {
        if ((ret = ftdi_usb_find_all(ftdic, &Buffer[j].devlist, Buffer[j].VID, Buffer[j].PID)) < 0) 
        {
			DEBUGOUT("ftdi_usb_find_all failed.");
			qngStatus->SetStatus(QNG_E_IO_ERROR);
			return;
        }
        num_devs += ret;	// increment device count
    }

    DEBUGOUT(num_devs);

    if (num_devs == 0) 
    {	// if no devices found, quit
		qngStatus->SetStatus(QNG_E_DEVICE_NOT_OPENED);
		return;
    }
	    
    struct QNGparam QNG[num_devs]; // Init QNGparam structure

    i = 0; // initialize iterator 
    
    // get devices' parameters
    for (int k=0; k<2; k++) 
    {
        for (Buffer[k].curdev = Buffer[k].devlist; Buffer[k].curdev != NULL; i++) 
		{
			if ((ftdiStatus = ftdi_usb_get_strings(ftdic, Buffer[k].curdev->dev, QNG[i].manufacturer, 128, QNG[i].description, 128, QNG[i].serialnumber, 9)) < 0) 
			{
			    DEBUGOUT("ftdi_usb_get_strings failed");
			    qngStatus->SetStatus(QNG_E_IO_ERROR);
			    return;
			}
			Buffer[k].curdev = Buffer[k].curdev->next;	// increment next device list
        }
    }

	// Free libftdi USB device lists
    ftdi_list_free(&Buffer[0].devlist);
    ftdi_list_free(&Buffer[1].devlist);

    // Open First available QNG Model R2000KU, PQ4000KU or PQ4000KS with  Prefix "QWR2", "QWR4", "QWR6", or "QWR1"
    for (int j=0; j<num_devs; j++) 
    {
		if ( (memcmp(QNG[j].serialnumber, "QWR2", 4) == 0) || (memcmp(QNG[j].serialnumber, "QWR4", 4) == 0) || (memcmp(QNG[j].serialnumber, "QWR6", 4) == 0) || (memcmp(QNG[j].serialnumber, "QWR1", 4) == 0) ) 
		{
			if ((ftdiStatus = ftdi_usb_open_desc(ftdic, Buffer[0].VID, Buffer[0].PID, QNG[j].description, QNG[j].serialnumber)) < 0 ) 
			{
				// if device already claimed
				if ( ftdiStatus == -5) 
					continue;
				// failed to open device
				DEBUGOUT("ftdi_usb_open_desc failed");
		  		qngStatus->SetStatus(QNG_E_IO_ERROR);
		  		return;
			}

			// Store opened device's parameters for internal use
			memcpy(manuInfo,QNG[j].manufacturer, 128);
			memcpy(serialInfo,QNG[j].serialnumber, 9);
			memcpy(descInfo,QNG[j].description, 128);
			devPID = Buffer[0].PID;
			devVID = Buffer[0].VID;

			// Set deviceId = Serial Number for Public Use
			SetDeviceId(QNG[j].serialnumber);

			// Set internal buffer size
			txchunksize = FTDIDEVICE_WRITE_FRAME_SIZE_;
			rxchunksize = FTDIDEVICE_READ_FRAME_SIZE_;
			
			DEBUGOUT(serialInfo);
			break;
		}
      	// If no R2000KU/PQ4000KU found, open first available QNG Model R32MU or PQ32MU with  Prefix "QWR3" or "QWR5"
      	else if ( (memcmp(QNG[j].serialnumber, "QWR3", 4) == 0) || (memcmp(QNG[j].serialnumber, "QWR5", 4) == 0) || (memcmp(QNG[j].serialnumber, "QWR7", 4) == 0) || (memcmp(QNG[j].serialnumber, "QWR8", 4) == 0) ) 
		{ 
			if ((ftdiStatus = ftdi_usb_open_desc(ftdic, Buffer[1].VID, Buffer[1].PID, QNG[j].description, QNG[j].serialnumber)) < 0 ) 
			{
				// if device already claimed
				if ( ftdiStatus == -5) 
					continue;
				// failed to open device
				DEBUGOUT("ftdi_usb_open_desc failed");
		  		qngStatus->SetStatus(QNG_E_IO_ERROR);
		  		return;
			}

			// Store opened device's parameters  for internal use
			memcpy(manuInfo,QNG[j].manufacturer, 128);
			memcpy(serialInfo,QNG[j].serialnumber, 9);
			memcpy(descInfo,QNG[j].description, 128);
			devPID = Buffer[1].PID;
			devVID = Buffer[1].VID;

			// Set deviceId = Serial Number for Public Use
			SetDeviceId(QNG[j].serialnumber);

			// Set internal buffer size
			txchunksize = FTDIDEVICE_WRITE_FRAME_SIZE_;
			rxchunksize = FTDIDEVICE_READ_FRAME_SIZE_;
			if (memcmp(QNG[j].serialnumber, "QWR7", 4) == 0) {
				txchunksize = FTDIDEVICE_FAST_WRITE_FRAME_SIZE_;
				rxchunksize = FTDIDEVICE_FAST_READ_FRAME_SIZE_;
			}
			if (memcmp(QNG[j].serialnumber, "QWR8", 4) == 0) {
				txchunksize = FTDIDEVICE_FAST_WRITE_FRAME_SIZE_;
				rxchunksize = FTDIDEVICE_FAST_READ_FRAME_SIZE_;
			}
			break;
		}
      	qngStatus->SetStatus(QNG_E_DEVICE_NOT_OPENED);	// Set Status Device Not Opened
    }
}


// initialize QNG device
void FtdiDevice::Initialize() 
{    
    DEBUGTOUCH();

	if (qngStatus->GetStatus() != QNG_S_OK) 
    {
		DEBUGOUT("BAD OPEN");
		return;
    }
 
    deviceType_ = FTDIDEVICE_QNG_GEN1_;
    if (serialInfo[3] >= '2')
        deviceType_ = FTDIDEVICE_QNG_GEN2_;
    if (serialInfo[3] >= '4')
        deviceType_ = FTDIDEVICE_QNG_GEN3_;
    if (serialInfo[3] >= '7')
        deviceType_ = FTDIDEVICE_QNG_GEN4_;
	if (serialInfo[3] >= '8')
        deviceType_ = FTDIDEVICE_QNG_GEN5_;
    
    DEBUGOUT(serialInfo);
    DEBUGOUT(deviceType_);

    	// Set synchronous FT245 FIFO mode for high speed gen4 devices
	unsigned char Mask = 0xff;
	unsigned char Mode;
	if (deviceType_ >= FTDIDEVICE_QNG_GEN4_) {
		Mode = 0x00; // reset
		//ftdiStatus = FT_SetBitMode(deviceHandle_, Mask, Mode);
		ftdiStatus = ftdi_set_bitmode(ftdic, Mask, Mode); 	
		if (ftdiStatus != 0) {
			DEBUGOUT("SetBitMode Reset Failed");
			qngStatus->SetStatus(QNG_E_IO_ERROR);
			return;
		}
		usleep(10*1000);
		Mode = 0x40; //Sync FIFO mode
		//ftdiStatus = FT_SetBitMode(deviceHandle_, Mask, Mode);
		ftdiStatus = ftdi_set_bitmode(ftdic, Mask, Mode); 
		if (ftdiStatus != 0) {
			DEBUGOUT("SetBitMode Sync Mode Failed");
			qngStatus->SetStatus(QNG_E_IO_ERROR);
			return;
		}
	}

    // set read and write timeouts
    ftdic->usb_read_timeout = FTDIDEVICE_STREAM_TIMEOUT_MILLIS_;
    ftdic->usb_write_timeout = FTDIDEVICE_STREAM_TIMEOUT_MILLIS_;
    DEBUGOUT(ftdic->usb_read_timeout);
    DEBUGOUT(ftdic->usb_write_timeout);

    // set read frame size, TX flush timer to minimum
    if ( (ftdi_write_data_set_chunksize(ftdic, txchunksize) < 0) || (ftdi_read_data_set_chunksize(ftdic, rxchunksize) < 0) ) 
    {
		DEBUGOUT("ftdi_write/read_data_set_chunksize failed");
		qngStatus->SetStatus(QNG_E_IO_ERROR);
		return;
    }
	// Set Flow Control for PQ128MU
	if (deviceType_ >= FTDIDEVICE_QNG_GEN4_) {
	    if ((ftdiStatus = ftdi_setflowctrl(ftdic, SIO_RTS_CTS_HS)) < 0 ) 
	    {
			DEBUGOUT("ftdi_setflowctrl failed");
			qngStatus->SetStatus(QNG_E_IO_ERROR);
			return;
	    }
 	}
	

	// Set Latency
    if ( deviceType_ >= FTDIDEVICE_QNG_GEN2_)
    {
	    if ((ftdiStatus = ftdi_set_latency_timer(ftdic, 20)) < 0 ) 
	    {
			DEBUGOUT("ftdi_set_latency_timer failed");
			qngStatus->SetStatus(QNG_E_IO_ERROR);
			return;
	    }
    }
    // if internal test capable model, look for internal delimiter, will clear bufffers
    if (deviceType_ >= FTDIDEVICE_QNG_GEN2_)
    {
        if (DetermineDelimiter() != true)
        {
            DEBUGOUT("COULD NOT DETERMINE DELIMITER!");
            qngStatus->SetStatus(QNG_E_DEVICE_NOT_OPENED);
            return;
        }
        else
            qngStatus->SetStatus(QNG_S_OK);

        FtdiCommandResponse(FTDIDEVICE_TEST_CLEAR_STATS_FLAG_, NULL);
    }
    else
    {
        // gen 1 device
        qngStatus->SetStatus(QNG_S_OK);
    }

    // start streaming (also flushes buffers)
    if (StartStreaming() != true)
    {
		DEBUGOUT("COULD NOT START STREAMING!");
        return;
    }

    usleep(10*1000);

    // fill normU1, so immediate calls to GetNormal will be correct
    uint64_t uint48 = 0;
    FtdiRead(&uint48, 6, false);
    normU1_ = Uint48ToUniform(uint48);
    normU1_ += FTDIDEVICE_HALF_OF_UNIFORM_LSB_;
    haveNormU1_ = true; 
}


void FtdiDevice::FtdiRead(void* receiveBuffer, int bytesRequested, bool statsUnchecked=false) 
{
    if (bytesRequested <= 0)
    	return;
    
    // Read random bytes Requested. Bytes are stored as unsigned character
    ftdiStatus = ftdi_read_data(ftdic, (unsigned char*)receiveBuffer, bytesRequested);	  
    if (ftdiStatus < 0) 
    {
		DEBUGOUT(ftdiStatus);
		qngStatus->SetStatus(QNG_E_IO_ERROR);
    }
    else if (ftdiStatus != bytesRequested) 
    {
		DEBUGOUT(bytesRequested);
		qngStatus->SetStatus(QNG_E_IO_TIMEOUT);
	
		// test status & command response use statsUnchecked = false to prevent infinite loop
        if (statsUnchecked==false && deviceType_>=FTDIDEVICE_QNG_GEN2_)
            CheckTestStatsStatus();
    }   
}


void FtdiDevice::FtdiWrite(void* transmitBuffer, int bytesToTransmit) 
{
    // Send Data to Device
    ftdiStatus = ftdi_write_data(ftdic, (unsigned char*)transmitBuffer, bytesToTransmit);  
    if (ftdiStatus < 0) 
    {
		DEBUGOUT(ftdiStatus);
		qngStatus->SetStatus(QNG_E_IO_ERROR);
    }
    else if (ftdiStatus != bytesToTransmit) 
    {
		DEBUGOUT(bytesToTransmit);
		qngStatus->SetStatus(QNG_E_IO_TIMEOUT);
    }
}


// pre 3rd gen device, no guarantee that commands get received, so a multiple try scheme
// 3rd gen device will simply try once
bool FtdiDevice::FtdiCommandResponse(uint8_t command, uint8_t* response) 
{
    DEBUGTOUCH();
    DEBUGOUT((int)command);

    switch (command)
    {
		case FTDIDEVICE_START_COMMAND_:
		    return StartStreaming();
		    break;
		case FTDIDEVICE_STOP_COMMAND_:
		    return StopStreaming();
		    break;
		default:
		    if (isStreaming_ == true)
		        StopStreaming();

		FtdiWrite(&command, 1);

		// zero return-length command sent, return;
		DEBUGOUT(commandResponseLength_[command]);
		if (commandResponseLength_[command] == 0)
		{
			if (qngStatus->GetStatus() == QNG_S_OK)
				return true;
			else
				return false;
		}

		ftdiStatus = ftdi_read_data(ftdic, (unsigned char*)response, commandResponseLength_[command]);
		if (ftdiStatus < 0)
		{
			DEBUGOUT(ftdiStatus);
			qngStatus->SetStatus(QNG_E_IO_ERROR);
			return false;
		}

		if (ftdiStatus != commandResponseLength_[command])
		{
			DEBUGOUT(ftdiStatus);
			qngStatus->SetStatus(QNG_E_IO_TIMEOUT);
			return false;
		}
    }

    return true;
}


bool FtdiDevice::StartStreaming() 
{
    if (isStreaming_ == true)
        return true;

    int txd;
    int bytesAvailable;
    std::vector<uint8_t> readBuffer;

    if (deviceType_ >= FTDIDEVICE_QNG_GEN2_)
    {
        // set rx/tx timeouts
    	ftdic->usb_read_timeout = FTDIDEVICE_TX_TIMEOUT_MILLIS_;
	    ftdic->usb_write_timeout = FTDIDEVICE_TX_TIMEOUT_MILLIS_;
    	DEBUGOUT(ftdic->usb_read_timeout);
	    DEBUGOUT(ftdic->usb_write_timeout);

        int attemptsLeft = FTDIDEVICE_TX_ATTEMPTS_;
        do
        {
            // start timeout stopwatch
			elapsedTime = 0;
			gettimeofday(&start, NULL);

            // send redundant stop command, just in case
            uint8_t stopCommand = FTDIDEVICE_STOP_COMMAND_;
			ftdiStatus = ftdi_write_data(ftdic, &stopCommand, 1);
			DEBUGOUT(stopCommand);  
			if (ftdiStatus < 0) 
			{
				DEBUGOUT("Write stopCommand Failed.");
				qngStatus->SetStatus(QNG_E_IO_ERROR);
				return false;
			}
	
			txd = 0;
			// read out old data here (clear buffer)
            do
            {            
                // Get Queue Status
				bytesAvailable = ftdic->readbuffer_remaining;
				DEBUGOUT(bytesAvailable);

                if (bytesAvailable > 0)
                {
                    readBuffer.resize(bytesAvailable+txd);
					txd = ftdi_read_data(ftdic, &readBuffer[0], bytesAvailable);	  
					if (txd < 0) 
					{
						DEBUGOUT("read TX buffer failed.");
						qngStatus->SetStatus(QNG_E_IO_ERROR);
						return false;
					}
                }

				// stop timer
				gettimeofday(&stop, NULL);
				elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0;      // sec to ms
				elapsedTime += (stop.tv_usec - start.tv_usec) / 1000.0;   // us to ms
				DEBUGOUT(elapsedTime);

			} while (bytesAvailable!=0 && elapsedTime < FTDIDEVICE_TX_TIMEOUT_MILLIS_);

            // send delimiter command
            uint8_t delimCommand = FTDIDEVICE_DELIMIT_COMMAND_;
			ftdiStatus = ftdi_write_data(ftdic, &delimCommand, 1); 
			DEBUGOUT(delimCommand);  
			if (ftdiStatus < 0) 
			{
				DEBUGOUT("Write delimCommand Failed.");
				qngStatus->SetStatus(QNG_E_IO_ERROR);
				return false;
			}

            // read out leftover data trailed by delimiter
            int haveBytes = 0;
			bool foundDelimiter = false;
            do
            {
                // Get Queue Status
				// read one byte to refresh context
				readBuffer.resize(haveBytes+1);
				ftdiStatus = ftdi_read_data(ftdic, &readBuffer[haveBytes], 1);
				if (ftdiStatus < 0)
			    {
					DEBUGOUT("read TX buffer failed.");
			        qngStatus->SetStatus(QNG_E_IO_ERROR);
			        return false;
			    }
				if(ftdiStatus > 0)
					haveBytes += 1;

				bytesAvailable = ftdic->readbuffer_remaining;
		    	DEBUGOUT(bytesAvailable);

                txd = 0;
                if (bytesAvailable > 0)
                {
                    readBuffer.resize(haveBytes+bytesAvailable);
					txd = ftdi_read_data(ftdic, &readBuffer[haveBytes], bytesAvailable);	  
					if (txd < 0) 
					{
						DEBUGOUT("Read TX buffer failed.");
						qngStatus->SetStatus(QNG_E_IO_ERROR);
						return false;
					}
                }

                haveBytes += txd;
	    		DEBUGOUT(haveBytes); 

                // if the last bytes found the delimiter - stopped and cleared here, done
                if (haveBytes>=6 && memcmp(&readBuffer[haveBytes-6], delimiter_, 6) == 0)
				{
					foundDelimiter = true;
				    break;
				}

				// stop timer
				gettimeofday(&stop, NULL);
				elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0;      // sec to ms
				elapsedTime += (stop.tv_usec - start.tv_usec) / 1000.0;   // us to ms
		    	DEBUGOUT(elapsedTime);

			} while (elapsedTime < FTDIDEVICE_TX_TIMEOUT_MILLIS_);

			if (foundDelimiter == true)
		   	{

			    // send start command
			    uint8_t startCommand = FTDIDEVICE_START_COMMAND_;
				ftdiStatus = ftdi_write_data(ftdic, &startCommand, 1);  
				DEBUGOUT(startCommand); 
				if (ftdiStatus < 0) 
				{
					DEBUGOUT("Write startCommand Failed.");
					qngStatus->SetStatus(QNG_E_IO_ERROR);
					return false;
				}
			    
			    std::vector<uint8_t> discardBuffer(FTDIDEVICE_START_DISCARD_BYTES_);
				ftdiStatus = ftdi_read_data(ftdic, &discardBuffer[0], FTDIDEVICE_START_DISCARD_BYTES_);
			    if (ftdiStatus < 0)
			    {
			        DEBUGOUT("discard TX buffer failed.");
			        qngStatus->SetStatus(QNG_E_IO_ERROR);
			        return false;
			    }

			    // read out discard bytes, streaming data, done
			    if (ftdiStatus == FTDIDEVICE_START_DISCARD_BYTES_)
			    {
			        // reset rx/tx timeouts to default
					ftdic->usb_read_timeout = FTDIDEVICE_STREAM_TIMEOUT_MILLIS_;
			    	ftdic->usb_write_timeout = FTDIDEVICE_STREAM_TIMEOUT_MILLIS_;
					DEBUGOUT(ftdic->usb_read_timeout);
				    DEBUGOUT(ftdic->usb_write_timeout);

			        isStreaming_ = true;
			        return true;
	        	}
			}

        } while (--attemptsLeft > 0);

        qngStatus->SetStatus(QNG_E_IO_TIMEOUT);
        return false;
    }
    else // first generation device (J1000KU)
    {
        int attemptsLeft = FTDIDEVICE_TX_ATTEMPTS_;
        do
        {
            // start timeout stopwatch
			elapsedTime = 0;
			gettimeofday(&start, NULL);

            // send start command
            uint8_t startCommand = FTDIDEVICE_START_COMMAND_;
			FtdiWrite(&startCommand, 1);

            do
            {                
                std::vector<uint8_t> discardBuffer(FTDIDEVICE_START_DISCARD_BYTES_);
	    		ftdiStatus = ftdi_read_data(ftdic, &discardBuffer[0], FTDIDEVICE_START_DISCARD_BYTES_);
                if (ftdiStatus < 0)
                {
                    DEBUGOUT("discard TX buffer failed.");
                    qngStatus->SetStatus(QNG_E_IO_ERROR);
                    return false;
                }

                // read out discard bytes, now streaming data, done
                if (ftdiStatus == FTDIDEVICE_START_DISCARD_BYTES_)
                {
                    isStreaming_ = true;
                    return true;
                }
		
				// stop timer
				gettimeofday(&stop, NULL);
				elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0;      // sec to ms
				elapsedTime += (stop.tv_usec - start.tv_usec) / 1000.0;   // us to ms

			} while (elapsedTime < FTDIDEVICE_TX_TIMEOUT_MILLIS_);

            DEBUGOUT(attemptsLeft);

        } while (--attemptsLeft > 0);

        qngStatus->SetStatus(QNG_E_IO_TIMEOUT);
        return false;
    }
}


bool FtdiDevice::StopStreaming() 
{
    if (isStreaming_ == false)
        return true;

    int txd;
    int bytesAvailable;
    std::vector<uint8_t> readBuffer;

    if (deviceType_ >= FTDIDEVICE_QNG_GEN2_)
    {
        // set rx/tx timeouts
    	ftdic->usb_read_timeout = FTDIDEVICE_TX_TIMEOUT_MILLIS_;
	    ftdic->usb_write_timeout = FTDIDEVICE_TX_TIMEOUT_MILLIS_;
    	DEBUGOUT(ftdic->usb_read_timeout);
	    DEBUGOUT(ftdic->usb_write_timeout);

        int attemptsLeft = FTDIDEVICE_TX_ATTEMPTS_;
        do
        {
            // start timeout stopwatch
			elapsedTime = 0;
			gettimeofday(&start, NULL);

            // send stop command
            uint8_t stopCommand = FTDIDEVICE_STOP_COMMAND_;
			ftdiStatus = ftdi_write_data(ftdic, &stopCommand, 1); 
            if (ftdiStatus < 0)
            {
	    		DEBUGOUT("write stopCommand failed");
                qngStatus->SetStatus(QNG_E_IO_ERROR);
                return false;
            }

            // read out old data here (clear buffer)
            do
            {            
                //Get Queue Status
				bytesAvailable = ftdic->readbuffer_remaining;
				DEBUGOUT(bytesAvailable);

                if (bytesAvailable > 0)
                {
                    readBuffer.resize(bytesAvailable);
					ftdiStatus = ftdi_read_data(ftdic, &readBuffer[0], bytesAvailable);	  
					if (ftdiStatus < 0) 
					{
						DEBUGOUT("read TX buffer failed.");
						qngStatus->SetStatus(QNG_E_IO_ERROR);
						return false;
					}
                }

				// stop timer
				gettimeofday(&stop, NULL);
				elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0;      // sec to ms
				elapsedTime += (stop.tv_usec - start.tv_usec) / 1000.0;   // us to ms
				DEBUGOUT(elapsedTime);

			} while (bytesAvailable!=0 && elapsedTime < FTDIDEVICE_TX_TIMEOUT_MILLIS_);

            // send delimiter command
            uint8_t delimCommand = FTDIDEVICE_DELIMIT_COMMAND_;
			ftdiStatus = ftdi_write_data(ftdic, &delimCommand, 1); 
            if (ftdiStatus < 0)
            {
	    		DEBUGOUT("write delimCommand Failed.");
                qngStatus->SetStatus(QNG_E_IO_ERROR);
                return false;
            }

            // read out data trailed by delimiter
            int haveBytes = 0;
            do
            {
                //Get Queue Status
				readBuffer.resize(haveBytes+1);
				ftdiStatus = ftdi_read_data(ftdic, &readBuffer[haveBytes], 1);
				if (ftdiStatus < 0)
			    {
					DEBUGOUT("read TX buffer failed.");
			        qngStatus->SetStatus(QNG_E_IO_ERROR);
			        return false;
			    }
				if(ftdiStatus > 0)
					haveBytes += 1;

				bytesAvailable = ftdic->readbuffer_remaining;
				DEBUGOUT(bytesAvailable);

                txd = 0;
                if (bytesAvailable > 0)
                {
                    readBuffer.resize(haveBytes+bytesAvailable);
                    txd = ftdi_read_data(ftdic, &readBuffer[haveBytes], bytesAvailable);	  
					if (txd < 0) 
					{
						DEBUGOUT("read TX buffer failed");
						qngStatus->SetStatus(QNG_E_IO_ERROR);
						return false;
					}
                }

                haveBytes += txd;

                // if the last bytes were the delimiter - stopped and cleared here, done
                if (haveBytes>=6 && memcmp(&readBuffer[haveBytes-6], delimiter_, 6) == 0)
                {
	                // reset rx/tx timeouts to default
					ftdic->usb_read_timeout = FTDIDEVICE_STREAM_TIMEOUT_MILLIS_;
			    	ftdic->usb_write_timeout = FTDIDEVICE_STREAM_TIMEOUT_MILLIS_;
					DEBUGOUT(ftdic->usb_read_timeout);
					DEBUGOUT(ftdic->usb_write_timeout);
	                isStreaming_ = false;
	                return true;
                }

				// stop timer
				gettimeofday(&stop, NULL);
				elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0;      // sec to ms
				elapsedTime += (stop.tv_usec - start.tv_usec) / 1000.0;   // us to ms
				DEBUGOUT(elapsedTime);

			} while (elapsedTime < FTDIDEVICE_TX_TIMEOUT_MILLIS_);

        } while (--attemptsLeft > 0);
	
    	DEBUGOUT(attemptsLeft);

        qngStatus->SetStatus(QNG_E_IO_TIMEOUT);
        return false;
    }
    else // first generation device (J1000KU)
    {
        // Purge usb RX buffer
		ftdiStatus = ftdi_usb_purge_rx_buffer(ftdic);
		if (ftdiStatus < 0 ) 
		{
  			DEBUGOUT("ftdi_usb_purge_rx failed");
  			qngStatus->SetStatus(QNG_E_IO_ERROR);
  			return false;
	    }

        do
        {
            // Get Queue Status
            bytesAvailable = ftdic->readbuffer_remaining;
			DEBUGOUT(bytesAvailable);

            if (bytesAvailable > 0)
            {
                readBuffer.resize(bytesAvailable);
                ftdiStatus = ftdi_read_data(ftdic, &readBuffer[0], bytesAvailable);	  
				if (ftdiStatus < 0) 
				{
					DEBUGOUT("read TX buffer failed");
					qngStatus->SetStatus(QNG_E_IO_ERROR);
					return false;
				}
            }
        } while (bytesAvailable > 0);

        isStreaming_ = false;
        return true;
    }
}


bool FtdiDevice::DetermineDelimiter()
{
        DEBUGTOUCH();

		std::vector<uint8_t> readBuffer;
        uint8_t stopCommand = FTDIDEVICE_STOP_COMMAND_;
        int txd;
        int bytesAvailable = 0;

        // start StopWatch 
		elapsedTime = 0;
		gettimeofday(&start, NULL);

        do
        {
            // Send stop command
	    	ftdiStatus = ftdi_write_data(ftdic, &stopCommand, 1); 
            if (ftdiStatus < 0)
            {
                DEBUGOUT("write stopCommand failed");
                qngStatus->SetStatus(QNG_E_IO_ERROR);
                return false;
            }

            isStreaming_ = false;

            // Get Queue Status
	    	bytesAvailable = ftdic->readbuffer_remaining;
	   		DEBUGOUT(bytesAvailable);

            if (bytesAvailable > 0)
            {
                std::vector<uint8_t> discardBytes(bytesAvailable);
				ftdiStatus = ftdi_read_data(ftdic, &discardBytes[0], bytesAvailable);	  
				if (ftdiStatus < 0) 
				{
		    		DEBUGOUT("read TX buffer failed");
		    		qngStatus->SetStatus(QNG_E_IO_ERROR);
		    		return false;
				}
            }

            FtdiCommandResponse(FTDIDEVICE_DELIMIT_COMMAND_, delimiter_);

            uint8_t confirmDelimiter[6];
            memset(confirmDelimiter, 0xcd, 6);

            FtdiCommandResponse(FTDIDEVICE_DELIMIT_COMMAND_, confirmDelimiter);

            if (memcmp(delimiter_, confirmDelimiter, 6) == 0)
                return true;

			// stop timer
			gettimeofday(&stop, NULL);
			elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0;      // sec to ms
			elapsedTime += (stop.tv_usec - start.tv_usec) / 1000.0;   // us to ms

			DEBUGOUT(elapsedTime);

		} while (elapsedTime < FTDIDEVICE_TX_ATTEMPTS_*FTDIDEVICE_TX_TIMEOUT_MILLIS_);

        return false;
}


void FtdiDevice::CheckTestStatsStatus() 
{
    DEBUGTOUCH();

    // look for the internal delimiter
    uint8_t responseDelimiter[6];
    FtdiCommandResponse(FTDIDEVICE_DELIMIT_COMMAND_, responseDelimiter);
    if (memcmp(delimiter_, responseDelimiter, 6)!=0)
    {
        if (qngStatus->GetStatus() == QNG_S_OK)
            qngStatus->SetStatus(QNG_E_IO_ERROR);
        return;
    }

    // get test stats status
    uint32_t rngStatus = 0;
    FtdiCommandResponse(FTDIDEVICE_TEST_STATUS_COMMAND_, (uint8_t*)&rngStatus);
    if (rngStatus & FTDIDEVICE_TEST_BAD_STATS_MASK_)
		qngStatus->SetStatus(QNG_E_STATS_EXCEPTION);
}


double FtdiDevice::Uint48ToUniform(uint64_t uint48) 
{
	// copy 6 bytes into mantissa
	double uniform = (double)uint48;
	uniform /= 281474976710656.0;  // 2^(6*8)

	return uniform;
}


float FtdiDevice::CalcEntropy(double p)
{
    return (float)(-(p*log(p) + (1.0-p)*log(1.0-p)) / log(2.0));
}
    

/*************************************************** QngStatus Class *******************************************/

/* Constructor */
QngStatus::QngStatus()
    : status_(QNG_E_DEVICE_NOT_OPENED)
    , statusString_("QNG device not found or already in use.")
{ }


/* Destructor */
QngStatus::~QngStatus() { }



long QngStatus::GetStatus() 
{
    return status_;
}


std::string& QngStatus::GetStatusString() 
{
    return statusString_;
}


long QngStatus::SetStatus(long newStatus) 
{
    if (status_ != QNG_S_OK) 
    {
		if ((newStatus != QNG_S_OK) && (newStatus != QNG_E_STATS_EXCEPTION) && (newStatus != QNG_E_DEVICE_NOT_OPENED))
	    	return status_;
    }
    status_ = newStatus;
    
    switch (status_) 
    {
        case QNG_S_OK:
            statusString_ = "QNG device reports success.";
            break;
        case QNG_E_GENERAL_FAILURE:
            statusString_ = "QNG general error.";
            break;
        case QNG_E_IO_ERROR:
            statusString_ = "QNG I/O error.";
            break;
        case QNG_E_IO_TIMEOUT:
            statusString_ = "QNG I/O request has timed out.";
            break;
        case QNG_E_IO_ARRAY_OVERSIZED:
            statusString_ = "QNG read array size exceeds max size.";
            break;
        case QNG_E_DEVICE_NOT_OPENED:
            statusString_ = "QNG device not found or already in use.";
            break;
        case QNG_E_STATS_EXCEPTION:
            statusString_ = "QNG test statistics exception.";
            break;
        case QNG_E_STATS_UNSUPPORTED:
            statusString_ = "QNG stats not supported with this device.";
            break;
        case QNG_E_DIAGX_UNSUPPORTED:
            statusString_ = "QNG diagnostics not supported with this device.";
            break;
        default:
            status_ = QNG_E_GENERAL_FAILURE;
            statusString_ = "QNG general error.";
    }

    DEBUGOUT(status_);
    return status_;
}


/*************************************************** Mutex Classes *******************************************/

Mutex::Mutex()
{
    // initialize
    pthread_mutex_init(&mutex_, NULL);
}


Mutex::~Mutex()
{
    // destroy
    pthread_mutex_destroy(&mutex_);
}


void Mutex::Lock()
{	
    // lock mutex
    pthread_mutex_lock(&mutex_);
}


void Mutex::Unlock()
{
    // unlock mutex
    pthread_mutex_unlock(&mutex_);
}


MutexSync::MutexSync(Mutex& mutex) : mutex_(mutex)
{
    mutex_.Lock();
}


MutexSync::~MutexSync() 
{
    mutex_.Unlock();
}  
