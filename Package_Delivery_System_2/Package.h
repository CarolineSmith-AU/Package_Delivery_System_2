/*
 * This class contains the components of an address.
 */

#include <stdlib.h>
#include <iostream>
#include <string>
#include "Parcel.h"

using namespace std;
class Package : public Parcel {
private:
	int lengthInches, widthInches, heightInches;

public:
	//constructor
	Package(string trackingNumIn, Address* deliveryAddrIn, Address* returnAddrIn, bool isPriorityIn, int statusIn, int holdFlagIn, int redirectFlagIn, int lengthInchesIn, int widthInchesIn, int heightInchesIn) 
		: Parcel(trackingNumIn, deliveryAddrIn, returnAddrIn, isPriorityIn, statusIn, holdFlagIn, redirectFlagIn)
	{
		lengthInches = lengthInchesIn;
		widthInches = widthInchesIn;
		heightInches = heightInchesIn;
	}

	/**************Getters and Setters**************/
	int getLengthInches() {
		return lengthInches;
	}

	void setLengthInches(int lengthInches) {
		this->lengthInches = lengthInches;
	}

	int getWidthInches() {
		return widthInches;
	}

	void setWidthInches(int widthInches) {
		this->widthInches = widthInches;
	}

	int getHeightInches() {
		return heightInches;
	}

	void setHeightInches(int heightInches) {
		this->heightInches = heightInches;
	}
};