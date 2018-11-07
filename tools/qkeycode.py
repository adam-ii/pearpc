#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import json

# Map QKeyCode names to PearPC 
kPearPCNames = { 
	'equal' : '=',
	'minus' : '-',
	'bracket_right' : ']',
	'bracket_left' : '[',
	'ret' : 'Return',
	'apostrophe' : '\'',
	'semicolon' : ';',
	'backslash' : '\\',
	'comma' : ',',
	'slash' : '/',
	'dot' : '.',
	'spc' : 'Space',
	'grave_accent' : '`',
	'esc' : 'Escape',
	'caps_lock' : 'Caps-Lock',
	'meta_l' : 'Right-Alt',
	'kp_decimal' : 'Keypad-.',
	'kp_multiply' : 'Keypad-*',
	'kp_add' : 'Keypad-+',
	'num_lock' : 'Numlock',
	'kp_divide' : 'Keypad-/',
	'kp_enter' : 'Keypad-Enter',
	'kp_subtract' : 'Keypad--',
	'kp_0' : 'Keypad-0',
	'kp_1' : 'Keypad-1',
	'kp_2' : 'Keypad-2',
	'kp_3' : 'Keypad-3',
	'kp_4' : 'Keypad-4',
	'kp_5' : 'Keypad-5',
	'kp_6' : 'Keypad-6',
	'kp_7' : 'Keypad-7',
	'kp_8' : 'Keypad-8',
	'kp_9' : 'Keypad-9',
	'scroll_lock' : 'Scrolllock',
	'pgup' : 'Pageup',
	'pgdn' : 'Pagedown'
}

def enumFromName(name):
	return 'Q_KEY_CODE_%s' % (name.encode('utf8').upper())

def generateEnum(j):
	maxLen = max([len(enumFromName(k)) for k in j['data']])
	code = 0

	print 'typedef enum QKeyCode {'
	for k in j['data']:
		print '\t%s = %d,' % (enumFromName(k).ljust(maxLen), code)
		code = code + 1
	print '\t%s = %d' % ('Q_KEY_CODE__MAX'.ljust(maxLen), code)
	print '} QKeyCode;'

def generateNames(j, varName):
	print 'const char *%s[Q_KEY_CODE__MAX] = {' % (varName)
	for k in j['data']:
		print '\t\"%s\", // %s' % (k, enumFromName(k))
	print '};\n'

	print 'const size_t %s_len = sizeof(%s) / sizeof(%s[0]);' % (varName, varName, varName)

def generatePearPCNames(j, varName):
	qkeys = dict({(k, enumFromName(k)) for k in j['data']})
	qkeyMaxLen = max([len(i) for i in j['data']])
	ppcKeyMaxLen = max([len(v) for k,v in kPearPCNames.iteritems()])

	# Validate PearPC mapping names
	try:
		for k,v in kPearPCNames.iteritems():
			qkeys[k]
	except (KeyError):
		print 'kPearPCNames contains an unrecognized QEMU key: %s' % (k)
		return

	print 'struct QKeyCodeNames { const char *qemu; const char *pearpc; } %s[Q_KEY_CODE__MAX] = {' % (varName)

	for k in j['data']:
		qemuVal = ('"%s"' % (k)).ljust(qkeyMaxLen + 2)

		ppcStr = k in kPearPCNames and '"%s"' % (kPearPCNames[k].encode('string_escape')) or 'NULL'
		ppcVal = ('%s' % (ppcStr)).ljust(ppcKeyMaxLen + 2)

		print '\t{ %s, %s }, // %s' % (qemuVal, ppcVal, qkeys[k])
	print '};\n'

	print 'const size_t %s_len = sizeof(%s) / sizeof(%s[0]);' % (varName, varName, varName)


parser = argparse.ArgumentParser(description='QEMU QKeyCode generator')
parser.add_argument(dest='file', metavar='file', type=str, help='QEMU input.json file')

cmdGroup = parser.add_argument_group(title='Output options').add_mutually_exclusive_group(required=True)
cmdGroup.add_argument('-e', '--enum', dest='genEnum', action='store_true', help='Generate QKeyCode enum')
cmdGroup.add_argument('-n', '--names', dest='genNames', action='store_true', help='Generate QKeyCode names')
cmdGroup.add_argument('-p', '--pearpc-names', dest='genPearPC', action='store_true', help='Generate QKeyCode and PearPC names')

args = parser.parse_args()

with open(args.file) as f:
	jsonStr = ''
	entered = False

	lines = f.readlines()
	for l in lines:
		if l.startswith('{') and l.find('QKeyCode') > 0:
			entered = True
		if entered:
			jsonStr = jsonStr + l.replace('\'', '"')
			if l.strip().endswith('}'):
				break

	j = json.loads(jsonStr)

	if args.genEnum:
		generateEnum(j)
	elif args.genNames:
		generateNames(j, 'qkeycode_names')
	elif args.genPearPC:
		generatePearPCNames(j, 'qkeycode_names')
