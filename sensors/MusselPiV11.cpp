//Code for connecting the Raspberry PI to the iBionics Mux System for Mussels
//Tested with Raspberry PI OS 3.6.
//Written by James Reynolds (05/2022) Version 1.1

#include "math.h"
#include <wiringPiI2C.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>

using namespace std;

int I2C_write (uint8_t slave_address, uint8_t reg_address, uint8_t configData, int byteCount);
int I2C_read(uint8_t slave_address, uint8_t reg_address);
void PCA_Reg_Write (uint8_t reg_address, uint8_t data);
int16_t LSM_Acc_Reg_Read(uint8_t reg_address);
void LSM_Acc_Read(uint8_t channel, int16_t *acc_all);
void LSM_Acc_Reg_Write (uint8_t reg_address, uint8_t data);
void LSM_Init(uint8_t channel);
float calculateTemp(uint8_t ch_bot);
void calculateAverageAcc(uint8_t ch_bot, uint8_t ch_top, int16_t *ACC_bot, int16_t *ACC_top);
bool I2Ccheck(uint8_t ch_bot, uint8_t ch_top, int16_t *ACC_bot, int16_t *ACC_top);

uint32_t devID = 101;
//I2C Addresses
int PCAwiring = -1;
int LSMwiring = -1;
uint8_t LSM_ACC_ADDR = 25;
uint8_t PCA_ADDR = 0x70; // 1110000b

//Register Addresses
uint8_t ACC_addr[3] = {0x28, 0x2A, 0x2C};
uint8_t TEMP_addr[2] = {0x0C, 0x0D};

//Moving Average Parameters
const int filt_len = 25;
float mov_avg = 0.0;
double sum_prod;
double sq_bot;
double sq_top;
uint8_t i;
uint8_t q;
int sampleNum = 0;

//Pair1
int16_t ACC_Pb1[3], ACC_Pt1[3];
uint8_t ch_Pb1 = 6;
uint8_t ch_Pt1 = 7;
bool pair1Failed = false;
//Pair2
int16_t ACC_Pb2[3], ACC_Pt2[3];
uint8_t ch_Pb2 = 4;
uint8_t ch_Pt2 = 5;
bool pair2Failed = false;
//Pair3
int16_t ACC_Pb3[3], ACC_Pt3[3];
uint8_t ch_Pb3 = 2;
uint8_t ch_Pt3 = 3;
bool pair3Failed = false;
//Pair4
int16_t ACC_Pb4[3], ACC_Pt4[3];
uint8_t ch_Pb4 = 0;
uint8_t ch_Pt4 = 1;
bool pair4Failed = false;

int main(void)
{
    PCAwiring = wiringPiI2CSetup(PCA_ADDR); //Open the Linux port
	  for (i = 0; i < 8; i++)
	  {
	    LSM_Init(i); //Initialize all of the channels (0-7)
	  }
	  //Check each pair to see which ones are connected
	  pair1Failed = I2Ccheck(ch_Pb1, ch_Pt1, ACC_Pb1, ACC_Pt1);
	  pair2Failed = I2Ccheck(ch_Pb2, ch_Pt2, ACC_Pb2, ACC_Pt2);
	  pair3Failed = I2Ccheck(ch_Pb3, ch_Pt3, ACC_Pb3, ACC_Pt3);
	  pair4Failed = I2Ccheck(ch_Pb4, ch_Pt4, ACC_Pb4, ACC_Pt4);

	  while(1)
	  {
		  		string dataString = ""; // Create a string to write to Serial and the SD card
		        string statusString = ""; // Create a string to keep track of what was recorded
		        statusString += to_string(devID) + ",";
					if (!pair1Failed) //Measure angle and magnetic intensity then add to string
					  {
					    calculateAverageAcc(ch_Pb1, ch_Pt1, ACC_Pb1, ACC_Pt1);

					    for (int i = 0; i < 3; i++) // Add the XYZ data to the string
					    {
					      dataString += to_string(ACC_Pb1[i]);
					      dataString += ",";
					    }
					    for (int i = 0; i < 3; i++)
					    {
					      dataString += to_string(ACC_Pt1[i]);
					      dataString += ",";
					    }
					    statusString += "1A";

					  }

					  if (!pair2Failed)
					  {
					    calculateAverageAcc(ch_Pb2, ch_Pt2, ACC_Pb2, ACC_Pt2);

					    for (int i = 0; i < 3; i++) // Add the XYZ data to the string
					    {
					      dataString += to_string(ACC_Pb2[i]);
					      dataString += ",";
					    }
					    for (int i = 0; i < 3; i++)
					    {
					      dataString += to_string(ACC_Pt2[i]);
					      dataString += ",";
					    }
					    statusString += "2A";

					  }

					  if (!pair3Failed)
					  {
					    calculateAverageAcc(ch_Pb3, ch_Pt3, ACC_Pb3, ACC_Pt3);

					    for (int i = 0; i < 3; i++) // Add the XYZ data to the string
					    {
					      dataString += to_string(ACC_Pb3[i]);
					      dataString += ",";
					    }
					    for (int i = 0; i < 3; i++)
					    {
					      dataString += to_string(ACC_Pt3[i]);
					      dataString += ",";
					    }
					    statusString += "3A";

					  }

					  if (!pair4Failed)
					  {
					    calculateAverageAcc(ch_Pb4, ch_Pt4, ACC_Pb4, ACC_Pt4);

					    for (int i = 0; i < 3; i++) // Add the XYZ data to the string
					    {
					      dataString += to_string(ACC_Pb4[i]);
					      dataString += ",";
					    }
					    for (int i = 0; i < 3; i++)
					    {
					      dataString += to_string(ACC_Pt4[i]);
					      dataString += ",";
					    }
					    statusString += "4A";

					  }

					  dataString = statusString + "," + dataString; //Combine the status info with the data
					  cout << dataString << endl; // Print the data out (or replace with method to save or graph the data)
					  sleep(10);
	  }
}

