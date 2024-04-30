from regex import compile_regex
from pprint import pprint
from string import ascii_letters, digits, punctuation, whitespace


# cother = list(" \t\n,.-+*/\\&^%;|~()[]{}=\0")
cother = list(punctuation + " \t\n")
cletters = list(ascii_letters)
cdigits = list(digits)

cpath = list(ascii_letters + digits + r"_.:;!@#$%^&(){}[]<>\/&")

# символы, с которых можно перейти в UNDEFINED
def_undef = set(ascii_letters + "_" + digits)

tokens = [
    ["SPACE",        "[\n \t]+", set()],
    # ["DEC_NUMBER",   r"[0123456789]+",               set()],
    # ["FLOAT_NUMBER", r"[0123456789]+.[0123456789]+", set()],
    ["LET",          r"let",                         def_undef.copy()],
    ["IN",           r"in",                          def_undef.copy()],
    ["REC",          r"rec",                         def_undef.copy()],
    ["WITH",         r"with",                        def_undef.copy()],
    ["INHERIT",      r"inherit",                     def_undef.copy()],
    ["SEMICOLON",    r";",                           set()],
    ["DOLLAR",       r"$",                           set()],
    ["PLUS",         r"\+",                          set()],
    ["MINUS",        r"-",                           set()],
    ["SLASH",        r"/",                           set()],
    ["STAR",         r"\*",                          set()],
    ["DOT",          r"\.",                          set()],
    ["COMMA",        r",",                           set()],
    ["EPSILON",      r"\.\.\.",                      set()],
    ["LBR",          r"(",                           set()],
    ["RRB",          r")",                           set()],
    ["LCBR",         r"{",                           set()],
    ["RCBR",         r"}",                           set()],
    ["LSBR",         r"\[",                          set()],
    ["RSBR",         r"\]",                          set()],
    ["ASSIGN",       r"=",                           set()],
    ["EQUALS",       r"==",                          set()],
    ["NOT_EQUALS",   r"!=",                          set()],
    ["LESS_EQUALS",  r"<=",                          set()],
    ["MORE_EQUALS",  r">=",                          set()],
    ["TRUE",         r"true",                        def_undef.copy()],
    ["FALSE",        r"false",                       def_undef.copy()],
]

nfa = {
    1: {
        "rules": {
            2: list(ascii_letters + "_"),
            3: ['"'],
            6: ["'"],
            8: list("0123456789"),
            -1: ["_other"]
        },
        "type": -1,
        "using": False
    },

    2: {
        "rules": {
            2: def_undef.copy(),
            0: ["_other"],
        },
        "type": "UNDEFINED",
        "using": False
    },

    3: {
        "rules": {
            -1: ["\n"],
            5: ['"'],
            3: ["_other"],
            4: ["\\"],
        },
        "type": 0,
        "using": False,
    },

    4: {
        "rules": {
            -1: ["\n"],
            3: ["_other"],
        },
        "type": 0,
        "using": False
    },

    5: {
        "rules": {
            0: ["_other"],
        },
        "type": "STRING",
        "using": False
    },

    6: {
        "rules": {
            6: ["_other"],
            7: ["'"],
        },
        "type": 0,
        "using": False
    },

    7: {
        "rules": {
            0: ["_other"],
        },
        "type": "PATH",
        "using": False
    },

    8: {
        "rules": {
            8: list("0123456789"),
            9: ["."],
            0: ["_other"],
        },
        "type": "DEC_NUMBER",
        "using": False
    },

    9: {
        "rules": {
            10: list("0123456789"),
            -1: ["_other"],
        },
        "type": 0,
        "using": False
    },

    10: {
        "rules": {
            10: list("0123456789"),
            0: ["_other"],
        },
        "type": "FLOAT_NUMBER",
        "using": False
    },
}
id = max(nfa) + 1


