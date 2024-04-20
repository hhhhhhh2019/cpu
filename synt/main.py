from lexer_gen import rules
# from synt_gen import srules

from pprint import pprint


class Token:
    value: str
    type: str

    def __init__(self, value: str, type: str) -> None:
        self.value = value
        self.type = type

    def __str__(self):
        return str(self.type) + ' ' + self.value

    def __repr__(self) -> str:
        return str(self)


string = """
3 * (1 + 2) + 4 * 5
"""

i = 0

token = ""
tokens = []

state = 1

while i < len(string):
    c = string[i]

    nstate = -1
    otherid = -1

    for j in rules[state][0]:
        if "_other" in rules[state][0][j]:
            otherid = j

        if c in rules[state][0][j]:
            nstate = j

    nstate = nstate if nstate != -1 else otherid

    if nstate == -1:
        print("rules error")
        break
    elif nstate == 0:
        tokens.append(Token(token, rules[state][1]))
        token = ""
        state = 1
    else:
        token += c
        i += 1
        state = nstate


tokens = [i for i in tokens if i.type not in ["SPACE", "NEWLINE", "TAB"]] + \
        [Token("$", "EOI")]


# 0 - текущий узел
# 1..n - узлы, в том порядке, в котором они в массиве
# значение при 0 заменяет текущий узел

srules = {
    "Start": {
        "DEC_NUMBER": [["S1", "M"], {
            0: 1,
            1: [2],
        }],
        "LBR": [["S1", "M"], {
            0: 1,
            1: [2],
        }],
    },

    "S1": {
        "PLUS": [["S1", "M", "PLUS"], {
            0: 3,
            3: [1, 2]
        }],
        "EOI": [[]],
        "RBR": [[]],
    },

    "M": {
        "DEC_NUMBER": [["M1", "B"], {
            0: 1,
            1: [2],
        }],
        "LBR": [["M1", "B"], {
            0: 1,
            1: [2],
        }],
    },

    "M1": {
        "STAR": [["M1", "B", "STAR"], {
            0: 3,
            3: [1, 2]
        }],
        "EOI": [[]],
        "PLUS": [[]],
        "RBR": [[]],
    },

    "B": {
        "LBR": [["RBR", "Start", "LBR"], {
            0: 2,
        }],
        "DEC_NUMBER": [["DEC_NUMBER"], {
            0: 1,
        }],
    }
}


stack = ["Start"]

# на Си это делать проще(там есть нормальные указатели)

root = {
    "token": Token("", "root"),
    "childs": [],
    "parent": None
}

root["childs"].append({
    "token": Token("", "S"),
    "childs": [],
    "parent": root
})

nstack = [root["childs"][0]]

c = 0


def print_node(node, level=0):
    print("\t" * level, node["token"], sep='')

    for i in node["childs"]:
        print_node(i, level + 1)


while len(stack) > 0:
    state = stack.pop(-1)
    node = nstack.pop(-1)
    token = tokens[0]

    if token.type == state:
        node["token"] = token
        tokens.pop(0)

        continue

    if token.type not in srules[state]:
        print("error")
        break

    todo = srules[state][token.type][0]

    stack += todo

    if len(todo) == 0:
        for i in node["childs"]:
            i["parent"] = node["parent"]

        if node["parent"] is None:
            continue

        node["parent"]["childs"].remove(node)

        for i in node["childs"]:
            node["parent"]["childs"].append(i)

        continue

    if len(todo) == 1:
        node["token"].type = todo[0]
        nstack += [node]
    else:
        nrule = srules[state][token.type][1]

        nnodes = [{"token": Token("", i), "childs": [], "parent": None}
                  for i in todo]

        if node["parent"] is not None:
            node["parent"]["childs"][
                node["parent"]["childs"].index(node)
            ] = nnodes[nrule[0] - 1]

        nnodes[nrule[0] - 1]["parent"] = node["parent"]
        nnodes[nrule[0] - 1]["childs"] = node["childs"]

        for i in nrule:
            if i == 0:
                continue

            nnodes[i - 1]["childs"] += [nnodes[j - 1] for j in nrule[i]]

            for j in nrule[i]:
                nnodes[j - 1]["parent"] = nnodes[i - 1]

        nstack += nnodes


while node["parent"] is not None:
    node = node["parent"]

print_node(node)
