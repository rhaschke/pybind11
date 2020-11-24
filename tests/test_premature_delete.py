# -*- coding: utf-8 -*-
import pytest

from pybind11_tests import premature_delete as m


def test_pointee_and_ptr_owner():
    m.to_cout("")
    obj = m.pointee()
    assert obj.get_int() == 213
    owner = m.ptr_owner(obj)
    with pytest.raises(RuntimeError) as exc_info:
        obj.get_int()
    assert str(exc_info.value) == "Invalid object instance"
    assert owner.is_owner()
    m.to_cout("before give up")
    if 1:  # Behavior is the same with 0 or 1 here.
        reclaimed = owner.give_up_ownership_via_unique_ptr()
    else:
        reclaimed = owner.give_up_ownership_via_shared_ptr()
    m.to_cout("after give up")
    assert not owner.is_owner()
    if 0:
        # This is desired.
        assert reclaimed.get_int() == 213
    else:
        # But obj is actually disowned.
        with pytest.raises(RuntimeError) as exc_info:
            obj.get_int()
        assert str(exc_info.value) == "Invalid object instance"
    m.to_cout("before del")
    del reclaimed
    m.to_cout("after del")
    m.to_cout("")