for token in tokens:
    fa = compile_regex(token[1])

    chars = {}

    for i in fa[1]:
        if i == "can_skip":
            continue

        for j in fa[1][i]:
            if j == "_other":
                continue
            if j not in chars:
                chars[j] = []
            chars[j].append(i)

    for i in chars:
        for j in chars[i]:
            if j - 2 + id not in nfa[1]["rules"]:
                nfa[1]["rules"][j - 2 + id] = []
            nfa[1]["rules"][j - 2 + id].append(i)

    for i in fa:
        if i == 1:
            continue

        nfa[i - 2 + id] = {
            "rules": {0: ["_other"]}, "type": "UNDEFINED", "using": False
        }

        for j in fa[i]:
            if j == "can_skip":
                continue

            if j == -1:
                nfa[i - 2 + id]["rules"][2] = token[2].copy()
            elif j == 0:
                nfa[i - 2 + id]["type"] = token[0]
                nfa[i - 2 + id]["rules"][2] = token[2].copy()
                # nfa[i - 2 + id]["rules"].pop(0)
            else:
                nfa[i - 2 + id]["rules"][j - 2 + id] = fa[i][j]

    id += max(fa) - 1


def collapse(nfa, id):
    if nfa[id]["using"] is True:
        return False

    nfa[id]["using"] = True

    chars = {}

    for i in nfa[id]["rules"]:
        for j in nfa[id]["rules"][i]:
            if j not in chars:
                chars[j] = []
            chars[j].append(i)

    found = False

    for i in chars:
        if len(chars[i]) < 2:
            continue

        found = True

        nfa[max(nfa) + 1] = {
            "rules": {},
            "type": -1,
            "using": False,
        }

        for j in nfa[id]["rules"]:
            if i not in nfa[id]["rules"][j]:
                continue
            nfa[id]["rules"][j].remove(i)

        nfa[id]["rules"][max(nfa)] = [i]

        for j in chars[i]:
            if nfa[j]["type"] == -1:
                continue

            if nfa[max(nfa)]["type"] in ["UNDEFINED", -1]:
                nfa[max(nfa)]["type"] = nfa[j]["type"]
            elif nfa[j]["type"] != "UNDEFINED":
                # Error
                pass

        for j in chars[i]:
            for k in nfa[j]["rules"]:
                nfa[max(nfa)]["rules"][k] = nfa[j]["rules"][k]

    if found:
        return True

    for i in nfa[id]["rules"]:
        if i in [-1, 0, 2, id]:
            continue

        if len(nfa[id]["rules"][i]) == 0:
            continue

        if collapse(nfa, i):
            return True

    return False


while collapse(nfa, 1):
    for i in nfa:
        nfa[i]["using"] = False

for i in nfa:
    nfa[i]["using"] = False


nfa[1]["using"] = True

for i in nfa:
    for j in nfa[i]["rules"]:
        if j == -1 or j == 0:
            continue
        nfa[j]["using"] = True


offset = 0

for i in nfa:
    nfa[i]["offset"] = offset

    if not nfa[i]["using"]:
        offset -= 1


dfa = {}

for i in nfa:
    dfa[i + nfa[i]["offset"]] = {
        "rules": {},
        "type": nfa[i]["type"]
    }

    for j in nfa[i]["rules"]:
        k = j
        if j not in [-1, 0, 1]:
            k += nfa[j]["offset"]
        dfa[i + nfa[i]["offset"]]["rules"][k] = nfa[i]["rules"][j]


rules = {}

for i in dfa:
    rules[i] = [dfa[i]["rules"], dfa[i]["type"]]


# pprint(rules)

if __name__ == "__main__" and False:
    def get_id(ch, r) -> int:
        if ch == 256:
            ch = "_other"
        else:
            ch = chr(ch)

        for i in r:
            if ch in r[i]:
                return i

        return -1

    print(f"int rules[{len(rules)}][258] = " + "{")

    for i in sorted(rules):
        # print(f"{','.join(
        #     [str(get_id(j, rules[i])) for j in range(256)]
        # )},")

        print("\t{", end='')

        for j in range(257):
            print(get_id(j, rules[i][0]), end=',')
        print(rules[i][1], end='},\n')

    print("};")
