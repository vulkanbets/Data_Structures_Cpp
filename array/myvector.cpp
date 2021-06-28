#ifndef MYVECTOR_H
#define MYVECTOR_H
#include <memory>
#include <iostream>


template <class T>
class myvector
{
    private:
        // arr is the integer pointer
        // which stores the address of our vector
        std::unique_ptr<T[]> arr;
        // capacity is the total storage
        // capacity of the vector
        int capacity;
        // current is the number of elements
        // currently present in the vector
        int current;
    public:
        myvector()
        {
            arr = std::make_unique<T[]>(1);
            capacity = 1;
            current = 0;
        }
        // Function to add an element at the last
        void push(T data)
        {
            std::cout << "\"capacity\" Variable == " << capacity << std::endl;
            std::cout << "\"current\"  Variable == " << current << std::endl;
            // if the number of elements is equal to the
            // capacity, that means we don't have space to
            // accommodate more elements. We need to double the
            // capacity
            if (current == capacity)
            {
                std::shared_ptr<T[]> temp = std::make_shared<T[]>(2 * capacity);
                // // copying old array elements to new array
                // for (int i = 0; i < capacity; i++)
                // {
                //     temp[i] = arr[i];
                // }
                // // deleting previous array
                // delete[] arr;
                // capacity *= 2;
                // arr = temp;




            }
    
            // Inserting data
            arr[current] = data;
            current++;
        }
    
        // // function to add element at any index
        // void push(int data, int index)
        // {
    
        //     // if index is equal to capacity then this
        //     // function is same as push defined above
        //     if (index == capacity)
        //         push(data);
        //     else
        //         arr[index] = data;
        // }
    
        // // function to extract element at any index
        // T get(int index)
        // {
        //     // if index is within the range
        //     if (index < current)
        //         return arr[index];
        // }
    
        // // function to delete last element
        // void pop() { current--; }
    
        // // function to get size of the vector
        // int size() { return current; }
    
        // // function to get capacity of the vector
        // int getcapacity() { return capacity; }
    
        // // function to print array elements
        // void print()
        // {
        //     for (int i = 0; i < current; i++)
        //     {
        //         std::cout << arr[i] << " ";
        //     }
        //     std::cout << std::endl;
        // }
};

#endif
