#include <iostream>
#include <vector>
#include <string>
#include <iomanip>  // For setw()
#include <algorithm>  // For transform
#include <cctype>     // For tolower

using namespace std;

/*Event Management/Directory - in progress

1st Week - User options

*/

struct Events{
	
	vector<string> title;
	vector<string> date;
	vector<string> tag;
	vector<string> org;
	
	bool hasData() { //function returns true when the vectors are not empty, meaning there are existing book entries
    return !title.empty() && !date.empty() && !tag.empty() && !org.empty();
}
	
	void userOptions(){
		
		//since I have not yet coded the inside of each option I will comment the if-else statement needed for the options as of now
		
		//if (hasData()){
			
			int n;
			
			do {
				
            cout << endl << endl << "There are existing events, what would you like to do with it?" << endl << endl;
            cout << "1. Add" << endl;
            cout << "2. Edit" << endl;
            cout << "3. Delete" << endl;
            cout << "4. Search" << endl;
            cout << "5. Directory" << endl << endl;
            cout << "Please enter desired number: ";
            cin >> n;

            if (cin.fail() || n < 1 || n > 5) {
                cin.clear();  // Clear error flag
                cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
                cout << "Invalid Input. Please enter a number between 1 and 5." << endl;
            }
        } while (n < 1 || n > 5);
        
        switch (n){
        	
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
		
		
		/*} else{
			
			cout << "There is no data in the directory yet.";
			
			addData();
		}*/
		

	
	}
	
	void addData(){
		
		cout << endl << "Adding Data" << endl;
		
		//put code here for adding data
		
	}
	
	void editData(){
		
		cout << endl << "Editing Data" << endl;
		
		//put code here for editing the data
		
	}
	
	void deleteData(){
		
		cout << endl << "Deleting Data" << endl;
		
		//put code here for deleting data
		
	}
	
	void searchData(){
		
		cout << endl << "Searching Data" << endl;
		
		//put code here for searching
		
	}
		
	void showDirectory(){
		
		cout << endl << "Show Data" << endl;
		
		//put code here for showing the directory
		
	}
};



int main(){
	
	int n;
	
	cout << "Event Management/Directory" << endl;
	
	Events event;
	
	event.userOptions();
	
	return 0;
	
	
}