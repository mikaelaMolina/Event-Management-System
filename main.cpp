#include <iostream>
#include <mysql.h>
#include <mysqld_error.h>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <windows.h>
#include <limits>

/*Event Management/Directory - in progress

1st Week - User options

2nd Week -  View Directory (Think of something else to add) - let's try using database

3rd Week - Delete from the directory

*/

using namespace std;

//these are Database credentials, reminder to open the database access windows + R, cmd then -u root -p, USE eventDirectory (database name)

char HOST[] = "localhost";
char USER[] = "root";
char PASS[] = "";
char DB[] = "eventDirectory";

struct Events { //using struct, user-defined structure
	
	//these are vectors for the data
	
    vector<string> title;
    vector<string> date;
    vector<string> tag;
    vector<string> org;

    MYSQL* obj; // Database connection object

    // this part is to initialize the database connection, making sure that the database has connected
    bool initDatabase() { //will indicate whether true or false
    	
        obj = mysql_init(0);
        
        if (!obj) {
        	
            cout << "ERROR: MySQL object could not be created." << endl;
            return false;
            
        }

        if (!mysql_real_connect(obj, HOST, USER, PASS, DB, 3306, NULL, 0)) {
        	
            cout << "ERROR: Could not connect to database." << endl;
            cout << mysql_error(obj) << endl;
            return false;
            
        }
        return true;
    }

    // to close the database connection
    
    void closeDatabase() {
    	
        if (obj) {
        	
            mysql_close(obj);
            cout << "Database connection closed." << endl;
            
        }
    }

    // Check if there is data in the database
    
    bool hasData() {
    	
        if (mysql_query(obj, "SELECT COUNT(*) FROM events")) {
        	
            cout << "ERROR: Could not execute query." << endl;
            cout << mysql_error(obj) << endl;
            return false;
            
        }

        MYSQL_RES* result = mysql_store_result(obj);
        if (!result) {
            cout << "ERROR: Could not store result." << endl;
            cout << mysql_error(obj) << endl;
            return false;
        }

        MYSQL_ROW row = mysql_fetch_row(result);
        
        if (row && row[0]) {
        	
            int count = atoi(row[0]);
            mysql_free_result(result);
            return count > 0;
            
        }

        mysql_free_result(result);
        
        return false;
        
    }

void userOptions() {
    if (hasData()) {
        rearrangeIDs();
        int n;
        do {

            
            cout << endl << endl << "There are existing events, what would you like to do with it?" << endl << endl;
            cout << "1. Add" << endl;
            cout << "2. Edit" << endl;
            cout << "3. Delete" << endl;
            cout << "4. Search" << endl;
            cout << "5. View Directory" << endl << endl;

            asciiBorder();

            cout << endl << "Please enter desired action: ";
            cin >> n;

            if (cin.fail() || n < 1 || n > 5) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid Input. Please enter a number between 1 and 5." << endl;
                system("pause"); // Wait for user input before clearing
            }

        } while (n < 1 || n > 5);

        switch (n) {
            case 1:
                addData();
                userOptions();
                break;
            case 2:
                editData();
                userOptions();
                break;
            case 3:
                deleteData();
                userOptions();
                break;
            case 4:
                searchData();
                userOptions();
                break;
            case 5:
                showDirectory();
                userOptions();
                break;
        }
    } else {
        cout << "There is no data in the directory yet.";
        addData();
    }
}


    void addData() {
    
    	
    	
    asciiBorder();

    string title, date, tag, org;

    cout << "Enter Title: ";

    if (cin.peek() == '\n') cin.ignore(); // Clear leftover newline only if it exists
    getline(cin, title); // Now reads correctly

    cout << "Enter Date (YYYY-MM-DD): ";
    getline(cin, date);

    cout << "Enter Tag: ";
    getline(cin, tag);

    cout << "Enter Organization: ";
    getline(cin, org);

    // Insert data into the database
    string query = "INSERT INTO events (title, date, tag, org) VALUES ('" +
                   title + "', '" + date + "', '" + tag + "', '" + org + "')";

    if (mysql_query(obj, query.c_str())) {
        cout << "ERROR: Could not insert data." << endl;
            asciiBorder();
        system("pause");
        cout << mysql_error(obj) << endl;
    } else {
        cout << "Data added successfully!" << endl;
            asciiBorder();
        system("pause");
    }
    
    system("pause");
    
    system("cls");
    
    userOptions();
    }

    void editData() {
    	
        asciiBorder();
        //cout << endl << "Editing Data" << endl;
        //put code here for editing the data
        
        int a;
        
        cout << "What data would you like to edit?" << endl;
        cout << "1. Title" << endl;
		cout << "2. Date" << endl;
		cout << "3. Tag" << endl;
		cout << "4. Organization" << endl << endl;
        
        cout << "Enter option: " << endl;
        
        cin >> a;

                if (cin.fail() || a < 1 || a > 5) {
                	
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid Input. Please enter a number between 1 and 5." << endl;\
                    system("pause");
                    
                    userOptions(); //temporary
                    
                }
                
                //add data 
        
        
        
        asciiBorder();
        system("pause");
        system("cls");
        
    }

