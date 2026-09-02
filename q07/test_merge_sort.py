from merge_sort import merge_sort

def test_given_input():
    assert merge_sort([3, 1, 4, 1, 5, 9, 2, 6]) == [1, 1, 2, 3, 4, 5, 6, 9]

def test_duplicate_elements():
    assert merge_sort([2, 2, 1, 3, 1, 2, 1]) == [1, 1, 1, 2, 2, 2, 3]
