#include <cstdio>
#include <utility>
#include <typeinfo>
#include <type_traits>
#include <pybind11/detail/typeid.h>

struct copy_move {
	copy_move() = default;
	copy_move(const copy_move& other) { printf("copy"); }
	copy_move(copy_move&& other) { printf("move"); }
};

struct no_copy {
	no_copy() = default;
	no_copy(const no_copy& other) = delete;
	no_copy(no_copy&& other) { printf("move"); }
};

struct no_move {
	no_move() = default;
	no_move(const no_move& other) { printf("copy"); }
	no_move(no_move&& other) = delete;
};

struct no_copy_move {
	no_copy_move() = default;
	no_copy_move(const no_copy_move& other) = delete;
	no_copy_move(no_copy_move&& other) = delete;
};

template <typename T>
struct caster {
	T value;
	caster() { printf("\ncaster<%s>:\n", pybind11::type_id<T>().c_str()); }
	operator T*() { return &value; }
	operator T&() { return value; }
	operator T&&() { return std::move(value); }
};

/// Helper template to strip away type modifiers
template <typename T> struct intrinsic_type                       { using type = T; };
template <typename T> struct intrinsic_type<const T>              { using type = typename intrinsic_type<T>::type; };
template <typename T> struct intrinsic_type<T*>                   { using type = typename intrinsic_type<T>::type; };
template <typename T> struct intrinsic_type<T&>                   { using type = typename intrinsic_type<T>::type; };
template <typename T> struct intrinsic_type<T&&>                  { using type = typename intrinsic_type<T>::type; };
template <typename T, size_t N> struct intrinsic_type<const T[N]> { using type = typename intrinsic_type<T>::type; };
template <typename T, size_t N> struct intrinsic_type<T[N]>       { using type = typename intrinsic_type<T>::type; };
template <typename T> using intrinsic_t = typename intrinsic_type<T>::type;

template <typename T>
using cast_op_type =
	std::conditional_t<std::is_pointer<typename std::remove_reference<T>::type>::value,
		typename std::add_pointer<intrinsic_t<T>>::type,
		std::conditional_t<std::is_rvalue_reference<T>::value,
			typename std::add_rvalue_reference<intrinsic_t<T>>::type,
			typename std::add_lvalue_reference<intrinsic_t<T>>::type>>;

static_assert(std::is_same<cast_op_type<copy_move&&>, copy_move&&>::value, "rvalue reference will be moved to new T");
static_assert(std::is_same<cast_op_type<copy_move&>, copy_move&>::value, "lvalue reference");
static_assert(std::is_same<cast_op_type<copy_move>, copy_move&>::value, "lvalue reference");
static_assert(std::is_same<cast_op_type<no_move&&>, no_move&&>::value, "rvalue reference will be moved to new T");

template <typename T>
cast_op_type<T> cast_op(caster<intrinsic_t<T>> &caster) {
    return caster.operator cast_op_type<T>();
}

template <typename ARG, typename CAST = ARG>
void consume(ARG o) { printf(" %s(%s) from %s\n", __FUNCTION__, pybind11::full_type_id<ARG>().c_str(), pybind11::full_type_id<CAST>().c_str()); }

#define TEST(CASTER, FUNC_ARG) consume<FUNC_ARG, cast_op_type<FUNC_ARG>>(cast_op<FUNC_ARG>(CASTER))

int main(int argc, char const *argv[])
{
	copy_move x;
	consume(x);
	consume(std::move(x));

	no_copy y; printf("\n");
	// consume(y);  // cannot copy
	consume(std::move(y));

	no_move z; printf("\n");
	consume(z);
	// consume(std::move(z));  // cannot move

	caster<copy_move> cm;
	TEST(cm, copy_move);
	TEST(cm, copy_move&&);
	TEST(cm, copy_move&);

	caster<no_copy> nc;
	// TEST(nc, no_copy);  // cannot copy
	TEST(nc, no_copy&&);
	TEST(nc, no_copy&);

	caster<no_move> nm;
	TEST(nm, no_move);
	// TEST(nm, no_move&&);  // cannot move
	TEST(nm, no_move&);

	return 0;
}
