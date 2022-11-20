#pragma once

/*
 * wiringJetImplementation.h, portions of wiringPi.h
 *  moved to their own file in wiringJet to make the library public API file cleaner
 *
 *    wiringJet is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringJet is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringJet.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */


struct wiringJetNodeStruct
{
	int     pinBase;
	int     pinMax;

	int          fd;   	// Node specific
	unsigned int data0;  	
	unsigned int data1;  	
	unsigned int data2;  	
	unsigned int data3;  	

	void(*pinMode)(struct wiringJetNodeStruct *node, int pin, int mode);
	void(*pullUpDnControl)(struct wiringJetNodeStruct *node, int pin, int mode);
	int(*digitalRead)(struct wiringJetNodeStruct *node, int pin);
	
	void(*digitalWrite)(struct wiringJetNodeStruct *node, int pin, int value);
	void(*pwmWrite)(struct wiringJetNodeStruct *node, int pin, int value);
	void(*pwmSetFrequency)(struct wiringJetNodeStruct *node, float frequency);
	int(*analogRead)(struct wiringJetNodeStruct *node, int pin);
	void(*analogWrite)(struct wiringJetNodeStruct *node, int pin, int value);

	struct wiringJetNodeStruct *next ;
}
;


// Pointer to the first node in the linked list of nodes
extern struct wiringJetNodeStruct *jetGpioNodes;


#define I2CBUSSPEED100k (0)
#define I2CBUSSPEED400k (1)
#define I2CBUSSPEED1000k (2)
	
extern int i2cBusSpeed;


// Manage nodes functions
extern struct wiringJetNodeStruct *wiringJetFindNode(int pin);
extern struct wiringJetNodeStruct *wiringJetNewNode(int pinBase, int numPins);