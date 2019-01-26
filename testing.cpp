#include <iostream>
#include <cassert>
#include "my_function.h"
#include <functional>

int foo() {
    return 1;
}

int bar() {
    return 2;
}

double pi() {
    // kak grubo
    return 3.14;
}

void test_defaultConstructor() {
    my_function<int(void)> f(foo);
    assert(f() == 1);
}

void test_copyConstructor() {
    my_function<int(void)> b(bar);
    my_function<int(void)> second(b);
    assert(second() == 2);
}

void test_nullptrConstructor() {
    my_function<void(void)> f(nullptr);
}

void test_moveConstructor() {
    my_function<int(void)> b(bar);
    my_function<int(void)> second(std::move(b));
    assert(second() == 2);
}

void test_operatorAssignment() {
    my_function<int(void)> b(bar);
    my_function<int(void)> second = b;
    assert(second() == 2);
}

void test_moveAssignment() {
    my_function<int(void)> b(bar);
    my_function<int(void)> second(foo);
    second = std::move(b);
    assert(second() == 2);
}

void test_explicitOperatorBool() {
    my_function<int(void)> f(nullptr);
    assert(!f);
    f = foo;
    assert(f);
}

void test_lambda() {
    int a = 10;
    my_function<int(int)> l = [a](int x) {
        return a + x;
    };
    assert(l(5) == 15);
}

void test_swap() {
    my_function<int()> f(foo);
    my_function<int()> b(bar);
    assert(f() == 1);
    assert(b() == 2);

    f.swap(b);

    assert(f() == 2);
    assert(b() == 1);
}

void test_diffTypes() {
    my_function<int()> f = foo;
    assert(f() == 1);
    f = pi;
    assert(pi() == 3.14);
}

void test_copy() {
    std::vector<int> buffer(100, -1);
    my_function<int()> g;
    {
        my_function<int()> f = [buffer]() {
            return buffer[99];
        };
        g = f;
        my_function<int()> h(f);
        assert(f() == -1);
        assert(g() == -1);
        assert(h() == -1);
    }
    assert(g() == -1);
}

void test_copy_small_object() {
    std::shared_ptr<std::vector<int>> buffer = std::make_shared<std::vector<int>>(100, -1);
    my_function<int()> g;
    {
        my_function<int()> f = [buffer]() {
            return (*buffer)[99];
        };
        g = f;
        my_function<int()> h(f);
        assert(f() == -1);
        assert(g() == -1);
        assert(h() == -1);
    }
    assert(g() == -1);
}



void NIKITOZZZZ_test() {
    // тут хз, мб плохой тест (для решение нужна убрать const после invoke/call/etc)
    int foo = 1;
    double bar = 3;
    double bar2 = 3;
    double bar3 = 3;

    my_function<int (std::ostream &)> f([=](std::ostream &os) mutable {
        os << "test " << foo << " " << bar << std::endl;
        os << "test " << bar2 << " " << bar3 << std::endl;
        foo *= 2;
        foo += 2;
        bar -= 0.1;
        os << "test " << foo << " " << bar << std::endl;
        return foo;
    });

    f(std::cout);
}


void all_test() {
    test_defaultConstructor();
    test_copyConstructor();
    test_nullptrConstructor();
    test_moveConstructor();
    test_operatorAssignment();
    test_moveAssignment();
    test_explicitOperatorBool();
    test_swap();
    test_lambda();
    test_diffTypes();
    test_copy();
    test_copy_small_object();
    NIKITOZZZZ_test();
    std::cout << "OK!";
}

int main() {
    all_test();
    return 0;
}