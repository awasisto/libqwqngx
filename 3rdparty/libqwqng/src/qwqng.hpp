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

#ifndef __qwqng_hpp__
#define __qwqng_hpp__

//
// Custom definitions
//

#define FTDIDEVICE_ID_PREFIX_               "QWR"
#define FTDIDEVICE_ID_DEVELOPMENT_          "D"
#define FTDIDEVICE_STREAM_TIMEOUT_MILLIS_   5000
#define FTDIDEVICE_TX_TIMEOUT_MILLIS_       100
#define FTDIDEVICE_TX_ATTEMPTS_             40
#define FTDIDEVICE_START_DISCARD_BYTES_     256

#define FTDIDEVICE_START_COMMAND_           0x96U
#define FTDIDEVICE_STOP_COMMAND_            0xe0U
#define FTDIDEVICE_DELIMIT_COMMAND_         0x60U

#define FTDIDEVICE_TEST_STATUS_COMMAND_     0xb0U
#define FTDIDEVICE_TEST_FINAL_BASE_         0xa0U
#define FTDIDEVICE_TEST_CHANNELS_BASE_      0x70U
#define FTDIDEVICE_TEST_BAD_STATS_MASK_     0x0004
#define FTDIDEVICE_TEST_CLEAR_STATS_FLAG_   0xc0U

#define FTDIDEVICE_QNG_GEN1_                1
#define FTDIDEVICE_QNG_GEN2_                2
#define FTDIDEVICE_QNG_GEN3_                3
#define FTDIDEVICE_QNG_GEN4_                4
#define FTDIDEVICE_QNG_GEN5_                5

#define FTDIDEVICE_MAX_ARRAY_SIZE_          8192
#define FTDIDEVICE_READ_FRAME_SIZE_         512
#define FTDIDEVICE_WRITE_FRAME_SIZE_	    512

#define FTDIDEVICE_FAST_MAX_ARRAY_SIZE_     65536
#define FTDIDEVICE_FAST_READ_FRAME_SIZE_    65536
#define FTDIDEVICE_FAST_WRITE_FRAME_SIZE_   65536

#define FTDIDEVICE_HALF_OF_UNIFORM_LSB_     1.7763568394002505e-15
#define FTDIDEVICE_2_PI_                    6.283185307179586

#define FTDIDEVICE_RNG_OUTPUT_ENABLED_MASK_ 0X0001
#define FTDIDEVICE_USB_STREAMING_MASK_ 	    0X0002
#define FTDIDEVICE_TEST_BAD_STATS_MASK_     0x0004
#define FTDIDEVICE_CLIENT_CONNECTED_MASK_   0X0008

//
// Custom QNG HRESULT messages
//

//
// MessageId:   QNG_S_OK
// MessageText: QNG device reports success.
//
#define QNG_S_OK						0x00044400L

//
// MessageId:   QNG_E_GENERAL_FAILURE
// MessageText: QNG general error.
//
#define QNG_E_GENERAL_FAILURE			0x80044401L

//
// MessageId:   QNG_E_IO_ERROR
// MessageText: QNG I/O error.
//
#define QNG_E_IO_ERROR					0x80044402L

//
// MessageId:   QNG_E_IO_TIMEOUT
// MessageText: QNG I/O request has timed out.
//
#define QNG_E_IO_TIMEOUT				0x80044403L

//
// MessageId:   QNG_E_IO_ARRAY_OVERSIZED
// MessageText: QNG read array size exceeds max size.
//
#define QNG_E_IO_ARRAY_OVERSIZED		0x80044404L

//
// MessageId:   QNG_E_STATS_EXCEPTION
// MessageText: QNG test statistics exception.
//
#define QNG_E_STATS_EXCEPTION   		0x80044406L

//
// MessageId:   QNG_E_STATS_UNSUPPORTED
// MessageText: QNG stats not supported with this device.
//
#define QNG_E_STATS_UNSUPPORTED   		0x80044407L

