#!/usr/bin/python3

import argparse


import sys

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)



aparser = argparse.ArgumentParser(
    prog="ll0_generator",
    description="table generator for ll(0) parser",
)

aparser.add_argument('filename')
aparser.add_argument('-t', '--type', choices=["python", "c"], default="python")

args = aparser.parse_args()


terminals = {
    "EOI": "",
    "UNDEFINED": "",
}
nonterminals = {}


with open(args.filename) as f:
    mode = 0 # 0 - terminals, 1 - nonterminals

    for line in f:
        line = line.strip()

        if len(line) == 0:
            continue

        if line[0] == '#':
            if line == "#terminals":
                mode = 0
            elif line == "#nonterminals":
                mode = 1

            continue


        if mode == 0: # terminals
            val = line.split(' ')

            name = val[0]
            regex = ""
            for i in ''.join(val[1:]):
                if i == '"':
                    regex += r"\""
                elif i == "\\":
                    regex += r"\\"
                else:
                    regex += i

            terminals[name] = "^" + regex

            continue

        # nonterminals

        val = line.split(' ') + ['|']

        name = val[0]
        nonterminals[name] = []
        arr = []

        for i in val[1:]:
            if i == '':
                continue

            if i == '|':
                nonterminals[name].append(arr.copy())
                del arr[:]
                continue

            arr.append(i)


table = {}
todo  = [[]]


empty_row = {}

for i in terminals:
    empty_row[i] = -1

for i in nonterminals:
    empty_row[i] = -1


for i in nonterminals:
    table[i] = empty_row.copy()

for i in terminals:
    table[i] = empty_row.copy()









def get_first_terminals(type):
    if nodes[type]["first"] != None:
        first: list = nodes[type]["first"]
        return sum(first, start=[])

    nodes[type]["first"] = []

    for i in nonterminals[type]:
        if len(i) == 0:
            continue

        if i[0] in terminals:
            nodes[type]["first"].append([i[0]])
            continue

        id = 0

        add = []

        while id < len(i):
            add += get_first_terminals(i[id])

            if [] in nonterminals[i[id]]:
                id += 1
            else:
                break

        nodes[type]["first"].append(add)

    return sum(nodes[type]["first"], start=[])


def ignore(nonterm, term):
    if table[nonterm][term] == -1:
        table[nonterm][term] = 0


def ignore_right(nonterm, right, visited):
    if nonterm in visited:
        return visited

    visited.append(nonterm)

    if [] in nonterminals[nonterm]:
        for i in right:
            ignore(nonterm, i)

    for i in nonterminals[nonterm]:
        for j in i:
            if j in terminals:
                continue

            visited = ignore_right(j, right, visited)

    return visited



nodes = {}


for i in nonterminals:
    nodes[i] = {
        "first": None
    }



for i in nonterminals:
    get_first_terminals(i)

    for id,j in enumerate(nodes[i]["first"]):
        todo.append(list(reversed(nonterminals[i][id])))

        for k in j:
            table[i][k] = len(todo) - 1



for i in nonterminals:
    for j in nonterminals[i]:
        if len(j) == 0:
            continue

        if j[-1] in nonterminals:
            ignore(j[-1], "EOI")

        right = []

        for k in reversed(j):
            if k in terminals:
                right = [k]
                continue

            ignore_right(k, right, [])
            right = sum(nodes[k]["first"], start=[])



#for i in table:
#    print(i, end=":")
#
#    print(''.join([str(table[i][j]) for j in table[i]]))
#
#    print("")
#
#for id, i in enumerate(todo):
#    print(id, *i)
#


output_data = ""


table2 = table.copy()

table.clear()

for i in terminals:
    table[i] = table2[i]

for i in nonterminals:
    table[i] = table2[i]



if args.type == "python":
    output_data += "from enum import Enum\n"
    output_data += "class TokenType(Enum):\n"

    id = 1

    for i in terminals:
        output_data += f"{i} = {id}\n"
        id += 1

    for i in nonterminals:
        output_data += f"{i} = {id}\n"
        id += 1


    output_data += "\nregexes = {\n"

    for i in terminals:
        if i == "EOI" or i == "UNDEFINED":
            continue

        output_data += f"TokenType.{i}: r\"{terminals[i]}\",\n"

    output_data += "}\n"


    output_data += "\ntable = {\n"

    for i in table:
        output_data += f"TokenType.{i}: " + "{\n"

        for j in table[i]:
            output_data += f"TokenType.{j}: {table[i][j]},\n"

        output_data += "},\n"

    output_data += "}\n\ntodo = [\n"

    for i in todo:
        output_data += "[" + ",".join(["TokenType." + j for j in i]) + "],\n"

    output_data += "]"


elif args.type == "c":
    output_data += "#include <stdlib.h>\n#include <regex.h>\n\nenum Token_type {\n"

    for i in terminals:
        output_data += f"\t{i},\n"

    for i in nonterminals:
        output_data += f"\t{i},\n"

    output_data += "};\n\n"



    output_data += "typedef struct Type_regex {\n\tenum Token_type type;\n\tchar* regex_str;\n\tregex_t regex;\n} Type_regex;\n\n"
    output_data += "Type_regex types_regex[] = {\n"

    for i in terminals:
        if i == "EOI" or i == "UNDEFINED":
            continue

        output_data += "\t{" + f"{i}, \"{terminals[i]}\"" + "},\n"

    output_data += "};\n\n"


    output_data += "char** token_type_names;\n\nvoid prepeare_names() {\n"

    id = 0

    output_data += f"token_type_names = malloc(sizeof(char*) * {len(terminals) + len(nonterminals)});\n"

    for i in terminals:
        #output_data += f"token_type_names[{id}] = malloc({len(i) + 1});\n"
        output_data += f"token_type_names[{id}] = \"{i}\\0\";\n"

        id += 1

    for i in nonterminals:
        #output_data += f"token_type_names[{id}] = malloc({len(i) + 1});\n"
        output_data += f"token_type_names[{id}] = \"{i}\\0\";\n"

        id += 1

    output_data += "}\n\n"


    output_data += f"int table[{len(terminals) + len(nonterminals)}][{len(terminals) + len(nonterminals)}] = " + "{\n"

    for i in table:
        output_data += "{"

        for j in table[i]:
            output_data += f"{table[i][j]}, "

        output_data += "},\n"

    output_data += "};\n\n"


    output_data += "enum Token_type** todo;\n\n"

    output_data += "void prepeare_todo() {\n"

    output_data += f"todo = malloc(sizeof(void*) * {len(todo)});\n"

    for id, i in enumerate(todo):
        output_data += f"todo[{id}] = malloc(sizeof(enum Token_type) * {len(i) + 1});\n"

        output_data += f"todo[{id}][0] = {len(i)};\n"

        for id2, j in enumerate(i):
            output_data += f"todo[{id}][{id2+1}] = {j};\n"

    output_data += "}\n\n"


    output_data += "void free_todo() {\n"

    for id, i in enumerate(todo):
        output_data += f"free(todo[{id}]);\n"

    output_data += "free(todo);\n"

    output_data += "}\n"


    output_data += "void free_names() {\n"

    #for i in range(len(terminals) + len(nonterminals)):
    #    output_data += f"free(token_type_names[{i}]);\n"

    output_data += "free(token_type_names);\n"

    output_data += "}\n"


else:
    print("Unkown output type")
    exit(1)


print(output_data)
