delims = list(" \t\n-+*/%^&|,.;:()[]{}'\"<>=")


tokens = [
	["NEWLINE", "\n"],
	["SPACE", " "],
	["TAB", "\t"],
	["TIMES", "times"],
	["STRING", ""],
	["CHAR", ""],
	["DEC_NUMBER", ""],
	["HEX_NUMBER", ""],
	["BIN_NUMBER", ""],
	["COMMA", ""],
	["PLUS", "+"],
	["MINUS", "-"],
	["STAR", "*"],
	["SLASH", "/"],
	["PIPE", "|"],
	["CARET", "^"],
	["AMPERSAND", "&"],
	["TILDA", "~"],
	["LEFT_PAREN", "("],
	["RIGHT_PAREN", ")"],
	["DEFINE", "#define"],
	["INCLUDE", "#include"],
	["IF", "#if"],
	["IFDEF", "#ifdef"],
	["IFNDEF", "#ifndef"],
	["ELSE", "#else"],
	["ENDIF", "#endif"],
	["BACK_SLASH", "\\"],
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
