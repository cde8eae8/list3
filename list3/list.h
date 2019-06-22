//
// Created by nikita on 22.06.19.
//

#ifndef LIST_LIST_H
#define LIST_LIST_H

#include <memory>
#include <iostream>


template <bool is_const, typename T>
struct constness {
    using type = T;
};

template <typename T>
struct constness<true, T> {
    using type = const T;
};

struct node_base {
   node_base *next, *prev;
};

template <typename T>
struct node : node_base {
    node(T const& value_) : value(value_) {};
    T value;
};

template <typename T>
node<T>* to_node(node_base* v) {
    return static_cast<node<T>*>(v);
}

template <typename T>
struct list;

template <bool is_const, typename T>
struct listIterator {
    friend struct listIterator<!is_const, T>;
    friend struct list<T>;
    using inner_type = typename constness<is_const, node<T>>::type*;

    listIterator() : current(nullptr) {
        current = nullptr;
    }

    explicit listIterator(inner_type pos) : current(pos) {}

    listIterator(typename constness<is_const, node_base>::type* pos)
        : current(static_cast<inner_type>(pos)) {}

    listIterator(listIterator<!is_const, T> const& rhs)
            : current(const_cast<inner_type>(rhs.current)) {}

    template<bool is_const1, bool is_const2, typename U>
    friend bool operator==(listIterator<is_const1, U> const& lhs, listIterator<is_const2, U> const& rhs);

    template<bool is_const1, bool is_const2, typename U>
    friend bool operator!=(listIterator<is_const1, U> const& lhs, listIterator<is_const2, U> const& rhs);

    listIterator& operator++() {
        current = static_cast<inner_type>(current->next);
        return *this;
    }

    listIterator const operator++(int a) {
        auto tmp = listIterator(*this);
        current = static_cast<inner_type>(current->next);
        return tmp;
    }

    listIterator& operator--() {
        current = static_cast<inner_type>(current->prev);
        return *this;
    }

    listIterator const operator--(int a) {
        auto tmp = listIterator(*this);
        current = static_cast<inner_type>(current->prev);
        return tmp;
    }

    typename constness<is_const, T>::type& operator*() const {
        return current->value;
    }

    typename constness<is_const, T>::type* operator->() const {
        return &current->value;
    }

    template <bool is_const1, bool is_const2, typename U>
    friend void swap(listIterator<is_const1, U>& a, listIterator<is_const2, U>& b);


 private:
     inner_type current;
};

template <typename T>
struct std::iterator_traits<listIterator<false, T>> {
    using difference_type = ptrdiff_t;
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using iterator_category = std::bidirectional_iterator_tag;
};

template <typename T>
struct std::iterator_traits<listIterator<true, T>> {
    using difference_type = ptrdiff_t;
    using value_type = const T;
    using reference = T const&;
    using pointer = T const*;
    using iterator_category = std::bidirectional_iterator_tag;
};

template <bool is_const1, bool is_const2, typename U>
bool operator==(listIterator<is_const1, U> const& lhs, listIterator<is_const2, U> const& rhs) {
    return lhs.current == rhs.current;
}

template <bool is_const1, bool is_const2, typename U>
bool operator!=(listIterator<is_const1, U> const& lhs, listIterator<is_const2, U> const& rhs) {
    return !(lhs == rhs);
}



template <typename T>
struct list {
    using iterator = listIterator<false, T>;
    using const_iterator = listIterator<true, T>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    list() {
        begin_.next = &begin_;
        begin_.prev = &begin_;
    }

    list(list const& rhs) : list() {
        for (const auto &item : rhs) {
            push_back(item);
        }
    }

    list& operator=(list const& rhs) {
        if (this == &rhs) return *this;
        node_base* current = begin_.next;
        node_base* r_current = rhs.begin_.next;
        while (current != &begin_ && r_current != &rhs.begin_) {
            to_node<T>(current)->value = to_node<T>(r_current)->value;
            current = current->next;
            r_current = r_current->next;
        }
        if (current != &begin_) {
            begin_.prev = current->prev;
            current->prev = &begin_;
        }
        while (current != &begin_) {
            delete(static_cast<node<T>*>(current));
        }
        while (r_current != &rhs.begin_) {
            push_back(to_node<T>(r_current)->value);
            r_current = r_current->next;
        }
        return *this;
    }

