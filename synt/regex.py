from pprint import pprint


input = [
    {"chars": list("0123456789"),
     "count": (0, -1)},
    {"chars": list("a"),
     "count": (1, 4)},
    {"chars": list("b"),
     "count": (1, 1)},
]


def compile_regex(rules):
    result = {
        1: {
            "can_skip": False,
            -1: ["_other"],
        }
    }

    for id, rule in enumerate(rules):
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
            result[j][max(result) - count + 1] = rule["chars"]
            j -= 1

        if rule["count"][1] == -1:
            # result[max(result) + 1] = {-1: ["_other"],
            #                            "can_skip": False}
            # result[max(result) - 1][max(result)] = rule["chars"]
            result[max(result)][max(result)] = rule["chars"]

    result[max(result)].pop(-1)
    result[max(result)][0] = ["_other"]

    return result


pprint(compile_regex(input))
