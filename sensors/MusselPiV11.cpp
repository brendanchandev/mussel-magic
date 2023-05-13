//Code for connecting the Raspberry PI to the iBionics Mux System for Mussels
//Tested with Raspberry PI OS 3.6.
//Written by James Reynolds (05/2022) Version 1.1

#include <wiringPiI2C.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <SerialStream.h>
#include <fstream>
#include <string>

using namespace std;
using namespace LibSerial;

int I2C_write(uint8_t slave_address, uint8_t reg_address, uint8_t configData, int byteCount);
int I2C_read(uint8_t slave_address, uint8_t reg_address);
void PCA_Reg_Write(uint8_t reg_address, uint8_t data);
int16_t LSM_Acc_Reg_Read(uint8_t reg_address);
void LSM_Acc_Read(uint8_t channel, int16_t *acc_all);
void LSM_Acc_Reg_Write(uint8_t reg_address, uint8_t data);
void LSM_Init(uint8_t channel);
float calculateTemp(uint8_t ch_bot);
void calculateAverageAcc(uint8_t ch, int16_t *acc);
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
uint8_t i;
uint8_t q;
int sampleNum = 0;

//Sensor1
int16_t ACC1[3];
uint8_t ch1 = 7;
//Sensor2
int16_t ACC2[3];
uint8_t ch2 = 6;
//Sensor3
int16_t ACC3[3];
uint8_t ch3 = 5;
//Sensor4
int16_t ACC4[3];
uint8_t ch4 = 4;
//Sensor5
int16_t ACC5[3];
uint8_t ch5 = 3;
//Sensor6
int16_t ACC6[3];
uint8_t ch6 = 2;
//Sensor7
int16_t ACC7[3];
uint8_t ch7 = 1;
//Sensor8
int16_t ACC8[3];
uint8_t ch8 = 0;

