#include "chainerx/routines/logic.h"

#include "chainerx/array.h"
#include "chainerx/backprop_mode.h"
#include "chainerx/device.h"
#include "chainerx/dtype.h"
#include "chainerx/kernels/logic.h"
#include "chainerx/routines/creation.h"
#include "chainerx/routines/manipulation.h"
#include "chainerx/routines/routines_util.h"
#include "chainerx/routines/type_util.h"
#include "chainerx/shape.h"

namespace chainerx {

namespace {

template <typename KernelType>
class LogicBinaryImpl {
public:
    LogicBinaryImpl() = default;
    void operator()(const Array& x1, const Array& x2, Array& out) const {
        NoBackpropModeScope scope{};
        x1.device().backend().CallKernel<KernelType>(x1, x2, out);
    }
};

template <typename KernelType>
Array LogicBinary(const Array& x1, const Array& x2) {
    if ((x1.dtype() == Dtype::kBool) != (x2.dtype() == Dtype::kBool)) {
        throw DtypeError{"Comparison of ", GetDtypeName(x1.dtype()), " and ", GetDtypeName(x2.dtype()), " is not supported."};
    }
    return internal::BroadcastBinary(LogicBinaryImpl<KernelType>{}, x1, x2, Dtype::kBool);
}

}  // namespace

Array Equal(const Array& x1, const Array& x2) { return LogicBinary<EqualKernel>(x1, x2); }

Array NotEqual(const Array& x1, const Array& x2) { return LogicBinary<NotEqualKernel>(x1, x2); }

Array Greater(const Array& x1, const Array& x2) { return LogicBinary<GreaterKernel>(x1, x2); }

Array GreaterEqual(const Array& x1, const Array& x2) { return LogicBinary<GreaterEqualKernel>(x1, x2); }

Array LogicalNot(const Array& x) {
    Array out = Empty(x.shape(), Dtype::kBool, x.device());
    {
        NoBackpropModeScope scope{};
        x.device().backend().CallKernel<LogicalNotKernel>(x, out);
    }
    return out;
}

Array LogicalAnd(const Array& x1, const Array& x2) { return LogicBinary<LogicalAndKernel>(x1, x2); }

Array LogicalOr(const Array& x1, const Array& x2) { return LogicBinary<LogicalOrKernel>(x1, x2); }

Array LogicalXor(const Array& x1, const Array& x2) { return LogicBinary<LogicalXorKernel>(x1, x2); }

Array All(const Array& a, const OptionalAxes& axis, bool keepdims) {
    Axes sorted_axis = internal::GetSortedAxesOrAll(axis, a.ndim());
    Array out = internal::EmptyReduced(a.shape(), Dtype::kBool, sorted_axis, keepdims, a.device());
    {
        NoBackpropModeScope scope{};
        a.device().backend().CallKernel<AllKernel>(a, sorted_axis, out);
    }
    return out;
}

Array Any(const Array& a, const OptionalAxes& axis, bool keepdims) {
    Axes sorted_axis = internal::GetSortedAxesOrAll(axis, a.ndim());
    Array out = internal::EmptyReduced(a.shape(), Dtype::kBool, sorted_axis, keepdims, a.device());
    {
        NoBackpropModeScope scope{};
        a.device().backend().CallKernel<AnyKernel>(a, sorted_axis, out);
    }
    return out;
}

}  // namespace chainerx
