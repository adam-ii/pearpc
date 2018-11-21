#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import json
import re
import csv

kKeycodeMapsFile = 'qkeycode_maps.csv'

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

def loadKeyMaps(filename, qkeys):
	with open(filename) as f:
		maps = csv.DictReader(f)

		# Validate maps
		mapsList = list(maps)
		mapsLen = len(mapsList)
		qkeysLen = len(qkeys['data'])

		if mapsLen != qkeysLen:
			print 'Key map length (%s) does not match QKeyCode length (%s)' % (mapsLen, qkeysLen)
			return None

		for row in range(0, qkeysLen):
			qkeyEnum = enumFromName(qkeys['data'][row])
			qkeyMap = mapsList[row]['QKeyCode']

			if qkeyEnum != qkeyMap:
				print 'Mismatch on row %d: %s vs %s' % (row + 1, qkeyEnum, qkeyMap)
				return None

		return mapsList

def generateNames(j, varName):
	print 'const char *%s[Q_KEY_CODE__MAX] = {' % (varName)
	for k in j['data']:
		print '\t\"%s\", // %s' % (k, enumFromName(k))
	print '};\n'

	print 'const size_t %sLen = sizeof(%s) / sizeof(%s[0]);' % (varName, varName, varName)

def generatePearPCNames(qkeys, maps, varName):
	qkeysMap = dict([(row['QKeyCode'], row) for row in maps])
	ppcKeys = [row['PearPC'] for row in maps]

	ppcKeyMaxStrLen = max([len(s) for s in ppcKeys])
	qkeyMaxStrLen = max([len(i) for i in qkeys['data']])

	print 'struct QKeyCodeNames { const char *qemu; const char *pearpc; } %s[Q_KEY_CODE__MAX] = {' % (varName)

	for k in qkeys['data']:
		qemuEnum = enumFromName(k)
		qemuStr = ('"%s"' % (k)).ljust(qkeyMaxStrLen + 2)
		ppcStr = qkeysMap[qemuEnum]['PearPC']
		if len(ppcStr.strip()) > 0:
			ppcStr = '"%s"' % (ppcStr)
		else:
			ppcStr = 'NULL'
		ppcVal = ('%s' % (ppcStr)).ljust(ppcKeyMaxStrLen + 2)

		print '\t{ %s, %s }, // %s' % (qemuStr, ppcVal, qemuEnum)
	print '};\n'

	print 'const size_t %sLen = sizeof(%s) / sizeof(%s[0]);' % (varName, varName, varName)

def generateSDLMap(j, maps, varName):
	print 'const std::map<SDLKey, QKeyCode> %s = {' % (varName)

	for row in maps:
		qkeycode = row['QKeyCode']
		sdl = row['SDL1.2']

		if len(sdl.strip()) > 0:
			print '\t{ %s, %s },' % (sdl, qkeycode)

	print '};'

def generateAppleGlyphMap(j, maps, varName):
	print 'NSDictionary* %s = @{' % (varName)

	for row in maps:
		qkeycode = row['QKeyCode']
		glyph = row['AppleGlyph']

		if len(glyph.strip()) > 0:
			print '\t@(%s) : @"%s",' % (qkeycode, glyph)

	print '};'

parser = argparse.ArgumentParser(description='QEMU QKeyCode generator')
parser.add_argument(dest='file', metavar='file', type=str, help='QEMU input.json file')

cmdGroup = parser.add_argument_group(title='Output options').add_mutually_exclusive_group(required=True)
cmdGroup.add_argument('-e', '--enum', dest='genEnum', action='store_true', help='Generate QKeyCode enum')
cmdGroup.add_argument('-n', '--names', dest='genNames', action='store_true', help='Generate QKeyCode names')
cmdGroup.add_argument('-p', '--pearpc-names', dest='genPearPC', action='store_true', help='Generate QKeyCode and PearPC names')
cmdGroup.add_argument('-s', '--sdl-map', dest='genSDL', action='store_true', help='Generate QKeyCode to SDL map')
cmdGroup.add_argument('-a', '--apple-map', dest='genApple', action='store_true', help='Generate QKeyCode to Apple keyboard glyph map')

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

	qkeys = json.loads(jsonStr)

	if args.genEnum:
		generateEnum(qkeys)
	elif args.genNames:
		generateNames(qkeys, 'kQKeyCodeNames')
	elif args.genPearPC:
		generatePearPCNames(qkeys, loadKeyMaps(kKeycodeMapsFile, qkeys), 'kQKeyCodeNames')
	elif args.genSDL:
		generateSDLMap(qkeys, loadKeyMaps(kKeycodeMapsFile, qkeys), 'kSDLToQKeyCode')
	elif args.genApple:
		generateAppleGlyphMap(qkeys, loadKeyMaps(kKeycodeMapsFile, qkeys), 'kMacOSKeyGlpyhs')