int main(void)
{
	PCAwiring = wiringPiI2CSetup(PCA_ADDR); //Open the Linux port
	for (i = 0; i < 8; i++)
	{
		LSM_Init(i); //Initialize all of the channels (0-7)
	}
	//Check each pair to see which ones are connected
	bool acc1Present = I2Ccheck(ch1, ACC1);
	bool acc2Present = I2Ccheck(ch2, ACC2);
	bool acc3Present = I2Ccheck(ch3, ACC3);
	bool acc4Present = I2Ccheck(ch4, ACC4);
	bool acc5Present = I2Ccheck(ch5, ACC5);
	bool acc6Present = I2Ccheck(ch6, ACC6);
	bool acc7Present = I2Ccheck(ch7, ACC7);
	bool acc8Present = I2Ccheck(ch8, ACC8);

	ofstream accel1F("../data/accelerometer1.csv");
	ofstream accel2F("../data/accelerometer2.csv");
	ofstream accel3F("../data/accelerometer3.csv");
	ofstream accel4F("../data/accelerometer4.csv");
	ofstream accel5F("../data/accelerometer5.csv");
	ofstream accel6F("../data/accelerometer6.csv");
	ofstream accel7F("../data/accelerometer7.csv");
	ofstream accel8F("../data/accelerometer8.csv");
	ofstream tempF("../data/temperature.csv");
	ofstream salinityF("../data/salinity.csv");

	SerialStream arduino("/dev/ttyACM0", ios_base::in);
    arduino.SetBaudRate(SerialStreamBuf::BAUD_9600);
    arduino.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
    arduino.SetNumOfStopBits(1);
    arduino.SetParity(SerialStreamBuf::PARITY_NONE);
    arduino.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_NONE);

    string line;
	float tempData;
	while (1)
	{

		bool tempSampled = false;
		if (acc1Present) //Measure angle and magnetic intensity then add to string
		{
			if (!tempSampled)
			{
				tempData = calculateTemp(ch1);
				tempF << getCurrentTimestampMillisString() << to_string(tempData) << endl;
				tempSampled = true;
			}
			calculateAverageAcc(ch1, ACC1);
            processAccelerometerData(ch1, ACC1, accel1F);
		}

		if (acc2Present)
		{
			if (!tempSampled)
			{
				tempData = calculateTemp(ch2);
				dataString += to_string(tempData);
				tempSampled = true;
			}
			calculateAverageAcc(ch2, ACC2);
            processAccelerometerData(ch2, ACC2, accel2F);
		}

		if (acc3Present)
		{
			if (!tempSampled)
			{
				tempData = calculateTemp(ch3);
				dataString += to_string(tempData);
				tempSampled = true;
			}
			calculateAverageAcc(ch3, ACC3);
            processAccelerometerData(ch3, ACC3, accel3F);
		}

		if (acc4Present)
		{
			if (!tempSampled)
			{
				tempData = calculateTemp(ch4);
				dataString += to_string(tempData);
				tempSampled = true;
			}
			calculateAverageAcc(ch4, ACC4);
            processAccelerometerData(ch4, ACC4, accel4F);
		}
		if (acc5Present)
		{
			if (!tempSampled)
			{
				tempData = calculateTemp(ch5);
				dataString += to_string(tempData);
				tempSampled = true;
			}
			calculateAverageAcc(ch5, ACC5);
            processAccelerometerData(ch5, ACC5, accel5F);
		}
		if (acc6Present)
		{
			if (!tempSampled)
			{
				tempData = calculateTemp(ch6);
				dataString += to_string(tempData);
				tempSampled = true;
			}
			calculateAverageAcc(ch6, ACC6);
            processAccelerometerData(ch6, ACC6, accel6F);
		}
		if (acc7Present)
		{
			if (!tempSampled)
			{
				tempData = calculateTemp(ch7);
				dataString += to_string(tempData);
				tempSampled = true;
			}
			calculateAverageAcc(ch7, ACC7);
            processAccelerometerData(ch7, ACC7, accel7F);
		}
		if (acc8Present)
		{
			if (!tempSampled)
			{
				tempData = calculateTemp(ch8);
				dataString += to_string(tempData);
				tempSampled = true;
			}
			calculateAverageAcc(ch8, ACC8);
            processAccelerom
			eterData(ch8, ACC8, accel8F);
		}
		if (arduino.IsDataAvailable())
		{
			float voltage;
			arduino >> voltage;
			getline(arduino, line); // Read the remaining newline character
			float salinity = tempSampled ? calculateSalinity(voltage, tempData) : calculateSalinity(voltage, 16.0);
			salinityF << getCurrentTimestampMillisString() << salinity << endl;
		}
		sleep(10);
	}
	accel1F.close();
	accel2F.close();
	accel3F.close();
	accel4F.close();
	accel5F.close();
	accel6F.close();
	accel7F.close();
	accel8F.close();
}

int I2C_write(int slave_address, uint8_t reg_address, uint8_t configData, int byteCount)
{
	return wiringPiI2CWriteReg8(slave_address, (int)reg_address, (int)configData);
}

int I2C_read(int slave_address, uint8_t reg_address)
{
	return wiringPiI2CReadReg8(slave_address, (int)reg_address);
}

void PCA_Reg_Write(uint8_t reg_address, uint8_t data)
{
	I2C_write(PCAwiring, reg_address, data, 1);
}

int16_t LSM_Acc_Reg_Read(uint8_t reg_address)
{
	uint8_t configData[2];
	configData[0] = I2C_read(LSMwiring, reg_address);
	configData[1] = I2C_read(LSMwiring, reg_address + 1);
	int16_t accData;
	accData = configData[1];
	accData = (accData << 8) | configData[0];
	return accData;
}

