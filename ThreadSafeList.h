#ifndef THREAD_SAFE_LIST_H_
#define THREAD_SAFE_LIST_H_

#define DEMVAL 0

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
            this->demi_head = new Node(DEMVAL);
            this->head = nullptr;
            this->demi_head->next = head;
            pthread_mutex_init(&this->size_mtx, NULL);
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
            delete demi_head;
        }

        class Node {
         public:
          T data;
          Node *next;
          pthread_mutex_t mtx;

            Node(T _data) {
                pthread_mutex_init(&this->mtx, NULL);
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
                // __remove_test_hook();
                cur->unlock();
                cur = cur->next;
                cur->lock();
            }

            if (cur->next == nullptr) { // got to end of list
                // __remove_test_hook();
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

                // __insert_test_hook();
                cur->unlock();
                cur = cur->next; // Was locked already
            }

            if (cur->next == nullptr) { // got to end of list
                __insert_test_hook();
                cur->unlock();
                return cur;
            }

            return cur;
        }

        void chnge_size(bool dec) {
            pthread_mutex_lock(&this->size_mtx);
            if (dec == false) { // Increase size
                this->size++;
            }
            else this->size--; // Dec size
            pthread_mutex_unlock(&this->size_mtx);
        }

        /**
         * Insert new node to list while keeping the list ordered in an ascending order
         * If there is already a node has the same data as @param data then return false (without adding it again)
         * @param data the new data to be added to the list
         * @return true if a new node was added and false otherwise
         */
        bool insert(const T& data) {
            Node* toAdd = new Node(data);
            Node* cur = demi_head;

            cur->lock();
            while (cur->next != nullptr) {
                cur->next->lock();
                if (cur->next->data >= data) break;
                // Node* prev = cur;
                cur->unlock();
                cur = cur->next;
            }

            if (cur->next != nullptr && cur->next->data == data) { // Already exists
                cur->unlock();
                if (cur->next != NULL) cur->next->unlock();
                delete toAdd;
                return false;
            }

            toAdd->next = cur->next;
            cur->next = toAdd;
            chnge_size(0);
            if (cur == demi_head) head = toAdd;
            __insert_test_hook();
            cur->unlock();
            if (toAdd->next != NULL) toAdd->next->unlock();
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
            Node* cur = demi_head;
            cur->lock();

            while (cur->next != nullptr) {
                cur->next->lock();
                if (cur->next->data == value) break;
                cur->unlock();
                cur = cur->next;
            }

            if (cur->next == nullptr) {
                cur->unlock();
                return false;
            }

            Node* toDelete = cur->next;
            cur->next = toDelete->next;
            if (cur == demi_head) head = cur->next; // If we deleted the head
            chnge_size(1);
            __remove_test_hook();
            cur->unlock();
            if (cur->next != nullptr) cur->next->unlock();
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

    pthread_mutex_t dummy_mutex;

    /// for testing only  // TODO: add this func to "ThreadSafeList.h" and make adjustments before the test, don't forget to remove before submit
    bool isSorted(){
        pthread_mutex_lock(&dummy_mutex);
        if(!head) {
            pthread_mutex_unlock(&dummy_mutex);
            return true;
        }else{
            pthread_mutex_lock(&head->mtx);
            pthread_mutex_unlock(&dummy_mutex);
        }
        Node* prev = head;
        Node* curr = head->next;
        while(curr) {
            pthread_mutex_lock(&curr->mtx);
            if(prev->data >= curr->data) {
                pthread_mutex_unlock(&curr->mtx);
                pthread_mutex_unlock(&prev->mtx);
                return false;
            }
            pthread_mutex_unlock(&prev->mtx);
            prev = curr;
            curr = curr->next;
        }
        pthread_mutex_unlock(&prev->mtx);
        return true;
    }

    private:
        Node* demi_head;
        Node* head;
        int size;
        pthread_mutex_t size_mtx;

    // TODO: Add your own methods and data members
};

#endif //THREAD_SAFE_LIST_H_