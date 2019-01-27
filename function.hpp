#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <array>
#include <cstddef>
#include <memory>

template<typename>
class function;

template<typename Return, typename ...Args>
class function<Return(Args...)> {

private:

    class holder_base;

    template<typename Functor>
    class functor_holder;

    typedef std::unique_ptr<holder_base> big_t;
    static std::size_t const SMALL_SIZE = 5 * sizeof(big_t);
    typedef char small_t[SMALL_SIZE];

    struct storage {
        bool _is_small;

        union {
            small_t small_object;
            big_t big_object;
        };

        storage(bool is_small) : _is_small(is_small), big_object() {
            if (_is_small) {
                big_object.~big_t();
            }
        }

        ~storage() {
            if (_is_small) {
                reinterpret_small()->~holder_base();
            }
        }

        storage(storage &&other) : _is_small(other._is_small) {
            if (_is_small) {
                std::copy(other.small_object, other.small_object + SMALL_SIZE, small_object);
            } else {
                big_object = std::move(other.big_object);
            }
            other._is_small = false;
        }

        holder_base const *reinterpret_small() const {
            return reinterpret_cast<holder_base const *>(small_object);
        }

        holder_base *reinterpret_small() {
            return reinterpret_cast<holder_base *>(small_object);
        }

        holder_base const *get() const {
            return _is_small ? reinterpret_small() : big_object.get();
        }

        holder_base *get() {
            return _is_small ? reinterpret_small() : big_object.get();
        }
    } _invoker;

public:

    function() noexcept : _invoker(false) {}

    function(nullptr_t) noexcept : _invoker(false) {}

    ~function() {}

    template<typename Functor>
    function(Functor functor) : _invoker(sizeof(functor_holder<Functor>) <= SMALL_SIZE) {
        if (_invoker._is_small) {
            new(_invoker.small_object) functor_holder<Functor>(functor);
        } else {
            _invoker.big_object = std::make_unique<functor_holder<Functor>>(functor);
        }
    }

    function(function const &other) : _invoker(other._invoker._is_small) {
        if (_invoker._is_small) {
            other._invoker.reinterpret_small()->place_copy(_invoker.small_object);
        } else {
            _invoker.big_object = other._invoker.big_object->copy();
        }
    }

    function(function &&other) : _invoker(std::move(other._invoker)) {}

    function &operator=(function const &other) {
        function(other).swap(*this);
        return *this;
    }

    function &operator=(function &&other) {
        function(other).swap(*this);
        return *this;
    }

    Return operator()(Args ...args) {
        _invoker.get()->invoke(args...);
    }

    operator bool() const noexcept {
        return _invoker.get() != nullptr;
    }

    void swap(function &other) {
        std::swap(_invoker._is_small, other._invoker._is_small);
        // I think it's better not to do this, but for now it's ok
        std::swap(_invoker.small_object, other._invoker.small_object);
    }

private:

    class holder_base {
    public:
        holder_base() = default;

        virtual ~holder_base() = default;

        virtual Return invoke(Args ...args) = 0;

        virtual std::unique_ptr<holder_base> copy() const = 0;

        virtual void place_copy(small_t &location) const = 0;
    };

    template<typename Functor>
    class functor_holder : public holder_base {
    public:
        explicit functor_holder(Functor functor) : holder_base(), _functor(functor) {}

        Return invoke(Args ...args) override {
            return _functor(args...);
        }

        std::unique_ptr<holder_base> copy() const override {
            return std::make_unique<functor_holder<Functor>>(_functor);
        }

        void place_copy(small_t &location) const override {
            new(location) functor_holder<Functor>(_functor);
        }

    private:
        Functor _functor;
    };

};

#endif // FUNCTION_HPP