void LSM_Acc_Read(uint8_t channel, int16_t *acc_all)
{
	PCA_Reg_Write(0x09, 0x08 | channel); // Select the correct channel
	for (i = 0; i < 3; i++)				 // Sequential read
	{
		acc_all[i] = LSM_Acc_Reg_Read(ACC_addr[i]);
	}
}

void LSM_Acc_Reg_Write(uint8_t reg_address, uint8_t data)
{
	I2C_write(LSMwiring, reg_address, data, 1);
}

void LSM_Init(uint8_t channel)
{
	//printf("Start Init... \n");
	PCA_Reg_Write(0x09, 0x08 | channel);
	LSMwiring = wiringPiI2CSetup(LSM_ACC_ADDR);
	//cout << LSMwiring << endl;
	if (LSMwiring > 0)
	{
		LSM_Acc_Reg_Write(0x21, 0x00); //HPF settings
		LSM_Acc_Reg_Write(0x22, 0x00); //all interrupt disabled
		LSM_Acc_Reg_Write(0x23, 0x88); //BDU, FS = 2g, high resl
		LSM_Acc_Reg_Write(0x24, 0x00); //memory settings
		LSM_Acc_Reg_Write(0x25, 0x00); //all interrupt disabled
		LSM_Acc_Reg_Write(0x20, 0x57); //1 Hz, XYZ enabled 0x17 for 1Hz, 0x57 for 100Hz, 0x77 for 400Hz
		LSM_Acc_Reg_Write(0x1F, 0x03); //Temperature measurement enabled
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
	int16_t ACC_sum[3];
	LSM_Init(ch);
	//HAL_Delay(100);
	for (i = 0; i < 3; i++)
	{ //Initialize the variables
		ACC_sum[i] = 0;
	}
	for (q = 0; q < 16; q++)
	{
		LSM_Acc_Read(ch, acc); //Get the XYZ values for both sensors
		for (i = 0; i < 3; i++)
		{
			ACC_sum[i] += (acc[i] / 16); // Shift the data by 4 bits to account for left justification
		}
		usleep(1100); // Slightly longer than 10ms for 100Hz to make sure we have a new sample.
	}
	for (i = 0; i < 3; i++)
	{
		acc[i] = (int)(ACC_sum[i] / 16); // Save the average of the data
	}
	PCA_Reg_Write(0x09, 0x08 | ch);
	LSM_Acc_Reg_Write(0x20, 0x00); //Shut down sensor
}

bool I2Ccheck(uint8_t ch, int16_t *acc)
{
	//Check the Who Am I register to see if the pair is there.
	PCA_Reg_Write(0x09, 0x08 | ch);				   // Select the correct channel
	int16_t check = LSM_Acc_Reg_Read(0x0F) & 0xff; // Check to see if the response is correct
	//cout << hex << botCheck << endl;
	//botCheck = botCheck & 0xff;
	bool present = (check == 0x33) ? true : false;
	return present;
}

string getCurrentTimestampMillisString()
{
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = chrono::duration_cast<chrono::milliseconds>(duration).count();
    return to_string(millis);
}

void printRowToCSV(ofstream& file, long long timestamp, int16_t* acc)
{
    file << timestamp << "," << acc[0] << "," << acc[1] << "," << acc[2] << endl;
}

void processAccelerometerData(int ch, int16_t* acc, ofstream& file)
{
    file << getCurrentTimestampMillisString();
    for (int i = 0; i < 3; i++) {
        file << "," << acc[i];
    }
    file << endl;
}

float calculateSalinity(float voltage, float temperature) {
  // Temperature compensation formula: fFinalResult(25^C) = fFinalResult(current) / (1.0 + 0.02 * (fTP - 25.0))
  float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
  float compensationVoltage = voltage / compensationCoefficient;

  // Convert voltage value to TDS value
  float tdsValue = (133.42 * pow(compensationVoltage, 3) - 255.86 * pow(compensationVoltage, 2) + 857.39 * compensationVoltage) * 0.5;

  return tdsValue;
}