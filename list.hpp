#ifndef LIST_HPP
#define LIST_HPP

#include <iterator>
#include <memory>

template <class T>
class List {
private:
    struct Element;  // forward-declaration
    size_t size = 0; // size of the list
public:
    List() = default; 

    class forward_iterator {
    public:
        using value_type = T;
        using reference = value_type &;
        using pointer = value_type *;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        
        forward_iterator(Element *ptr);
        T &operator*();
        forward_iterator &operator++();
        forward_iterator operator++(int);
        bool operator==(const forward_iterator &other) const;
        bool operator!=(const forward_iterator &other) const;

    private:
        Element *it_ptr;
        friend List;
    };

    forward_iterator begin();
    forward_iterator end();
    void PushBack(const T &value); // Add element at the beginning of the list
    void PushFront(const T &value); // Add element to the end of the list
    T &Front(); //Get element from the beginning of the list
    T &Back(); //Get the element from the end of the list
    void PopBack(); //Remove element from the end of the list
    void PopFront(); //Remove element from the beginning of the list
    size_t Length(); //Get size of the list
    bool Empty(); //Check emptiness of the list
    void EraseByIterator(forward_iterator d_it); //Remove element by iterator
    void EraseByNumber(size_t N); //Remove element by number
    void InsertByIterator(forward_iterator ins_it, T &value); //Add element by iterator
    void InsertByNumber(size_t N, T &value); //Add element by number
    List &operator=(const List &other);
    T &operator[](size_t index);

private:
    struct Element {
        T value;
        std::unique_ptr<Element> next_element;
        Element *prev_element = nullptr;
        Element(const T &value_) : value(value_) {}
        forward_iterator Next();
    };

    std::unique_ptr<Element> head;
    Element *tail = nullptr;
};

template <class T>
typename List<T>::forward_iterator List<T>::begin() {
    return forward_iterator(head.get());
}

template <class T>
typename List<T>::forward_iterator List<T>::end() {
    return forward_iterator(nullptr);
}

template <class T>
size_t List<T>::Length() {
    return size;
}

template <class T>
bool List<T>::Empty() {
    return Length() == 0;
}

template <class T>
void List<T>::PushBack(const T &value) {
    if (!size) {
        head = std::make_unique<Element>(value);
        tail = head.get();
        size++;
        return;
    }

    tail->next_element = std::make_unique<Element>(value);
    Element *temp = tail; 
    tail = tail->next_element.get();
    tail->prev_element = temp; 

    size++;
}

template <class T>
void List<T>::PushFront(const T &value) {
    size++;

    std::unique_ptr<Element> tmp = std::move(head);
    head = std::make_unique<Element>(value);
    head->next_element = std::move(tmp);

    if (head->next_element != nullptr)
        head->next_element->prev_element = head.get();
        
    if (size == 1) {
        tail = head.get();
    }
    if (size == 2) {
        tail = head->next_element.get();
    }
}

template <class T>
void List<T>::PopFront() {
    if (size == 1) {
        head = nullptr;
        tail = nullptr;
        size--;
        return;
    }
    head = std::move(head->next_element);
    head->prev_element = nullptr;
    size--;
}

template <class T>
void List<T>::PopBack() {
    if (tail->prev_element) {
        Element *tmp = tail->prev_element;
        tail->prev_element->next_element = nullptr;
        tail = tmp;
    }
    else {
        head = nullptr;
        tail = nullptr;
    }
    size--;
}

template <class T>
T &List<T>::Front() {
    if (size == 0) {
        throw std::logic_error("error: list is empty");
    }
    return head->value;
}

template <class T>
T &List<T>::Back() {
    if (size == 0) {
        throw std::logic_error("error: list is empty");
    }
    forward_iterator i = this->begin();
    while (i.it_ptr->Next() != this->end()) {
        i++;
    }
    return *i;
}

template <class T>
List<T> &List<T>::operator=(const List<T> &other) {
    if (this == &other) 
        return *this;

    size = other.size;
    head = std::move(other.head);

    return *this;
}

template <class T>
void List<T>::EraseByIterator(List<T>::forward_iterator d_it) {
    forward_iterator i = this->begin(), end = this->end();
    if (d_it == end)
        throw std::logic_error("error: out of range");
    if (d_it == this->begin()) {
        this->PopFront();
        return;
    }
    if (d_it.it_ptr == tail) {
        this->PopBack();
        return;
    }
    if (d_it.it_ptr == nullptr)
        throw std::logic_error("error: out of range");
    auto temp = d_it.it_ptr->prev_element;
    std::unique_ptr<Element> temp1 = std::move(d_it.it_ptr->next_element);
    d_it.it_ptr = d_it.it_ptr->prev_element;
    d_it.it_ptr->next_element = std::move(temp1);
    d_it.it_ptr->next_element->prev_element = temp;
    size--;
}

template <class T>
void List<T>::EraseByNumber(size_t N) {
    forward_iterator it = this->begin();
    for (size_t i = 0; i < N; ++i) {
        ++it;
    }
    this->EraseByIterator(it);
}

template <class T>
void List<T>::InsertByIterator(List<T>::forward_iterator ins_it, T &value) {
    std::unique_ptr<Element> tmp = std::make_unique<Element>(value);
    forward_iterator i = this->begin();
    if (ins_it == this->begin()) {
        this->PushFront(value);
        return;
    }
    if (ins_it.it_ptr == nullptr) {
        this->PushBack(value);
        return;
    }

    tmp->prev_element = ins_it.it_ptr->prev_element;
    ins_it.it_ptr->prev_element = tmp.get();
    tmp->next_element = std::move(tmp->prev_element->next_element);
    tmp->prev_element->next_element = std::move(tmp);

    size++;
}

template <class T>
void List<T>::InsertByNumber(size_t N, T &value) {
    forward_iterator it = this->begin();
    for (size_t i = 0; i < N; ++i) {
        ++it;
    }
    this->InsertByIterator(it, value);
}

template <class T>
typename List<T>::forward_iterator List<T>::Element::Next() {
    return forward_iterator(this->next_element.get());
}

template <class T>
List<T>::forward_iterator::forward_iterator(List<T>::Element *ptr) {
    it_ptr = ptr;
}

template <class T>
T &List<T>::forward_iterator::operator*() {
    return this->it_ptr->value;
}

template <class T>
T &List<T>::operator[](size_t index) {
    if (index < 0 || index >= size) {
        throw std::logic_error("error: out of range");
    }
    forward_iterator it = this->begin();
    for (size_t i = 0; i < index; i++) {
        it++;
    }
    return *it;
}

template <class T>
typename List<T>::forward_iterator &List<T>::forward_iterator::operator++() {
    if (it_ptr == nullptr)
        throw std::logic_error("error: out of range");
    *this = it_ptr->Next();
    return *this;
}

template <class T>
typename List<T>::forward_iterator List<T>::forward_iterator::operator++(int) {
    forward_iterator old = *this;
    ++*this;
    return old;
}

template <class T>
bool List<T>::forward_iterator::operator==(const forward_iterator &other) const
{
    return it_ptr == other.it_ptr;
}

template <class T>
bool List<T>::forward_iterator::operator!=(const forward_iterator &other) const {
    return it_ptr != other.it_ptr;
}

#endif /* LIST_HPP */