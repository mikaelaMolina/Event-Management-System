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
#include <fstream> // For file export functionality
#include <ctime> // For date-related functions

/*Event Management/Directory - in progress

1st Week - User options

2nd Week -  View Directory (Think of something else to add) - let's try using database

3rd Week - Delete from the directory

4th Week - Searching

5th Week - Editing

New plans: file export and event statistics

*/

using namespace std;

//these are Database credentials, reminder to open the database access windows + R, cmd then -u root -p, USE eventDirectory (database name)

char HOST[] = "localhost";
char USER[] = "root";
char PASS[] = "";
char DB[] = "eventDirectory";

//Database name is eventDirectory, table name is events

struct Events { //using struct, user-defined structure
    // Vectors for the data
    vector<string> title;
    vector<string> date;
    vector<string> tag;
    vector<string> org;

    MYSQL* obj; // MySQL connection object (pointer)
    
    // Error handling function to centralize error messages
    void displayError(const string& operation) {
        cout << "ERROR: " << operation << endl;
        cout << mysql_error(obj) << endl;
    }

    // Initialize the database connection
    bool initDatabase() {
        // Initialize MySQL object
        obj = mysql_init(0);
        
        if (!obj) {
            cout << "ERROR: MySQL object could not be created." << endl;
            return false;
        }
        
        // Connect to database server: host, user, password, database name, port
        if (!mysql_real_connect(obj, HOST, USER, PASS, DB, 3306, NULL, 0)) {
            displayError("Could not connect to database.");
            return false;
        }
        return true; // Successful connection
    }

    // Close the database connection
    void closeDatabase() {
        if (obj) {
            mysql_close(obj); // Close connection
            cout << "Database connection closed." << endl;
        }
    }

