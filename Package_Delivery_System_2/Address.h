/*
 * This class contains the components of an address. 
 */

#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;
class Address {
private:
	int houseNum;
	string streetName;
	string zipCode;
	string city;
	string state;

public:
	//constructor
	Address(int houseNumIn, string streetNameIn, string zipCodeIn, string cityIn, string stateIn) {
		houseNum = houseNumIn;
		streetName = streetNameIn;
		zipCode = zipCodeIn;
		city = cityIn;
		state = stateIn;
	}

	/*
	 * Takes components of an Address and concatenates them into one string.
	 */
	string toString() {
		return to_string(this->houseNum) + " " + this->streetName + " " + this->city + ", " + this->state + " " + this->zipCode;
	}

	/**************Getters and Setters**************/
	int getHouseNum() {
		return houseNum;
	}

	void setHouseNum( int houseNum) {
		this->houseNum = houseNum;
	}

	string getStreetName() {
		return streetName;
	}

	void setStreetName(string streetName) {
		this->streetName = streetName;
	}

	string getZipCode() {
		return zipCode;
	}

	void setZipCode(string zipCode) {
		this->zipCode = zipCode;
	}

	string getCity() {
		return city;
	}

	void setCity(string city) {
		this->city = city;
	}

	string getState() {
		return state;
	}

	void setState(string state) {
		this->state = state;
	}
};