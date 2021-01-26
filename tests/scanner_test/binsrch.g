
/*
    This is a mock binary search implementation in glang.
    It probably will not work, but it demonstrates real code
    written in the language.
*/

class enter {

    int first, last, mid
    string find
    list find_list
    constructor(string, list)
}

enter.constructor(find, find_list) {

    first = 0
    last = find_list.length()
    mid = (first + last) / 2

    while(first <= last) {
        if(find_list[mid] < find) {
            first = mid - 1
        }
        else(find_list[mid] > find) {
            last = mid-1
        }
        else {
            // the item was found
            print("item was found")
            break
        }
        mid = (first + last) / 2
    }

    print("item was not found")
}