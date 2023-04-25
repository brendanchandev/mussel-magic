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
void calculateAverageAcc(uint8_t ch,int16_t *acc);
bool I2Ccheck(uint8_t ch, int16_t *acc);

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

//Sensor1
int16_t ACC1[3];
uint8_t ch1 =7;
//Sensor2
int16_t ACC2[3];
uint8_t ch2 =6;
//Sensor3
int16_t ACC3[3];
uint8_t ch3 =5;
//Sensor4
int16_t ACC4[3];
uint8_t ch4 =4;
//Sensor5
int16_t ACC5[3];
uint8_t ch5 =3;
//Sensor6
int16_t ACC6[3];
uint8_t ch6 =2;
//Sensor7
int16_t ACC7[3];
uint8_t ch7 =1;
//Sensor8
int16_t ACC8[3];
uint8_t ch8 =0;

int main(void)
{
    PCAwiring = wiringPiI2CSetup(PCA_ADDR); //Open the Linux port
	  for (i = 0; i < 8; i++)
	  {
	    LSM_Init(i); //Initialize all of the channels (0-7)
	  }
	  //Check each pair to see which ones are connected
	  acc1Present = I2Ccheck(ch1, ACC1);
	  acc2Present = I2Ccheck(ch2, ACC2);
	  acc3Present = I2Ccheck(ch3, ACC3);
	  acc4Present = I2Ccheck(ch4, ACC4);
	  acc5Present = I2Ccheck(ch5, ACC5);
	  acc6Present = I2Ccheck(ch6, ACC6);
	  acc7Present = I2Ccheck(ch7, ACC7);
	  acc8Present = I2Ccheck(ch8, ACC8);

	  while(1)
	  {
		  		bool tempSampled = false;
		  		string dataString = ""; // Create a string to write to Serial and the SD card
		        string statusString = ""; // Create a string to keep track of what was recorded
		        statusString += to_string(devID) + ",";
					if (acc1Present) //Measure angle and magnetic intensity then add to string
					  {
						if(!tempSampled) {
							tempData = calculateTemp(ch1);
							dataString += to_string(tempData);
							tempSampled = true;
					  	}
					    calculateAverageAcc(ch1, ACC1);
					    for (int i = 0; i < 3; i++) // Add the XYZ data to the string
					    {
					      dataString += to_string(ACC1[i]);
					      dataString += ",";
					    }
					    statusString += "1A";

					  }

					  if (acc2Present)
					   {
						   if(!tempSampled) {
							tempData = calculateTemp(ch2);
							dataString += to_string(tempData);
							tempSampled = true;
					  	}
					    calculateAverageAcc(ch2, ACC2);

					    for (int i = 0; i < 3; i++) // Add the XYZ data to the string
					    {
					      dataString += to_string(ACC2[i]);
					      dataString += ",";
					    }
					    statusString += "2A";

					  }

					  if (acc3Present)
					   {
						   if(!tempSampled) {
							tempData = calculateTemp(ch3);
							dataString += to_string(tempData);
							tempSampled = true;
					  	}
					    calculateAverageAcc(ch3, ACC3);

					    for (int i = 0; i < 3; i++) // Add the XYZ data to the string
					    {
					      dataString += to_string(ACC3[i]);
					      dataString += ",";
					    }
					    statusString += "3A";

					  }

					  if (acc4Present)
					   {
						   if(!tempSampled) {
							tempData = calculateTemp(ch4);
							dataString += to_string(tempData);
							tempSampled = true;
					  	}
					    calculateAverageAcc(ch4, ACC4);

					    for (int i = 0; i < 3; i++) // Add the XYZ data to the string
					    {
					      dataString += to_string(ACC4[i]);
					      dataString += ",";
					    }
					    statusString += "4A";

					  }
					  if (acc5Present)
					   {
						   if(!tempSampled) {
							tempData = calculateTemp(ch5);
							dataString += to_string(tempData);
							tempSampled = true;
					  	}
					    calculateAverageAcc(ch5, ACC5);

					    for (int i = 0; i < 3; i++) // Add the XYZ data to the string
					    {
					      dataString += to_string(ACC5[i]);
					      dataString += ",";
					    }
					    statusString += "5A";

					  }
					  if (acc6Present)
					   {
						   if(!tempSampled) {
							tempData = calculateTemp(ch6);
							dataString += to_string(tempData);
							tempSampled = true;
					  	}
					    calculateAverageAcc(ch6, ACC6);

					    for (int i = 0; i < 3; i++) // Add the XYZ data to the string
					    {
					      dataString += to_string(ACC6[i]);
					      dataString += ",";
					    }
					    statusString += "6A";

					  }
					  if (acc7Present)
					   {
						if(!tempSampled) {
							tempData = calculateTemp(ch7);
							dataString += to_string(tempData);
							tempSampled = true;
					  	}
					    calculateAverageAcc(ch7, ACC7);

					    for (int i = 0; i < 3; i++) // Add the XYZ data to the string
					    {
					      dataString += to_string(ACC7[i]);
					      dataString += ",";
					    }
					    statusString += "7A";

					  }
					  if (acc8Present)
					   {
						   if(!tempSampled) {
							tempData = calculateTemp(ch8);
							dataString += to_string(tempData);
							tempSampled = true;
					  	}
					    calculateAverageAcc(ch8, ACC8);

					    for (int i = 0; i < 3; i++) // Add the XYZ data to the string
					    {
					      dataString += to_string(ACC8[i]);
					      dataString += ",";
					    }
					    statusString += "8A";

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

void calculateAverageAcc(uint8_t ch, int16_t *acc)
{
  int16_t ACC_sum[3]
  LSM_Init(ch);
  HAL_Delay(100);
  for (i = 0; i < 3; i++) { //Initialize the variables
    ACC_sum[i] = 0;
  }
  for (q = 0; q < 16; q++) {
    LSM_Acc_Read(ch, acc); //Get the XYZ values for both sensors
    for (i = 0; i < 3; i++) {
      ACC_sum[i] += (acc[i]/16); // Shift the data by 4 bits to account for left justification
    }
    usleep(1100); // Slightly longer than 10ms for 100Hz to make sure we have a new sample.
  }
  for (i = 0; i < 3; i++) {
      acc[i] = (int)(ACC_sum[i]/16); // Save the average of the data
  }
  PCA_Reg_Write(0x09, 0x08 | ch);
  LSM_Acc_Reg_Write(0x20, 0x00);  //Shut down sensor

}

bool I2Ccheck(uint8_t ch, int16_t *acc)
{
  //Check the Who Am I register to see if the pair is there.
 PCA_Reg_Write(0x09, 0x08 | ch); // Select the correct channel
 int16_t check = LSM_Acc_Reg_Read(0x0F) & 0xff; // Check to see if the response is correct
 //cout << hex << botCheck << endl;
 //botCheck = botCheck & 0xff;
 bool present = (check == 0x33) ? true : false;
 return present;
}
