#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <array>
#include <cstddef>
#include <memory>

namespace doreshnikov {

    template<typename>
    class function;

    template<typename ReturnType, typename ...ArgumentTypes>
    class function<ReturnType(ArgumentTypes...)> {

    public:
        function() {

        }
        ~function() {

        }
        template<typename FunctionType>
        function(FunctionType) {

        }

        ReturnType operator()(ArgumentTypes...) {

        }

    private:

        class callable_base {
        public:
            callable_base() = default;
            virtual ~callable_base() = default;
            virtual ReturnType invoke(ArgumentTypes... args) = 0;
        };

        template<typename CallableType>
        class callable_holder : public callable_base {
        public:
            explicit callable_holder(CallableType callable_object) : callable_base(),
                                                                     _callable_object(callable_object) {}
            ReturnType invoke(ArgumentTypes... args) {
                return _callable_object(args...);
            }
        private:
            CallableType _callable_object;
        };

        std::shared_ptr<callable_base>

    };

}

#endif // FUNCTION_HPP
