/*
 * This is the systems boundary class. It takes user input from WebsiteUI, and manipulates or
 * pulls data from the MySQL database depending on the action.
 */

#include <stdlib.h>
#include <iostream>

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

class DB_Conn_Handler {
private:
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
    sql::ResultSet* res;
    string databaseName;

public:

    /* Constructor */
    DB_Conn_Handler(string databaseNameIn) { 
        databaseName = databaseNameIn;
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
            con->setSchema(this->databaseName);
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

    /*Getters and Setters*/
    sql::ResultSet* getRes() {
        return res;
    }

    void setRes(sql::ResultSet *res) {
        this->res = res;
    }
};
