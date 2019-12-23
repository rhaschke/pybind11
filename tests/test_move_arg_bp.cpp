#include "test_move_arg.h"
#include <boost/python.hpp>
#include <sstream>

namespace py = boost::python;

std::string item_repr(const Item& item) {
	std::stringstream ss;
	ss << "py " << item;
	return ss.str();
}

void item_access(const Item& item) {
	std::cout << "access " << item << "\n";
}

void item_access_ptr(const std::auto_ptr<Item>& item) {
	std::cout << "access ptr " << item.get() << " ";
	if (item.get()) std::cout << *item;
	std::cout << "\n";
}

void item_consume(std::auto_ptr<Item>& item) {
	std::cout << "consume " << *item << "\n  ";
	Item sink(std::move(*item.release()));
	std::cout << "  old: " << item.get() << "\n  new: " << sink << "\n";
}

BOOST_PYTHON_MODULE(test_move_arg_bp) {
	py::class_<Item, std::auto_ptr<Item>, boost::noncopyable>("Item", py::init<int>())
		.def("__repr__", &item_repr);

	py::def("access", item_access);
	py::def("access_ptr", item_access_ptr);

	py::def("consume", item_consume);
}
