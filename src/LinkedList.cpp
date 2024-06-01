#ifndef LINKED_LIST
#define LINKED_LIST

template <typename T>
class Node {
public:
    T data;
    Node* next;

    Node(T value) : data(value), next(nullptr) {}
};

template <typename T>
class LinkedList {
private:
    Node<T*>* head;

public:
    LinkedList() : head(nullptr) {}

    ~LinkedList() {
        clear();
    }

    void append(T* value) {
        Node<T*>* newNode = new Node<T*>(value);
        if (!head) {
            head = newNode;
        } else {
            Node<T*>* temp = head;
            while (temp->next) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    void prepend(T* value) {
        Node<T*>* newNode = new Node<T*>(value);
        newNode->next = head;
        head = newNode;
    }

    bool remove(T* value) {
        if (!head) return false;
        if (head->data == value) {
            Node<T*>* toDelete = head;
            head = head->next;
            delete toDelete;
            return true;
        }

        Node<T*>* temp = head;
        while (temp->next && temp->next->data != value) {
            temp = temp->next;
        }

        if (temp->next) {
            Node<T*>* toDelete = temp->next;
            temp->next = temp->next->next;
            delete toDelete;
            return true;
        }
        return false;
    }


    void clear() {
        while (head) {
            Node<T*>* toDelete = head;
            head = head->next;
            delete toDelete;
        }
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    class Iterator {
    private:
        Node<T*>* current;

    public:
        Iterator(Node<T*>* node) : current(node) {}

        T*& operator*() const {
            return current->data;
        }

        Iterator& operator++() {
            current = current->next;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
    };

    Iterator begin() const {
        return Iterator(head);
    }

    Iterator end() const {
        return Iterator(nullptr);
    }
};
#endif /* LINKED_LIST */
