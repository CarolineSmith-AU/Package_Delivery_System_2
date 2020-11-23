/*
 * This is the systems boundary class. It takes user input from WebsiteUI, and manipulates or
 * pulls data from the MySQL database depending on the action.
 */

#include <stdlib.h>
#include <iostream>
#include <vector>
#include "Address.h"
#include "DB_Connection_Handler.h"

/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

/* Structs */
struct Status {
    int status;
    Address* addr;
    string dateTime;
};

class WebsiteBoundary {
private:
    /*Database connection handler*/
    DB_Conn_Handler *handler = new DB_Conn_Handler("package_delivery_sys");

public:

    /* Constructor */
    WebsiteBoundary() { }   

    /*
     * Checks if a tracking number exists in the DB.
     * @param - tracking number
     * return - returns positive int if found, and negative int if not found.
     */
    int checkTrackNumExists(string trackingNum) {
        string query = "SELECT * FROM parcels WHERE TRACKING_NUM = \"" + trackingNum + "\"";
        handler->getQuery(query);
      
        if (handler->getRes()->next()) {
            return 1;
        }
        else {
            return -1;
        }
    }

    /*
     * Checks if a parcel with the tracking number exists in the DB.
     * @param - tracking number
     * return - returns positive int if found, and negative int if not found.
     */
    int checkAddressExists(Address *addr) {
        string query = "SELECT * FROM addresses WHERE HOUSE_NUM = " + to_string(addr->getHouseNum()) + " AND STREET_NAME = \"" + addr->getStreetName() + "\"" + " AND ZIP_CODE = \"" + addr->getZipCode() + "\"" + " AND CITY = \"" + addr->getCity() + "\"" + " AND STATE = \"" + addr->getState() + "\"";

        handler->getQuery(query);

        if (handler->getRes()->next()) {
            int addrID = handler->getRes()->getInt("ID");
            return addrID;
        }
        else {
            return -1;
        }
    }

    /*
     * Checks if a parcel has already been redirected.
     * @param - tracking number
     * return - returns true if it has been redirected, and false if it hasn't.
     */
    bool hasBeenRedirected(string trackingNum) {
        string query = "SELECT * FROM parcels WHERE TRACKING_NUM = \"" + trackingNum + "\"";
        handler->getQuery(query);
        handler->getRes()->next();
        int flag = handler->getRes()->getInt("REDIRECT_FLAG");
        if (flag == 1) {
            return true;
        }
        return false;
    }

    /*
     * Updates the status of a parcel.
     * @param - tracking number, status you want to update parcel to, and the related location.
     * return - returns positive int if found, and negative int if not found.
     */
    void updateParcelStatus(string trackingNum, int status, int location) {
        string nowQuery = "SELECT NOW()";
        handler->getQuery(nowQuery);
        handler->getRes()->next();
        string now = handler->getRes()->getString("NOW()");
        string insertQuery = "INSERT INTO statuses (TRACKING_NUM, TIME_STAMP, STAT, LOCATION) VALUES (\"" + trackingNum + "\", \"" + now + "\", " + to_string(status) + ", " + (location == NULL ? to_string(0) : to_string(location)) + ")";
        handler->postQuery(insertQuery);
    }

    /*
     * Changes destination address of a parcel.
     * @param - the ID of an address in the DB, and a tracking number
     */
    void redirectPackage(int addrID, string trackingNum) {
        string changeAddrQuery = "UPDATE parcels SET DEST_ADDR = " + to_string(addrID) + " WHERE TRACKING_NUM = \"" + trackingNum + "\"";
        string changeRedirectFlagQuery = "UPDATE parcels SET REDIRECT_FLAG = 1 WHERE TRACKING_NUM = \"" + trackingNum + "\"";
        handler->postQuery(changeAddrQuery);
        handler->postQuery(changeRedirectFlagQuery);
        updateParcelStatus(trackingNum, 7, addrID);
    }

    /*
     * Checks if a parcel is being held for pickup.
     * @param - tracking number
     * return - returns true if it is being held, and false if it is not.
     */
    bool isBeingHeld(string trackingNum) {
        string query = "SELECT * FROM parcels WHERE TRACKING_NUM = \"" + trackingNum + "\"";
        handler->getQuery(query);
        handler->getRes()->next();
        int flag = handler->getRes()->getInt("HOLD_FLAG");
        if (flag == 1) {
            return true;
        }
        return false;
    }

    /*
     * Checks if a parcel is currently out for delivery.
     * @param - tracking number
     * return - returns positive int if it is out for delivery, and negative if it's not
     */
    bool isEnroute(string trackingNum) {
        string query = "SELECT * FROM parcels WHERE TRACKING_NUM = \"" + trackingNum + "\"";
        handler->getQuery(query);
        handler->getRes()->next();
        int flag = handler->getRes()->getInt("CURR_STAT");
        if (flag == 4) {
            return true;
        }
        return false;
    }

    /*
     * Marks a parcel as "Hold for Pickup" in the DB.
     * @param - tracking number
     */
    void holdPackage(string trackingNum) {
        string query = "UPDATE parcels SET HOLD_FLAG = 1 WHERE TRACKING_NUM = \"" + trackingNum + "\"";
        handler->postQuery(query);
        updateParcelStatus(trackingNum, 6, NULL);
    }

    /*
     * Gets an address from the DB.
     * @param - ID of address
     * return - returns a pointer to an Address object (Address.h).
     */
    Address* getAddress(int addrID) {
        string query = "SELECT * FROM addresses WHERE ID = " + to_string(addrID);
        handler->getQuery(query);
        handler->getRes()->next();
        Address* addr = new Address(handler->getRes()->getInt(2), handler->getRes()->getString(3), handler->getRes()->getString(4), handler->getRes()->getString(5), handler->getRes()->getString(6));
        return addr;
    }

    /*
     * Gets all statuses of a parcel in order of most recent to least recent.
     * @param - tracking number
     * return - returns a vector of Triplet structs, containing the date and time stamp, the status and the location.
     */
    vector<Status> getStatuses(string trackingNum) {
        vector<Status> statuses;
        Status trip;
        string query = "SELECT * FROM STATUSES WHERE TRACKING_NUM = \"" + trackingNum + "\"" + " ORDER BY TIME_STAMP DESC";
        handler->getQuery(query);
        sql::ResultSet* tempRes;
        while (handler->getRes()->next()) {
            int status = handler->getRes()->getInt("STAT");
            string dateTime = handler->getRes()->getString("TIME_STAMP");
            tempRes = handler->getRes(); //save res pointer
            Address* addr = this->getAddress(handler->getRes()->getInt("LOCATION"));
            
            trip.status = status;
            trip.addr = addr;
            trip.dateTime = dateTime;
            statuses.push_back(trip);

            handler->setRes(tempRes); //Restore res pointer
        }
        return statuses;
    }
};