int I2C_write (int slave_address, uint8_t reg_address, uint8_t configData, int byteCount)
{
  return wiringPiI2CWriteReg8(slave_address, (int) reg_address, (int) configData);
}

int I2C_read(int slave_address, uint8_t reg_address)
{
  return wiringPiI2CReadReg8(slave_address, (int) reg_address);
}

void PCA_Reg_Write (uint8_t reg_address, uint8_t data)
{
  I2C_write(PCAwiring, reg_address, data, 1);
}

int16_t LSM_Acc_Reg_Read(uint8_t reg_address)
{
  uint8_t configData[2];
  configData[0] = I2C_read (LSMwiring, reg_address);
  configData[1] = I2C_read (LSMwiring, reg_address + 1);
  int16_t accData;
  accData = configData[1];
  accData = (accData << 8) | configData[0];
  return accData;
}

void LSM_Acc_Read(uint8_t channel, int16_t *acc_all)
{
  PCA_Reg_Write(0x09, 0x08 | channel); // Select the correct channel
  for (i = 0; i < 3; i++) // Sequential read
  {
    acc_all[i] = LSM_Acc_Reg_Read(ACC_addr[i]);
  }
}

void LSM_Acc_Reg_Write (uint8_t reg_address, uint8_t data)
{
  I2C_write(LSMwiring, reg_address, data, 1);
}

void LSM_Init(uint8_t channel)
{
  //printf("Start Init... \n");
  PCA_Reg_Write(0x09, 0x08 | channel);
  LSMwiring = wiringPiI2CSetup (LSM_ACC_ADDR);
  //cout << LSMwiring << endl;
  if (LSMwiring > 0)
  {
    LSM_Acc_Reg_Write(0x21, 0x00);  //HPF settings
    LSM_Acc_Reg_Write(0x22, 0x00);  //all interrupt disabled
    LSM_Acc_Reg_Write(0x23, 0x88);  //BDU, FS = 2g, high resl
    LSM_Acc_Reg_Write(0x24, 0x00);  //memory settings
    LSM_Acc_Reg_Write(0x25, 0x00);  //all interrupt disabled
    LSM_Acc_Reg_Write(0x20, 0x57);  //1 Hz, XYZ enabled 0x17 for 1Hz, 0x57 for 100Hz, 0x77 for 400Hz
    LSM_Acc_Reg_Write(0x1F, 0x03);  //Temperature measurement enabled
  //printf("Finished Init... \n");
  }
  else
  {
    //printf("Init Failed. \n");
  }
}

float calculateTemp(uint8_t ch_bot)
{
  //https://github.com/intel-iot-devkit/upm/blob/master/src/lsm303agr/lsm303agr.c
  int16_t tempData;
  PCA_Reg_Write(0x09, 0x08 | ch_bot);
  tempData = LSM_Acc_Reg_Read(0x0C);
  float tempCalc = (tempData / 256.0) + 25.0;
  return tempCalc;
}

void calculateAverageAcc(uint8_t ch_bot, uint8_t ch_top, int16_t *ACC_bot, int16_t *ACC_top)
{
  int16_t ACC_sumb[3], ACC_sumt[3];
  LSM_Init(ch_bot);
  LSM_Init(ch_top);
  HAL_Delay(100);
  for (i = 0; i < 3; i++) { //Initialize the variables
    ACC_sumb[i] = 0;
    ACC_sumt[i] = 0;
  }
  for (q = 0; q < 16; q++) {
    LSM_Acc_Read(ch_bot, ACC_bot); //Get the XYZ values for both sensors
    LSM_Acc_Read(ch_top, ACC_top);
    for (i = 0; i < 3; i++) {
      ACC_sumb[i] += (ACC_bot[i]/16); // Shift the data by 4 bits to account for left justification
      ACC_sumt[i] += (ACC_top[i]/16);
    }
    usleep(1100); // Slightly longer than 10ms for 100Hz to make sure we have a new sample.
  }
  for (i = 0; i < 3; i++) {
      ACC_bot[i] = (int)(ACC_sumb[i]/16); // Save the average of the data
      ACC_top[i] = (int)(ACC_sumt[i]/16);
  }
  PCA_Reg_Write(0x09, 0x08 | ch_bot);
  LSM_Acc_Reg_Write(0x20, 0x00);  //Shut down sensor
  PCA_Reg_Write(0x09, 0x08 | ch_top);
  LSM_Acc_Reg_Write(0x20, 0x00);  //Shut down sensor

}

bool I2Ccheck(uint8_t ch_bot, uint8_t ch_top, int16_t *ACC_bot, int16_t *ACC_top)
{
  //Check the Who Am I register to see if the pair is there.
 PCA_Reg_Write(0x09, 0x08 | ch_bot); // Select the correct channel
 int16_t botCheck = LSM_Acc_Reg_Read(0x0F) & 0xff; // Check to see if the response is correct
 //cout << hex << botCheck << endl;
 //botCheck = botCheck & 0xff;
 bool botPresent = (botCheck == 0x33) ? true : false;
 PCA_Reg_Write(0x09, 0x08 | ch_top); // Select the correct channel
 int16_t topCheck = LSM_Acc_Reg_Read(0x0F) & 0xff;
 bool topPresent = (topCheck == 0x33) ? true : false;
 //cout << hex << botCheck << "," << topCheck << endl;

  if (botPresent && topPresent)
  {
    return false;
  }
  else
  {
    return true;
  }
}
