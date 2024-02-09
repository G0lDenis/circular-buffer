#ifndef CIRCULAR_BUFFER_CIRCULARBUFFER_H
#define CIRCULAR_BUFFER_CIRCULARBUFFER_H

#include <memory>


namespace addons {
    template<typename Ptr>
    class CBuffIterator;

    /**
     * @brief A container which offers fixed time access to
     *  individual elements in any order. Differs from vector only
     *  in cyclic memory representing.
     *  Improves <a href="https://en.wikipedia.org/wiki/Circular_buffer">Circular buffer</a> as stl container
     *
     *  @ingroup sequences
     *
     * @tparam Tp  Type of element
     */
    template<typename Tp, typename Alloc=std::allocator<Tp>>
    class CircularBuffer {
    public:
        typedef Tp value_type;
        typedef Alloc allocator_type;
        typedef Tp* pointer;
        typedef const Tp* const_pointer;
        typedef Tp& reference;
        typedef const Tp& const_reference;
        typedef CBuffIterator<pointer> iterator;
        typedef const CBuffIterator<pointer> const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    private:
        pointer start;
        pointer finish;
        pointer start_of_storage;
        pointer end_of_storage;
        allocator_type alloc;

    public:
        // Constructors / destructors / copy

        CircularBuffer() noexcept
            : start(), finish(), start_of_storage(), end_of_storage()
        {}

        CircularBuffer(const CircularBuffer& _x) noexcept
        {
            start = start_of_storage = alloc.allocate(_x.capacity());
            finish = start + _x.size();
            end_of_storage = start_of_storage + _x.capacity();
            std::__uninitialized_copy_a(_x.begin(), _x.end(), start, alloc);
        }

        CircularBuffer(std::initializer_list<value_type> _list)
        {
            start = start_of_storage = alloc.allocate(_list.size());
            finish = end_of_storage = start + _list.size();
            std::__uninitialized_copy_a(_list.begin(), _list.end(), start, alloc);
        }

        CircularBuffer& operator=(const CircularBuffer& _x)
        {
            clear();
            start = start_of_storage = alloc.allocate(_x.capacity());
            finish = start + _x.size();
            end_of_storage = start_of_storage + _x.capacity();
            std::__uninitialized_copy_a(_x.begin(), _x.end(), start, alloc);

            return *this;
        }

        CircularBuffer& operator=(std::initializer_list<value_type> _list)
        {
            clear();
            start = start_of_storage = alloc.allocate(_list.size());
            finish = end_of_storage = start + _list.size();
            std::__uninitialized_copy_a(_list.begin(), _list.end(), start, alloc);

            return *this;
        }

        explicit CircularBuffer(size_type _n)
        {
            start = start_of_storage = finish = alloc.allocate(_n);
            end_of_storage = start_of_storage + _n;
        }

        explicit CircularBuffer(size_type _n, const_reference _value)
        {
            start = start_of_storage = alloc.allocate(_n);
            end_of_storage = start_of_storage + _n;
            finish = std::__uninitialized_fill_n_a(start, _n, _value, alloc);
        }

        template<typename InputIterator, typename = std::_RequireInputIter<InputIterator>>
        CircularBuffer(InputIterator _first, InputIterator _last)
        : start(), finish(), start_of_storage(), end_of_storage()
        {
            assign(_first, _last);
        }

        ~CircularBuffer() noexcept
        {
            this->clear();
        }

        // Public member functions

        template<typename InputIterator, typename = std::_RequireInputIter<InputIterator>>
        void assign(InputIterator _first, InputIterator _last)
        {
            if (!empty()) {
                iterator cur = begin();
                const iterator end_it = end();
                difference_type n = _last - _first;
                size_type int_part = n / size();
                if (int_part) {
                    for (_first += (int_part - 1) * size(); _first != _last && cur != end_it; ++cur, ++_first) {
                        *cur = *_first;
                    }
                }
                cur = begin();
                for (; _first != _last && cur != end_it; ++cur, ++_first) {
                    *cur = *_first;
                }
            }
        }

        void assign(std::initializer_list<value_type> _list)
        {
            assign(_list.begin(), _list.end());
        }

        void assign(size_type _n, const value_type& _value)
        {
            iterator cur = begin();
            const iterator end_it = end();
            size_type _filled = 0;
            for (; _filled != _n && cur != end_it; ++cur, ++_filled) {
                *cur = _value;
            }
        }

        iterator begin() noexcept
        {
            return iterator(start, *this);
        }

        const_iterator begin() const noexcept
        {
            return const_iterator(start, *this);
        }


        iterator end() noexcept
        {
            return iterator(finish, *this);
        }

        const_iterator end() const noexcept
        {
            return const_iterator(finish, *this);
        }

        const_iterator cbegin() const noexcept
        {
            return const_iterator(start, *this);
        }

        const_iterator cend() const noexcept
        {
            return const_iterator(start, *this);
        }

        reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(end());
        }