//
// MessageId:   QNG_E_DIAGX_UNSUPPORTED
// MessageText: QNG diagnostics not supported with this device.
//
#define QNG_E_DIAGX_UNSUPPORTED   		0x80044408L

//
// MessageId:   QNG_E_DEVICE_NOT_OPENED
// MessageText: QNG device not found or already in use.
//
#define QNG_E_DEVICE_NOT_OPENED			0x8004440AL

//
// MessageId:   S_OK
// MessageText: No error occurred.
//
#define S_OK							0x00000000L

//
// Includes
//

#include "stdint.h"
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h> 
#include <vector>
#include <math.h>
#include <string>
#include <string.h>
#include <iostream>
#include <ftdi.h>

//
// DEBUG MACROS
//

#if __STDC_VERSION__ < 199901L
     # if __GNUC__ >= 2
     	    #  define __func__ __FUNCTION__
     	# else
     	    #  define __func__ "<unknown>"
     # endif
#endif

// uncomment line below to use debug macro
//#define DEBUGOUTDO

#ifdef DEBUGOUTDO
    #define DEBUGTOUCH() std::cout << "@ " << __FUNCTION__ << " " << __LINE__ << "\n"
    #define DEBUGOUT(val) std::cout << "> " << __FUNCTION__ << " " << __LINE__ << std::hex << " " #val ": " << val << "\n" << std::dec
    //#define DEBUGTEST(left, op, right) if ((left op right) == false) std::cout << "? " << __FUNCTION__ << " " << __LINE__ << std::hex << " " #left ": " << left << " " #op " " #right ": " << right << " FALSE\n" << std::dec
#else
    #define DEBUGTOUCH()
    #define DEBUGOUT(val)
    //#define DEBUGTEST(left, op, right)

#endif

//
// CLASSES
//

/* Forward Declaration */
class FtdiDevice;
class IDevice;
class QngStatus;
class Mutex;
class MutexSync; 

class QWQNG 
{  
public:
  /* Public Constructor */
  QWQNG();
  
  /* Construct Indirectly (backwards compatibility)*/
  static QWQNG* Instance();  
  
  /* Public Destructor */
  ~QWQNG();
  
   
protected:
  /* Pointer to Class IDevice */
  IDevice* pDevice;	
     
public:
  /* Properties */
  char* statusString_;	
  char* devId_;		
  
  /* Device Manipulators */
  char* StatusString(); 
  char* DeviceID();   
  int RandInt32(long* pVal);
  int RandUniform(double* pVal);
  int RandNormal(double *pVal);
  int RandBytes(char* pval, long length);
  int Clear(); 
  int Reset();
  int RuntimeInfo(float* pVal);
  int Diagnostics(char dxCode, char* dxInfo); 
};


class IDevice 
{
public:
    int deviceType_;
protected:
    std::string deviceId_;
    int commandResponseLength_[256];
    //int deviceType_;
    QngStatus* qngStatus;

public:
    IDevice(QngStatus* qngStatus) : deviceType_(0), qngStatus(qngStatus)
    {
        // Return Length For Command Bytes
        //  00       :     0 bytes - NEVER USE
        //  01 - 0F  :  1024 bytes
        //  10 - 3F  :   128 bytes
        //  40 - 5F  :     1 byte
        //  60       :     6 bytes - DELIMITER
        //  61 - 95  :     4 bytes
        //  96       :     0 bytes - START
        //  97 - BF  :     4 bytes
        //  C0 - FE  :     0 bytes
        //  FF       :     0 bytes - NEVER USE

        commandResponseLength_[0x00] = 0;
        for (int i=0x01; i<=0x0F; i++)
            commandResponseLength_[i] = 1024;
        for (int i=0x10; i<=0x3F; i++)
            commandResponseLength_[i] = 128;
        for (int i=0x40; i<=0x5F; i++)
            commandResponseLength_[i] = 1;
        commandResponseLength_[0x60] = 6;
        for (int i=0x61; i<=0xBF; i++)
            commandResponseLength_[i] = 4;
        commandResponseLength_[0x96] = 0;
        for (int i=0xC0; i<=0xFF; i++)
            commandResponseLength_[i] = 0;
    }