    // Checks if there is data in the database
    bool hasData() {
        // Query to count total records
        if (mysql_query(obj, "SELECT COUNT(*) FROM events")) {
            displayError("Could not execute query.");
            return false;
        }

        MYSQL_RES* result = mysql_store_result(obj);
        
        if (!result) {
            displayError("Could not store result.");
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

    // Display decorative border
    void asciiBorder() {
        cout << endl;
        for (int i = 0; i < 115; i++) {
            cout << "■";
        }
        cout << endl;
    }
    
    // Display application header
    void displayHeader(const string& title) {
        system("cls");
        asciiBorder();
        cout << endl << " " << title << endl;
        asciiBorder();
    }

    // Input validation for integer within range
    int getValidatedInput(int min, int max) {
        int choice;
        while (true) {
            cout << "Enter your choice: ";
            cin >> choice;
            
            if (cin.fail() || choice < min || choice > max) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number between " << min << " and " << max << "." << endl;
            } else {
                break;
            }
        }
        return choice;
    }

    // Escape string to prevent SQL injection
    string escapeString(const string& str) {
        char* escaped = new char[str.length() * 2 + 1];
        mysql_real_escape_string(obj, escaped, str.c_str(), str.length());
        string result(escaped);
        delete[] escaped;
        return result;
    }

    // Rearranging IDs based on date
    void rearrangeIDs() {
        // Reset new_id counter
        if (mysql_query(obj, "SET @new_id = 0")) {
            displayError("Could not reset new_id counter.");
            return;
        }

        // Update new_id column, ordering by latest date first
        string updateQuery = "UPDATE events "
                            "SET new_id = (@new_id := @new_id + 1) "
                            "ORDER BY date DESC";

        if (mysql_query(obj, updateQuery.c_str())) {
            displayError("Could not update IDs.");
        }
    }

    // Displays options for users and prompts them to choose an option
    void userOptions() {
        displayHeader("EVENT MANAGEMENT SYSTEM - MAIN MENU");
        
        if (hasData()) {
            rearrangeIDs(); // Rearrange IDs based on dates
            
            int choice;
            do {
                cout << endl << "There are existing events. What would you like to do?" << endl << endl;
                cout << "1. Add Event" << endl;
                cout << "2. Edit Event" << endl;
                cout << "3. Delete Event" << endl;
                cout << "4. Search Event" << endl;
                cout << "5. View Directory" << endl;
                cout << "6. Export Events" << endl;
                cout << "7. Exit Program" << endl << endl;

                asciiBorder();
                
                choice = getValidatedInput(1, 8);

                switch (choice) {
                    case 1:
                        addData();
                        break;
                    case 2:
                        editData();
                        break;
                    case 3:
                        deleteData();
                        break;
                    case 4:
                        searchData();
                        break;
                    case 5:
                        showDirectory();
                        break;
					case 7:
                        cout << "Exiting program. Thank you for using the Event Management System!" << endl;
                        return;
					case 6:
                        exportEvents();
                        break;
                    
                }
                
                // Only reached if option 8 was not selected
                userOptions();
                return;
                
            } while (choice != 8);
        } else {
            cout << "There is no data in the directory yet." << endl;
            cout << "1. Add Event" << endl;
            cout << "2. Exit Program" << endl;
            
            int choice = getValidatedInput(1, 2);
            
            if (choice == 1) {
                addData();
                userOptions();
            } else {
                cout << "Exiting program. Thank you for using the Event Management System!" << endl;
                return;
            }
        }
    }

    // Adding data into the database using prepared statements to prevent SQL injection
    void addData() {
        displayHeader("ADD NEW EVENT");

        string title, date, tag, org;

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

        cout << "Enter Title: ";
        getline(cin, title);

        cout << "Enter Date (YYYY-MM-DD): ";
        getline(cin, date);

        cout << "Enter Tag: ";
        getline(cin, tag);

        cout << "Enter Organization: ";
        getline(cin, org);

        // Using prepared statement to prevent SQL injection
        MYSQL_STMT *stmt = mysql_stmt_init(obj);
        if (!stmt) {
            displayError("Could not initialize statement");
            system("pause");
            return;
        }

        string query = "INSERT INTO events (title, date, tag, org) VALUES (?, ?, ?, ?)";
        
        if (mysql_stmt_prepare(stmt, query.c_str(), query.length())) {
            displayError("Could not prepare statement");
            mysql_stmt_close(stmt);
            system("pause");
            return;
        }

        // Bind parameters
        MYSQL_BIND bind[4];
        memset(bind, 0, sizeof(bind));

        // Title
        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (void*)title.c_str();
        bind[0].buffer_length = title.length();

        // Date
        bind[1].buffer_type = MYSQL_TYPE_STRING;
        bind[1].buffer = (void*)date.c_str();
        bind[1].buffer_length = date.length();

        // Tag
        bind[2].buffer_type = MYSQL_TYPE_STRING;
        bind[2].buffer = (void*)tag.c_str();
        bind[2].buffer_length = tag.length();

        // Organization
        bind[3].buffer_type = MYSQL_TYPE_STRING;
        bind[3].buffer = (void*)org.c_str();
        bind[3].buffer_length = org.length();

        if (mysql_stmt_bind_param(stmt, bind)) {
            displayError("Could not bind parameters");
            mysql_stmt_close(stmt);
            system("pause");
            return;
        }

        // Execute the statement
        if (mysql_stmt_execute(stmt)) {
            displayError("Could not execute statement");
            mysql_stmt_close(stmt);
            system("pause");
            return;
        }

        cout << "Data added successfully!" << endl;
        mysql_stmt_close(stmt);
        asciiBorder();
        
        system("pause");
    }

    // Editing data with improved UI and validation
    void editData() {
        displayHeader("EDIT EVENT");

        // Display current events to help user choose which one to edit
        MYSQL_RES* result;
        MYSQL_ROW row;

        if (mysql_query(obj, "SELECT new_id, title, date, tag, org FROM events ORDER BY new_id ASC")) {
            displayError("Could not retrieve events.");
            system("pause");
            return;
        }

        result = mysql_store_result(obj);
        vector<int> eventIDs;

        // Display table header with improved formatting
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
            
            cout << left << setw(5) << id
                 << setw(40) << (row[1] ? row[1] : "NULL")
                 << setw(15) << (row[2] ? row[2] : "NULL")
                 << setw(20) << (row[3] ? row[3] : "NULL")
                 << setw(20) << (row[4] ? row[4] : "NULL")
                 << endl;
        }

        asciiBorder();
        mysql_free_result(result);

        // Get event ID to edit with validation
        int eventID;
        cout << "Enter the ID of the event you wish to edit (0 to cancel): ";
        cin >> eventID;

        // Input validation
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Returning to menu..." << endl;
            system("pause");
            return;
        }
        
