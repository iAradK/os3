#ifndef THREAD_SAFE_LIST_H_
#define THREAD_SAFE_LIST_H_

#include <pthread.h>
// #include <mutex>
#include <iostream>
#include <iomanip> // std::setw
using namespace std;

template <typename T>
class List 
{
    public:
        /**
         * Constructor
         */
        List() {
            this->size = 0;
            this->head = nullptr;
        }

        /**
         * Destructor
         */
        ~List() {
            // this->size = 0;
            Node* cur = head;
            while (cur != nullptr) {
                Node* tmp = cur;
                cur = cur->next;
                delete tmp;
            }
        }

        class Node {
         public:
          T data;
          Node *next;
          pthread_mutex_t mtx;

          Node(T _data) {
              data = _data;
              next = nullptr;
          };

          void lock() {
              pthread_mutex_lock(&mtx);
          }

          void unlock() {
              pthread_mutex_unlock(&mtx);
          }
        };

        Node* findPlace2(const T& data) {
            Node* cur = head;
            if (cur == nullptr) return  cur;
            cur->lock();

            while (cur->next != nullptr && cur->next->data != data) {
                cur->unlock();
                cur = cur->next;
                cur->lock();
            }

            if (cur->next == nullptr) { // got to end of list
                cur->unlock();
            }
            else if (cur->next->data == data) {
                if (cur->next->next != nullptr) {
                    cur->next->lock();
                }
            }

            return cur;
        }

        Node* findPlace(const T& data) {
            Node* cur = head;
            cur->lock();

            while (cur->next != nullptr && cur->next->data < data) {
                cur->next->lock();
                // cur->next == nullptr ||
                if (cur->next->data >= data) {
                    return cur; // cur and cur->next are locked
                }

                cur->unlock();
                cur = cur->next; // Was locked already
            }

            if (cur->next == nullptr) { // got to end of list
                cur->unlock();
                return cur;
            }

            return cur;
        }

        /**
         * Insert new node to list while keeping the list ordered in an ascending order
         * If there is already a node has the same data as @param data then return false (without adding it again)
         * @param data the new data to be added to the list
         * @return true if a new node was added and false otherwise
         */
        bool insert(const T& data) {
            Node* toAdd = new Node(data);
            if (head == nullptr) { // first node
                head = toAdd;
                return true;
            }
            if (head->data > data) { // new node is the first
                Node* tmp = head;
                toAdd->next = tmp;
                head = toAdd;
                return true;
            }

            Node* prev = findPlace(data);
            if (prev->next != nullptr && prev->next->data == data) { // Already exists
                prev->unlock();
                prev->next->unlock();
                delete toAdd;
                return false;
            }

            toAdd->next = prev->next;
            prev->next = toAdd;
            prev->unlock();
            toAdd->next->unlock();
            return true;
        }

        void myPrint() {
            Node* cur = head;
            while (cur != nullptr) {
                cout << "num: " << cur->data << " ";
                cur = cur->next;
            }
            cout << "\n";
        }

        /**
         * Remove the node that its data equals to @param value
         * @param value the data to lookup a node that has the same data to be removed
         * @return true if a matched node was found and removed and false otherwise
         */
        bool remove(const T& value) {
            if (head->data == value) { // delete head
                Node* node = head;
                head = head->next;
                delete node;
                return true;
            }

            Node* prev = findPlace2(value);
            if (prev == nullptr) return false;
            Node* toDelete = prev->next;
            prev->next = toDelete->next;
            prev->unlock();
            if (prev->next != nullptr) prev->next->unlock();
            delete toDelete;
            return true;
        }

        /**
         * Returns the current size of the list
         * @return current size of the list
         */
        unsigned int getSize() {
            return this->size;
        }

		// Don't remove
        void print() {
          Node* temp = head;
          if (temp == NULL)
          {
            cout << "";
          }
          else if (temp->next == NULL)
          {
            cout << temp->data;
          }
          else
          {
            while (temp != NULL)
            {
              cout << right << setw(3) << temp->data;
              temp = temp->next;
              cout << " ";
            }
          }
          cout << endl;
        }

		// Don't remove
        virtual void __insert_test_hook() {}
		// Don't remove
        virtual void __remove_test_hook() {}

    private:
        Node* head;
        int size;
    // TODO: Add your own methods and data members
};

#endif //THREAD_SAFE_LIST_H_