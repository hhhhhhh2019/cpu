delims = list(" \t\n-+*/%^&|,.;:()[]{}'\"<>=")


tokens = [
    ["NEWLINE", "\n"],
    ["SPACE", " "],
    ["TAB", "\t"],
    ["ALIGNAS", "alignas"],
    ["ALIGNOF", "alignof"],
    ["AUTO", "auto"],
    ["BREAK", "break"],
    ["CASE", "case"],
    ["CHAR", "char"],
    ["CONST", "const"],
    ["COUNTINUE", "countinue"],
    ["DEFAULT", "default"],
    ["DO", "do"],
    ["ELSE", "else"],
    ["ENUM", "enum"],
    ["EXTERN", "extern"],
    ["FALSE", "false"],
    ["FLOAT", "float"],
    ["FOR", "for"],
    ["GOTO", "goto"],
    ["IF", "if"],
    ["INLINE", "inline"],
    ["INT", "int"],
    ["LONG", "long"],
    ["REGISTER", "register"],
    ["RESTRICT", "restrict"],
    ["RETURN", "return"],
    ["SHORT", "short"],
    ["SIGNED", "signed"],
    ["SIZEOF", "sizeof"],
    ["STRUCT", "struct"],
    ["SWITCH", "switch"],
    ["TYPEDEF", "typedef"],
    ["UNION", "union"],
    ["UNSIGNED", "unsigned"],
    ["VOID", "void"],
    ["VOLATILE", "volatile"],
    ["WHILE", "while"],
    ["PLUS", "+"],
	["MINUS", "-"],
	["STAR", "*"],
	["SLASH", "/"],
	["PERCENT", "%"],
	["AMPERSAND", "&"],
	["PIPE", "|"],
	["CARET", "^"],
	["LSHIFT", "<<"],
	["RSHIFT", ">>"],
	["ARROW", "->"],
	["DOT", "."],
	["COMMA", ","],
	["SEMICOLON", ";"],
    ["DOUBLE_PLUS", "++"],
	["DOUBLE_MINUS", "--"],
	["DOUBLE_AMPERSAND", "&&"],
	["DOUBLE_PIPE", "||"],
	["ASSIGN", "="],
	["ASSIGN_PLUS", "+="],
	["ASSIGN_MINUS", "-="],
	["ASSIGN_STAR", "*="],
	["ASSIGN_SLASH", "/="],
	["ASSIGN_PERCENT", "%="],
	["ASSIGN_AMPERSAND", "&="],
	["ASSIGN_PIPE", "|="],
	["ASSIGN_CARET", "^="],
	["ASSIGN_LSHIFT", "<<="],
	["ASSIGN_RSHIFT", ">>="],
	["EQUALS", "=="],
	["NOT_EQUALS", "!="],
	["LESS", "<"],
	["LESS_EQUALS", "<="],
	["MORE", ">"],
	["MORE_EQUALS", ">="],
    ["LBR", "("],
	["RBR", ")"],
	["LSBR", "["],
	["RSBR", "]"],
	["LCBR", "{"],
	["RCBR", "}"],
]


nodes = [
    {"id": 2, "offset": 0, "rules": {
        2: ["_other"],
        0: delims
    }, "tokens": [], "type": "UNDEFINED"},

    {"id": 3, "offset": 0, "rules": {
        0: delims,
        3: list("0123456789"),
        4: ['x'],
        6: ['b'],
    }, "tokens": [], "type": "DEC_NUMBER"},

    {"id": 4, "offset": 0, "rules": {
        5: list("0123456789")
    }, "tokens": [], "type": -1},

    {"id": 5, "offset": 0, "rules": {
        0: delims,
        5: list("0123456789abcdefABCDEF")
    }, "tokens": [], "type": "HEX_NUMBER"},

    {"id": 6, "offset": 0, "rules": {
        7: list("01")
    }, "tokens": [], "type": -1},

    {"id": 7, "offset": 0, "rules": {
        0: delims,
        7: list("01")
    }, "tokens": [], "type": "BIN_NUMBER"},
]

queue = [
    {"id": 1, "offset": 0, "rules": {
        2: ["_other"],
        3: list("0123456789")
    }, "tokens": tokens, "type": -1}
]

id = 8
