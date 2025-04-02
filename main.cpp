#include <iostream>
#include <mysql.h> // MySQL C API
#include <mysqld_error.h> // MySQL error handling
#include <vector>
#include <string>
#include <iomanip> // for setw
#include <algorithm>
#include <cctype>
#include <windows.h> // Windows-specific functions (e.g., clearing console)
#include <limits> // Numeric limits for input validation

/*Event Management/Directory - in progress

1st Week - User options

2nd Week -  View Directory (Think of something else to add) - let's try using database

3rd Week - Delete from the directory

4th Week - Searching

*/

using namespace std;

//these are Database credentials, reminder to open the database access windows + R, cmd then -u root -p, USE eventDirectory (database name)

char HOST[] = "localhost";
char USER[] = "root";
char PASS[] = "";
char DB[] = "eventDirectory";

//Database name is eventDirectory, table name is events

struct Events { //using struct, user-defined structure
	
	//these are vectors for the data
	
    vector<string> title;
    vector<string> date;
    vector<string> tag;
    vector<string> org;

    MYSQL* obj; // MySQL connection object (pointer)

    // this part is to initialize the database connection, making sure that the database has connected
    bool initDatabase() { //will indicate whether true or false
    	
    	// Initialize MySQL object
    	
        obj = mysql_init(0);
        
        if (!obj) {
        	
            cout << "ERROR: MySQL object could not be created." << endl;
            return false;
            
        }
        
        // Connect to database server: host, user, password, database name, port

        if (!mysql_real_connect(obj, HOST, USER, PASS, DB, 3306, NULL, 0)) {
        	
            cout << "ERROR: Could not connect to database." << endl;
            cout << mysql_error(obj) << endl;
            return false;
            
        }
        return true; // Successful connection
    }

    // to close the database connection
    
    void closeDatabase() {
    	
        if (obj) {
        	
            mysql_close(obj); // Close connection
            cout << "Database connection closed." << endl;
            
        }
    }

    // Checks if there is data in the database
    bool hasData() {
    	
    	// Query to count total records
        if (mysql_query(obj, "SELECT COUNT(*) FROM events")) { //if query fails it will display error
        	
            cout << "ERROR: Could not execute query." << endl;
            cout << mysql_error(obj) << endl;
            return false;
            
        }

        MYSQL_RES* result = mysql_store_result(obj); //once the query succeeds the result of the query will be stored in the initialized result here
        
		if (!result) { //error if there's no result
		
            cout << "ERROR: Could not store result." << endl;
            cout << mysql_error(obj) << endl;
            return false;
            
        }

        MYSQL_ROW row = mysql_fetch_row(result); //fetches first row of the result
        
        if (row && row[0]) {
        	
            int count = atoi(row[0]); //If the row is valid and row[0] exists, it converts that string into an integer using atoi()
            mysql_free_result(result); //count is the actual number of records in my table here
            return count > 0; //returns true if count is greater than 0
            
        }

        mysql_free_result(result); //free memory
        
        return false;
        
    }

