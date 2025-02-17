#include <iostream>

using namespace std;

//Event Management/Directory

struct Events{
	
	vector<string> title;
	vector<string> date;
	vector<string> tag;
	vector<string> org;
	
	bool hasData() { //function returns true when the vectors are not empty, meaning there are existing book entries
    return !title.empty() && !date.empty() && !tag.empty() && !org.empty();
}
	
	void showDirectory(){
		
	}
	
	
	void userOptions(){
		
		if hasData(){
			
			int n;
			
			do {
            cout << endl << endl << "There are existing events, what would you like to do with it?" << endl << endl;
            cout << "1. Search" << endl;
            cout << "2. Add" << endl;
            cout << "3. Delete" << endl;
            cout << "4. Data List" << endl << endl;
            cout << "5. Edit" << endl << endl;
            cout << "Please enter desired number: ";
            cin >> n;

            if (cin.fail() || n < 1 || n > 4) {
                cin.clear();  // Clear error flag
                cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
                cout << "Invalid Input. Please enter a number between 1 and 4." << endl;
            }
        } while (n < 1 || n > 4);
		}

	
	}
}



int main(){
	
	cout << "Event Management/Directory" << endl;
	
	
	
	
}