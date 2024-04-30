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
string = "[1+2,2,3,4]\n"

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


srules = {
        "Array": [
            ["LSBR", "Aargs", "RSBR"],
            ["LSBR", "E", "RSBR"],
        ],

        "Aargs": [
            ["E", "COMMA", "E"],
            ["Aargs", "COMMA", "E"],
        ],

        "E": [
            ["E", "PLUS", "E"],
            ["E1"],
        ],

        "E1": [
            ["E1", "STAR", "E1"],
            ["E2"],
        ],

        "E2": [
            ["DEC_NUMBER"],
            ["LBR", "E", "RBR"]
        ],
}


stack = []

stack.append(Node(tokens.pop(0), [], None))

while len(tokens) > 0:
    start_with = []
    suitable = []

    # input()
    # print(stack)

    for type in srules:
        for rule in srules[type]:
            if rule == [i.token.type for i in stack[-len(rule):]]:
                suitable.append((type, len(rule)))
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
    # print(suitable)

    if len(suitable) == 0:
        stack.append(Node(tokens.pop(0), [], None))
        continue

    sw_new = []

    min_len = min([i[1] for i in suitable])

    for i in start_with:
        if i[2][i[1]] == tokens[0].type and len(i[2]) > min_len:
            sw_new.append(i)

    # print("sw_new:", sw_new)

    if len(sw_new) != 0:
        stack.append(Node(tokens.pop(0), [], None))
        continue

    s = sorted(suitable, key=lambda x: x[1])[-1]

    node = Node(Token("", s[0]), [], None)
    for i in range(s[1]):
        node.childs.append(stack.pop())
    node.childs.reverse()
    stack.append(node)

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