    /* Functions */
    virtual void Open() = 0;
    virtual void Close() = 0;
    virtual void Clear() = 0;
    virtual void Reset() = 0;

    virtual void GetBytes(char* bytes, int bytesRequested) = 0;
    virtual void GetInt32(int32_t* int32) = 0;
    virtual void GetUniform(double* uniform) = 0;
    virtual void GetNormal(double* normal) = 0;

    virtual void GetRuntimeInfo(float* testData) = 0;
    virtual void Diagnostics(uint8_t dxCode, uint8_t* dxData) = 0;

    virtual std::string& GetDeviceId() = 0;
    virtual void SetDeviceId(std::string deviceId) = 0;
    virtual int GetDeviceType() = 0;
};


class FtdiDevice : public IDevice 
{
private:
    /* Main context structure for all libftdi functions */
    struct ftdi_context* ftdic; 			
    
    /* Structure for Finding Multiple FTDI Devices */
    struct QNGdev 
	{
      struct ftdi_device_list *devlist, *curdev;
      int VID;
      int PID;
    };  
    struct QNGdev Buffer[2]; // Init QNGdev Structure 

    /* Structure for Connected FTDI Devices' Parameters ( Initialize in FindOpen() ) */
    struct QNGparam 
	{
      char manufacturer[128], description[128], serialnumber[9];
    };
    
    /* Properties */
    char serialInfo[9];
    char descInfo[128];
    char manuInfo[128];
    int devPID;
    int devVID;   
    int txchunksize;
    int rxchunksize; 
    int ftdiStatus;	
	     
    uint8_t delimiter_[6];
    bool haveNormU1_;
    double normU1_;
    double normalConjugate_;

    bool isStreaming_;
    uint32_t prevDxCode_;

    timeval start, stop;
    double elapsedTime;

public:
    /* Constructor, Destructor */
    FtdiDevice(QngStatus* qngStatus);
    ~FtdiDevice();

    /* Functions */
    virtual void Open();
    virtual void Close();
    virtual void Clear();
    virtual void Reset();

    virtual void GetBytes(char* bytes, int bytesRequested);
    virtual void GetInt32(int32_t* int32);
    virtual void GetUniform(double* uniform);
    virtual void GetNormal(double* normal);

    virtual void GetRuntimeInfo(float* runtimeInfo);
    virtual void Diagnostics(uint8_t dxCode, uint8_t* dxData);

    virtual std::string& GetDeviceId();
    virtual void SetDeviceId(std::string deviceId);
    virtual int GetDeviceType();
    
private:   
    /* Functions */
    void FtdiClose();
    void FindOpen();
    void Initialize();
    void FtdiRead(void* receiveBuffer, int bytesRequested, bool statsUnchecked);
    void FtdiWrite(void* transmitBuffer, int bytesToTransmit);
    bool FtdiCommandResponse(uint8_t command, uint8_t* response);
    bool StartStreaming();
    bool StopStreaming();
    bool DetermineDelimiter();
    void CheckTestStatsStatus();
    double Uint48ToUniform(uint64_t uint48);
    float CalcEntropy(double p);  
};


class QngStatus 
{
public:
    /* Constructor, Destructor */
    QngStatus();
    ~QngStatus();
    
    /* Functions */
    long GetStatus();
    std::string& GetStatusString();
    long SetStatus(long newStatus);

private:   
    /* properties */
    long status_;
    std::string statusString_;  
};


class Mutex
{
public:
	/* Constructor, Destructor */
    Mutex();
    ~Mutex();

	/* Functions */
    void Lock();
    void Unlock();

private:
	/* properties */
    pthread_mutex_t mutex_;	
};


class MutexSync 
{
public:
    /* Constructor, Destructor */
    MutexSync(Mutex& mutex);
    ~MutexSync();

private:
	/* properties */
   	Mutex& mutex_;
};

#endif


