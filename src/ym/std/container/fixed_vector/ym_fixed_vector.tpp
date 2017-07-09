#include <ym_macros.h>
#include <ym_algorithm.h>
#include <type_traits>

#ifdef YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
#error "YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE already defined, this macro is used for internal purposes"
#endif
#ifdef YM_FIXED_VECTOR_PROTOTYPE
#error "YM_FIXED_VECTOR_PROTOTYPE already defined, this macro is used for internal purposes"
#endif

#define YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE \
template <class T, class Allocator>
#define YM_FIXED_VECTOR_PROTOTYPE \
ym::fixed_vector<T, Allocator>

// Todo: Add template specialization that inlines all the functions,
//       in case we are working with the stack_frame_allocator

/////////////////////////////////////////////////////////////////////
/// Constructors / Destructors
/////////////////////////////////////////////////////////////////////
YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
YM_FIXED_VECTOR_PROTOTYPE::fixed_vector(size_type cap,
                                        Allocator* allocator)
    : allocator_([=]
                 {
                    YM_ASSERT(allocator, "Allocator cannot be nullptr");
                    return allocator;
                 }())
    , begin_(static_cast<T*>(allocator_->allocate_aligned(sizeof(T) * cap, alignof(T))))
    , end_(begin_)
    , cap_(begin_ + cap)
{

}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
YM_FIXED_VECTOR_PROTOTYPE::fixed_vector(size_type cap,
                                        size_type count,
                                        const_reference value,
                                        Allocator* allocator)
    : allocator_([=]
                 {
                    YM_ASSERT(allocator, "Allocator cannot be nullptr");
                    return allocator;
                 }())
    , begin_(static_cast<T*>(allocator_->allocate_aligned(sizeof(T) * cap, alignof(T))))
    , end_(begin_ + count)
    , cap_(begin_ + cap)
{
    YM_ASSERT(count <= cap, "Count is more than capacity");

    ym::uninitialized_fill(begin_,
                           end_,
                           value);
}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
YM_FIXED_VECTOR_PROTOTYPE::fixed_vector(const fixed_vector& rhs) YM_NOEXCEPT
    : fixed_vector(rhs, rhs.allocator_)
{

}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
YM_FIXED_VECTOR_PROTOTYPE::fixed_vector(const fixed_vector& rhs,
                                        Allocator* allocator) YM_NOEXCEPT
    : allocator_([=]
                 {
                    YM_ASSERT(allocator, "Allocator cannot be nullptr");
                    return allocator;
                 }())
    , begin_(static_cast<T*>(allocator_->allocate_aligned(sizeof(T) * rhs.capacity(), alignof(T))))
    , end_(ym::uninitialized_copy(rhs.begin_,
                                  rhs.end_,
                                  begin_))
    , cap_(begin_ + rhs.capacity())
{

}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
YM_FIXED_VECTOR_PROTOTYPE::fixed_vector(fixed_vector&& rhs) YM_NOEXCEPT
    : allocator_(rhs.allocator_)
    , begin_(rhs.begin_)
    , end_(rhs.end_)
    , cap_(rhs.cap_)
{
    rhs.begin_ = nullptr;
    rhs.end_ = nullptr;
    rhs.cap_ = nullptr;
}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
YM_FIXED_VECTOR_PROTOTYPE::fixed_vector(fixed_vector&& rhs,
                                        Allocator* allocator) YM_NOEXCEPT
    : allocator_([=]
                 {
                    YM_ASSERT(allocator, "Allocator cannot be nullptr");
                    return allocator;
                 }())
    , begin_(static_cast<T*>(allocator_->allocate_aligned(sizeof(T) * rhs.capacity(), alignof(T))))
    , end_(ym::uninitialized_move(rhs.begin_,
                                  rhs.end_,
                                  begin_))
    , cap_(begin_ + rhs.capacity())
{

}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
YM_FIXED_VECTOR_PROTOTYPE::~fixed_vector() YM_NOEXCEPT
{
    clear();
    allocator_->deallocate_aligned(begin_, sizeof(T) * capacity(), alignof(T));
}

