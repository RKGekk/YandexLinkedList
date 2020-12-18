#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>
#include <algorithm>
#include <new>

template <typename Type>
class SingleLinkedList {
    // ���� ������
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next) : value(val), next_node(next) {}
        Type value = {};
        Node* next_node = nullptr;
    };

    // ������ ������ ������� ��������.
    // ���������� ��������� ��������� �� �������� ������������ ������
    // ValueType - ��������� � Type (��� Iterator) ���� � const Type (��� ConstIterator)
    template <typename ValueType>
    class BasicIterator {
        // ����� ������ ����������� �������������, ����� �� ������� ������
        // ��� ������ � ��������� ������� ���������
        friend class SingleLinkedList;

        // �������������� ����������� ��������� �� ��������� �� ���� ������
        explicit BasicIterator(SingleLinkedList<Type>::Node* node) : node_(node) {}
        explicit BasicIterator(const SingleLinkedList<Type>::Node* node) : node_(const_cast<SingleLinkedList<Type>::Node*>(node)) {}

    public:
        // ����������� ���� ���� �������� ����������� ���������� � ��������� ����� ���������

        // ��������� ��������� - forward iterator
        // (��������, ������� ������������ �������� ���������� � ������������ �������������)
        using iterator_category = std::forward_iterator_tag;
        // ��� ���������, �� ������� ������������ ��������
        using value_type = Type;
        // ���, ������������ ��� �������� �������� ����� �����������
        using difference_type = std::ptrdiff_t;
        // ��� ��������� �� ����������� ��������
        using pointer = ValueType*;
        // ��� ������ �� ����������� ��������
        using reference = ValueType&;
        // ��� ��������� �� ����������� ��������
        using const_pointer = const ValueType*;
        // ��� ������ �� ����������� ��������
        using const_reference = const ValueType&;


        BasicIterator() = default;

        // �������������� �����������/����������� �����������
        // ��� ValueType, ����������� � Type, ������ ���� ����������� ������������
        // ��� ValueType, ����������� � const Type, ������ ���� ��������������� ������������
        BasicIterator(const BasicIterator<Type>& other) noexcept : node_(other.node_) {}

        // ����� ���������� �� ������� �������������� �� ���������� ��������� = ��� �������
        // ����������������� ������������ �����������, ���� ������� �������� = �
        // �������� ���������� ������������� ��� �� ���.
        BasicIterator& operator=(const BasicIterator& rhs) = default;

        // �������� ��������� ���������� (� ���� ������� ��������� ��������� ����������� ��������)
        // ��� ��������� �����, ���� ��� ��������� �� ���� � ��� �� ������� ������, ���� �� end()
        [[nodiscard]]
        bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        // ��������, �������� ���������� �� �����������
        // �������������� !=
        [[nodiscard]]
        bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        // �������� ��������� ���������� (� ���� ������� ��������� ��������)
        // ��� ��������� �����, ���� ��� ��������� �� ���� � ��� �� ������� ������, ���� �� end()
        [[nodiscard]]
        bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        // ��������, �������� ���������� �� �����������
        // �������������� !=
        [[nodiscard]]
        bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        // �������� ��������������. ����� ��� ������ �������� ��������� �� ��������� ������� ������
        // ���������� ������ �� ������ ����
        // ��������� ���������, �� ������������ �� ������������ ������� ������, �������� � �������������� ���������
        BasicIterator& operator++() noexcept {
            if (node_ != nullptr) {
                node_ = node_->next_node;
            }
            return *this;
        }

        // �������� ��������������. ����� ��� ������ �������� ��������� �� ��������� ������� ������.
        // ���������� ������� �������� ���������
        // ��������� ���������, �� ������������ �� ������������ ������� ������,
        // �������� � �������������� ���������
        BasicIterator operator++(int) noexcept {
            BasicIterator temp(*this);
            if (node_ != nullptr) {
                node_ = node_->next_node;
            }
            return temp;
        }

        // �������� �������������. ���������� ������ �� ������� �������
        // ����� ����� ���������, � ���������, �� ������������ �� ������������ ������� ������,
        // �������� � �������������� ���������
        [[nodiscard]]
        reference operator*() const noexcept {
            return node_->value;
        }

        // �������� ������� � ����� ������. ���������� ��������� �� ������� ������� ������.
        // ����� ����� ���������, � ���������, �� ������������ �� ������������ ������� ������,
        // �������� � �������������� ���������
        [[nodiscard]]
        pointer operator->() const noexcept {
            return &node_->value;
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    // ��������, ����������� ��������� ��������� ������
    using Iterator = BasicIterator<Type>;
    // ����������� ��������, ��������������� ������ ��� ������ � ��������� ������
    using ConstIterator = BasicIterator<const Type>;

    // ���������� ��������, ����������� �� ������ �������
    // ���� ������ ������, ������������ �������� ����� ����� end()
    [[nodiscard]]
    Iterator begin() noexcept {
        return Iterator(head_->next_node);
    }

    // ���������� ��������, ����������� �� �������, ��������� �� ��������� ��������� ������������ ������
    // �������������� ���� �������� ������ - ������� ������������� ������� � �������������� ���������
    [[nodiscard]]
    Iterator end() noexcept {
        return Iterator(tail_);
    }

    // ���������� ����������� ��������, ����������� �� ������ �������
    // ���� ������ ������, ������������ �������� ����� ����� end()
    // ��������� ������ ������������ ������ ������ cbegin()
    [[nodiscard]]
    ConstIterator begin() const noexcept {
        return ConstIterator(head_->next_node);
    }

    // ���������� ����������� ��������, ����������� �� �������, ��������� �� ��������� ��������� ������������ ������
    // �������������� ���� �������� ������ - ������� ������������� ������� � �������������� ���������
    // ��������� ������ ������������ ������ ������ cend()
    [[nodiscard]]
    ConstIterator end() const noexcept {
        return ConstIterator(tail_);
    }

    // ���������� ����������� ��������, ����������� �� ������ �������
    // ���� ������ ������, ������������ �������� ����� ����� cend()
    [[nodiscard]]
    ConstIterator cbegin() const noexcept {
        return ConstIterator(head_->next_node);
    }

    // ���������� ����������� ��������, ����������� �� �������, ��������� �� ��������� ��������� ������������ ������
    // �������������� ���� �������� ������ - ������� ������������� ������� � �������������� ���������
    [[nodiscard]]
    ConstIterator cend() const noexcept {
        return ConstIterator(tail_);
    }

    SingleLinkedList() : tail_(new Node({}, nullptr)), head_(new Node({}, tail_)), size_(0) {}

    SingleLinkedList(std::initializer_list<Type> values) : tail_(new Node({}, nullptr)), head_(new Node({}, tail_)), size_(0) {
        Node* newHead = new Node({}, nullptr);
        size_t count = 0;
        Node* newEnd = Copy(values.begin(), values.end(), newHead, count);
        newEnd->next_node = tail_;
        head_ = newHead;
        size_ = count;
    }

    SingleLinkedList(const SingleLinkedList& other) : tail_(new Node({}, nullptr)), head_(new Node({}, tail_)), size_(0) {
        Node* newHead = new Node({}, nullptr);
        size_t count = 0;
        Node* newEnd = Copy(other.begin(), other.end(), newHead, count);
        newEnd->next_node = tail_;
        head_ = newHead;
        size_ = count;
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        Node* newHead = new Node({}, nullptr);
        size_t count = 0;
        Node* newEnd = Copy(rhs.begin(), rhs.end(), newHead, count);
        Clear();
        newEnd->next_node = tail_;
        head_ = newHead;
        size_ = rhs.size_;
        return *this;
    }

    // ���������� ���������� ������� �� ����� O(1)
    void swap(SingleLinkedList& other) noexcept {
        Node* tempTail = tail_;
        Node* tempHead = head_;
        size_t tempSize = size_;
        tail_ = other.tail_;
        head_ = other.head_;
        size_ = other.size_;
        other.tail_ = tempTail;
        other.head_ = tempHead;
        other.size_ = tempSize;
    }

    ~SingleLinkedList() {
        Clear();
        delete tail_;
        delete head_;
    }

    // ���������� ���������� ��������� � ������ �� ����� O(1)
    [[nodiscard]]
    size_t GetSize() const noexcept {
        // ��������. ���������� ����� ��������������
        return size_;
    }

    // ��������, ������ �� ������ �� ����� O(1)
    [[nodiscard]]
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // ��������� ������� value � ������ ������ �� ����� O(1)
    void PushFront(const Type& value) {
        head_->next_node = new Node(value, head_->next_node);
        ++size_;
    }

    // ��������� ������� value � ������ ������ �� ����� O(1)
    void PushBack(const Type& value) {
        tail_->next_node = new Node({}, nullptr);
        tail_->value = value;
        tail_ = tail_->next_node;
        ++size_;
    }

    // ������� ������ �� ����� O(N)
    void Clear() noexcept {
        Node* next = head_->next_node;
        while (next != tail_) {
            head_->next_node = next->next_node;
            delete next;
            next = head_->next_node;
        }
        size_ = 0;
    }

    // ���������� ��������, ����������� �� ������� ����� ������ ��������� ������������ ������.
    // �������������� ���� �������� ������ - ������� ������������� ������� � �������������� ���������
    [[nodiscard]]
    Iterator before_begin() noexcept {
        return Iterator(head_);
    }

    // ���������� ����������� ��������, ����������� �� ������� ����� ������ ��������� ������������ ������.
    // �������������� ���� �������� ������ - ������� ������������� ������� � �������������� ���������
    [[nodiscard]]
    ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(head_);
    }

    // ���������� ����������� ��������, ����������� �� ������� ����� ������ ��������� ������������ ������.
    // �������������� ���� �������� ������ - ������� ������������� ������� � �������������� ���������
    [[nodiscard]]
    ConstIterator before_begin() const noexcept {
        return ConstIterator(head_);
    }

    // ��������� ������� value ����� ��������, �� ������� ��������� pos.
    // ���������� �������� �� ����������� �������
    // ���� ��� �������� �������� ����� ��������� ����������, ������ ��������� � ������� ���������
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* newNode = new Node(value, pos.node_->next_node);
        pos.node_->next_node = newNode;
        ++size_;
        return Iterator(newNode);
    }

    void PopFront() noexcept {
        if (size_ == 0 || head_->next_node == tail_) {
            return;
        }
        Node* temp = head_->next_node->next_node;
        delete head_->next_node;
        head_->next_node = temp;
        --size_;
    }

    // ������� �������, ��������� �� pos.
    // ���������� �������� �� �������, ��������� �� ��������
    Iterator EraseAfter(ConstIterator pos) noexcept {
        Node* temp = pos.node_->next_node->next_node;
        delete pos.node_->next_node;
        pos.node_->next_node = temp;
        --size_;
        return Iterator(temp);
    }

private:

    template <typename ContainerIterator>
    Node* Copy(ContainerIterator from_begin, ContainerIterator from_end, Node* to_head, size_t& count) {
        auto it = from_begin;
        Node* current = to_head;
        try {
            while (it != from_end) {
                current->next_node = new Node(*it, nullptr);
                ++it;
                ++count;
                current = current->next_node;
            }
        }
        catch(const std::bad_alloc&) {
            Node* slider = to_head;
            while (slider != current) {
                Node* temp = slider;
                slider = slider->next_node;
                delete temp;
                --count;
            }
            throw;
        }
        return current;
    }

    // ��������� ����, ������������ ��� ������� "����� ������ ���������"
    Node* tail_;
    Node* head_;
    size_t size_;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    for (auto it1 = lhs.begin(), it2 = rhs.begin(); it1 != lhs.end();) {
        if (*it1 != *it2) {
            return false;
        }
        ++it1;
        ++it2;
    }
    return true;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](auto const& p1, auto const& p2) { return p1 < p2; });
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs > rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), [](auto const& p1, auto const& p2) { return p1 > p2; });
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}