#include "test_move_arg.h"
#include <pybind11/pybind11.h>
#include <pybind11/iostream.h>
#include <sstream>

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(T, std::auto_ptr<T>)

PYBIND11_MODULE(test_move_arg, m) {
	py::class_<Item, std::auto_ptr<Item>>(m, "Item")
		.def(py::init<int>(), py::call_guard<py::scoped_ostream_redirect>())
		.def("__repr__", [](const Item& item) {
			std::stringstream ss;
			ss << "py " << item;
			return ss.str();
		}, py::call_guard<py::scoped_ostream_redirect>());

	m.def("access", [](const Item& item) {
		std::cout << "access " << item << "\n";
	}, py::call_guard<py::scoped_ostream_redirect>());

#if 0 // rvalue arguments fail during compilation
	m.def("consume", [](Item&& item) {
		std::cout << "consume " << item << "\n  ";
		Item sink(std::move(item));
		std::cout << "  old: " << item << "\n  new: " << sink << "\n";
	}, py::call_guard<py::scoped_ostream_redirect>());
#endif

#if 0 // the boost::python trick, using auto_ptr's release() doesn't compile either
	m.def("consume", [](std::auto_ptr<Item>& item) {
		std::cout << "consume " << *item << "\n  ";
		Item sink(std::move(*item.release()));
		std::cout << "  old: " << item.get() << "\n  new: " << sink << "\n";
	}, py::call_guard<py::scoped_ostream_redirect>());
#endif
	m.def("working", [](int&& a) { std::cout << a << "\n"; },
	      py::call_guard<py::scoped_ostream_redirect>());
}
