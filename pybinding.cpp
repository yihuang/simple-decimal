#include "decimal.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(cdecimal, m) {
    py::class_<Decimal>(m, "Decimal")
        .def(py::init<const std::string &>())
        .def("__eq__",  &Decimal::operator==)
        .def("__lt__",  &Decimal::operator<)
        .def("__add__", &Decimal::operator+)
        .def("__sub__", &Decimal::operator-)
        .def("__mul__", &Decimal::operator*)
        .def("__div__", &Decimal::operator/)
        .def("__str__", (std::string (Decimal::*)(void) const)  &Decimal::to_string);
}
