#include <algorithm>
#include <iostream>
#include <functional>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

using namespace std;



class Book { // creating a book class with 3 data members, ISBN,language, and type
  public:
    Book(string ISNB_input, string language, string type) : language(language), type(type) {
      convert_ISNB(ISNB_input);
    };

    void convert_ISNB(string ISNB_input) {
      int temp = stoi(ISNB_input);
      ISNB = temp;
    }

    int get_ISNB() {return ISNB;}
    string get_language() {return language;}
    string get_type() {return type;}

  private:
    int ISNB;
    string language;
    string type;
};



class Clock {
  public:
    Clock() : result_time(0) {};

    void start_time() {
      start = chrono::high_resolution_clock::now();
    }

    void end_time() {
      end = chrono::high_resolution_clock::now();
    }

    void calculate_time() {
      result_time = chrono::duration<double, std::micro>(end-start).count();
    }

    double get_time() {
      return result_time;
    }


  private:
    chrono::time_point <chrono::high_resolution_clock> start;
    chrono::time_point <chrono::high_resolution_clock> end;
    double result_time;

};



int create_vector(vector<Book>& input_vector, ifstream& file_to_be_read) {
  if (file_to_be_read) {
    string input;
    while(file_to_be_read >> input) { // read a single line of input
      string ISNB;
      string language;
      string type;
      int comma = 0;
      int length_of_input = input.size();
      for (int i = 0; i < length_of_input; i++) { // deconstruct that line into 3 data members
        if (comma == 0) {
          if (input[i] == ',') {
            comma+=1;
            continue;
          }
          ISNB+=input[i];  // fill in isbn
        } else if (comma == 1) {
          if (input[i] == ',') {
            comma+=1;
            continue;
          }
          language+=input[i]; // fill in language
        } else {
          type+=input[i];  // fill in type
        }
    }
    Book new_book(ISNB, language, type); // create a book with these data members
    input_vector.push_back(new_book); // add this book to the new books vector
    }
    return 0;
  } else {
    cerr << "Error: cannot open file ";
    return 1;
  }
}



bool ISNB_compare(Book left, Book right) {  // custom operator to order books in terms of ISBN, lowest to greatest
  if (left.get_ISNB() > right.get_ISNB()) {
    return true;
  } else if (left.get_ISNB() == right.get_ISNB()) {
    if ((left.get_type() == "new" && right.get_type() != "new") || 
    (left.get_type() == "used" && right.get_type() == "digital")) {
      return true;
    } else if (left.get_type() == right.get_type()) {
      if (left.get_language()[0] > right.get_language()[0]) {
        return true;
      }
    }
  }
  return false;
}



bool linearsearch(Book book, vector<Book> search_vector) {
  int length_of_vector = search_vector.size();
  for (int i = 0; i < length_of_vector; i++) {
    if (book.get_ISNB() == search_vector[i].get_ISNB() && 
        book.get_language() == search_vector[i].get_language() &&
        book.get_type() == search_vector[i].get_type()) {
          return true;
        }
  }
  return false;
}



bool binarysearch(Book book, vector<Book> search_vector) {
  int left = 0;
  int right = search_vector.size()-1;
  
  while (left < right) {
      int middle = (left + right) / 2;
      if (book.get_ISNB() > search_vector[middle].get_ISNB()) {
        left = middle + 1;
      }  else {
        right = middle;
      }
      if ((book.get_ISNB() == search_vector[left].get_ISNB()) && 
        (book.get_language() == search_vector[left].get_language()) &&
        (book.get_type() == search_vector[left].get_type())) {
          return true;
      }
  }
  return false;
}



int main(int argc, char** argv) {

  if (argc <= 2) {
    cerr << "Error: not enough arguments provided" << endl;
    return 0;
  } else if (argc >= 5) {
    cerr << "Error: too many arguments provided" << endl;
    return 0;
  }

  vector<Book> new_books_vector;
  vector<Book> requested_books_vector;
  string search_choice;
  int temp;

  ifstream new_books_file (argv[1]); // reading from new books file
  temp = create_vector(new_books_vector, new_books_file);
  if (temp == 1) {
    cerr << argv[1] << endl;
    exit(0);
  }
  new_books_file.close();


  ifstream requested_books_file (argv[2]); // reading from requested books file
  temp = create_vector(requested_books_vector, requested_books_file);
  if (temp == 1) {
    cerr << argv[2] << endl;
    exit(0);
  }
  requested_books_file.close();


  cout << "Choice of search method ([l]inear, [b]inary)?" << endl;
  while (cin >> search_choice) {
      if ((search_choice == "l" || search_choice == "b") && search_choice.size() < 2) { // configuring input to make sure only l or b is chosen
      break;
    } else {
      cout << "Incorrect Choice" << endl; 
      continue;
    }
  }

  Clock time; // create a clock to determine time
  int count = 0; // find the amount of occurances 
  if (search_choice == "l") {
    time.start_time();
    int vector_size = requested_books_vector.size();
    for (int i = 0; i < vector_size; i++) {
      if(linearsearch(requested_books_vector[i], new_books_vector)) { // run linear search 
        count+=1;
      }
    }
  } else if (search_choice == "b") {
    time.start_time();
    sort(new_books_vector.begin(), new_books_vector.end(), ISNB_compare); // sort new books vector
    int vector_size = requested_books_vector.size();
    for (int i = 0; i < vector_size; i++) {
      if(binarysearch(requested_books_vector[i], new_books_vector)) { // run binary search
        count+=1;
      }
    }
  }

  time.end_time();
  time.calculate_time();
  cout << "CPU time: " << time.get_time() << " microseconds" << endl; // print out result
  

  ofstream output_file(argv[3]);
  output_file << count << endl;  // writing to the 3rd file
  output_file.close();

  return 0;
}