    ~list() {
        clear();
    }

    bool empty() {
        return begin_.next == &begin_;
    }

    void clear() {
        std::cout << "clear" << std::endl;
        node_base* current = begin_.next;
        while(!empty())
            pop_back();
        begin_.next = &begin_;
        begin_.prev = &begin_;
    }

    void push_back(T const& value) {
        insert(end(), value);
    }

    void push_front(T const& value) {
        insert(begin(), value);
    }

    void pop_back() {
        erase(--end());
    }

    void pop_front() {
        erase(begin());
    }

    iterator insert(const_iterator pos_, T const& value) {
        iterator pos = static_cast<iterator>(pos_);
        node_base* end = pos.current->prev;
        node_base* new_element = new node<T>(value);
        end->next = new_element; new_element->next = pos.current;
        pos.current->prev = new_element; new_element->prev = end;
        return iterator(new_element);
    }

    iterator erase(const_iterator pos_) {
        iterator pos = static_cast<iterator>(pos_);
        node_base* deleted = pos.current;
        node_base* new_end = pos.current->prev;
        node_base* new_beg = pos.current->next;
        new_end->next = new_beg;
        new_beg->prev = new_end;
        delete to_node<T>(deleted);
        return iterator(new_beg);
    }

    void splice(const_iterator pos_, list&, const_iterator first_, const_iterator last_) {
        if (first_ == last_)
            return;
        auto pos = static_cast<iterator>(pos_);
        auto first = static_cast<iterator>(first_);
        auto last = static_cast<iterator>(--last_);
        first.current->prev->next = last.current->next;
        last.current->next->prev = first.current->prev;

        first.current->prev = pos.current->prev;
        pos.current->prev->next = first.current;
        last.current->next = pos.current;
        pos.current->prev = last.current;
    }

    iterator begin() { return iterator(begin_.next); }
    iterator end() { return iterator(&begin_); }

    reverse_iterator rbegin() {
        return std::reverse_iterator<iterator>(end());
    }
    reverse_iterator rend() {
        return std::reverse_iterator<iterator>(begin());
    };

    const_reverse_iterator rbegin() const {
        return std::reverse_iterator<const_iterator>(end());
    }
    const_reverse_iterator rend() const {
        return std::reverse_iterator<const_iterator>(begin());
    };
    const_iterator begin() const { return const_iterator(begin_.next); }
    const_iterator end() const { return const_iterator(&begin_); }

    T& front() { return *begin(); }
    T& back() { return *(--end()); }
    T const& front() const { return *begin(); }
    T const& back() const { return *(--end()); }

    template <typename U>
    friend void swap(list<U>& a, list<U>& b);

 private:
    node_base begin_;

};

template <typename T>
void swap(list<T>& a, list<T>& b) {
    if (a.empty() && b.empty()) return;
    if (a.empty() || b.empty()) {
        auto l = &a;
        auto r = &b;
        if (r->empty()) std::swap(l, r);
        // l - empty
        l->begin_ = r->begin_;
        r->begin_.next->prev = &l->begin_;
        r->begin_.prev->next = &l->begin_;
        r->begin_.next = &r->begin_;
        r->begin_.prev = &r->begin_;
        return;
    }
    std::swap(a.begin_.next->prev, b.begin_.next->prev);
    std::swap(a.begin_.prev->next, b.begin_.prev->next);
    if (!a.empty() && !b.empty())
        std::swap(a.begin_, b.begin_);
}

template <bool is_const1, bool is_const2, typename U>
void swap(listIterator<is_const1, U>& a, listIterator<is_const2, U>& b) {
    std::swap(a.current, b.current);
}

#endif //LIST_LIST_H
