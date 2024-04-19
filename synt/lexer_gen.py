from regex import compile_regex
from pprint import pprint
from string import ascii_letters, digits, punctuation


# cother = list(" \t\n,.-+*/\\&^%;|~()[]{}=\0")
cother = list(punctuation + " \t\n")
cletters = list(ascii_letters)
cdigits = list(digits)

# символы, с которых можно перейти в UNDEFINED
def_undef = set(ascii_letters + "_" + digits)

tokens = [
    ["NEWLINE",          "\n",                           set()],
    ["SPACE",            r" ",                           set()],
    ["TAB",              "\t",                           set()],
    ["ALIGNAS",          r"alignas",                     def_undef.copy()],
    ["ALIGNOF",          r"alignof",                     def_undef.copy()],
    ["AUTO",             r"auto",                        def_undef.copy()],
    ["BREAK",            r"break",                       def_undef.copy()],
    ["CASE",             r"case",                        def_undef.copy()],
    ["CHAR",             r"char",                        def_undef.copy()],
    ["CONST",            r"const",                       def_undef.copy()],
    ["COUNTINUE",        r"countinue",                   def_undef.copy()],
    ["DEFAULT",          r"default",                     def_undef.copy()],
    ["DO",               r"do",                          def_undef.copy()],
    ["ELSE",             r"else",                        def_undef.copy()],
    ["ENUM",             r"enum",                        def_undef.copy()],
    ["EXTERN",           r"extern",                      def_undef.copy()],
    ["FALSE",            r"false",                       def_undef.copy()],
    ["FLOAT",            r"float",                       def_undef.copy()],
    ["FOR",              r"for",                         def_undef.copy()],
    ["GOTO",             r"goto",                        def_undef.copy()],
    ["IF",               r"if",                          def_undef.copy()],
    ["INLINE",           r"inline",                      def_undef.copy()],
    ["INT",              r"int",                         def_undef.copy()],
    ["LONG",             r"long",                        def_undef.copy()],
    ["RETURN",           r"return",                      def_undef.copy()],
    ["SHORT",            r"short",                       def_undef.copy()],
    ["SIGNED",           r"signed",                      def_undef.copy()],
    ["SIZEOF",           r"sizeof",                      def_undef.copy()],
    ["STRUCT",           r"struct",                      def_undef.copy()],
    ["SWITCH",           r"switch",                      def_undef.copy()],
    ["TYPEDEF",          r"typedef",                     def_undef.copy()],
    ["UNION",            r"union",                       def_undef.copy()],
    ["UNSIGNED",         r"unsigned",                    def_undef.copy()],
    ["VOID",             r"void",                        def_undef.copy()],
    ["WHILE",            r"while",                       def_undef.copy()],
    ["PLUS",             r"\+",                          set()],
    ["MINUS",            r"-",                           set()],
    ["STAR",             r"\*",                          set()],
    ["SLASH",            r"/",                           set()],
    ["PERCENT",          r"%",                           set()],
    ["AMPERSAND",        r"&",                           set()],
    ["TILDA",            r"~",                           set()],
    ["PIPE",             r"|",                           set()],
    ["CARET",            r"^",                           set()],
    ["LSHIFT",           r"<<",                          set()],
    ["RSHIFT",           r">>",                          set()],
    ["ARROW",            r"->",                          set()],
    ["DOT",              r".",                           set()],
    ["COMMA",            r",",                           set()],
    ["SEMICOLON",        r";",                           set()],
    ["DOUBLE_PLUS",      r"\+\+",                        set()],
    ["DOUBLE_MINUS",     r"--",                          set()],
    ["DOUBLE_AMPERSAND", r"&&",                          set()],
    ["DOUBLE_PIPE",      r"||",                          set()],
    ["ASSIGN",           r"=",                           set()],
    ["ASSIGN_PLUS",      r"\+=",                         set()],
    ["ASSIGN_MINUS",     r"-=",                          set()],
    ["ASSIGN_STAR",      r"\*=",                         set()],
    ["ASSIGN_SLASH",     r"/=",                          set()],
    ["ASSIGN_PERCENT",   r"%=",                          set()],
    ["ASSIGN_AMPERSAND", r"&=",                          set()],
    ["ASSIGN_PIPE",      r"|=",                          set()],
    ["ASSIGN_CARET",     r"^=",                          set()],
    ["ASSIGN_LSHIFT",    r"<<=",                         set()],
    ["ASSIGN_RSHIFT",    r">>=",                         set()],
    ["EQUALS",           r"==",                          set()],
    ["NOT_EQUALS",       r"!=",                          set()],
    ["LESS",             r"<",                           set()],
    ["LESS_EQUALS",      r"<=",                          set()],
    ["MORE",             r">",                           set()],
    ["MORE_EQUALS",      r">=",                          set()],
    ["LBR",              r"(",                           set()],
    ["RBR",              r")",                           set()],
    ["LSBR",             r"\[",                          set()],
    ["RSBR",             r"\]",                          set()],
    ["LCBR",             r"{",                           set()],
    ["RCBR",             r"}",                           set()],
    ["DEC_NUMBER",       r"[0123456789]+",               set()],
    ["HEX_NUMBER",       r"0x[0123456789abcdefABCDEF]+", set()],
    ["BIN_NUMBER",       r"0b[01]+",                     set()],
]


nfa = {
    1: {
        "rules": {
            2: list(ascii_letters + "_"),
            -1: ["_other"]
        },
        "type": -1,
        "using": True,
    },

    2: {
        "rules": {
            2: def_undef.copy(),
            0: ["_other"],
        }, "type": "UNDEFINED",
        "using": False
    },
}
id = 3


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
    pass


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

if __name__ == "__main__":
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