/////////////////////////////////////////////////////////////////////
/// Element Access
/////////////////////////////////////////////////////////////////////
YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
typename
YM_FIXED_VECTOR_PROTOTYPE::reference
YM_FIXED_VECTOR_PROTOTYPE::operator[](size_type pos) YM_NOEXCEPT
{
    YM_ASSERT(pos < size(), "Out of bounds");
    return begin_[pos];
}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
typename
YM_FIXED_VECTOR_PROTOTYPE::const_reference
YM_FIXED_VECTOR_PROTOTYPE::operator[](size_type pos) const YM_NOEXCEPT
{
    YM_ASSERT(pos < size(), "Out of bounds");
    return begin_[pos];
}

/////////////////////////////////////////////////////////////////////
/// Iterators
/////////////////////////////////////////////////////////////////////
YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
typename
YM_FIXED_VECTOR_PROTOTYPE::iterator
YM_FIXED_VECTOR_PROTOTYPE::begin() YM_NOEXCEPT
{
    return begin_;
}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
typename
YM_FIXED_VECTOR_PROTOTYPE::const_iterator
YM_FIXED_VECTOR_PROTOTYPE::begin() const YM_NOEXCEPT
{
    return begin_;
}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
typename
YM_FIXED_VECTOR_PROTOTYPE::const_iterator
YM_FIXED_VECTOR_PROTOTYPE::cbegin() const YM_NOEXCEPT
{
    return begin_;
}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
typename
YM_FIXED_VECTOR_PROTOTYPE::iterator
YM_FIXED_VECTOR_PROTOTYPE::end() YM_NOEXCEPT
{
    return end_;
}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
typename
YM_FIXED_VECTOR_PROTOTYPE::const_iterator
YM_FIXED_VECTOR_PROTOTYPE::end() const YM_NOEXCEPT
{
    return end_;
}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
typename
YM_FIXED_VECTOR_PROTOTYPE::const_iterator
YM_FIXED_VECTOR_PROTOTYPE::cend() const YM_NOEXCEPT
{
    return end_;
}

/////////////////////////////////////////////////////////////////////
/// Capacity
/////////////////////////////////////////////////////////////////////
YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
bool
YM_FIXED_VECTOR_PROTOTYPE::empty() const YM_NOEXCEPT
{
    return begin_ == end_;
}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
typename
YM_FIXED_VECTOR_PROTOTYPE::size_type
YM_FIXED_VECTOR_PROTOTYPE::size() const YM_NOEXCEPT
{
    return end_ - begin_;
}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
typename
YM_FIXED_VECTOR_PROTOTYPE::size_type
YM_FIXED_VECTOR_PROTOTYPE::capacity() const YM_NOEXCEPT
{
    return cap_ - begin_;
}

/////////////////////////////////////////////////////////////////////
/// Modifiers
/////////////////////////////////////////////////////////////////////
YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
void
YM_FIXED_VECTOR_PROTOTYPE::clear() YM_NOEXCEPT
{
    if constexpr (!std::is_trivially_destructible_v<T>)
        ym::destruct(begin_, end_);

    end_ = begin_;
}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
void
YM_FIXED_VECTOR_PROTOTYPE::push_back(const_reference value) YM_NOEXCEPT
{
    YM_ASSERT(size() < capacity(), "Allocating more than possible");
    ym::construct(end_++, value);
}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
void
YM_FIXED_VECTOR_PROTOTYPE::push_back(value_type&& value) YM_NOEXCEPT
{
    YM_ASSERT(size() < capacity(), "Allocating more than possible");
    ym::construct(end_++, std::move(value));
}

YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
void
YM_FIXED_VECTOR_PROTOTYPE::pop_back() YM_NOEXCEPT
{
    YM_ASSERT(size() != ym::size_t(0), "Popping an empty fixed vector");
    ym::destruct(--end_);
}

#undef YM_FIXED_VECTOR_TEMPLATE_PROTOTYPE
#undef YM_FIXED_VECTOR_PROTOTYPE