	//Displays options for users and prompts them to choose an option
	void userOptions() {
	
	asciiBorder();
	
    if (hasData()) {
    	
        rearrangeIDs(); //function to rearrange new_id based on the dates, this will automatically be called every time userOptions is called
        
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

	//Adding data into the database
    void addData() {
    	
    asciiBorder();

    string title, date, tag, org; 

    cout << "Enter Title: ";

    if (cin.peek() == '\n') cin.ignore(); // Clear leftover newline only if it exists (ensures that the first letter is not erased as it usually happens with cin.ignore)
    getline(cin, title); // Now reads correctly

    cout << "Enter Date (YYYY-MM-DD): ";
    getline(cin, date);

    cout << "Enter Tag: ";
    getline(cin, tag);

    cout << "Enter Organization: ";
    getline(cin, org);

    // Insert data into the database
    string query = "INSERT INTO events (title, date, tag, org) VALUES ('" +
                   title + "', '" + date + "', '" + tag + "', '" + org + "')"; //MySQL query for inserting the inputted data into the database

	//error handling incase query fails
	
    if (mysql_query(obj, query.c_str())) {
    	
        cout << "ERROR: Could not insert data." << endl;
            asciiBorder();
            
        cout << mysql_error(obj) << endl;
        
    } else {
    	
        cout << "Data added successfully!" << endl;
            asciiBorder();
            
    }
    
    system("pause"); //pauses the screen before the clearing of the console
    
    system("cls");
    
    userOptions(); //calls userOptions again
    
    }

	//Editing data
    void editData() {
    	
    	//currently coding
    	
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

	//Deleting data
	void deleteData() {
	
	system("cls");
	
    asciiBorder();
    cout << "\nDeleting Data" << endl;

    // Display current events
    MYSQL_RES* result;
    MYSQL_ROW row;

    if (mysql_query(obj, "SELECT new_id, title, date, tag, org FROM events ORDER BY new_id ASC")) { //sql query to retrieve the events from the database table, otherwise displays an error message
    	
        cout << "ERROR: Could not execute query." << endl;
        cout << mysql_error(obj) << endl;
        return;
        
    }

    result = mysql_store_result(obj); //Stores the query result and prepares a vector<int> to keep track of valid IDs for verification later

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
    while ((row = mysql_fetch_row(result))) { //iterates to each row to display the values
    	
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

	//Searching for the data - very messy right now
    void searchData() {
    	
    system("cls");

    asciiBorder();
    
    int n;
    
    do{    
    cout << endl << "What category would you like to search for?" << endl;
    
    asciiBorder();
    
    cout << endl << "1. Title" << endl << "2. Tag" << endl << "3. Organization" << endl;
    
    asciiBorder();
    
    cout << "Enter your desired choice: ";
    cin >> n;
    
    asciiBorder();
    
    if (cin.fail() || n < 1 || n > 3) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid Input. Please enter a number between 1 and 3." << endl;
                system("pause"); // Wait for user input before clearing
            }
    
} while(n<1 || n>3);

	string keyword;
	
	if(n==1){
		
		cout << endl << "Search Events by Title" << endl;
    
    		asciiBorder();

    		cout << "Enter keyword to search in titles: ";
    		
    		if (cin.peek() == '\n') cin.ignore(); // clear newline
    		getline(cin, keyword);
    		
    		if (keyword.empty()) {
    	
        	cout << "No keyword entered. Returning to menu..." << endl;
        	system("pause");
        	system("cls");
        	return;
        	}
    		
    		// Construct SQL query with LIKE operator for partial matches
    		string query = "SELECT new_id, title, date, tag, org FROM events "
                   		"WHERE title LIKE '%" + keyword + "%' ORDER BY new_id ASC";
        	
        	
    if (mysql_query(obj, query.c_str())) {
        cout << "ERROR: Could not execute search query." << endl;
        cout << mysql_error(obj) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(obj);
    
    MYSQL_ROW row;

    int numRows = mysql_num_rows(result);

    if (numRows == 0) {
    	
        cout << "\nNo events found with keyword: \"" << keyword << "\"" << endl;
        
    } else {
    	
        cout << "\nFound " << numRows << " event(s) matching \"" << keyword << "\":" << endl;

        asciiBorder();

        cout << left << setw(5) << "ID"
             << setw(40) << "Title"
             << setw(15) << "Date"
             << setw(20) << "Tag"
             << setw(20) << "Org"
             << endl;

        asciiBorder();

        while ((row = mysql_fetch_row(result))) {
        	
            cout << left << setw(5) << (row[0] ? row[0] : "NULL")
                 << setw(40) << (row[1] ? row[1] : "NULL")
                 << setw(15) << (row[2] ? row[2] : "NULL")
                 << setw(20) << (row[3] ? row[3] : "NULL")
                 << setw(20) << (row[4] ? row[4] : "NULL")
                 << endl;
                 
        }

        asciiBorder();
    }
    
	

    mysql_free_result(result);

    system("pause");
    system("cls");
                   
		
	} else if(n == 2){
		
		
    		cout << endl << "Search Events by Tag" << endl;
    		
    		asciiBorder();
    		
    		cout << "Enter keyword to search in tags: ";
    		
    		if (cin.peek() == '\n') cin.ignore();
    		getline(cin, keyword);
    		
    		if (keyword.empty()) {
    	
        	cout << "No keyword entered. Returning to menu..." << endl;
        	system("pause");
        	system("cls");
        	return;
        	}
        	
    		
    		// Construct SQL query with LIKE operator for partial matches
    		string query = "SELECT new_id, title, date, tag, org FROM events "
                   		"WHERE tag LIKE '%" + keyword + "%' ORDER BY new_id ASC";
                   		
        	
    if (mysql_query(obj, query.c_str())) {
        cout << "ERROR: Could not execute search query." << endl;
        cout << mysql_error(obj) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(obj);
    
    MYSQL_ROW row;

    int numRows = mysql_num_rows(result);

    if (numRows == 0) {
    	
        cout << "\nNo events found with keyword: \"" << keyword << "\"" << endl;
        
    } else {
    	
        cout << "\nFound " << numRows << " event(s) matching \"" << keyword << "\":" << endl;

        asciiBorder();

        cout << left << setw(5) << "ID"
             << setw(40) << "Title"
             << setw(15) << "Date"
             << setw(20) << "Tag"
             << setw(20) << "Org"
             << endl;

        asciiBorder();

        while ((row = mysql_fetch_row(result))) {
        	
            cout << left << setw(5) << (row[0] ? row[0] : "NULL")
                 << setw(40) << (row[1] ? row[1] : "NULL")
                 << setw(15) << (row[2] ? row[2] : "NULL")
                 << setw(20) << (row[3] ? row[3] : "NULL")
                 << setw(20) << (row[4] ? row[4] : "NULL")
                 << endl;
                 
        }

        asciiBorder();
    }
    
	

    mysql_free_result(result);

    system("pause");
    system("cls");
        
	} else if(n == 3){
		
		
    		cout << endl << "Search Events by Organization" << endl;
    		
    		asciiBorder();
    		
    		cout << "Enter keyword to search in organizations: ";
    		
    		if(cin.peek() == '\n') cin.ignore();
    		getline(cin, keyword);
    		
    		if (keyword.empty()) {
    	
        	cout << "No keyword entered. Returning to menu..." << endl;
        	system("pause");
        	system("cls");
        	return;
        	}
    		
    		// Construct SQL query with LIKE operator for partial matches
    		string query = "SELECT new_id, title, date, tag, org FROM events "
                   		"WHERE org LIKE '%" + keyword + "%' ORDER BY new_id ASC";
                   		
            
            
    if (mysql_query(obj, query.c_str())) {
        cout << "ERROR: Could not execute search query." << endl;
        cout << mysql_error(obj) << endl;
        return;
    }

    MYSQL_RES* result = mysql_store_result(obj);
    
    MYSQL_ROW row;

    int numRows = mysql_num_rows(result);

    if (numRows == 0) {
    	
        cout << "\nNo events found with keyword: \"" << keyword << "\"" << endl;
        
    } else {
    	
        cout << "\nFound " << numRows << " event(s) matching \"" << keyword << "\":" << endl;

        asciiBorder();

        cout << left << setw(5) << "ID"
             << setw(40) << "Title"
             << setw(15) << "Date"
             << setw(20) << "Tag"
             << setw(20) << "Org"
             << endl;

        asciiBorder();

        while ((row = mysql_fetch_row(result))) {
        	
            cout << left << setw(5) << (row[0] ? row[0] : "NULL")
                 << setw(40) << (row[1] ? row[1] : "NULL")
                 << setw(15) << (row[2] ? row[2] : "NULL")
                 << setw(20) << (row[3] ? row[3] : "NULL")
                 << setw(20) << (row[4] ? row[4] : "NULL")
                 << endl;
                 
        }

        asciiBorder();
    }
    
	

    mysql_free_result(result);

    system("pause");
    system("cls");
    		
	}

    }

	//Decorative purpose - so the code has uniform decoration 
    void asciiBorder() {
        cout << endl;
        for (int i = 0; i < 115; i++) {
            cout << "■";
        }
        cout << endl;
    }
    
    //Rearranging IDs based on date
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

	//displays the current directory
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
    
    event.userOptions();

    // Close the database connection when done
    event.closeDatabase();

    return 0;
}

/*

Query notes for MySQL:

mysql_query(obj, query.c_str()): Executes SQL query.
mysql_store_result(obj): Stores the result of a query (used in SELECT).
mysql_fetch_row(result): Fetches one row at a time from the result set.
mysql_free_result(result): Frees memory after done. - a good practice


SELECT COUNT(*) FROM events                       -- Checks if any data exists in events table.
INSERT INTO events (...) VALUES (...)             -- Adds new event data into the table.
SELECT new_id, title, date, ...                   -- Fetch and display events from the database.
DELETE FROM events WHERE new_id = x               -- Deletes a specific event by ID.
UPDATE events SET new_id = ...                    -- Reassigns IDs based on order (for sorting).
SET @new_id = 0                                   -- Resets the counter used for reassigning new_id.

SELECT * FROM table_name;                         -- Get all rows and columns
SELECT column1, column2 FROM table_name;          -- Get specific columns
SELECT * FROM table_name WHERE condition;         -- Filter data
SELECT * FROM events ORDER BY date DESC;          -- Sort results
SELECT DISTINCT column FROM table_name;           -- Unique values only

INSERT INTO table_name (col1, col2) VALUES ('val1', 'val2');

UPDATE table_name SET column1 = 'value' WHERE condition;

DELETE FROM table_name WHERE condition;

SHOW TABLES;                                     -- List tables in DB
DESCRIBE events;                                 -- Table structure

*/