void deleteData() {
	
	system("cls");
	
    asciiBorder();
    cout << "\nDeleting Data" << endl;

    // Display current events
    MYSQL_RES* result;
    MYSQL_ROW row;

    if (mysql_query(obj, "SELECT new_id, title, date, tag, org FROM events ORDER BY new_id ASC")) {
        cout << "ERROR: Could not execute query." << endl;
        cout << mysql_error(obj) << endl;
        return;
    }

    result = mysql_store_result(obj);

    vector<int> eventIDs;

    // Display table header
    cout << left << setw(5) << "ID"
         << setw(40) << "Title"
         << setw(15) << "Date"
         << setw(20) << "Tag"
         << setw(20) << "Org"
         << endl;
    
    asciiBorder();

    // Display event details
    while ((row = mysql_fetch_row(result))) {
        int id = atoi(row[0]); // Convert new_id to integer
        eventIDs.push_back(id);
        cout << left << setw(5) << id  // new_id
             << setw(40) << (row[1] ? row[1] : "NULL")  // Title
             << setw(15) << (row[2] ? row[2] : "NULL")  // Date
             << setw(20) << (row[3] ? row[3] : "NULL")  // Tag
             << setw(20) << (row[4] ? row[4] : "NULL")  // Org
             << endl;
    }

    asciiBorder();
    mysql_free_result(result);

    int eventID;
    cout << "Enter the ID of the event you wish to delete: ";
    cin >> eventID;

    // Check if the entered ID exists
    if (find(eventIDs.begin(), eventIDs.end(), eventID) == eventIDs.end()) {
        cout << "Invalid ID. Returning to menu..." << endl;
        return;
    }

    cout << "Are you sure you want to delete this event? (y/n): ";
    char confirm;
    cin >> confirm;

    if (tolower(confirm) == 'y') {
        string query = "DELETE FROM events WHERE new_id = " + to_string(eventID);
        if (mysql_query(obj, query.c_str())) {
            cout << "ERROR: Could not delete the event." << endl;
            cout << mysql_error(obj) << endl;
        } else {
            cout << "Event deleted successfully!" << endl;
        }
    } else {
        cout << "Deletion cancelled." << endl;
    }

    asciiBorder();
    
    system("pause");
    
    system("cls");
}



    void searchData() {
    	
    	system("cls");
    	
    	
        asciiBorder();
        cout << endl << "Searching Data" << endl;
        //put code here for showing the directory
        asciiBorder();
        
        system("pause");
        
        system("cls");
    }

    void asciiBorder() {
        cout << endl;
        for (int i = 0; i < 115; i++) {
            cout << "■";
        }
        cout << endl;
    }
    
void rearrangeIDs() {
    // Reset new_id counter
    if (mysql_query(obj, "SET @new_id = 0")) {
        cout << "ERROR: Could not reset new_id counter." << endl;
        cout << mysql_error(obj) << endl;
        return;
    }

    // Update new_id column, ordering by latest date first
    string updateQuery = "UPDATE events "
                         "SET new_id = (@new_id := @new_id + 1) "
                         "ORDER BY date DESC";

    if (mysql_query(obj, updateQuery.c_str())) {
        cout << mysql_error(obj) << endl;
    } else {
    }
}


void showDirectory() {
	
	system("cls");
	
	
    MYSQL_RES* result;
    MYSQL_ROW row;

    // Fetch events sorted by new_id (1 = latest event)
    string query = "SELECT new_id, title, date, org, tag FROM events ORDER BY new_id ASC";

    if (mysql_query(obj, query.c_str())) {
        cout << "ERROR: Could not execute query." << endl;
        cout << mysql_error(obj) << endl;
        return;
    }

    result = mysql_store_result(obj);

    asciiBorder();

    cout << endl << endl;

    cout << left << setw(5) << "No."
         << setw(40) << "T I T L E"
         << setw(30) << "D A T E"
         << setw(30) << "O R G"
         << setw(30) << "T A G"
         << endl;
    
    asciiBorder();

    while ((row = mysql_fetch_row(result))) {
        cout << left << setw(5) << (row[0] ? row[0] : "NULL")  // new_id (arranged)
             << setw(40) << (row[1] ? row[1] : "NULL")  // Title
             << setw(30) << (row[2] ? row[2] : "NULL")  // Date
             << setw(30) << (row[3] ? row[3] : "NULL")  // Organization
             << setw(30) << (row[4] ? row[4] : "NULL")  // Tag
             << endl;
    }

    asciiBorder();
    
    system("pause");
    system("cls");
    

    mysql_free_result(result);
}


};

int main() {
    SetConsoleOutputCP(CP_UTF8);

    cout << endl << "                                   WELCOME TO THE EVENT MANAGEMENT/DIRECTORY" << endl;

    Events event;

    // Initialize the database connection
    if (!event.initDatabase()) {
        return 1; // Exit if the database connection fails
    }

    event.asciiBorder();
    event.userOptions();

    // Close the database connection when done
    event.closeDatabase();

    return 0;
}