        // Option to cancel
        if (eventID == 0) {
            cout << "Edit operation cancelled." << endl;
            system("pause");
            return;
        }

        // Check if the entered ID exists
        if (find(eventIDs.begin(), eventIDs.end(), eventID) == eventIDs.end()) {
            cout << "Invalid ID. Returning to menu..." << endl;
            system("pause");
            return;
        }

        // Fetch current values to show the user what they're editing
        string query = "SELECT title, date, tag, org FROM events WHERE new_id = " + to_string(eventID);
        
        if (mysql_query(obj, query.c_str())) {
            displayError("Could not retrieve event data.");
            system("pause");
            return;
        }

        result = mysql_store_result(obj);
        row = mysql_fetch_row(result);
        
        string currentTitle = row[0] ? row[0] : "";
        string currentDate = row[1] ? row[1] : "";
        string currentTag = row[2] ? row[2] : "";
        string currentOrg = row[3] ? row[3] : "";
        
        mysql_free_result(result);

        // Show menu for what to edit
        int choice;
        do {
            system("cls");
            asciiBorder();
            cout << "\nEditing Event #" << eventID << endl;
            cout << "Current Values:" << endl;
            cout << "1. Title: " << currentTitle << endl;
            cout << "2. Date: " << currentDate << endl;
            cout << "3. Tag: " << currentTag << endl;
            cout << "4. Organization: " << currentOrg << endl;
            cout << "5. Return to main menu" << endl;
            asciiBorder();
            
            choice = getValidatedInput(1, 5);

            // Return to main menu if option 5 is selected
            if (choice == 5) {
                return;
            }

            cin.ignore(); // Clear newline character after reading choice
            
            string newValue;
            string columnName;
            
            switch (choice) {
                case 1:
                    columnName = "title";
                    cout << "Enter new title: ";
                    getline(cin, newValue);
                    currentTitle = newValue; // Update current value for display
                    break;
                case 2:
                    columnName = "date";
                    cout << "Enter new date (YYYY-MM-DD): ";
                    getline(cin, newValue);
                    currentDate = newValue;
                    break;
                case 3:
                    columnName = "tag";
                    cout << "Enter new tag: ";
                    getline(cin, newValue);
                    currentTag = newValue;
                    break;
                case 4:
                    columnName = "org";
                    cout << "Enter new organization: ";
                    getline(cin, newValue);
                    currentOrg = newValue;
                    break;
            }
            
            // Check if the user entered anything
            if (newValue.empty()) {
                cout << "No changes made (empty input)." << endl;
                system("pause");
                continue;
            }
            
            // Update using prepared statement
            MYSQL_STMT *stmt = mysql_stmt_init(obj);
            if (!stmt) {
                displayError("Could not initialize statement");
                system("pause");
                continue;
            }

            string updateQuery = "UPDATE events SET " + columnName + " = ? WHERE new_id = ?";
            
            if (mysql_stmt_prepare(stmt, updateQuery.c_str(), updateQuery.length())) {
                displayError("Could not prepare statement");
                mysql_stmt_close(stmt);
                system("pause");
                continue;
            }

            // Bind parameters
            MYSQL_BIND bind[2];
            memset(bind, 0, sizeof(bind));

            // Value parameter
            bind[0].buffer_type = MYSQL_TYPE_STRING;
            bind[0].buffer = (void*)newValue.c_str();
            bind[0].buffer_length = newValue.length();

            // ID parameter
            int id_param = eventID;
            bind[1].buffer_type = MYSQL_TYPE_LONG;
            bind[1].buffer = (void*)&id_param;
            bind[1].is_null = 0;

            if (mysql_stmt_bind_param(stmt, bind)) {
                displayError("Could not bind parameters");
                mysql_stmt_close(stmt);
                system("pause");
                continue;
            }

            // Execute the statement
            if (mysql_stmt_execute(stmt)) {
                displayError("Could not update event");
                mysql_stmt_close(stmt);
                system("pause");
                continue;
            }

            cout << "Event updated successfully!" << endl;
            mysql_stmt_close(stmt);
            
            cout << "Do you want to edit something else for this event? (y/n): ";
            char continueEdit;
            cin >> continueEdit;
            
            if (tolower(continueEdit) != 'y') {
                choice = 5; // Exit the loop
            }
            
        } while (choice != 5);
        
