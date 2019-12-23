import pytest
from test_move_arg_bp import Item, access, access_ptr, consume


def test():
    item = Item(42)
    other = item
    print(item)
    access(item)
    consume(item)
    print("back in python")

    try:
        access_ptr(item)
        access(item)
    except Exception as e:
        print(e)

    del item

    try:
        print(other)
    except Exception as e:
        print(e)

if __name__ == "__main__":
    test()
