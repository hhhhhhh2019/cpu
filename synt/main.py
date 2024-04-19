from lexer_gen import rules
from synt_gen import srules


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

    1 + 2 * 3 * (4 + 6)

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


srules = {
    "Start": {
        "DEC_NUMBER": ["S1", "M"],
        "LBR": ["S1", "M"],
    },

    "S1": {
        "PLUS": ["S1", "M", "PLUS"],
        "EOI": [],
        "RBR": [],
    },

    "M": {
        "DEC_NUMBER": ["M1", "B"],
        "LBR": ["M1", "B"],
    },

    "M1": {
        "STAR": ["M1", "B", "STAR"],
        "EOI": [],
        "PLUS": [],
        "RBR": [],
    },

    "B": {
        "LBR": ["RBR", "Start", "LBR"],
        "DEC_NUMBER": ["DEC_NUMBER"],
    }
}


stack = ["Start"]

# на Си это делать проще(там есть нормальные указатели)

nstack = [{
    "token": Token("", "S"),
    "childs": [],
    "parent": None
}]

c = 0


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

    todo = srules[state][token.type]

    stack += todo

    if len(todo) == 0:
        if len(node["childs"]) == 0:
            if node["parent"] is not None:
                node["parent"]["childs"].remove(node)

            continue

        node["token"] = node["childs"][-1]["token"]
        for i in node["childs"][-1]["childs"]:
            i["parent"] = node
        node["childs"] = node["childs"][:-1] + node["childs"][-1]["childs"]
        continue

    if len(todo) == 1:
        node["token"].type = todo[0]
        nstack += [node]
    else:
        node["token"].type = todo[-1]

        nparent = {"token": Token("", todo[0]),
                   "childs": [node], "parent": node["parent"]}

        if node["parent"] is not None:
            node["parent"]["childs"][
                node["parent"]["childs"].index(node)
            ] = nparent

        node["parent"] = nparent

        nnodes = [{"token": Token("", i), "childs": [], "parent": node}
                  for i in todo[1:-1]]

        node["childs"] += nnodes

        nstack += [node["parent"]] + nnodes + [node]


root = node

while node["parent"] is not None:
    node = node["parent"]


def print_node(node, level=0):
    print("\t" * level, node["token"], sep='')

    for i in node["childs"]:
        print_node(i, level + 1)


print_node(root)
