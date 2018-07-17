
#ifndef ENUMITERATOR_H
#define ENUMITERATOR_H

#include <cassert> // assert
#include <cstddef> // ptrdiff_t, size_t
#include <iterator> // forward_iterator_tag
#include <memory> // unique_ptr
#include <limits> // numeric_limits
#include <type_traits> // {common,underlying}_type, is_enum, add_{pointer,lvalue_reference,const}, conditional
#include <utility> // swap



/**
 * Iterator for an enum of type @c T that starts with an item @c start
 * and ends *after* the item @c stop. Models ForwardIterator concept.
 *
 * @warning Following iterator DOES NOT WORK for non-contiguous enumerations
 *          like "enum class E { A = 100, B = 200, C == 300 }".
 *
 * @see http://en.cppreference.com/w/cpp/concept/ForwardIterator
 */
template <class T, T start, T stop>
class EnumIterator
{

    using raw_type = std::underlying_type_t<T>;  // value_type = T

    /**
     * If [start, stop] spawns [min, max], then past-the-end item is the MAX
     * of size_t or ssize_t depending on the sign of the value_type.
     *
     * Value of stop equal to selected MAX is not allowed!
     *
     * @{ */
    using stored_type =
            std::common_type_t<
                raw_type
              , std::conditional_t<
                    std::is_signed<raw_type>::value
                  , std::ptrdiff_t // FIXME: ssize_t?
                  , std::size_t
                  >
              >;

    static constexpr stored_type makeEnd()
    {
        return std::numeric_limits<stored_type>::max();
    }
    /* @} */

 public:

    /** Creates a singular iterator. */
    EnumIterator() = default;

    explicit EnumIterator(T t)
      : _current{static_cast<stored_type>(t)}
    {}

    // Iterator concept requirements {
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = std::add_pointer_t<value_type>;
    using reference         = std::add_lvalue_reference_t<value_type>;
    using iterator_category = std::forward_iterator_tag;

    // extra two:
    using const_reference   = std::add_lvalue_reference_t<std::add_const_t<value_type>>;
    using const_pointer     = std::add_pointer_t<std::add_const_t<value_type>>;

    EnumIterator(const EnumIterator&)            = default;
    EnumIterator(EnumIterator&&)                 = default;
    EnumIterator& operator=(const EnumIterator&) = default;
    EnumIterator& operator=(EnumIterator&&)      = default;
    ~EnumIterator()                              = default;

    friend void swap(EnumIterator& lhs, EnumIterator& rhs)
    {
        std::swap(lhs._current, rhs._current);
    }
    // }

    // EqualityComparable concept -- no floating points {
    bool operator==(EnumIterator rhs) const { return _current == rhs._current; }
    bool operator!=(EnumIterator rhs) const { return ! (*this == rhs);         }
    // }

    // InputIterator concept {
    reference operator->()             { return operator*(); }
    const_reference operator->() const { return operator*(); }

    EnumIterator& operator++()
    {
        if (makeEnd() != _current)  // no point to advance if at the end
        {
            if (_current < static_cast<stored_type>(stop))
            {
                ++_current;
            }
            else
            {
                _current = makeEnd();
            }
        }

        return *this;
    }

    EnumIterator operator++(int) const
    {
        return (makeEnd() != _current) ? ++(EnumIterator{*this}) : EnumIterator{};
    }
    // }

    EnumIterator begin() const { return EnumIterator{start}; }
    EnumIterator end()   const { return {};      } // value-initialised singular is past-the-end

    reference operator*()
    {
        assert(derefable());

        return *reinterpret_cast<pointer>(&_current);  // FIXME
    }

    const_reference operator*() const
    {
        assert(derefable());

        return *reinterpret_cast<const_pointer>(&_current); // FIXME
    }

 private:

    bool derefable() const
    {
        return
            (*this != end()) // not past-the-end?
         && (_current <= static_cast<stored_type>(std::numeric_limits<raw_type>::max())) // binary conversion possible?
         && (_current >= static_cast<stored_type>(std::numeric_limits<raw_type>::min()))
         && (_current <= static_cast<stored_type>(stop)) // fits into input domain?
         && (_current >= static_cast<stored_type>(start));

    }

    static_assert(std::is_enum<T>::value, "Enum expected");
    static_assert(static_cast<raw_type>(stop) >= static_cast<raw_type>(start), "Invalid range");
    static_assert(static_cast<raw_type>(stop) < makeEnd(), "Past-the-end value impossible");
    static_assert( ! std::is_floating_point<raw_type>::value, "Floating points not allowed");


    // singular guarantee -- initialised to the past-the-end on value-init
    stored_type _current = makeEnd();
};

#endif // ENUMITERATOR_H

