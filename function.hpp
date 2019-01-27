#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <array>
#include <cstddef>
#include <memory>
#include <utility>
#include <variant>

template<typename>
class function;

template<typename Return, typename ...Args>
class function<Return(Args...)> {

private:

    class holder_base;

    template<typename Functor>
    class functor_holder;

    typedef std::unique_ptr<holder_base> big_t;
    static std::size_t const SMALL_SIZE = 16;
    typedef std::array<std::byte, SMALL_SIZE> small_t;

    mutable std::variant<small_t, big_t> _invoker;

    holder_base *reinterpret_small() const {
        return reinterpret_cast<holder_base *>(std::get<small_t>(_invoker).data());
    }

public:

    function() noexcept : _invoker(nullptr) {}

    function(nullptr_t) noexcept : _invoker(nullptr) {}

    ~function() {}

    template<typename Functor>
    function(Functor functor) : _invoker() {
        if (std::is_nothrow_move_constructible_v<Functor> && sizeof(functor_holder<Functor>) <= SMALL_SIZE) {
            _invoker = small_t();
            new(std::get<small_t>(_invoker).data()) functor_holder<Functor>(functor);
        } else {
            _invoker = std::make_unique<functor_holder<Functor>>(functor);
        }
    }

    function(function const &other) : _invoker() {
        if (std::holds_alternative<small_t>(other._invoker)) {
            _invoker = small_t();
            other.reinterpret_small()->copy()->move_to(std::get<small_t>(_invoker));
        } else {
            _invoker = std::get<big_t>(other._invoker)->copy();
        }
    }

    function(function &&other) : _invoker() {
        if (std::holds_alternative<small_t>(other._invoker)) {
            _invoker = small_t();
            other.reinterpret_small()->move_to(std::get<small_t>(_invoker));
        } else {
            _invoker = std::move(other._invoker);
        }
    }

    function &operator=(function const &other) {
        function(other).swap(*this);
        return *this;
    }

    function &operator=(function &&other) {
        function(other).swap(*this);
        return *this;
    }

    Return operator()(Args &&...args) {
        if (std::holds_alternative<small_t>(_invoker)) {
            return reinterpret_small()->invoke(std::forward<Args>(args)...);
        } else {
            return std::get<big_t>(_invoker)->invoke(std::forward<Args>(args)...);
        }
    }

    operator bool() const noexcept {
        return std::holds_alternative<small_t>(_invoker) || std::get<big_t>(_invoker) != nullptr;
    }

    void swap(function &other) noexcept {
        if (std::holds_alternative<small_t>(_invoker) && std::holds_alternative<small_t>(other._invoker)) {
            small_t dummy;
            reinterpret_small()->move_to(dummy);
            other.reinterpret_small()->move_to(std::get<small_t>(_invoker));
            reinterpret_cast<holder_base *>(dummy.data())->move_to(std::get<small_t>(other._invoker));
        } else if (std::holds_alternative<small_t>(_invoker)) {
            big_t rhs = other ? std::get<big_t>(other._invoker)->copy() : nullptr;
            other._invoker = small_t();
            reinterpret_small()->move_to(std::get<small_t>(other._invoker));
            _invoker = std::move(rhs);
        } else if (std::holds_alternative<small_t>(other._invoker)) {
            big_t lhs = *this ? std::get<big_t>(_invoker)->copy() : nullptr;
            _invoker = small_t();
            other.reinterpret_small()->move_to(std::get<small_t>(_invoker));
            other._invoker = std::move(lhs);
        } else {
            std::swap(_invoker, other._invoker);
        }
    }

private:

    class holder_base {
    public:
        holder_base() = default;

        virtual ~holder_base() = default;

        virtual Return invoke(Args &&...args) = 0;

        virtual std::unique_ptr<holder_base> copy() const = 0;

        virtual void move_to(small_t &location) const = 0;
    };

    template<typename Functor>
    class functor_holder : public holder_base {
    public:
        explicit functor_holder(Functor functor) : holder_base(), _functor(functor) {}

        explicit functor_holder(std::remove_reference_t<Functor> &&functor) : holder_base(), _functor(functor) {}

        Return invoke(Args &&...args) override {
            return _functor(std::forward<Args>(args)...);
        }

        std::unique_ptr<holder_base> copy() const override {
            return std::make_unique<functor_holder<Functor>>(_functor);
        }

        void move_to(small_t &location) const override {
            new(location.data()) functor_holder<Functor>(std::move(_functor));
        }

    private:
        Functor _functor;
    };

};

#endif // FUNCTION_HPP
