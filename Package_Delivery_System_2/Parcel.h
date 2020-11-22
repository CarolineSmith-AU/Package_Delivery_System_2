/*
 * This class contains the components of an address.
 */

#include <stdlib.h>
#include <iostream>
#include <string>
#include "Address.h"

using namespace std;
class Parcel {
private:
	string trackingNum;
	Address *deliveryAddr;
	Address *returnAddr;
	bool isPriority;
	int status;
	int holdFlag;
	int redirectFlag;

public:
	//constructor
	Parcel(string trackingNumIn, Address* deliveryAddrIn, Address* returnAddrIn, bool isPriorityIn, int statusIn, int holdFlagIn, int redirectFlagIn) {
		trackingNum = trackingNumIn;
		deliveryAddr = deliveryAddrIn;
		returnAddr = returnAddrIn;
		isPriority = isPriorityIn;
		status = statusIn;
		holdFlag = holdFlagIn;
		redirectFlag = redirectFlagIn;
	}

	/**************Getters and Setters**************/
	string getTrackingNum() {
		return trackingNum;
	}

	void setTrackingNum(string trackingNum) {
		this->trackingNum = trackingNum;
	}

	Address* getDeliveryAddr() {
		return deliveryAddr;
	}

	void setDeliveryAddr(Address* deliveryAddr) {
		this->deliveryAddr = deliveryAddr;
	}

	Address* getReturnAddr() {
		return deliveryAddr;
	}

	void setReturnAddr(Address* returnAddr) {
		this->returnAddr = returnAddr;
	}

	bool returnPriority() {
		return isPriority;
	}

	void setPriority(bool isPriority) {
		this->isPriority = isPriority;
	}

	int getStatus() {
		return status;
	}  
	
	void setStatus(int status) {
		this->status = status;
	}

	int getHoldFlag() {
		return holdFlag;
	}

	void setHoldFlag(int holdFlag) {
		this->holdFlag = holdFlag;
	}

	int getRedirectFlag() {
		return redirectFlag;
	}

	void setRedirectFlag(int redirectFlag) {
		this->redirectFlag = redirectFlag;
	}
};