from pprint import pprint


terminals = [
    "UNDEFINED",
    "NEWLINE",
    "SPACE",
    "TAB",
    "ALIGNAS",
    "ALIGNOF",
    "AUTO",
    "BREAK",
    "CASE",
    "CHAR",
    "CONST",
    "COUNTINUE",
    "DEFAULT",
    "DO",
    "ELSE",
    "ENUM",
    "EXTERN",
    "FALSE",
    "FLOAT",
    "FOR",
    "GOTO",
    "IF",
    "INLINE",
    "INT",
    "LONG",
    "RETURN",
    "SHORT",
    "SIGNED",
    "SIZEOF",
    "STATIC",
    "STRUCT",
    "SWITCH",
    "TYPEDEF",
    "UNION",
    "UNSIGNED",
    "VOID",
    "WHILE",
    "DEC_NUMBER",
    "BIN_NUMBER",
    "HEX_NUMBER",
    "PLUS",
    "MINUS",
    "STAR",
    "SLASH",
    "PERCENT",
    "AMPERSAND",
    "PIPE",
    "CARET",
    "LSHIFT",
    "RSHIFT",
    "ARROW",
    "DOT",
    "COMMA",
    "SEMICOLON",
    "DOUBLE_PLUS",
    "DOUBLE_MINUS",
    "DOUBLE_AMPERSAND",
    "DOUBLE_PIPE",
    "ASSIGN",
    "ASSIGN_PLUS",
    "ASSIGN_MINUS",
    "ASSIGN_STAR",
    "ASSIGN_SLASH",
    "ASSIGN_PERCENT",
    "ASSIGN_AMPERSAND",
    "ASSIGN_PIPE",
    "ASSIGN_CARET",
    "ASSIGN_LSHIFT",
    "ASSIGN_RSHIFT",
    "EQUALS",
    "NOT_EQUALS",
    "LESS",
    "LESS_EQUALS",
    "MORE",
    "MORE_EQUALS",
    "LBR",
    "RBR",
    "LSBR",
    "RSBR",
    "LCBR",
    "RCBR",
]


grammar = {
    "Start": [
        ["S"],
        [],
    ],

    "S": [
        ["M", "S1"],
    ],

    "S1": [
        ["PLUS", "M", "S1"],
        [],
    ],

    "M": [
        ["B", "M1"],
    ],

    "M1": [
        ["STAR", "B", "M1"],
        [],
    ],

    "B": [
        ["DEC_NUMBER"],
        ["LBR", "S", "RBR"],
    ]
}


nodes = {}


for i in grammar:
    nodes[i] = {
        "start_terminals": set(),
        "last_terminals": set(),
        "next_terminals": set(),
        "checked": False,
        # checked используется для предотвращения бесконечной рекурсии
        # сбрасывать checked перед каждым рекурсивным проходом
    }

nodes["Start"]["next_terminals"].add("EOI")


def get_first_terminals(id) -> set:
    if nodes[id]["checked"]:
        return nodes[id]["start_terminals"]

    nodes[id]["checked"] = True

    for i in grammar[id]:
        for cid in range(len(i)):
            if i[cid] in terminals:
                nodes[id]["start_terminals"].add(i[cid])
                break

            nodes[id]["start_terminals"].update(get_first_terminals(i[cid]))

            if [] not in grammar[i[cid]]:
                break

    return nodes[id]["start_terminals"]


def get_last_terminals(id) -> set:
    if nodes[id]["checked"]:
        return nodes[id]["last_terminals"]

    nodes[id]["checked"] = True

    for i in grammar[id]:
        for cid in range(len(i) - 1, -1, -1):
            if i[cid] in terminals:
                nodes[id]["last_terminals"].add(i[cid])
                break

            nodes[id]["last_terminals"].update(get_last_terminals(i[cid]))

            if [] not in grammar[i[cid]]:
                break

    return nodes[id]["last_terminals"]


def get_next_terminals(pid) -> set:
    """
    эта функция находит последующие терминалы не для pid,
    а для его дочерних нетерминалов
    """
    if nodes[pid]["checked"]:
        return nodes[pid]["next_terminals"]

    nodes[pid]["checked"] = True

    for i in grammar[pid]:
        terms = set()

        for id in range(len(i)-1, -1, -1):
            if i[id] in terminals:
                terms.clear()
                terms.add(i[id])
                continue

            nodes[i[id]]["next_terminals"].update(terms)

            terms.update(get_next_terminals(i[id]))
            terms.update(get_first_terminals(i[id]))

            if [] not in grammar[i[id]]:
                terms.clear()

    return nodes[pid]["next_terminals"]


def update_next_terminals(pid) -> None:
    """
    добавляет к последним конечным нетерменалам pid
    последующие терминалы pid
    """
    if nodes[pid]["checked"]:
        return

    nodes[pid]["checked"] = True

    for i in grammar[pid]:
        for id in range(len(i)-1, -1, -1):
            if i[id] in terminals:
                break

            nodes[i[id]]["next_terminals"].update(nodes[pid]["next_terminals"])

            if [] not in grammar[i[id]]:
                break


for i in grammar:
    get_first_terminals(i)


for i in nodes:
    nodes[i]["checked"] = False


for i in grammar:
    get_last_terminals(i)


for i in nodes:
    nodes[i]["checked"] = False


for i in grammar:
    get_next_terminals(i)


for i in nodes:
    nodes[i]["checked"] = False


for i in grammar:
    update_next_terminals(i)


srules = {}


for i in nodes:
    srules[i] = {}

    for j in grammar[i]:
        if len(j) == 0:
            continue

        if j[0] in terminals:
            srules[i][j[0]] = list(reversed(j))
            continue

        for k in nodes[j[0]]["start_terminals"]:
            srules[i][k] = list(reversed(j))

    if [] not in grammar[i]:
        continue

    for j in nodes[i]["next_terminals"]:
        srules[i][j] = []


# pprint(srules)
