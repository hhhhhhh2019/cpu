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
        a = self.value.replace("\n", "").replace("\t", "")
        return str(self.type)# + ' ' + a

    def __repr__(self) -> str:
        return str(self)


# string = r"""
# a = {
#     b.c = "qwerty";
#     arr = [
#         123.123
#         321321
#         '/folder/space path/file'
#         "string"
#     ];
# }
# \n"""
# string = "[a/b './a/b' '/a/b']\n"
# string = "123\n"
string = "[1 + 2, 2, 3]\n"

type = 0

token = ""
tokens = []

state = 1

while type < len(string):
    c = string[type]

    nstate = -1
    otherid = -1

    for rule in rules[state][0]:
        if "_other" in rules[state][0][rule]:
            otherid = rule

        if c in rules[state][0][rule]:
            nstate = rule

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
        type += 1
        state = nstate


tokens = [i for i in tokens if not i.type == "SPACE"] + \
    [Token("EOI", "$")]


class Node:
    token: Token
    childs: list
    parent: object
    id: int

    def __init__(self, token: token, childs: list, parent):
        self.token = token
        self.childs = childs
        self.parent = parent
        self.id = 0

    def __str__(self):
        return str(self.token)

    def __repr__(self) -> str:
        return str(self)

    def print(self, offset=0):
        print("  " * offset + str(self.token))
        for i in self.childs:
            i.print(offset+1)

    def index(self, id: int = 0) -> int:
        self.id = id

        for i in self.childs:
            id = i.index(id + 1)

        return id

    def to_dot(self):
        print(f"{self.id} [label=\"{str(self.token.type)}\"]")
        print(f"{self.id} -> " + "{", end='')
        for i in self.childs:
            print(i.id, end=' ')
        print("}")

        for i in self.childs:
            i.to_dot()


# nodes = [Node(i, [], None) for i in tokens]

srules = {
        "Array": [
            # ["LSBR", "E", "RSBR"],
            ["LSBR", "Aargs", "RSBR"],
        ],

        "Aargs": [
            ["Aargs", "COMMA", "E"],
            ["E"],
        ],

        "E": [
            ["E", "PLUS", "E"],
            ["E1"],
            # ["DEC_NUMBER"],
        ],

        "E1": [
            ["E1", "STAR", "E1"],
            ["E2"],
        ],

        "E2": [
            ["DEC_NUMBER"],
        ],
}


stack = []

stack.append(Node(tokens.pop(0), [], None))

while len(tokens) > 0:
    start_with = []
    stuiable = []

    # input()

    # print(stack)

    for type in srules:
        for rule in srules[type]:
            if rule == [i.token.type for i in stack[-len(rule):]]:
                stuiable.append((type, len(rule)))
                continue

            ok = False
            max_len = 0

            for i in range(len(rule)):
                if rule[:i+1] == [j.token.type for j in stack[-i-1:]]:
                    ok = True
                    max_len = i+1
                    break

            if ok:
                start_with.append((type, max_len, rule))

    # print(start_with)
    # print(stuiable)

    if len(stuiable) == 0:
        stack.append(Node(tokens.pop(0), [], None))
        continue

    sw_new = []

    for i in start_with:
        if i[2][i[1]] == tokens[0].type:
            sw_new.append(i)

    # print("sw_new:", sw_new)

    if len(sw_new) != 0:
        stack.append(Node(tokens.pop(0), [], None))
        continue

    s = sorted(stuiable, key=lambda x: x[1])[-1]

    node = Node(Token("", s[0]), [], None)
    for i in range(s[1]):
        node.childs.append(stack.pop())
    stack.append(node)


# while len(tokens) > 0:
#     suitable = []
#     input()
#
#     print(stack)
#
#     for type in srules:
#         for rule in srules[type]:
#             ok = False
#             max_len = 0
#
#             ln = min(len(rule), len(stack))
#
#             for i in range(ln):
#                 if rule[i] != stack[len(stack)-ln+i].token.type:
#                     break
#                 ok = True
#                 max_len = i + 1
#
#             if ok:
#                 suitable.append((type, max_len, rule))
#
#     print(suitable)
#
#     if len(suitable) == 1:
#         if suitable[0][1] == len(suitable[0][2]):
#             node = Node(Token("", suitable[0][0]), [], None)
#             for i in range(len(suitable[0][2])):
#                 node.childs.append(stack.pop())
#             stack.append(node)
#             continue
#     elif len(suitable) > 1:
#         snew = []
#         all_ok = True
#
#         for i in suitable:
#             if i[1] == len(i[2]):
#                 snew.append(i)
#                 continue
#
#             if i[2][i[1]] == tokens[0].type:
#                 snew.append(i)
#                 all_ok = False
#                 continue
#
#         print("snew:", snew, all_ok)
#
#         if all_ok and len(snew) != 0:
#             s = sorted(snew, key=lambda x: x[2])[-1]
#
#             node = Node(Token("", s[0]), [], None)
#             for i in range(len(s[2])):
#                 node.childs.append(stack.pop())
#             stack.append(node)
#             continue
#
#     stack.append(Node(tokens.pop(0), [], None))


# pprint(stack)

# for i in stack:
#     i.print()

for i in stack:
    i.index()

did = 1
for i in stack:
    print(f"digraph {did} " + "{")
    i.to_dot()
    print("}")
    did += 1
