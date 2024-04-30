# from pprint import pprint
from string import digits, ascii_letters

# input = [
#     {"chars": list("0123456789"),
#      "count": (0, -1)},
#     {"chars": list("a"),
#      "count": (1, 4)},
#     {"chars": list("b"),
#      "count": (1, 1)},
# ]


def get_nfa(rules):
    result = {
        1: {
            "can_skip": False,
            -1: ["_other"],
        }
    }

    for rule in rules:
        count = 0

        if rule["count"][1] == -1:
            count = rule["count"][0]
        else:
            count = rule["count"][1]

        count = max(1, count)

        for j in range(count):
            result[max(result) + 1] = {-1: ["_other"],
                                       "can_skip": j >= rule["count"][0]}
            result[max(result) - 1][max(result)] = rule["chars"]

        j = max(result) - count

        while j > 1 and result[j]["can_skip"] is True:
            result[j-1][max(result) - count + 1] = rule["chars"]
            j -= 1

        if rule["count"][1] == -1:
            # result[max(result) + 1] = {-1: ["_other"],
            #                            "can_skip": False}
            # result[max(result) - 1][max(result)] = rule["chars"]
            result[max(result)][max(result)] = rule["chars"]

    j = max(result)

    while j > 1 and result[j]["can_skip"] is True:
        result[j-1].pop(-1)
        result[j-1][0] = ["_other"]
        j -= 1

    result[max(result)].pop(-1)
    result[max(result)][0] = ["_other"]

    return result


def compile_regex(string):
    id = 0

    mode = 0  # 0 - normal, 1 - list

    chars = []

    result = []

    while id < len(string):
        c = string[id]

        if mode == 0:
            if c == '[':
                mode = 1
            elif c == '+':
                result[-1]["count"][0] = 1
                result[-1]["count"][1] = -1
            elif c == '*':
                result[-1]["count"][0] = 0
                result[-1]["count"][1] = -1
            elif c == '?':
                result[-1]["count"][0] = 0
                result[-1]["count"][1] = 1
            elif c in list("]"):
                print("Error parsing", string, f"unexpected '{c}'")
                return None
            elif c == '\\':
                id += 1
                c = string[id]
                result.append({"chars": [c], "count": [1, 1]})
            else:
                result.append({"chars": [c], "count": [1, 1]})
        else:
            if c == ']':
                result.append({"chars": chars, "count": [1, 1]})
                chars = []
                mode = 0
            elif c in list("+*[?"):
                print("Error parsing", string, f"unexpected '{c}'")
                return None
            else:
                chars.append(c)

        id += 1

    return get_nfa(result)


# pprint(compile_regex(r"[-]?[0123456789]+"))
