#include "xchainer/python/device.h"

#include <memory>
#include <sstream>

#include "xchainer/backend.h"
#include "xchainer/device.h"

#include "xchainer/python/common.h"

namespace xchainer {

namespace py = pybind11;  // standard convention

class PyDeviceScope {
public:
    explicit PyDeviceScope(Device target) : target_(target) {}
    void Enter() { scope_ = std::make_unique<DeviceScope>(target_); }
    void Exit(py::args args) {
        (void)args;  // unused
        scope_.reset();
    }

private:
    // TODO(beam2d): better to replace it by "optional"...
    std::unique_ptr<DeviceScope> scope_;
    Device target_;
};

void InitXchainerDevice(pybind11::module& m) {
    py::class_<Device>(m, "Device")
        // We uses py::keep_alive because device keeps the pointer to the backend through its lifetime
        .def(py::init<const std::string&, Backend*>(), py::keep_alive<1, 3>())
        .def("__eq__", py::overload_cast<const Device&, const Device&>(&operator==))
        .def("__ne__", py::overload_cast<const Device&, const Device&>(&operator!=))
        .def("__repr__", &Device::ToString)
        .def_property_readonly("name", &Device::name)
        .def_property_readonly("backend", &Device::backend);

    m.def("get_default_device", []() { return GetDefaultDevice(); });
    m.def("set_default_device", [](const Device& device) { SetDefaultDevice(device); });

    py::class_<PyDeviceScope>(m, "DeviceScope").def("__enter__", &PyDeviceScope::Enter).def("__exit__", &PyDeviceScope::Exit);
    m.def("device_scope", [](Device device) { return PyDeviceScope(device); });
}

}  // namespace xchainer
