# "(?:(?=(\\?))(\\?).)*?"

from lexer_gen_c import queue, nodes, delims, id

while len(queue) > 0:
    node = queue.pop(0)

    letters = {}

    for i in node["tokens"]:
        if len(i[1]) == node["offset"]:
            continue

        c = i[1][node["offset"]]

        if c not in letters:
            letters[c] = []

        letters[c].append(i)

    for i in letters:
        nnode = {"id": id, "offset": node["offset"] + 1, "tokens": letters[i], "rules": {
            0: delims,
            2: ["_other"],
        }, "type": "UNDEFINED"}

        if len(nnode["tokens"][0][1]) == nnode["offset"]:
            # nnode["rules"][0][-1] = nnode["tokens"][0][0]
            nnode["type"] = nnode["tokens"][0][0]

        if nnode["tokens"][0][1][-1] in delims:
            nnode["rules"] = {
                0: ["_other"]
            }
            nnode["type"] = nnode["tokens"][0][0]

        queue.append(nnode)

        node["rules"][id] = [i]

        id += 1

    nodes.append(node)


rules = {}

for i in nodes:
    rules[i["id"]] = [i["rules"], i["type"]]


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


# string = """
# 1234
# 0x123
# 0b01
# """
# string = """
# unsigned int foo(unsigned int a, unsigned int b) {
# 	unsigned int voidc = 0;
#
# 	while (a != b) {
# 		if (a > b)
# 			a -= b;
# 		else
# 			b -= a;
#
# 		voidc++;
# 	}
#
# 	bar(a,b,voidc);
#
# 	return voidc;
# }
# """
# i = 0
#
# token = ""
# result = []
#
# state = 1
#
# while i < len(string):
#     c = string[i]
#
#     nstate = -1
#     otherid = -1
#
#     for j in rules[state]:
#         if "_other" in rules[state][j]:
#             otherid = j
#
#         if c in rules[state][j]:
#             nstate = j
#
#     nstate = nstate if nstate != -1 else otherid
#
#     if nstate == -1:
#         print("rules error")
#         break
#     elif nstate == 0:
#         result.append([token, rules[state][nstate][-1]])
#         token = ""
#         state = 1
#     else:
#         token += c
#         i += 1
#         state = nstate
#
#
# for i in result:
#     print(i)
