/*
 * This is the systems boundary class. It takes user input from WebsiteUI, and manipulates or
 * pulls data from the MySQL database depending on the action.
 */

#include <stdlib.h>
#include <iostream>
#include <vector>
#include "Address.h"

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
struct Triplet {
    int status;
    Address* addr;
    string dateTime;
};

/* Constructor */
class DB_Calls {
private:
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
    sql::ResultSet* res;

public:

    DB_Calls() {}   

    /*
     * Checks if a tracking number exists in the DB.
     * @param - tracking number
     * return - returns positive int if found, and negative int if not found.
     */
    int checkTrackNumExists(string trackingNum) {
        string query = "SELECT * FROM parcels WHERE TRACKING_NUM = \"" + trackingNum + "\"";
        getQuery(query);
        if (res->next()) {
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

        getQuery(query);

        if (res->next()) {
            int addrID = res->getInt("ID");
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
        getQuery(query);
        res->next();
        int flag = res->getInt("REDIRECT_FLAG");
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
        getQuery(nowQuery);
        res->next();
        string now = res->getString("NOW()");
        string insertQuery = "INSERT INTO statuses (TRACKING_NUM, TIME_STAMP, STAT, LOCATION) VALUES (\"" + trackingNum + "\", \"" + now + "\", " + to_string(status) + ", " + (location == NULL ? to_string(0) : to_string(location)) + ")";
        postQuery(insertQuery);
    }

    /*
     * Changes destination address of a parcel.
     * @param - the ID of an address in the DB, and a tracking number
     */
    void redirectPackage(int addrID, string trackingNum) {
        string changeAddrQuery = "UPDATE parcels SET DEST_ADDR = " + to_string(addrID) + " WHERE TRACKING_NUM = \"" + trackingNum + "\"";
        string changeRedirectFlagQuery = "UPDATE parcels SET REDIRECT_FLAG = 1 WHERE TRACKING_NUM = \"" + trackingNum + "\"";
        postQuery(changeAddrQuery);
        postQuery(changeRedirectFlagQuery);
        updateParcelStatus(trackingNum, 7, addrID);
    }

    /*
     * Checks if a parcel is being held for pickup.
     * @param - tracking number
     * return - returns true if it is being held, and false if it is not.
     */
    bool isBeingHeld(string trackingNum) {
        string query = "SELECT * FROM parcels WHERE TRACKING_NUM = \"" + trackingNum + "\"";
        getQuery(query);
        res->next();
        int flag = res->getInt("HOLD_FLAG");
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
        getQuery(query);
        res->next();
        int flag = res->getInt("CURR_STAT");
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
        postQuery(query);
        updateParcelStatus(trackingNum, 6, NULL);
    }

    /*
     * Gets an address from the DB.
     * @param - ID of address
     * return - returns a pointer to an Address object (Address.h).
     */
    Address* getAddress(int addrID) {
        string query = "SELECT * FROM addresses WHERE ID = " + to_string(addrID);
        getQuery(query);
        res->next();
        Address* addr = new Address(res->getInt(2), res->getString(3), res->getString(4), res->getString(5), res->getString(6));
        return addr;
    }

    /*
     * Gets all statuses of a parcel in order of most recent to least recent.
     * @param - tracking number
     * return - returns a vector of Triplet structs, containing the date and time stamp, the status and the location.
     */
    vector<Triplet> getStatuses(string trackingNum) {
        vector<Triplet> statuses;
        Triplet trip;
        string query = "SELECT * FROM STATUSES WHERE TRACKING_NUM = \"" + trackingNum + "\"" + " ORDER BY TIME_STAMP DESC";
        getQuery(query);
        sql::ResultSet* tempRes;
        while (res->next()) {
            int status = res->getInt("STAT");   
            string dateTime = res->getString("TIME_STAMP");
            tempRes = res; //save res pointer
            Address* addr = this->getAddress(res->getInt("LOCATION"));
            
            trip.status = status;
            trip.addr = addr;
            trip.dateTime = dateTime;
            statuses.push_back(trip);

            res = tempRes; //Restore res pointer
        }
        return statuses;
    }

    /*
     * Util method for sending/posting information to the DB.
     * @param - query to run
     */
    int postQuery(string query) {
        initCon();
        try {
            stmt->execute(query);
            deleteCon();
        }
        catch (sql::SQLException & e) {
            cout << "# ERR: SQLException in " << __FILE__;
            cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
            cout << "# ERR: " << e.what();
            cout << " (MySQL error code: " << e.getErrorCode();
            cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        }
    }

    /*
     * Utility method for getting data from the DB.
     * @param - query to run
     * return - returns a pointer to a ResultSet object, which holds the data from the specified table.
     */
    sql::ResultSet* getQuery(string query) {
        initCon();
        try {
            res = stmt->executeQuery(query);
            deleteCon();
            return res;
        }
        catch (sql::SQLException & e) {
            cout << "# ERR: SQLException in " << __FILE__;
            cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
            cout << "# ERR: " << e.what();
            cout << " (MySQL error code: " << e.getErrorCode();
            cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        }
    }

    /*
     * Initializes a connection to the DB.
     */
    void initCon() {
        try {
            string username = "root";
            string password = "hU8f6Dw";
            string hostname = "tcp://127.0.0.1:3306";

            /* Create a connection */
            driver = get_driver_instance();
            con = driver->connect("tcp://127.0.0.1:3306", username, password);

            /* Connect to the MySQL test database */
            con->setSchema("package_delivery_sys");
            stmt = con->createStatement();

        }
        catch (sql::SQLException & e) {
            cout << "# ERR: SQLException in " << __FILE__;
            cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
            cout << "# ERR: " << e.what();
            cout << " (MySQL error code: " << e.getErrorCode();
            cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        }
        cout << endl;
    }

    /*
     * Deletes the connection to the DB.
     */
    void deleteCon() {
        if (con != NULL) {
            delete this->con;
        } 
        if (stmt != NULL) {
            delete this->stmt;
        } 
    }
};