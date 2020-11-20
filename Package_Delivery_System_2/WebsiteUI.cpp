/*
 * This represents the website user interface that a person tracking or intercepting mail would interact with.
 */

/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include "DB_Calls.h"

using namespace std;

DB_Calls* db = new DB_Calls();

/* Constant Variables */
const int TRACK_PACKAGE = 1; 
const int REDIRECT = 2;
const int HOLD_PACKAGE = 3;
const int TRACK_NUM_SIZE = 12;
const string whitespace = " \n\r\t\f\v";

/* Method Identifiers */
void executeAction();
int displayInitPrompt();
string displayTrackNumPrompt();
int displayAddressPrompt();
void displayStatuses(string trackingNum);
void redirectPackage(int addrID, string trackingNum);
void holdPackage(string trackingNum);
void trimWhiteSpace(string& stringToTrim);

int main(void) {
	cout << "***Welcome to our Package Delivery System Simulation!***" << endl;
	executeAction();
	return 0;
}

/*
 * Decides what action should be taken based on user input.
 */
void executeAction() {
	cout << "Available actions:\n"
		<< "\t(1) Track a package\n\t(2) Redirect a package\n\t(3) Hold a package" << endl;
	int action = displayInitPrompt();
	string trackNum;

	if (action == TRACK_PACKAGE) {
		trackNum = displayTrackNumPrompt();
		displayStatuses(trackNum);
	}
	else if (action == REDIRECT) {
		trackNum = displayTrackNumPrompt();
		if (db->isEnroute(trackNum) == true) {
			cout << "The package with the entered tracking ID is out for delivery and cannot be redirected." << endl;
			return executeAction();
		}
		else if (db->hasBeenRedirected(trackNum) == true) {
			cout << "The package with the entered tracking ID has already been redirected and cannot be redirected again." << endl;
			return executeAction();
		}
		int addrID = displayAddressPrompt();
		redirectPackage(addrID, trackNum);
	}
	else { //action == HOLD_PACKAGE
		trackNum = displayTrackNumPrompt();
		if (db->isEnroute(trackNum) == true) {
			cout << "The package with the entered tracking ID is out for delivery and cannot be intercepted." << endl;
			return executeAction();
		}
		else if (db->isBeingHeld(trackNum) == true) {
			cout << "The package is already being held at the post office located at..." << endl;
			return executeAction();
		}
		holdPackage(trackNum);
	}
	return executeAction();
}

/*
 * Displays the statuses of a parcel to the UI.
 * @param - tracking number
 */
void displayStatuses(string trackingNum) {
	DB_Calls *db = new DB_Calls();
	vector<Triplet> statuses = db->getStatuses(trackingNum);
	cout << "Statuses for #" << trackingNum << ":" << endl;
	cout << "DATE & TIME\t\t\tSTATUSES\t\t\tLOCATION" << endl;
	for (int i = 0; i < statuses.size(); i++) {
		cout << statuses[i].dateTime << "\t\t\t" << statuses[i].status << "\t\t\t" << statuses[i].addr->toString() << endl;
	}
}

/*
 * Calls the method that handles package redirection in "DB_Calls.h" and displays a success message to user.
 * @param - address ID and tracking number
 */
void redirectPackage(int addrID, string trackingNum) {
	db->redirectPackage(addrID, trackingNum);
	cout << "Your package has been redirected to " << db->getAddress(addrID)->toString() << endl;
}

/*
 * Calls the method that handles holding a package in "DB_Calls.h" and displays a success message to user.
 * @param - tracking number
 */
void holdPackage(string trackingNum) {
	db->holdPackage(trackingNum);
	cout << "Your package intercept request is being processed." << endl;
}

/*
 * Displays the initial action prompt to the user.
 * return - the integer relating to the chosen action.
 */
int displayInitPrompt() {
	cout << "Please enter 1, 2 or 3 to complete an action: ";

	int action;
	string actionString;
	getline(cin, actionString);
	action = stoi(actionString);

	if (action != 1 && action != 2 && action != 3) { //Breaks with string entry
		cout << "This is not a valid option." << endl;
		return displayInitPrompt();
	}
	return action;
}

/*
 * Displays prompt asking for a tracking number.
 * return - returns the tracking number entered by the user.
 */
string displayTrackNumPrompt() {
	cout << "Enter your 12-digit tracking number: ";
	
	string trackNum;
	getline(cin, trackNum);

	if (trackNum.length() != TRACK_NUM_SIZE) {
		cout << "The tracking number you entered is not 12-digits." << endl;
		return displayTrackNumPrompt();
	}
	DB_Calls* db = new DB_Calls();
	if (db->checkTrackNumExists(trackNum) >= 0) {
		return trackNum;
	}
	else
	{
		cout << "The tracking number you entered was not found in ouor system." << endl;
		return displayTrackNumPrompt();
	}
}

/*
 * Displays the prompt asking for an address and checks to see if the address is in the DB.
 * return - returns ID of address if found, and displays and error message and asks for another address if not found.
 */
int displayAddressPrompt() {
	string street;
	string city;
	string state;
	string zipCode;

	cout << "Enter street: ";
	getline(cin, street);

	cout << "Enter city: ";
	getline(cin, city);

	cout << "Enter state: ";
	getline(cin, state);

	cout << "Enter zip code: ";
	getline(cin, zipCode);

	int lastIndex = street.length() - 1;
	string houseNum = street.substr(street.find_first_not_of(whitespace), street.find_first_of(whitespace));
	string streetName = street.substr(street.find_first_of(whitespace), lastIndex);

	trimWhiteSpace(houseNum);
	trimWhiteSpace(streetName);
	trimWhiteSpace(city);
	trimWhiteSpace(state);
	trimWhiteSpace(zipCode);

	Address *addr = new Address(stoi(houseNum), streetName, zipCode, city, state);

	DB_Calls* db = new DB_Calls();
	int addrID = db->checkAddressExists(addr);
	if (addrID >= 0) {
		return addrID;
	}
	else {
		cout << "This address was not found in our system." << endl;
		return displayAddressPrompt();
	}
}

/* Util Methods */
void trimWhiteSpace(string &stringToTrim) {
	stringToTrim.erase(0, stringToTrim.find_first_not_of(whitespace));
	stringToTrim.erase(stringToTrim.find_last_not_of(whitespace) + 1);
}