        // Re-arrange IDs based on date after editing
        rearrangeIDs();
    }

    // Deleting data with enhanced user interface
    void deleteData() {
        displayHeader("DELETE EVENT");

        // Display current events
        MYSQL_RES* result;
        MYSQL_ROW row;

        if (mysql_query(obj, "SELECT new_id, title, date, tag, org FROM events ORDER BY new_id ASC")) {
            displayError("Could not execute query.");
            system("pause");
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
            int id = atoi(row[0]);
            eventIDs.push_back(id);
            
            cout << left << setw(5) << id
                 << setw(40) << (row[1] ? row[1] : "NULL")
                 << setw(15) << (row[2] ? row[2] : "NULL")
                 << setw(20) << (row[3] ? row[3] : "NULL")
                 << setw(20) << (row[4] ? row[4] : "NULL")
                 << endl;
        }

        asciiBorder();
        mysql_free_result(result);

        int eventID;
        cout << "Enter the ID of the event you wish to delete (0 to cancel): ";
        cin >> eventID;

        // Input validation
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Returning to menu..." << endl;
            system("pause");
            return;
        }

        // Option to cancel
        if (eventID == 0) {
            cout << "Delete operation cancelled." << endl;
            system("pause");
            return;
        }

        // Check if ID exists
        if (find(eventIDs.begin(), eventIDs.end(), eventID) == eventIDs.end()) {
            cout << "Invalid ID. Returning to menu..." << endl;
            system("pause");
            return;
        }

        cout << "Are you sure you want to delete this event? (y/n): ";
        char confirm;
        cin >> confirm;

        if (tolower(confirm) == 'y') {
            // Use prepared statement for deletion
            MYSQL_STMT *stmt = mysql_stmt_init(obj);
            if (!stmt) {
                displayError("Could not initialize statement");
                system("pause");
                return;
            }

            string query = "DELETE FROM events WHERE new_id = ?";
            
            if (mysql_stmt_prepare(stmt, query.c_str(), query.length())) {
                displayError("Could not prepare statement");
                mysql_stmt_close(stmt);
                system("pause");
                return;
            }

            // Bind parameter
            MYSQL_BIND bind[1];
            memset(bind, 0, sizeof(bind));

            bind[0].buffer_type = MYSQL_TYPE_LONG;
            bind[0].buffer = (void*)&eventID;
            bind[0].is_null = 0;

            if (mysql_stmt_bind_param(stmt, bind)) {
                displayError("Could not bind parameters");
                mysql_stmt_close(stmt);
                system("pause");
                return;
            }

            // Execute the statement
            if (mysql_stmt_execute(stmt)) {
                displayError("Could not delete event");
                mysql_stmt_close(stmt);
                system("pause");
                return;
            }

            cout << "Event deleted successfully!" << endl;
            mysql_stmt_close(stmt);
            
            // Rearrange IDs after deletion
            rearrangeIDs();
        } else {
            cout << "Deletion cancelled." << endl;
        }

        asciiBorder();
        system("pause");
    }

    // Improved searching functionality with unified approach
    void searchData() {
        displayHeader("SEARCH EVENTS");
        
        int searchType;
        cout << "What category would you like to search for?" << endl;
        cout << "1. Title" << endl;
        cout << "2. Tag" << endl;
        cout << "3. Organization" << endl;
        cout << "4. Date Range" << endl;
        
        asciiBorder();
        
        searchType = getValidatedInput(1, 4);
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
        
        string keyword, column;
        bool isDateRange = false;
        string startDate, endDate;
        
        switch(searchType) {
            case 1:
                cout << "Search Events by Title" << endl;
                column = "title";
                break;
            case 2:
                cout << "Search Events by Tag" << endl;
                column = "tag";
                break;
            case 3:
                cout << "Search Events by Organization" << endl;
                column = "org";
                break;
            case 4:
                cout << "Search Events by Date Range" << endl;
                isDateRange = true;
                break;
        }
        
        asciiBorder();
        
        if (isDateRange) {
            cout << "Enter start date (YYYY-MM-DD): ";
            getline(cin, startDate);
            
            cout << "Enter end date (YYYY-MM-DD): ";
            getline(cin, endDate);
            
            if (startDate.empty() || endDate.empty()) {
                cout << "Date range cannot be empty. Returning to menu..." << endl;
                system("pause");
                return;
            }
        } else {
            cout << "Enter keyword to search in " << column << ": ";
            getline(cin, keyword);
            
            if (keyword.empty()) {
                cout << "No keyword entered. Returning to menu..." << endl;
                system("pause");
                return;
            }
        }
        
        string query;
        if (isDateRange) {
            query = "SELECT new_id, title, date, tag, org FROM events "
                    "WHERE date BETWEEN '" + startDate + "' AND '" + endDate + "' "
                    "ORDER BY date ASC";
        } else {
            query = "SELECT new_id, title, date, tag, org FROM events "
                    "WHERE " + column + " LIKE '%" + escapeString(keyword) + "%' "
                    "ORDER BY new_id ASC";
        }
        
        if (mysql_query(obj, query.c_str())) {
            displayError("Could not execute search query.");
            system("pause");
            return;
        }
        
        MYSQL_RES* result = mysql_store_result(obj);
        MYSQL_ROW row;
        
        int numRows = mysql_num_rows(result);
        
        if (numRows == 0) {
            if (isDateRange) {
                cout << "\nNo events found between " << startDate << " and " << endDate << endl;
            } else {
                cout << "\nNo events found with keyword: \"" << keyword << "\"" << endl;
            }
        } else {
            if (isDateRange) {
                cout << "\nFound " << numRows << " event(s) between " << startDate << " and " << endDate << ":" << endl;
            } else {
                cout << "\nFound " << numRows << " event(s) matching \"" << keyword << "\":" << endl;
            }
            
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
    }

    // Display the full event directory with enhanced formatting
    void showDirectory() {
        displayHeader("EVENT DIRECTORY");

        MYSQL_RES* result;
        MYSQL_ROW row;

        // Fetch events sorted by new_id
        string query = "SELECT new_id, title, date, org, tag FROM events ORDER BY new_id ASC";

        if (mysql_query(obj, query.c_str())) {
            displayError("Could not execute query.");
            system("pause");
            return;
        }

        result = mysql_store_result(obj);

        cout << left << setw(5) << "No."
             << setw(40) << "T I T L E"
             << setw(15) << "D A T E"
             << setw(25) << "O R G A N I Z A T I O N"
             << setw(25) << "T A G"
             << endl;
        
        asciiBorder();

        while ((row = mysql_fetch_row(result))) {
            cout << left << setw(5) << (row[0] ? row[0] : "NULL")
                 << setw(40) << (row[1] ? row[1] : "NULL")
                 << setw(15) << (row[2] ? row[2] : "NULL")
                 << setw(25) << (row[3] ? row[3] : "NULL")
                 << setw(25) << (row[4] ? row[4] : "NULL")
                 << endl;
        }

        asciiBorder();
        mysql_free_result(result);
        
        system("pause");
    }
    
    void exportEvents() {
        displayHeader("EXPORT EVENTS");
        
        MYSQL_RES* result;
        MYSQL_ROW row;
        
        // Get all events
        if (mysql_query(obj, "SELECT new_id, title, date, tag, org FROM events ORDER BY date DESC")) {
            displayError("Could not retrieve events for export.");
            system("pause");
            return;
        }
        
        result = mysql_store_result(obj);
        
        cout << "Select export format:" << endl;
        cout << "1. CSV Format" << endl;
        cout << "2. Text Format" << endl;
        
        int format = getValidatedInput(1, 2);
        
        cin.ignore();
        string filename;
        cout << "Enter filename to save (without extension): ";
        getline(cin, filename);
        
        if (filename.empty()) {
            cout << "Invalid filename. Export cancelled." << endl;
            mysql_free_result(result);
            system("pause");
            return;
        }
        
        string extension = (format == 1) ? ".csv" : ".txt";
        ofstream outFile(filename + extension);
        
        if (!outFile) {
            cout << "Error: Could not create file." << endl;
            mysql_free_result(result);
            system("pause");
            return;
        }
        
        // Write header
        if (format == 1) {
            outFile << "ID,Title,Date,Tag,Organization" << endl;
        } else {
            outFile << "EVENT DIRECTORY EXPORT" << endl;
            outFile << "======================" << endl << endl;
            outFile << "Generated on: " << __DATE__ << endl << endl;
        }
        
        // Write data
        while ((row = mysql_fetch_row(result))) {
            if (format == 1) {
                // CSV format with quote handling
                outFile << row[0] << ",\"" 
                       << (row[1] ? row[1] : "") << "\",\"" 
                       << (row[2] ? row[2] : "") << "\",\"" 
                       << (row[3] ? row[3] : "") << "\",\"" 
                       << (row[4] ? row[4] : "") << "\"" << endl;
            } else {
                // Text format
                outFile << "ID: " << row[0] << endl;
                outFile << "Title: " << (row[1] ? row[1] : "N/A") << endl;
                outFile << "Date: " << (row[2] ? row[2] : "N/A") << endl;
                outFile << "Tag: " << (row[3] ? row[3] : "N/A") << endl;
                outFile << "Organization: " << (row[4] ? row[4] : "N/A") << endl;
                outFile << "------------------------------" << endl;
            }
        }
        
        outFile.close();
        mysql_free_result(result);
        
        cout << "Export completed successfully to " << filename << extension << endl;
        system("pause");
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

void editData() { asciiBorder(); cout << endl << "Editing Data" << endl; asciiBorder();

int eventID;
cout << "Enter the ID of the event you wish to edit: ";
cin >> eventID;
cin.ignore(); // Clear newline character

string query = "SELECT * FROM events WHERE new_id = " + to_string(eventID);
if (mysql_query(obj, query.c_str())) {
    cout << "ERROR: Could not retrieve event." << endl;
    cout << mysql_error(obj) << endl;
    return;
}

MYSQL_RES* result = mysql_store_result(obj);
if (!result || mysql_num_rows(result) == 0) {
    cout << "No event found with ID: " << eventID << endl;
    mysql_free_result(result);
    return;
}
mysql_free_result(result);

int choice;
cout << "What data would you like to edit?" << endl;
cout << "1. Title" << endl;
cout << "2. Date" << endl;
cout << "3. Tag" << endl;
cout << "4. Organization" << endl;
cout << "Enter your choice: ";
cin >> choice;
cin.ignore();

string newValue;
cout << "Enter new value: ";
getline(cin, newValue);

string column;
switch (choice) {
    case 1: column = "title"; break;
    case 2: column = "date"; break;
    case 3: column = "tag"; break;
    case 4: column = "org"; break;
    default: cout << "Invalid choice." << endl; return;
}

query = "UPDATE events SET " + column + " = '" + newValue + "' WHERE new_id = " + to_string(eventID);
if (mysql_query(obj, query.c_str())) {
    cout << "ERROR: Could not update event." << endl;
    cout << mysql_error(obj) << endl;
} else {
    cout << "Event updated successfully!" << endl;
}

asciiBorder();
system("pause");
system("cls");

}


*/