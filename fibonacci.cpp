#include <iostream>
#include <vector>
#include <pthread.h>
#include <cstdlib> // for fun ; random term to calculate gen
#include <unistd.h> // For sleep function
using namespace std;

// Global vector and pThread synchronization variables
vector<int> fibo;
pthread_mutex_t fib_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
bool done = false; // signal for parent to indicate when the child thread is finished

// Function to calculate Fibonacci sequence (Child Thread)
void* calculate_fibonacci(void* arg) {
    int n = *(int*)arg; // Number of Fibonacci terms to compute
    int a = 0, b = 1;
    
    if(n == 1){
        fibo.push_back(a);
        done = true;
        pthread_cond_signal(&cond); 
    
        return nullptr;
    }else if(n == 2){
        fibo.push_back(a);
        fibo.push_back(b);
        done = true;
        pthread_cond_signal(&cond);
    
        return nullptr;
    }
    fibo.push_back(a);
    fibo.push_back(b);

    // calculating sequence to specified 'n' digits
    for (int i = 1; i <= n-2; i++) {
        int c = fibo[i] + fibo[i - 1];
        fibo.push_back(c);
    }



    // handling "thinking" times because it's fun
    if(n > 4 && n < 11){
        sleep(1);
    }else if(n > 10){
        sleep(2);
    }

    cout << "Calculation completed. " << endl;
    sleep(1);

    done = true; // Mark as done
    pthread_cond_signal(&cond); // Signal the parent thread

    return nullptr;
}

// Function for the parent thread to print the Fibonacci sequence
void* print_fibonacci(void* arg) {
    while (!done) { // Wait until the child thread finishes
        pthread_cond_wait(&cond, &fib_mutex);
    }
    
    // Print the computed Fibonacci sequence
    cout << "Fibonacci Sequence: ";
    for (int n : fibo)
        cout << n << ", ";
    cout << "..." << endl;

    
    return nullptr;
}

int main() {
    pthread_t child, parent;
    int n = 0; // Number of Fibonacci terms
    cout << "Thank you for your interest in the Fibonacci sequenece!\nHow many terms would you like to calculate?\n";
    cout << "Please enter number of terms: ";
    int count = 1;
    int countLarge = 1;
    cin >> n;
    while(n == 0 || n > 47){
        /*
        I realized pretty late a lot of this could be implemented with 
        switch statements but I did this on a whim and I'm
        too lazy to optimize it because there's two other parts
        I need to work on!
        Sorry :/
        */
        if(n > 47 && countLarge == 1){
            cout << "Too large. Enter a smaller number: ";
            cin >> n;
            countLarge++;
            continue;
        }else if(n > 47 && countLarge == 2){
            cout << "I'll give you the benefit of the doubt here. Pick a number under 47: ";
            cin >> n;
            countLarge++;
            continue;
        }else if(n > 47 && countLarge == 3){
            cout << "Stop trying to break me. I'm picking for you now." << endl;
            srand (time(NULL));
            n = rand() % 20 + 1; 

            count = 36; //arbitrary number for handling.
            break;
        }
        if(count == 2){
            cout << "I don't know how I'm supposed to calculate zero terms. Try again: ";
            cin >> n;
            count++;
            continue;
        }else if(count == 3){
            cout << "Stop playing games. Just enter a number larger than 0: ";
            cin >> n;
            count++;
            continue;
        }else if(count == 4){
            cout << "I mean it. Just enter a number larger than 0: ";
            cin >> n;
            count++;
            continue;
        }else if(count == 5){
            cout << "I'm tired of this. I'm picking for you." << endl;
            // generating random number between 1 & 20
            srand (time(NULL));
            n = rand() % 20 + 1; 

            count = 36; // arbitrary number for handling.
            break;
        }

        if(n == 0){
            cout << "How am I supposed to calculate zero terms? Try again: ";
            count++;
            cin >> n;
            continue;
        }
        break;
    }
    // handling bad inputs
    if(count < 5 && count > 1)
        cout << "Finally. Thank you! Now calculating " << n << " terms!" << endl;
    else if(count == 36)
        cout << "Now calculating: " << n << " terms." << endl;
    else
        cout << "Thank you! Now calculating " << n << " terms." << endl;

    // Create child thread to compute Fibonacci numbers
    if (pthread_create(&child, nullptr, calculate_fibonacci, &n)) {
        cerr << "Error creating child thread" << endl;
        return 1;
    }

    // Create parent thread to print the Fibonacci sequence
    if (pthread_create(&parent, nullptr, print_fibonacci, nullptr)) {
        cerr << "Error creating parent thread" << endl;
        return 1;
    }

    // Wait for both threads to complete
    pthread_join(child, nullptr);
    pthread_join(parent, nullptr);

    // Cleanup
    pthread_mutex_destroy(&fib_mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
