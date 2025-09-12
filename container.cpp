#include <asm-generic/errno.h>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <new>

using namespace std;

template <typename T>
class Node {
public:
    Node<T>* next;
    T data;

    Node(const T& value) : data(value), next(nullptr) {}
};


class Memo {
public:
    explicit Memo(size_t capacity) 
        : buffer_(new unsigned char[capacity]), capacity_(capacity), offset_(0) {}

    void* allocate(size_t bytes, size_t alignment) {
        size_t base = reinterpret_cast<size_t>(buffer_.get()) + offset_;
        size_t align = (base + (alignment - 1)) & ~(alignment - 1);
        size_t newOffset = (align - reinterpret_cast<size_t>(buffer_.get())) + bytes;
        if (newOffset > capacity_) throw bad_alloc();
        offset_ = newOffset;
        return reinterpret_cast<void*>(align);
    }

    void deallocate(void*, size_t) noexcept {}

private:
    unique_ptr<unsigned char[]> buffer_;
    size_t capacity_;
    size_t offset_;
};

template <typename T>
class SimpAllocator {
public:
    using value_type = T;

    template <typename U>
    struct rebind { using other = SimpAllocator<U>; };

   explicit SimpAllocator(size_t num_elements, bool is_map = false) {
    size_t bytes = num_elements * sizeof(T);
    if (is_map) {
        bytes = num_elements * sizeof(_Rb_tree_node<T>); 
    }
    arena_ = std::make_shared<Memo>(bytes);
}

    
    template <typename U>
    SimpAllocator(const SimpAllocator<U>& other) noexcept : arena_(other.arena_) {}

    T* allocate(size_t n) { 
        return static_cast<T*>(arena_->allocate(n * sizeof(T), alignof(T))); 
    }

    void deallocate(T* p, size_t n) noexcept { arena_->deallocate(p, n); }

    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) { new(p) U(std::forward<Args>(args)...); }

    template <typename U>
    void destroy(U* p) { p->~U(); }

    template <typename U>
    bool operator==(const SimpAllocator<U>& other) const noexcept { return arena_.get() == other.arena_.get(); }

    template <typename U>
    bool operator!=(const SimpAllocator<U>& other) const noexcept { return !(*this == other); }

private:
    template <typename> friend class SimpAllocator;
    shared_ptr<Memo> arena_;
};

template <typename T, typename Allocator = allocator<Node<T>>>
class Container {
public:
    class Iterator {
    public:
        using iterator_category = forward_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using difference_type = ptrdiff_t;

        Iterator(Node<T>* ptr) : p(ptr) {}
        reference operator*() const { return p->data; }
        pointer operator->() { return &(p->data); }

        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
        Iterator& operator++() { p = p->next; return *this; }

        bool operator==(const Iterator& other) const { return p == other.p; }
        bool operator!=(const Iterator& other) const { return p != other.p; }

    private:
        Node<T>* p;
    };

    using value_type = T;
    using allocator_type = Allocator;
    using iterator = Iterator;

    Container(const Allocator& alloc = Allocator()) : head(nullptr), tail(nullptr), alloc_(alloc), size_(0) {}
    ~Container() {
        Node<T>* current = head;
        while (current) {
            Node<T>* next = current->next;
            alloc_.destroy(current);
            alloc_.deallocate(current, 1);
            current = next;
        }
    }

    void push_back(const T& value) {
        Node<T>* newNode = alloc_.allocate(1);
        alloc_.construct(newNode, value);

        if (tail) {
            tail->next = newNode;
            tail = newNode;
        } else {
            head = tail = newNode;
        }
        ++size_;
    }

    iterator begin() { return iterator(head); }
    iterator end() { return iterator(nullptr); }
    size_t size() const { return size_; }

private:
    Node<T>* head;
    Node<T>* tail;
    Allocator alloc_;
    size_t size_;
};


int factorial(int x){
    int y = 1;
    for (int i = 2; i <= x; ++i){
        y *= i;
    }
    return y;
}

int main(){

    map<int, int> map_std_alloc;
    for (int v = 0; v <=9; ++v){
        map_std_alloc[v] = factorial(v);
    }

    using alloc_type = SimpAllocator<std::pair<const int,int>>;

    alloc_type map_alloc(10, true);

    map<int, int, less<int>, alloc_type> map_custom_alloc(map_alloc);
    for (int v = 0; v <=9; ++v){
        map_custom_alloc[v] = factorial(v);
    }

    cout << "Std map:" << "\n";
    for (const auto& [key, value] : map_std_alloc) {
    std::cout << key << " " << value << "\n";
}
    cout << "Custom map:" << "\n";
    for (const auto& [key, value] : map_custom_alloc) {
    std::cout << key << " " << value << "\n";
}

    SimpAllocator<Node<int>> cont_alloc(10);
    
    Container<int> cont_std_alloc;
    for (int v = 0; v <=9; ++v){
        cont_std_alloc.push_back(v);
    }

    Container<int, SimpAllocator<Node<int>>> cont_custom_alloc(cont_alloc);
    for (int v = 0; v <=9; ++v){
        cont_custom_alloc.push_back(v);
    }

    cout << "Custom container, stl allocator:" << "\n";
    for (const auto& i : cont_std_alloc) {
    std::cout << i << "\n";
}

    cout << "Custom container, custom allocator:" << "\n";
    for (const auto& i : cont_custom_alloc) {
    std::cout << i << "\n";
}
}