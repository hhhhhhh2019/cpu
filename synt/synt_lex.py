from lexer_gen import rules


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


srules = {
    "Start": {
        "Var_Func_decl":
    }
}