        [[nodiscard]] const_reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(end());
        }

        reverse_iterator rend() noexcept
        {
            return reverse_iterator(begin());
        }

        const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        const_reverse_iterator crbegin() const noexcept
        {
            return const_reverse_iterator(end());
        }

        const_reverse_iterator crend() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return start == finish;
        }

        [[nodiscard]] size_type size() const noexcept
        {
            return start <= finish && finish != end_of_storage ?
                   finish - start :
                   finish != end_of_storage ?
                   (finish - start_of_storage) + (end_of_storage - start) :
                   end_of_storage - start_of_storage;
        }

        [[nodiscard]] size_type max_size() const noexcept
        {
            const size_type diffmax = __gnu_cxx::__numeric_traits<ptrdiff_t>::__max / sizeof(Tp);
            const size_type allocmax = __gnu_cxx::__alloc_traits<Alloc>::max_size(alloc);
            return (std::min)(diffmax, allocmax);
        }

        [[nodiscard]] size_type capacity() const noexcept
        {
            return end_of_storage - start_of_storage;
        }

        void resize(size_type _new_size)
        {
            if (_new_size > max_size())
                throw std::length_error("resizing overheads maximum size");
            if (_new_size > size()) {
                if (_new_size > capacity())
                    _reallocate_storage_default(_new_size);
                else
                    finish = std::__uninitialized_default_n_a(end(), _new_size - size(), alloc).get_ptr();
            } else if (_new_size) {
                _erase_ending(size() - _new_size);
            }
        }

        void resize(size_type _new_size, const value_type& _value)
        {
            if (_new_size > max_size())
                throw std::length_error("resizing overheads maximum size");
            if (_new_size > size()) {
                if (_new_size > capacity())
                    _reallocate_storage_filled(_new_size, _value);
                else
                    finish = std::__uninitialized_fill_n_a(end(), _new_size - size(), _value, alloc).get_ptr();
            } else {
                _erase_ending(size() - _new_size);
            }
        }

        void reserve(size_type _n)
        {
            if (_n > max_size())
                throw std::length_error("reserving overheads capacity");
            if (_n > capacity())
                _reallocate_storage_default(_n);
        }

        void shrink_to_fit()
        {
            _reallocate_storage_default(size());
        };

        reference front() noexcept
        {
            return *start;
        }

        const_reference front() const noexcept
        {
            return *start;
        }

        reference back() noexcept
        {
            return *(--end());
        }

        const_reference back() const noexcept
        {
            return *(--end());
        }

        reference operator[](size_type _offset) noexcept
        {
            return begin()[_offset];
        }

        const_reference operator[](size_type _offset) const noexcept
        {
            return begin()[_offset];
        }

        reference at(size_type _offset)
        {
            return begin().at(_offset);
        }

        const_reference at(size_type _offset) const
        {
            return begin().at(_offset);
        }

        template<typename InputIterator, typename = std::_RequireInputIter<InputIterator>>
        void insert(iterator _pos, InputIterator _first, InputIterator _last)
        {
            if (_pos - begin() >= end() - _pos) {
                difference_type n_moved = end() - _pos;
                pointer tmp = alloc.allocate(n_moved);
                std::__relocate_a(_pos,
                                  end(),
                                  tmp, alloc);
                auto cur = _pos;
                for (; _first < _last; _first++, cur++) {
                    if (cur == end()) {
                        if (end().get_ptr() == end_of_storage) {
                            cur = begin();
                        } else {
                            finish++;
                            cur = --end();
                        }
                    }
                    *cur = *_first;
                }
                const_pointer moved_end = tmp + n_moved;
                for (; tmp < moved_end; tmp++, cur++) {
                    if (end().get_ptr() == end_of_storage) {
                        cur = begin();
                    } else {
                        finish++;
                        cur = --end();
                    }
                    *cur = *tmp;
                }
                finish = cur.get_ptr();
                alloc.deallocate(tmp, n_moved);
            } else {
                difference_type n_moved = end() - _pos;
                pointer tmp = alloc.allocate(n_moved);
                std::__relocate_a(reverse_iterator(_pos),
                                  reverse_iterator(begin()),
                                  tmp, alloc);
                auto cur = reverse_iterator(_pos);
                _first = std::reverse_iterator(_first);
                _last = std::reverse_iterator(_last);
                for (; _first < _last; _first++, cur++) {
                    if (cur == begin()) {
                        if (end().get_ptr() == end_of_storage) {
                            cur = --end();
                        } else {
                            start--;
                            cur = --begin();
                        }
                    }
                    *cur = *_first;
                }
                const_pointer moved_end = tmp + n_moved;
                for (; tmp < moved_end; tmp++, cur++) {
                    if (cur == begin()) {
                        if (end().get_ptr() == end_of_storage) {
                            cur = --end();
                        } else {
                            start--;
                            cur = --begin();
                        }
                    }
                    *cur = *tmp;
                }
                start = (cur + 1).get_ptr();
                alloc.deallocate(tmp, n_moved);
            }
        };

        void insert(const_iterator _pos, size_type _n, const value_type& _value)
        {
            if (_pos - begin() >= end() - _pos) {
                difference_type n_moved = end() - _pos;
                pointer tmp = alloc.allocate(n_moved);
                std::__relocate_a(_pos,
                                  end(),
                                  tmp, alloc);
                auto cur = _pos;
                for (auto filled = 0; filled < _n; filled++, cur++) {
                    if (cur == end()) {
                        if (end().get_ptr() == end_of_storage) {
                            cur = begin();
                        } else {
                            finish++;
                            cur = --end();
                        }
                    }
                    *cur = _value;
                }
                const_pointer moved_end = tmp + n_moved;
                for (; tmp < moved_end; tmp++, cur++) {
                    if (end().get_ptr() == end_of_storage) {
                        cur = begin();
                    } else {
                        finish++;
                        cur = --end();
                    }
                    *cur = *tmp;
                }
                finish = cur.get_ptr();
                alloc.deallocate(tmp, n_moved);
            } else {
                difference_type n_moved = end() - _pos;
                pointer tmp = alloc.allocate(n_moved);
                std::__relocate_a(const_reverse_iterator(_pos),
                                  reverse_iterator(begin()),
                                  tmp, alloc);
                auto cur = *const_cast<iterator*>(&_pos);
                for (auto filled = 0; filled < _n; filled++, --_pos) {
                    if (cur == begin()) {
                        if (end().get_ptr() == end_of_storage) {
                            cur = --end();
                        } else {
                            start--;
                            cur = --begin();
                        }
                    }
                    *cur = _value;
                }
                const_pointer moved_end = tmp + n_moved;
                for (; tmp < moved_end; tmp++, cur++) {
                    if (cur == begin()) {
                        if (end().get_ptr() == end_of_storage) {
                            cur = --end();
                        } else {
                            start--;
                            cur = --begin();
                        }
                    }
                    *cur = *tmp;
                }
                start = (++cur).get_ptr();
                alloc.deallocate(tmp, n_moved);
            }
        };

        void insert(const_iterator _pos, const value_type& _value)
        {
            if (_pos - begin() >= end() - _pos) {
                if (finish == end_of_storage) {
                    start = (++begin()).get_ptr();
                } else {
                    finish++;
                }
                auto r_pos = const_reverse_iterator(_pos);
                for (auto cur = rbegin(); cur != r_pos; cur++) {
                    std::swap(*cur, *(cur + 1));
                }
                *_pos = _value;
            } else {
                if (finish == end_of_storage) {
                    start = (--end()).get_ptr();
                } else {
                    if (start != start_of_storage)
                        start--;
                    else
                        start = end_of_storage - 1;
                }
                for (auto cur = begin(); cur != _pos - 1; ++cur) {
                    std::swap(*cur, *(cur + 1));
                }
                *(_pos - 1) = _value;
            }
        }

        iterator insert(const_iterator _pos, std::initializer_list<value_type> _list)
        {
            insert(_pos, _list.begin(), _list.end());
        }

        void push_back(const value_type& _value)
        {
            if (finish == end_of_storage) {
                *begin() = _value;
                start = (++begin()).get_ptr();
            } else {
                finish++;
                *(--end()) = _value;
            }
        }

        void push_front(const value_type& _value)
        {
            if (finish == end_of_storage) {
                *(--end()) = _value;
                start = (--end()).get_ptr();
            } else {
                if (start != start_of_storage)
                    start--;
                else
                    start = end_of_storage - 1;
                *begin() = _value;
            }
        }

        void pop_back()
        {
            if (!empty()) {
                std::_Destroy((--end()).get_ptr());
                finish = (--end()).get_ptr();
            }
        }

        void pop_front()
        {
            if (!empty()) {
                std::_Destroy(start);
                start = (++begin()).get_ptr();
            }
        }

        iterator erase(const_iterator _pos)
        {
            std::_Destroy(_pos.get_ptr());
            if (_pos - begin() >= end() - _pos) {
                for (auto cur = begin(); cur != _pos; cur++) {
                    std::swap(*cur, *(cur + 1));
                }
                if (finish == end_of_storage) {
                    finish = (--end()).get_ptr();
                } else {
                    finish--;
                }
            } else {
                const_iterator st_post = begin();
                for (auto cur = *const_cast<iterator*>(&_pos); cur != st_post; cur--) {
                    std::swap(*cur, *(cur - 1));
                }
                if (finish == end_of_storage) {
                    finish = start;
                    start = (++begin()).get_ptr();
                } else {
                    if (start != end_of_storage - 1) {
                        finish = start;
                        start++;
                    }
                    else {
                        start = start_of_storage;
                    }
                }
            }

            return _pos;
        }

        void swap(CircularBuffer& _x) noexcept
        {
            std::swap(start, _x.start);
            std::swap(finish, _x.finish);
            std::swap(start_of_storage, _x.start_of_storage);
            std::swap(end_of_storage, _x.end_of_storage);
            std::swap(alloc, _x.alloc);
        }

        void clear() noexcept
        {
            alloc.deallocate(start_of_storage, end_of_storage - start_of_storage);
            start_of_storage = start = end_of_storage = finish = pointer();
        }

        // Friend operators for iterator

        friend class CBuffIterator<pointer>;

        template<typename Ptr>
        friend CBuffIterator<Ptr> operator+(const CBuffIterator<Ptr>& _it, CBuffIterator<Ptr>::difference_type _offset) noexcept;

        template<typename Ptr>
        friend CBuffIterator<Ptr> operator+(CBuffIterator<Ptr>::difference_type _offset, CBuffIterator<Ptr> _it) noexcept;

        template<typename Ptr>
        friend CBuffIterator<Ptr> operator-(CBuffIterator<Ptr> _it, CBuffIterator<Ptr>::difference_type _offset) noexcept;

        template<typename Ptr>
        friend CBuffIterator<Ptr>::difference_type operator-(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept;

        template<typename Ptr>
        friend bool operator==(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept;

        template<typename Ptr>
        friend bool operator!=(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept;

        template<typename Ptr>
        friend bool operator>(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept;

        template<typename Ptr>
        friend bool operator<(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept;

        template<typename Ptr>
        friend bool operator>=(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept;

        template<typename Ptr>
        friend bool operator<=(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept;

        // Self operators

        friend bool operator==(const CircularBuffer& _left, const CircularBuffer& _right) noexcept
        {
            if (_left.size() != _right.size())
                return false;
            auto it_left = _left.begin();
            auto it_right = _right.begin();
            for (; it_left < _left.end(); it_left++, it_right++)
            {
                if (*it_left != *it_right)
                    return false;
            }

            return true;
        }

        friend bool operator!=(const CircularBuffer& _left, const CircularBuffer& _right) noexcept
        {
            if (_left.size() != _right.size())
                return true;
            auto it_left = _left.begin();
            auto it_right = _right.begin();
            for (; it_left < _left.end(); it_left++, it_right++)
            {
                if (*it_left != *it_right)
                    return true;
            }

            return false;
        }

        friend bool operator>(const CircularBuffer& _left, const CircularBuffer& _right) noexcept
        {
            auto it_left = _left.begin();
            auto it_right = _right.begin();
            for (; it_left < _left.end() && it_right < _right.end(); it_left++, it_right++)
            {
                if (*it_left > *it_right)
                    return true;
                if (*it_left < *it_right)
                    return false;
            }

            return _left.size() > _right.size();
        }

        friend bool operator<(const CircularBuffer& _left, const CircularBuffer& _right) noexcept
        {
            auto it_left = _left.begin();
            auto it_right = _right.begin();
            for (; it_left < _left.end() && it_right < _right.end(); ++it_left, ++it_right)
            {
                if (*it_left < *it_right)
                    return true;
                if (*it_left > *it_right)
                    return false;
            }

            return _left.size() < _right.size();
        }

        friend bool operator>=(const CircularBuffer& _left, const CircularBuffer& _right) noexcept
        {
            auto it_left = _left.begin();
            auto it_right = _right.begin();
            for (; it_left < _left.end() && it_right < _right.end(); it_left++, it_right++)
            {
                if (*it_left > *it_right)
                    return true;
                if (*it_left < *it_right)
                    return false;
            }

            return _left.size() >= _right.size();
        }

        friend bool operator<=(const CircularBuffer& _left, const CircularBuffer& _right) noexcept
        {
            auto it_left = _left.begin();
            auto it_right = _right.begin();
            for (; it_left < _left.end() && it_right < _right.end(); it_left++, it_right++)
            {
                if (*it_left < *it_right)
                    return true;
                if (*it_left > *it_right)
                    return false;
            }

            return _left.size() <= _right.size();
        }

    private:
        // Additional methods

        void _erase_ending(size_type _n)
        {
            auto new_ending = end() - _n;
            std::_Destroy(new_ending, end());
            finish = new_ending.get_ptr();
        }

        void _reallocate_storage_default(size_type _new_size)
        {
            if (_new_size >= size()) {
                pointer new_start = alloc.allocate(_new_size);
                try {
                    std::__uninitialized_default_n_a(new_start + size(), _new_size - size(), alloc);
                }
                catch (...) {
                    alloc.deallocate(new_start, _new_size);
                    throw std::length_error("allocator throw");
                }
                finish = std::__relocate_a(begin(),
                                           end(),
                                           new_start, alloc);
                alloc.deallocate(start_of_storage, end_of_storage - start_of_storage);
                start = start_of_storage = new_start;
                end_of_storage = new_start + _new_size;
            } else {
                pointer new_start = alloc.allocate(_new_size);
                finish = std::__relocate_a(begin(),
                                           begin() + _new_size,
                                           new_start, alloc);
                alloc.deallocate(start_of_storage, end_of_storage - start_of_storage);
                start = start_of_storage = new_start;
                end_of_storage = new_start + _new_size;
            }
        }

        void _reallocate_storage_filled(size_type _new_size, const value_type& _value)
        {
            if (_new_size >= size()) {
                pointer new_start = alloc.allocate(_new_size);
                try {
                    std::__uninitialized_fill_n_a(new_start + size(), _new_size - size(), _value, alloc);
                }
                catch (...) {
                    alloc.deallocate(new_start, _new_size);
                    throw std::length_error("allocator throw");
                }
                std::__relocate_a(begin(),
                                  end(),
                                  new_start, alloc);
                alloc.deallocate(start_of_storage, end_of_storage - start_of_storage);
                start = start_of_storage = new_start;
                finish = end_of_storage = new_start + _new_size;
            } else {
                pointer new_start = alloc.allocate(_new_size);
                finish = std::__relocate_a(begin(),
                                           begin() + _new_size,
                                           new_start, alloc);
                alloc.deallocate(start_of_storage, end_of_storage - start_of_storage);
                start = start_of_storage = new_start;
                end_of_storage = new_start + _new_size;
            }
        }
    };

    /**
     * @brief Buffer iterator which supports all features of
     * <a href="https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator">
     * RandomAccessIterator</a>
     *
     * @ingroup iterators
     *
     * @tparam Ptr Pointer to element&
     */
    template<typename Ptr>
    class CBuffIterator {
    public:
        typedef Ptr pointer;
        [[maybe_unused]] typedef std::random_access_iterator_tag iterator_category;
        typedef std::iterator_traits<pointer>::value_type value_type;
        typedef std::iterator_traits<pointer>::reference reference;
        typedef std::iterator_traits<pointer>::difference_type difference_type;
        typedef const CircularBuffer<value_type>* container_const_pointer;

    private:
        pointer stored_ptr;
        container_const_pointer buffer;

    public:
        // Constructors / destructors / copy

        CBuffIterator() noexcept
        : stored_ptr(), buffer()
        {}

        CBuffIterator(const CBuffIterator& _i) noexcept
        : stored_ptr(_i.stored_ptr), buffer(_i.buffer)
        {}

        CBuffIterator& operator=(const CBuffIterator& _i) noexcept
        {
            stored_ptr = _i.stored_ptr;
            buffer = _i.buffer;

            return *this;
        }

        explicit CBuffIterator(pointer _ptr, const CircularBuffer<value_type>& _buf) noexcept
        : stored_ptr(_ptr), buffer(&_buf)
        {}

        // Getting pointer

        pointer& get_ptr() noexcept
        {
            return stored_ptr;
        }

        [[nodiscard]] const pointer& get_ptr() const noexcept
        {
            return stored_ptr;
        }

        // Pointer operators

        reference operator*() const noexcept
        {
            return *stored_ptr;
        }

        pointer operator->() const noexcept
        {
            return stored_ptr;
        }

        reference operator[](difference_type _offset) const noexcept
        {
            _offset = _offset % buffer->size();
            if (stored_ptr + _offset >= buffer->end_of_storage)
                return *(buffer->start_of_storage + (stored_ptr + _offset - buffer->end_of_storage));
            return stored_ptr[_offset];
        }

        [[nodiscard]] reference at(difference_type _offset) const
        {
            _offset = _offset % buffer->size();
            if (stored_ptr + _offset >= buffer->end_of_storage)
                return *(buffer->start_of_storage + (stored_ptr + _offset - buffer->end_of_storage));
            return stored_ptr[_offset];
        }

        // Arithmetic

        CBuffIterator& operator++() noexcept
        {
            if (++stored_ptr == buffer->start)
                stored_ptr = buffer->end_of_storage;
            else if (stored_ptr == buffer->end_of_storage && buffer->start != buffer->start_of_storage)
                stored_ptr = buffer->start_of_storage;

            return *this;
        }

        CBuffIterator operator++(int) noexcept
        {
            auto old_ptr = stored_ptr;
            if (++stored_ptr == buffer->start)
                stored_ptr = buffer->end_of_storage;
            else if (stored_ptr == buffer->end_of_storage && buffer->start != buffer->start_of_storage)
                stored_ptr = buffer->start_of_storage;

            return CBuffIterator(old_ptr, *buffer);
        }

        CBuffIterator& operator--() noexcept
        {
            if (stored_ptr == buffer->start)
                stored_ptr = buffer->end_of_storage;
            else {
                if (stored_ptr == buffer->end_of_storage)
                    stored_ptr = buffer->start;
                if (stored_ptr == buffer->start_of_storage)
                    stored_ptr = buffer->end_of_storage - 1;
                else stored_ptr--;
            }

            return *this;
        }

        CBuffIterator operator--(int) noexcept
        {
            auto old_ptr = stored_ptr;
            if (stored_ptr == buffer->start)
                stored_ptr = buffer->end_of_storage;
            else {
                if (stored_ptr == buffer->end_of_storage)
                    stored_ptr = buffer->start;
                if (stored_ptr == buffer->start_of_storage)
                    stored_ptr = buffer->end_of_storage - 1;
                else stored_ptr--;
            }

            return CBuffIterator(old_ptr, *buffer);
        }

        CBuffIterator& operator+=(difference_type _offset) noexcept
        {
            if (_offset) {
                if (_offset < 0)
                    return operator-=(-_offset);
                _offset = _offset % buffer->size();
                if (stored_ptr + _offset >= buffer->end_of_storage) {
                    stored_ptr = buffer->start_of_storage + _offset - (buffer->end_of_storage - stored_ptr);
                    if (stored_ptr == buffer->start)
                        stored_ptr = buffer->end_of_storage;
                } else if (stored_ptr + _offset == buffer->start) {
                    stored_ptr = buffer->end_of_storage;
                } else
                    stored_ptr += _offset;
            }

            return *this;
        }

        CBuffIterator& operator-=(difference_type _offset) noexcept
        {
            if (_offset) {
                if (_offset < 0)
                    return operator+=(-_offset);
                _offset = _offset % buffer->size();
                if (stored_ptr == buffer->end_of_storage) {
                    stored_ptr = buffer->start;
                    if (stored_ptr == buffer->start_of_storage) {
                        stored_ptr = buffer->end_of_storage - 1;
                    } else {
                        stored_ptr = buffer->start - 1;
                    }
                    _offset--;
                }
                if (stored_ptr - _offset < buffer->start_of_storage) {
                    stored_ptr = buffer->end_of_storage - _offset + (stored_ptr - buffer->start_of_storage);
                } else
                    stored_ptr -= _offset;
            }

            return *this;
        }

        template<typename InnerPtr>
        friend CBuffIterator<InnerPtr> operator+(const CBuffIterator<InnerPtr>& _it, CBuffIterator<InnerPtr>::difference_type _offset) noexcept;

        template<typename InnerPtr>
        friend CBuffIterator<InnerPtr> operator+(CBuffIterator<InnerPtr>::difference_type _offset, CBuffIterator<InnerPtr> _it) noexcept;

        template<typename InnerPtr>
        friend CBuffIterator<InnerPtr> operator-(CBuffIterator<InnerPtr> _it, CBuffIterator<InnerPtr>::difference_type _offset) noexcept;

        // Difference

        template<typename InnerPtr>
        friend CBuffIterator<InnerPtr>::difference_type operator-(const CBuffIterator<InnerPtr>& _left, const CBuffIterator<InnerPtr>& _right) noexcept;

        // Comparison

        template<typename InnerPtr>
        friend bool operator==(const CBuffIterator<InnerPtr>& _left, const CBuffIterator<InnerPtr>& _right) noexcept;

        template<typename InnerPtr>
        friend bool operator!=(const CBuffIterator<InnerPtr>& _left, const CBuffIterator<InnerPtr>& _right) noexcept;

        template<typename InnerPtr>
        friend bool operator>(const CBuffIterator<InnerPtr>& _left, const CBuffIterator<InnerPtr>& _right) noexcept;

        template<typename InnerPtr>
        friend bool operator<(const CBuffIterator<InnerPtr>& _left, const CBuffIterator<InnerPtr>& _right) noexcept;

        template<typename InnerPtr>
        friend bool operator>=(const CBuffIterator<InnerPtr>& _left, const CBuffIterator<InnerPtr>& _right) noexcept;

        template<typename InnerPtr>
        friend bool operator<=(const CBuffIterator<InnerPtr>& _left, const CBuffIterator<InnerPtr>& _right) noexcept;
    };

    template<typename Ptr>
    CBuffIterator<Ptr> operator+(const CBuffIterator<Ptr>& _it, typename CBuffIterator<Ptr>::difference_type _offset) noexcept
    {
        auto new_it = CBuffIterator(_it);
        new_it.operator+=(_offset);

        return new_it;
    }

    template<typename Ptr>
    CBuffIterator<Ptr> operator+(typename CBuffIterator<Ptr>::difference_type _offset, CBuffIterator<Ptr> _it) noexcept
    {
        _it.operator+=(_offset);

        return _it;
    }

    template<typename Ptr>
    CBuffIterator<Ptr> operator-(CBuffIterator<Ptr> _it, typename CBuffIterator<Ptr>::difference_type _offset) noexcept
    {
        auto new_it = CBuffIterator(_it);
        new_it.operator-=(_offset);

        return new_it;
    }

    template<typename Ptr>
    CBuffIterator<Ptr>::difference_type operator-(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept
    {
        if (_left.buffer->start <= _left.buffer->finish
            || _left.stored_ptr <= _left.buffer->finish && _right.stored_ptr <= _left.buffer->finish
            || _left.stored_ptr >= _left.buffer->start && _right.stored_ptr >= _left.buffer->start)
            return _left.stored_ptr - _right.stored_ptr;

        return _left.stored_ptr <= _right.stored_ptr ?
               (_left.buffer->end_of_storage - _right.stored_ptr) +
               (_left.stored_ptr - _left.buffer->start_of_storage) :
               (_left.buffer->end_of_storage - _left.stored_ptr) +
               (_right.stored_ptr - _left.buffer->start_of_storage);
    }

    template<typename Ptr>
    bool operator==(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept
    {
        return _left.stored_ptr == _right.stored_ptr;
    }

    template<typename Ptr>
    bool operator!=(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept
    {
        return _left.stored_ptr != _right.stored_ptr;
    }

    template<typename Ptr>
    bool operator>(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept
    {
        if (_left.buffer->start <= _left.buffer->finish
            || _left.stored_ptr <= _left.buffer->finish && _right.stored_ptr <= _left.buffer->finish
            || _left.stored_ptr >= _left.buffer->start && _right.stored_ptr >= _left.buffer->start)
            return _left.stored_ptr > _right.stored_ptr;

        return _left.stored_ptr < _right.stored_ptr;
    }

    template<typename Ptr>
    bool operator<(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept
    {
        if (_left.buffer->start <= _left.buffer->finish
            || _left.stored_ptr <= _left.buffer->finish && _right.stored_ptr <= _left.buffer->finish
            || _left.stored_ptr >= _left.buffer->start && _right.stored_ptr >= _left.buffer->start)
            return _left.stored_ptr < _right.stored_ptr;

        return _left.stored_ptr > _right.stored_ptr;
    }

    template<typename Ptr>
    bool operator>=(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept
    {
        if (_left.buffer->start <= _left.buffer->finish
            || _left.stored_ptr <= _left.buffer->finish && _right.stored_ptr <= _left.buffer->finish
            || _left.stored_ptr >= _left.buffer->start && _right.stored_ptr >= _left.buffer->start)
            return _left.stored_ptr >= _right.stored_ptr;

        return _left.stored_ptr <= _right.stored_ptr;
    }

    template<typename Ptr>
    bool operator<=(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept
    {
        if (_left.buffer->start <= _left.buffer->finish
            || _left.stored_ptr <= _left.buffer->finish && _right.stored_ptr <= _left.buffer->finish
            || _left.stored_ptr >= _left.buffer->start && _right.stored_ptr >= _left.buffer->start)
            return _left.stored_ptr <= _right.stored_ptr;

        return _left.stored_ptr >= _right.stored_ptr;
    }

    /**
     * @brief A container which offers fixed time access to
     *  individual elements in any order. Differs from vector only
     *  in cyclic memory representing. Extends in methods
     *  assign, push_back, push_front, insert.
     *
     *  @ingroup sequences
     *
     * @tparam Tp  Type of element. Default - default allocator.
     *
     */
    template<typename Tp, typename Alloc=std::allocator<Tp>>
    class CircularBufferExt {
    public:
        typedef Tp value_type;
        typedef Alloc allocator_type;
        typedef Tp* pointer;
        typedef const Tp* const_pointer;
        typedef Tp& reference;
        typedef const Tp& const_reference;
        typedef CBuffIterator<pointer> iterator;
        typedef const CBuffIterator<pointer> const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

    private:
        pointer start;
        pointer finish;
        pointer start_of_storage;
        pointer end_of_storage;
        allocator_type alloc;

    public:
        // Constructors / destructors / copy

        CircularBufferExt() noexcept
            : start(), finish(), start_of_storage(), end_of_storage()
        {}

        CircularBufferExt(const CircularBufferExt& _x) noexcept
        {
            start = start_of_storage = alloc.allocate(_x.capacity());
            finish = start + _x.size();
            end_of_storage = start_of_storage + _x.capacity();
            std::__uninitialized_copy_a(_x.begin(), _x.end(), start, alloc);
        }

        CircularBufferExt(std::initializer_list<value_type> _list)
        {
            start = start_of_storage = alloc.allocate(_list.size());
            finish = end_of_storage = start + _list.size();
            std::__uninitialized_copy_a(_list.begin(), _list.end(), start, alloc);
        }

        CircularBufferExt& operator=(const CircularBufferExt& _x)
        {
            clear();
            start = start_of_storage = alloc.allocate(_x.capacity());
            finish = start + _x.size();
            end_of_storage = start_of_storage + _x.capacity();
            std::__uninitialized_copy_a(_x.begin(), _x.end(), start, alloc);

            return *this;
        }

        CircularBufferExt& operator=(std::initializer_list<value_type> _list)
        {
            clear();
            start = start_of_storage = alloc.allocate(_list.size());
            finish = end_of_storage = start + _list.size();
            std::__uninitialized_copy_a(_list.begin(), _list.end(), start, alloc);

            return *this;
        }

        explicit CircularBufferExt(size_type _n)
        {
            start = start_of_storage = finish = alloc.allocate(_n);
            end_of_storage = start_of_storage + _n;
        }

        explicit CircularBufferExt(size_type _n, const_reference _value)
        {
            start = start_of_storage = alloc.allocate(_n);
            end_of_storage = start_of_storage + _n;
            finish = std::__uninitialized_fill_n_a(start, _n, _value, alloc);
        }

        template<typename InputIterator, typename = std::_RequireInputIter<InputIterator>>
        CircularBufferExt(InputIterator _first, InputIterator _last)
            : start(), finish(), start_of_storage(), end_of_storage()
        {
            assign(_first, _last);
        }

        ~CircularBufferExt() noexcept
        {
            this->clear();
        }

        // Public member functions

        template<typename InputIterator, typename = std::_RequireInputIter<InputIterator>>
        void assign(InputIterator _first, InputIterator _last)
        {
            if (!empty()) {
                iterator cur = begin();
                const iterator end_it = end();
                difference_type n = _last - _first;
                size_type int_part = n / size();
                if (int_part) {
                    for (_first += (int_part - 1) * size(); _first != _last && cur != end_it; ++cur, ++_first) {
                        *cur = *_first;
                    }
                }
                cur = begin();
                for (; _first != _last && cur != end_it; ++cur, ++_first) {
                    *cur = *_first;
                }
            }
        }

        void assign(std::initializer_list<value_type> _list)
        {
            assign(_list.begin(), _list.end());
        }

        void assign(size_type _n, const value_type& _value)
        {
            iterator cur = begin();
            const iterator end_it = end();
            size_type _filled = 0;
            for (; _filled != _n && cur != end_it; ++cur, ++_filled) {
                *cur = _value;
            }
        }

        iterator begin() noexcept
        {
            return iterator(start, *this);
        }

        const_iterator begin() const noexcept
        {
            return const_iterator(start, *this);
        }


        iterator end() noexcept
        {
            return iterator(finish, *this);
        }

        const_iterator end() const noexcept
        {
            return const_iterator(finish, *this);
        }

        const_iterator cbegin() const noexcept
        {
            return const_iterator(start, *this);
        }

        const_iterator cend() const noexcept
        {
            return const_iterator(start, *this);
        }

        reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(end());
        }

        [[nodiscard]] const_reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(end());
        }

        reverse_iterator rend() noexcept
        {
            return reverse_iterator(begin());
        }

        const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        const_reverse_iterator crbegin() const noexcept
        {
            return const_reverse_iterator(end());
        }

        const_reverse_iterator crend() const noexcept
        {
            return const_reverse_iterator(begin());
        }

        [[nodiscard]] bool empty() const noexcept
        {
            return start == finish;
        }

        [[nodiscard]] size_type size() const noexcept
        {
            return start <= finish && finish != end_of_storage ?
                   finish - start :
                   finish != end_of_storage ?
                   (finish - start_of_storage) + (end_of_storage - start) :
                   end_of_storage - start_of_storage;
        }

        [[nodiscard]] size_type max_size() const noexcept
        {
            const size_type diffmax = __gnu_cxx::__numeric_traits<ptrdiff_t>::__max / sizeof(Tp);
            const size_type allocmax = __gnu_cxx::__alloc_traits<Alloc>::max_size(alloc);
            return (std::min)(diffmax, allocmax);
        }

        [[nodiscard]] size_type capacity() const noexcept
        {
            return end_of_storage - start_of_storage;
        }

        void resize(size_type _new_size)
        {
            if (_new_size > max_size())
                throw std::length_error("resizing overheads maximum size");
            if (_new_size > size()) {
                if (_new_size > capacity())
                    _reallocate_storage_default(_new_size);
                else
                    finish = std::__uninitialized_default_n_a(end(), _new_size - size(), alloc).get_ptr();
            } else if (_new_size) {
                _erase_ending(size() - _new_size);
            }
        }

        void resize(size_type _new_size, const value_type& _value)
        {
            if (_new_size > max_size())
                throw std::length_error("resizing overheads maximum size");
            if (_new_size > size()) {
                if (_new_size > capacity())
                    _reallocate_storage_filled(_new_size, _value);
                else
                    finish = std::__uninitialized_fill_n_a(end(), _new_size - size(), _value, alloc).get_ptr();
            } else {
                _erase_ending(size() - _new_size);
            }
        }

        void reserve(size_type _n)
        {
            if (_n > max_size())
                throw std::length_error("reserving overheads capacity");
            if (_n > capacity())
                _reallocate_storage_default(_n);
        }

        void shrink_to_fit()
        {
            _reallocate_storage_default(size());
        };

        reference front() noexcept
        {
            return *start;
        }

        const_reference front() const noexcept
        {
            return *start;
        }

        reference back() noexcept
        {
            return *(--end());
        }

        const_reference back() const noexcept
        {
            return *(--end());
        }

        reference operator[](size_type _offset) noexcept
        {
            return begin()[_offset];
        }

        const_reference operator[](size_type _offset) const noexcept
        {
            return begin()[_offset];
        }

        reference at(size_type _offset)
        {
            return begin().at(_offset);
        }

        const_reference at(size_type _offset) const
        {
            return begin().at(_offset);
        }

        template<typename InputIterator, typename = std::_RequireInputIter<InputIterator>>
        void insert(iterator _pos, InputIterator _first, InputIterator _last)
        {
            if (finish == end_of_storage)
                resize(capacity() + _last - _first);
            if (_pos - begin() >= end() - _pos) {
                difference_type n_moved = end() - _pos;
                pointer tmp = alloc.allocate(n_moved);
                std::__relocate_a(_pos,
                                  end(),
                                  tmp, alloc);
                auto cur = _pos;
                for (; _first < _last; _first++, cur++) {
                    if (cur == end()) {
                        if (end().get_ptr() == end_of_storage) {
                            cur = begin();
                        } else {
                            finish++;
                            cur = --end();
                        }
                    }
                    *cur = *_first;
                }
                const_pointer moved_end = tmp + n_moved;
                for (; tmp < moved_end; tmp++, cur++) {
                    if (end().get_ptr() == end_of_storage) {
                        cur = begin();
                    } else {
                        finish++;
                        cur = --end();
                    }
                    *cur = *tmp;
                }
                finish = cur.get_ptr();
                alloc.deallocate(tmp, n_moved);
            } else {
                difference_type n_moved = end() - _pos;
                pointer tmp = alloc.allocate(n_moved);
                std::__relocate_a(reverse_iterator(_pos),
                                  reverse_iterator(begin()),
                                  tmp, alloc);
                auto cur = reverse_iterator(_pos);
                _first = std::reverse_iterator(_first);
                _last = std::reverse_iterator(_last);
                for (; _first < _last; _first++, cur++) {
                    if (cur == begin()) {
                        if (end().get_ptr() == end_of_storage) {
                            cur = --end();
                        } else {
                            start--;
                            cur = --begin();
                        }
                    }
                    *cur = *_first;
                }
                const_pointer moved_end = tmp + n_moved;
                for (; tmp < moved_end; tmp++, cur++) {
                    if (cur == begin()) {
                        if (end().get_ptr() == end_of_storage) {
                            cur = --end();
                        } else {
                            start--;
                            cur = --begin();
                        }
                    }
                    *cur = *tmp;
                }
                start = (cur + 1).get_ptr();
                alloc.deallocate(tmp, n_moved);
            }
        };

        void insert(const_iterator _pos, size_type _n, const value_type& _value)
        {
            if (finish == end_of_storage)
                resize(capacity() + _n);
            if (_pos - begin() >= end() - _pos) {
                difference_type n_moved = end() - _pos;
                pointer tmp = alloc.allocate(n_moved);
                std::__relocate_a(_pos,
                                  end(),
                                  tmp, alloc);
                auto cur = _pos;
                for (auto filled = 0; filled < _n; filled++, cur++) {
                    if (cur == end()) {
                        if (end().get_ptr() == end_of_storage) {
                            cur = begin();
                        } else {
                            finish++;
                            cur = --end();
                        }
                    }
                    *cur = _value;
                }
                const_pointer moved_end = tmp + n_moved;
                for (; tmp < moved_end; tmp++, cur++) {
                    if (end().get_ptr() == end_of_storage) {
                        cur = begin();
                    } else {
                        finish++;
                        cur = --end();
                    }
                    *cur = *tmp;
                }
                finish = cur.get_ptr();
                alloc.deallocate(tmp, n_moved);
            } else {
                difference_type n_moved = end() - _pos;
                pointer tmp = alloc.allocate(n_moved);
                std::__relocate_a(const_reverse_iterator(_pos),
                                  reverse_iterator(begin()),
                                  tmp, alloc);
                auto cur = *const_cast<iterator*>(&_pos);
                for (auto filled = 0; filled < _n; filled++, --_pos) {
                    if (cur == begin()) {
                        if (end().get_ptr() == end_of_storage) {
                            cur = --end();
                        } else {
                            start--;
                            cur = --begin();
                        }
                    }
                    *cur = _value;
                }
                const_pointer moved_end = tmp + n_moved;
                for (; tmp < moved_end; tmp++, cur++) {
                    if (cur == begin()) {
                        if (end().get_ptr() == end_of_storage) {
                            cur = --end();
                        } else {
                            start--;
                            cur = --begin();
                        }
                    }
                    *cur = *tmp;
                }
                start = (++cur).get_ptr();
                alloc.deallocate(tmp, n_moved);
            }
        };

        virtual void insert(const_iterator _pos, const value_type& _value)
        {
            if (finish == end_of_storage)
                resize(capacity() + 1);
            if (_pos - begin() >= end() - _pos) {
                if (finish == end_of_storage) {
                    start = (++begin()).get_ptr();
                } else {
                    finish++;
                }
                auto r_pos = const_reverse_iterator(_pos);
                for (auto cur = rbegin(); cur != r_pos; cur++) {
                    std::swap(*cur, *(cur + 1));
                }
                *_pos = _value;
            } else {
                if (finish == end_of_storage) {
                    start = (--end()).get_ptr();
                } else {
                    if (start != start_of_storage)
                        start--;
                    else
                        start = end_of_storage - 1;
                }
                for (auto cur = begin(); cur != _pos - 1; ++cur) {
                    std::swap(*cur, *(cur + 1));
                }
                *(_pos - 1) = _value;
            }
        }

        iterator insert(const_iterator _pos, std::initializer_list<value_type> _list)
        {
            insert(_pos, _list.begin(), _list.end());
        }

        void push_back(const value_type& _value)
        {
            if (finish == end_of_storage)
                resize(capacity() + 1);
            if (finish == end_of_storage) {
                *begin() = _value;
                start = (++begin()).get_ptr();
            } else {
                finish++;
                *(--end()) = _value;
            }
        }

        void push_front(const value_type& _value)
        {
            if (finish == end_of_storage)
                resize(capacity() + 1);
            if (finish == end_of_storage) {
                *(--end()) = _value;
                start = (--end()).get_ptr();
            } else {
                if (start != start_of_storage)
                    start--;
                else
                    start = end_of_storage - 1;
                *begin() = _value;
            }
        }

        void pop_back()
        {
            if (!empty()) {
                std::_Destroy((--end()).get_ptr());
                finish = (--end()).get_ptr();
            }
        }

        void pop_front()
        {
            if (!empty()) {
                std::_Destroy(start);
                start = (++begin()).get_ptr();
            }
        }

        iterator erase(const_iterator _pos)
        {
            std::_Destroy(_pos.get_ptr());
            if (_pos - begin() >= end() - _pos) {
                for (auto cur = begin(); cur != _pos; cur++) {
                    std::swap(*cur, *(cur + 1));
                }
                if (finish == end_of_storage) {
                    finish = (--end()).get_ptr();
                } else {
                    finish--;
                }
            } else {
                const_iterator st_post = begin();
                for (auto cur = *const_cast<iterator*>(&_pos); cur != st_post; cur--) {
                    std::swap(*cur, *(cur - 1));
                }
                if (finish == end_of_storage) {
                    finish = start;
                    start = (++begin()).get_ptr();
                } else {
                    if (start != end_of_storage - 1) {
                        finish = start;
                        start++;
                    }
                    else {
                        start = start_of_storage;
                    }
                }
            }

            return _pos;
        }

        void swap(CircularBufferExt& _x) noexcept
        {
            std::swap(start, _x.start);
            std::swap(finish, _x.finish);
            std::swap(start_of_storage, _x.start_of_storage);
            std::swap(end_of_storage, _x.end_of_storage);
            std::swap(alloc, _x.alloc);
        }

        void clear() noexcept
        {
            alloc.deallocate(start_of_storage, end_of_storage - start_of_storage);
            start_of_storage = start = end_of_storage = finish = pointer();
        }

        // Friend operators for iterator

        friend class CBuffIterator<pointer>;

        template<typename Ptr>
        friend CBuffIterator<Ptr> operator+(const CBuffIterator<Ptr>& _it, CBuffIterator<Ptr>::difference_type _offset) noexcept;

        template<typename Ptr>
        friend CBuffIterator<Ptr> operator+(CBuffIterator<Ptr>::difference_type _offset, CBuffIterator<Ptr> _it) noexcept;

        template<typename Ptr>
        friend CBuffIterator<Ptr> operator-(CBuffIterator<Ptr> _it, CBuffIterator<Ptr>::difference_type _offset) noexcept;

        template<typename Ptr>
        friend CBuffIterator<Ptr>::difference_type operator-(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept;

        template<typename Ptr>
        friend bool operator==(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept;

        template<typename Ptr>
        friend bool operator!=(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept;

        template<typename Ptr>
        friend bool operator>(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept;

        template<typename Ptr>
        friend bool operator<(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept;

        template<typename Ptr>
        friend bool operator>=(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept;

        template<typename Ptr>
        friend bool operator<=(const CBuffIterator<Ptr>& _left, const CBuffIterator<Ptr>& _right) noexcept;

        // Self operators

        friend bool operator==(const CircularBufferExt& _left, const CircularBufferExt& _right) noexcept
        {
            if (_left.size() != _right.size())
                return false;
            auto it_left = _left.begin();
            auto it_right = _right.begin();
            for (; it_left < _left.end(); it_left++, it_right++)
            {
                if (*it_left != *it_right)
                    return false;
            }

            return true;
        }

        friend bool operator!=(const CircularBufferExt& _left, const CircularBufferExt& _right) noexcept
        {
            if (_left.size() != _right.size())
                return true;
            auto it_left = _left.begin();
            auto it_right = _right.begin();
            for (; it_left < _left.end(); it_left++, it_right++)
            {
                if (*it_left != *it_right)
                    return true;
            }

            return false;
        }

        friend bool operator>(const CircularBufferExt& _left, const CircularBufferExt& _right) noexcept
        {
            auto it_left = _left.begin();
            auto it_right = _right.begin();
            for (; it_left < _left.end() && it_right < _right.end(); it_left++, it_right++)
            {
                if (*it_left > *it_right)
                    return true;
                if (*it_left < *it_right)
                    return false;
            }

            return _left.size() > _right.size();
        }

        friend bool operator<(const CircularBufferExt& _left, const CircularBufferExt& _right) noexcept
        {
            auto it_left = _left.begin();
            auto it_right = _right.begin();
            for (; it_left < _left.end() && it_right < _right.end(); ++it_left, ++it_right)
            {
                if (*it_left < *it_right)
                    return true;
                if (*it_left > *it_right)
                    return false;
            }

            return _left.size() < _right.size();
        }

        friend bool operator>=(const CircularBufferExt& _left, const CircularBufferExt& _right) noexcept
        {
            auto it_left = _left.begin();
            auto it_right = _right.begin();
            for (; it_left < _left.end() && it_right < _right.end(); it_left++, it_right++)
            {
                if (*it_left > *it_right)
                    return true;
                if (*it_left < *it_right)
                    return false;
            }

            return _left.size() >= _right.size();
        }

        friend bool operator<=(const CircularBufferExt& _left, const CircularBufferExt& _right) noexcept
        {
            auto it_left = _left.begin();
            auto it_right = _right.begin();
            for (; it_left < _left.end() && it_right < _right.end(); it_left++, it_right++)
            {
                if (*it_left < *it_right)
                    return true;
                if (*it_left > *it_right)
                    return false;
            }

            return _left.size() <= _right.size();
        }

    private:
        // Additional methods

        void _erase_ending(size_type _n)
        {
            auto new_ending = end() - _n;
            std::_Destroy(new_ending, end());
            finish = new_ending.get_ptr();
        }

        void _reallocate_storage_default(size_type _new_size)
        {
            if (_new_size >= size()) {
                pointer new_start = alloc.allocate(_new_size);
                try {
                    std::__uninitialized_default_n_a(new_start + size(), _new_size - size(), alloc);
                }
                catch (...) {
                    alloc.deallocate(new_start, _new_size);
                    throw std::length_error("allocator throw");
                }
                finish = std::__relocate_a(begin(),
                                           end(),
                                           new_start, alloc);
                alloc.deallocate(start_of_storage, end_of_storage - start_of_storage);
                start = start_of_storage = new_start;
                end_of_storage = new_start + _new_size;
            } else {
                pointer new_start = alloc.allocate(_new_size);
                finish = std::__relocate_a(begin(),
                                           begin() + _new_size,
                                           new_start, alloc);
                alloc.deallocate(start_of_storage, end_of_storage - start_of_storage);
                start = start_of_storage = new_start;
                end_of_storage = new_start + _new_size;
            }
        }

        void _reallocate_storage_filled(size_type _new_size, const value_type& _value)
        {
            if (_new_size >= size()) {
                pointer new_start = alloc.allocate(_new_size);
                try {
                    std::__uninitialized_fill_n_a(new_start + size(), _new_size - size(), _value, alloc);
                }
                catch (...) {
                    alloc.deallocate(new_start, _new_size);
                    throw std::length_error("allocator throw");
                }
                std::__relocate_a(begin(),
                                  end(),
                                  new_start, alloc);
                alloc.deallocate(start_of_storage, end_of_storage - start_of_storage);
                start = start_of_storage = new_start;
                finish = end_of_storage = new_start + _new_size;
            } else {
                pointer new_start = alloc.allocate(_new_size);
                finish = std::__relocate_a(begin(),
                                           begin() + _new_size,
                                           new_start, alloc);
                alloc.deallocate(start_of_storage, end_of_storage - start_of_storage);
                start = start_of_storage = new_start;
                end_of_storage = new_start + _new_size;
            }
        }
    };
}

#endif
