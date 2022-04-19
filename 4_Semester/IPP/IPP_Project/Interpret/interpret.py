# IPP Projekt cast 2.1 - interpret.py
# Autor: Michal Pysik (login: xpysik00)


import sys
import xml.etree.ElementTree as ET
import operator
import re



# seznam vsech validnich nazvu instrukci
opcode_dictionary = ['MOVE', 'CREATEFRAME', 'PUSHFRAME', 'POPFRAME', 'DEFVAR', 'CALL', 'RETURN', 'PUSHS', 'POPS', 'ADD', 'SUB', 'MUL', 'IDIV', 'LT', 'GT', 'EQ',
'AND', 'OR', 'NOT', 'INT2CHAR', 'STRI2INT', 'READ', 'WRITE', 'CONCAT', 'STRLEN', 'GETCHAR', 'SETCHAR', 'TYPE', 'LABEL', 'JUMP', 'JUMPIFEQ', 'JUMPIFNEQ', 'EXIT', 'DPRINT', 'BREAK']



#pomocna funkce pro konec v pripade chyby a vypis chybove zpravy
def error_exit(retval, message):
	print("Error: " + message + "\nInterpreter terminated with error code " + str(retval), file=sys.stderr)
	sys.exit(retval)



# vypis napovedy a ukonceni programu s kodem 0
def help():
	print("This is IPPcode21 interpreter\nIt reads IPPcode21 source code in XML format and runs it, while also checking for errors in the code\n")
	print("List of valid parameters:\n\'--source=file\' - reads the source code from the specified file\n\'--input=file\' - reads the (optional) program input from the specified file\n")
	print("If only one of these two parameters is specified, the other is automatically read from the standard input")
	sys.exit(0)



# kontroluje hlavicku a dalsi na prvnim textovem radku XML souboru
def check_header(root):
	if root.tag != 'program':
		error_exit(32, "Missing program root element in source file")
	missing_lang = True
	for attribute in root.attrib:
		if attribute == 'language':
			if root.attrib[attribute].lower() != 'ippcode21':
				error_exit(32, "Invalid source code language attribute, expected IPPcode21")
			missing_lang = False
		elif attribute != 'name' and attribute != 'description':
			error_exit(32, attribute + " is not valid root element attribute")
	if missing_lang:
		error_exit(32, "Missing language attribute in program root element")



# kontroluje zda je zadane slovo platny nazev instrukce
def opcode_dictionary_check(opcode):
	global opcode_dictionary
	if str.upper(opcode) in opcode_dictionary:
		return True
	else:
		return False



# kontroluje zda je dany typ argumentu platny typ symbolu
def isSymbol(type):
	return type == 'var' or type == 'int' or type == 'bool' or type == 'string' or type == 'nil'

# Pomocna funkce na prevod decimalnich sekvenci pri tisku retezcu instrukci write
def replace_regex(match):
	return chr(int(match.group(1)))


# trida pro argumenty instrukci
class InsArgument:
	def __init__(self, value, type):
		if type:
			type = str.lower(type)
		if type != None and type != 'int' and type != 'string' and type != 'bool' and type != 'nil' and type != 'label' and type != 'var' and type!= 'type':
			error_exit(32, "Invalid \'type\' attribute in instruction argument")

		self.value = value
		self.type = type

		if type == 'var':
			if not re.fullmatch('^(GF|LF|TF)@\S+$', value):
				error_exit(32, "Invalid variable name format in variable: " + value)
		elif type == 'bool':
			if value == 'true':
				self.value = True
			elif value == 'false':
				self.value = False
			else:
				error_exit(32, "Invalid boolean value, can be only either true or false")
		elif type == 'int':
			try:
				self.value = int(value)
			except:
				error_exit(32, "Invalid integer value")
		elif type == 'nil':
			if value == 'nil':
				self.value = None
			else:
				error_exit(32, "Literal of type nil can only have value \'nil\'")
		elif type == 'type':
			if value != 'int' and value != 'string' and value != 'bool':
				error_exit(32, "Literal of type \'type\' can only have value \'int\', \'string\', or \'bool\'")
		elif type == 'string':
			if not value:
				self.value = '' # osetreni toho, ze by se misto prazdneho retezce ulozilo 'None'
			else:
				regex = re.compile(r"\\(\d{1,3})") # u stringu musime prevest dekadicke escape sekvence
				self.value = regex.sub(replace_regex, value)



# trida pro radek s instrukci
class Instruction:
	def __init__(self, order, opcode, arg1=None, arg1type=None, arg2=None, arg2type=None, arg3=None, arg3type=None):
		self.order = order
		if opcode_dictionary_check(opcode):
			self.opcode = str.upper(opcode)
		else:
			error_exit(32, "Invalid instruction name " + opcode)

		self.arg1 = InsArgument(arg1, arg1type)
		self.arg2 = InsArgument(arg2, arg2type)
		self.arg3 = InsArgument(arg3, arg3type)



# zkontroluje spravny zapis xml elementu, vrati serazenou mnozinu instrukci
def make_instruction_list(root):
	ins_list = []
	duplicates = []

	for child in root:
		if str.lower(child.tag) != 'instruction':
			error_exit(32, "Every element inside program (except for header) has to be named instruction")
		try:
			opcode = child.attrib['opcode']
		except:
			error_exit(32, "Opcode attribute either missing for 1 or more instructions")
		try:
			order = int(child.attrib['order'])
		except:
			error_exit(32, "Order attribute either missing or written in invalid format in 1 or more instructions")

		if order in duplicates:
			error_exit(32, "Cannot have instructions with duplicit order in source XML file")
		duplicates.append(order)

		args = [None, None, None]
		argtps = [None, None, None]

		for kid in child:
			if kid.tag[:3] != 'arg' or (kid.tag[3:] != '1' and kid.tag[3:] != '2' and kid.tag[3:] != '3'):
				error_exit(32, "Instructions can only have subelements named arg{1,2,3}")
			else:
				i = int(kid.tag[3:]) - 1 # pole jsou indexovana od 0, ale argy v XML od 1
				try:
					args[i] = kid.text
					argtps[i] = str.lower(kid.attrib['type'])
				except:
					error_exit(32, "Instruction sub-element arg{1,2,3} is missing attribute type")

		new_ins = Instruction(order, opcode, args[0], argtps[0], args[1], argtps[1], args[2], argtps[2])

		ins_list.append(new_ins)

	sorted_list = sorted(ins_list, key=lambda x: x.order)
	if len(sorted_list):
		if sorted_list[0].order < 1:
			error_exit(32, "Cannot have instruction with order smaller than 1 in source XML file")
	return sorted_list



# Trida pro promennou umistenou v nejakem ramci
class Variable:
	def __init__(self, name, type=None, value=None):
		self.name = name
		self.type = type
		self.value = value



# Trida pro dany ramec obsahujici promenne
class Frame:
	def __init__(self):
		self.vars = []

	def pushVar(self, variable):
		self.vars.append(variable)

	def popVar(self, name):
		for var in self.vars:
			if var.name == name:
				self.vars.remove(var)
				return var
		error_exit(54, "Cannot access undeclared variable " + name)

	def isVar(self, name):
		for var in self.vars:
			if var.name == name:
				return True
		return False



# Trida pro label, uklada se jmeno a pozice pro program counter
class Label:
	def __init__(self, name, location):
		self.name = name
		self.loc = location



# trida kde jsou ramce s promennymi, a dale pole labelu, atd. pro dany beh programu
class RuntimeMemory:
	def __init__(self):
		self.Labels = [] # seznam vytvorenych labelu
		self.pcStack = [] # zasobnik pro interni citac
		self.dataStack = [] # datovy zasobnik pro zasobnikove instrukce
		# definice ramcu
		self.GF = Frame()
		self.LFs = []
		self.TF = None

	# prohleda dany ramec ktery vyvodi ze jmena prommenne a zkusi danou promennou najit
	# POZOR: PROMENNA BUDE Z RAMCE SMAZANA, JE TREBA JI ZNOVU ULOZIT METODOU saveVar
	def loadVar(self, name):
		split = name.split('@', 1)
		if split[0] == 'GF':
			return self.GF.popVar(name)
		elif split[0] == 'LF':
			if self.LFs:
				return self.LFs[len(self.LFs)-1].popVar(name)
			else:
				error_exit(55, "The local frame does not exist")
		else:
			if self.TF:
				return self.TF.popVar(name)
			else:
				error_exit(55, "The temporary frame does not exist")

	# ulozi novou promennou do daneho ramce (cilovy ramec se vyvodi ze jmena prommene ve tvaru frame@name)
	def saveVar(self, variable):
		split = variable.name.split('@', 1)

		if split[0] == 'GF':
			self.GF.pushVar(variable)
		elif split[0] == 'LF':
			if self.LFs:
				self.LFs[len(self.LFs)-1].pushVar(variable)
			else:
				error_exit(55, "The local frame does not exist")
		else:
			if self.TF:
				self.TF.pushVar(variable)
			else:
				error_exit(55, "The temporary frame does not exist")

	# kontroluje zda je promenna s danym jmenem definovana, pokud ano vraci true, jinak false
	def isDefinedVar(self, name):
		split = name.split('@', 1)
		if split[0] == 'GF':
			return self.GF.isVar(name)
		elif split[0] == 'LF':
			if self.LFs:
				return self.LFs[len(self.LFs)-1].isVar(name)
			else:
				error_exit(55, "The local frame does not exist")
		else:
			if self.TF:
				return self.TF.isVar(name)
			else:
				error_exit(55, "The temporary frame does not exist")

	def createTF(self):
		self.TF = Frame()

	def pushLF(self):
		if self.TF:
			for var in self.TF.vars:
				var.name = re.sub('TF@', 'LF@', var.name, 1) # aktualizovani nazvu promenne kvuli predpone
			self.LFs.append(self.TF)
			self.TF = None
		else:
			error_exit(55, "Cannot push undefined temporary frame to local frames")

	def popLF(self):
		if self.LFs:
			tmp = self.LFs.pop()
			for var in tmp.vars:
				var.name = re.sub('LF@', 'TF@', var.name, 1) # aktualizovani nazvu promenne kvuli predpone
			self.TF = tmp
		else:
			error_exit(55, "Cannot remove non-existing local frame")

	def writeLabel(self, name, location):
		label = Label(name, location)
		self.Labels.append(label)

	def readLabel(self, name):
		for l in self.Labels:
			if l.name == name:
				return l.loc
		error_exit(52, "Cannot jump to undefined label: " + name)

	def isLabel(self, name):
		for l in self.Labels:
			if l.name == name:
				return True
		return False

	def pcPush(self, pos):
		self.pcStack.append(pos)

	def pcPop(self):
		try:
			return self.pcStack.pop()
		except:
			error_exit(56, "Cannot execute RETURN, program counter stack is empty")

	def dataStackPush(self, type, value):
		pushed = Variable(None, type, value) # abych nevytvarel extra tridu, pouziva data stack vlastne nepojmenovane promenne
		self.dataStack.append(pushed)

	def dataStackPop(self):
		try:
			return self.dataStack.pop()
		except:
			error_exit(56, "Cannot pop value from empty data stack")




##############################################################################
##################### DEFINICE HLAVNI FUNKCE MAIN ############################
##############################################################################

def main():
# nazvy vstupnich souboru predane jako argumenty interpretu a seznam argumentu
	source_file = ''
	input_fn = ''
	args = sys.argv[1:]

	# Osetreni argumentu
	if len(args) > 2:
		error_exit(10, "Entered too many arguments")
	elif len(args) < 1:
		error_exit(10, "No arguments entered, run the program with only --help to see the correct usage")

	for arg in args:
		if arg == '--help' or arg == '-help':
			if len(args) == 1:
				help()
			else:
				error_exit(10, "Cannot combine --help with other arguments")
		elif arg[:9] == '--source=':
			source_file = arg[9:]
			if not source_file:
				error_exit(10, "Source file name cannot be empty string")
		elif arg[:8] == '-source=':
			source_file = arg[8:]
			if not source_file:
				error_exit(10, "Source file name cannot be empty string")
		elif arg[:8] == '--input=':
			input_fn = arg[8:]
			if not input_fn:
				error_exit(10, "Input file name cannot be empty string")
		elif arg[:7] == '-input=':
			input_fn = arg[7:]
			if not input_fn:
				error_exit(10, "Input file name cannot be empty string")
		else:
			error_exit(10, "Invalid argument(s)")

	# otevreni souboru, pokud byl zadan jen jeden vstupni soubor, druhy nastavime na stdin
	if not source_file:
		source_file = sys.stdin

	if not input_fn:
		input_file = sys.stdin
	else:
		try:
			input_file = open(input_fn, "r")
		except:
			error_exit(11, "Could not open selected input file")


	# nacteni a parsing vstupniho xml souboru do stromove struktury, zaroven kontrola zda je well formed
	try:
		tree = ET.parse(source_file)
	except OSError:
		error_exit(11, "Could not open selected source file")
	except:
		error_exit(31, "Selected source file is not well-formed XML file")

	root = tree.getroot()

	check_header(root) #kontrola ocekavaneho korenoveho elementu

	instruction_list = make_instruction_list(root)

	RM = RuntimeMemory() # nastaveni pameti kde ukladame ramce promennych a labely daneho behu

	# instruction PRE-RUN na definici labelu
	for i in range(len(instruction_list)):
		current_ins = instruction_list[i]
		if current_ins.opcode == 'LABEL':
			if current_ins.arg1.type != 'label' or current_ins.arg2.type or current_ins.arg3.type:
				error_exit(32, "Label instruction expects single argument of type label")
			if RM.isLabel(current_ins.arg1.value):
				error_exit(52, "Cannot redefine already defined label: " + current_ins.arg1.value)
			else:
				RM.writeLabel(current_ins.arg1.value, i)

	pc = 0 # program counter
	lines_read = 0 # pocitadlo prectenych radku s instrukci kvuli vypisu pri BREAK

	# cyklus vykonavani instrukci
	while(True):
		try:
			current_ins = instruction_list[pc] # nacteni prvni nebo dalsi instrukce
		except:
			break # dosli jsme na konec programu, pripadne treba program neobsahuje zadne instrukce
		# v pythonu neni switch :(
		opcode = current_ins.opcode



		if opcode == 'MOVE':
			if current_ins.arg1.type != 'var' or not isSymbol(current_ins.arg2.type) or current_ins.arg3.type:
				error_exit(32, "Instruction MOVE expects 2 arguments in this order: variable and symbol")
			toVar = RM.loadVar(current_ins.arg1.value)
			if current_ins.arg2.type == 'var' and current_ins.arg2.value != current_ins.arg1.value: #druha podminka preskakuje kopirovani promenne samo do sebe
				fromVar = RM.loadVar(current_ins.arg2.value)
				if not fromVar.type:
					error_exit(56, "Cannot copy value from uninitilized variable: " + fromVar.name)
				toVar.type = fromVar.type
				toVar.value = fromVar.value
				RM.saveVar(fromVar)
			elif current_ins.arg2.value != current_ins.arg1.value:
				toVar.type = current_ins.arg2.type
				toVar.value = current_ins.arg2.value
			RM.saveVar(toVar)



		elif opcode == 'CREATEFRAME':
			if current_ins.arg1.type or current_ins.arg2.type or current_ins.arg3.type:
				error_exit(32, "Instruction CREATEFRAME does not accept any arguments")
			RM.createTF()



		elif opcode == 'PUSHFRAME':
			if current_ins.arg1.type or current_ins.arg2.type or current_ins.arg3.type:
				error_exit(32, "Instruction PUSHFRAME does not accept any arguments")
			RM.pushLF()



		elif opcode == 'POPFRAME':
			if current_ins.arg1.type or current_ins.arg2.type or current_ins.arg3.type:
				error_exit(32, "Instruction POPFRAME does not accept any arguments")
			RM.popLF()



		elif opcode == 'DEFVAR':
			if current_ins.arg1.type != 'var' or current_ins.arg2.type or current_ins.arg3.type:
				error_exit(32, "Instruction DEFVAR expects single argument of type var")
			if RM.isDefinedVar(current_ins.arg1.value):
				error_exit(52, "Cannot redefine already defined variable: " + current_ins.arg1.value)
			newVar = Variable(current_ins.arg1.value)
			RM.saveVar(newVar)



		elif opcode == 'CALL':
			if current_ins.arg1.type != 'label' or current_ins.arg2.type or current_ins.arg3.type:
				error_exit(32, "Instruction CALL expects single argument of type label")
			RM.pcPush(pc)
			pc = RM.readLabel(current_ins.arg1.value)



		elif opcode == 'RETURN':
			if current_ins.arg1.type or current_ins.arg2.type or current_ins.arg3.type:
				error_exit(32, "Instruction RETURN does not accept any arguments")
			pc = RM.pcPop()



		elif opcode == 'PUSHS':
			if not isSymbol(current_ins.arg1.type) or current_ins.arg2.type or current_ins.arg3.type:
				error_exit(32, "Instruction PUSHS expects single argument of type symbol")
			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig1value = current_ins.arg1.value
			orig1type = current_ins.arg1.type

			if current_ins.arg1.type == 'var':
				tmpVar = RM.loadVar(current_ins.arg1.value) # symbol je promenna
				if tmpVar.type == None:
					error_exit(56, "Cannot push uninitilized variable to stack")
				current_ins.arg1.type = tmpVar.type
				current_ins.arg1.value = tmpVar.value
				RM.saveVar(tmpVar)
				varSwitch = True

			RM.dataStackPush(current_ins.arg1.type, current_ins.arg1.value)
			if varSwitch:
				current_ins.arg1.value = orig1value
				current_ins.arg1.type = orig1type



		elif opcode == 'POPS':
			if current_ins.arg1.type != 'var' or current_ins.arg2.type or current_ins.arg3.type:
				error_exit(32, "Instruction POPS expects single argument of type variable")
			toVar = RM.loadVar(current_ins.arg1.value)
			popped = RM.dataStackPop()
			toVar.type = popped.type
			toVar.value = popped.value
			RM.saveVar(toVar)



		elif opcode == 'ADD' or opcode == 'SUB' or opcode == 'MUL' or opcode == 'IDIV':
			if current_ins.arg1.type != 'var' or not isSymbol(current_ins.arg2.type) or not isSymbol(current_ins.arg3.type):
				error_exit(32, "Arithmetic instructions expect 3 arguments in this order: variable, symbol, symbol")
			toVar = RM.loadVar(current_ins.arg1.value)
			toVar.type = 'int'

			if current_ins.arg2.type == 'int':
				val1 = current_ins.arg2.value
			elif current_ins.arg2.type == 'var':
				if current_ins.arg2.value == current_ins.arg1.value: #jedna se o stejnou promennou jako je cil vysledku operace
					val1 = toVar.value
				else:
					tmpVar = RM.loadVar(current_ins.arg2.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform arithmetic operation with uninitilized variable")
					if tmpVar.type != 'int':
						error_exit(53, "Invalid arithmetic instruction operand value read from variable, must be an integer")
					val1 = tmpVar.value
					RM.saveVar(tmpVar)
			else:
				error_exit(53, "Second and third arithmetic instruction operands must be either constant or variable of type int")

			if current_ins.arg3.type == 'int':
				val2 = current_ins.arg3.value
			elif current_ins.arg3.type == 'var':
				if current_ins.arg3.value == current_ins.arg1.value: #jedna se o stejnou promennou jako je cil vysledku operace
					val2 = toVar.value
				elif current_ins.arg3.value == current_ins.arg2.value: #jedna se o stejnou promennou jako je predesly operand
					val2 = val1
				else:
					tmpVar = RM.loadVar(current_ins.arg3.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform arithmetic operation with uninitilized variable")
					if tmpVar.type != 'int':
						error_exit(53, "Invalid arithmetic instruction operand value read from variable, must be an integer")
					val2 = tmpVar.value
					RM.saveVar(tmpVar)
			else:
				error_exit(53, "Second and third arithmetic instruction operands must be either constant or variable of type int")

			if opcode == 'ADD':
				toVar.value = val1 + val2
			elif opcode == 'SUB':
				toVar.value = val1 - val2
			elif opcode == 'MUL':
				toVar.value = val1 * val2
			else: # IDIV
				if val2 == 0:
					error_exit(57, "Zero division attempt")
				toVar.value = int(val1 / val2)
			RM.saveVar(toVar)



		elif opcode == 'LT' or opcode == 'GT' or opcode == 'EQ':
			if current_ins.arg1.type != 'var' or not isSymbol(current_ins.arg2.type) or not isSymbol(current_ins.arg3.type):
				error_exit(32, "Comparison instructions expect 3 arguments in this order: var, symbol, symbol")
			GeorgeBool = False
			toVar = RM.loadVar(current_ins.arg1.value)

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig2value = current_ins.arg2.value
			orig2type = current_ins.arg2.type
			orig3value = current_ins.arg3.value
			orig3type = current_ins.arg3.type

			if current_ins.arg2.type == 'var': # prvni symbol je promenna
				if current_ins.arg2.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg2.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform comparison operation with uninitilized variable")
					current_ins.arg2.type = tmpVar.type
					current_ins.arg2.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg2.type = toVar.type
					current_ins.arg2.value = toVar.value
				varSwitch = True

			if current_ins.arg3.type == 'var': # druhy symbol je promenna
				if current_ins.arg3.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg3.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform comparison operation with uninitilized variable")
					current_ins.arg3.type = tmpVar.type
					current_ins.arg3.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg3.type = toVar.type
					current_ins.arg3.value = toVar.value
				varSwitch = True

			toVar.type = 'bool'
			toVar.value = False
			if current_ins.arg2.type != current_ins.arg3.type: # operandy jsou ruznych typu
				if current_ins.arg2.type != 'nil' and current_ins.arg3.type != 'nil':
					error_exit(53, "Both symbols must be of the same type (or at least one of them must be nil and the operation must be EQ) when using comparing instructions")
				elif opcode != 'EQ':
					error_exit(53, "If one of the compared symbols is of type nil, the only comparing instruction done can be EQ")

			else: # operandy jsou stejneho typu
				if current_ins.arg2.type == 'nil':
					if opcode == 'EQ':
						GeorgeBool = True
					else:
						error_exit(53, "Two symbols of type nil can only be compared with EQ instruction")

				elif current_ins.arg2.type == 'int':
					if opcode == 'LT' and current_ins.arg2.value < current_ins.arg3.value:
						GeorgeBool = True
					elif opcode == 'GT' and current_ins.arg2.value > current_ins.arg3.value:
						GeorgeBool = True
					elif opcode == 'EQ' and current_ins.arg2.value == current_ins.arg3.value:
						GeorgeBool = True

				elif current_ins.arg2.type == 'bool':
					if opcode == 'LT' and not current_ins.arg2.value and current_ins.arg3.value:
						GeorgeBool = True
					elif opcode == 'GT' and current_ins.arg2.value and not current_ins.arg3.value:
						GeorgeBool = True
					elif opcode == 'EQ' and current_ins.arg2.value == current_ins.arg3.value:
						GeorgeBool = True

				else: # oba symboly typu string
					if opcode == 'LT' and current_ins.arg2.value < current_ins.arg3.value:
						GeorgeBool = True
					elif opcode == 'GT' and current_ins.arg2.value > current_ins.arg3.value:
						GeorgeBool = True
					elif opcode == 'EQ' and current_ins.arg2.value == current_ins.arg3.value:
						GeorgeBool = True

			if varSwitch:
				current_ins.arg2.value = orig2value
				current_ins.arg2.type = orig2type
				current_ins.arg3.value = orig3value
				current_ins.arg3.type = orig3type
			if GeorgeBool: # pokud byla podminka splnena
				toVar.value = True
			RM.saveVar(toVar)



		elif opcode == 'AND' or opcode == 'OR':
			if current_ins.arg1.type != 'var' or not isSymbol(current_ins.arg2.type) or not isSymbol(current_ins.arg3.type):
				error_exit(32, "Logical AND, OR instructions expect 3 arguments in this order: var, symbol, symbol")
			GeorgeBool = False
			toVar = RM.loadVar(current_ins.arg1.value)

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig2value = current_ins.arg2.value
			orig2type = current_ins.arg2.type
			orig3value = current_ins.arg3.value
			orig3type = current_ins.arg3.type

			if current_ins.arg2.type == 'var': # prvni symbol je promenna
				if current_ins.arg2.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg2.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform logic operation with uninitilized variable")
					current_ins.arg2.type = tmpVar.type
					current_ins.arg2.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg2.type = toVar.type
					current_ins.arg2.value = toVar.value
				varSwitch = True

			if current_ins.arg3.type == 'var': # druhy symbol je promenna
				if current_ins.arg3.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg3.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform logic operation with uninitilized variable")
					current_ins.arg3.type = tmpVar.type
					current_ins.arg3.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg3.type = toVar.type
					current_ins.arg3.value = toVar.value
				varSwitch = True

			toVar.type = 'bool'
			toVar.value = False
			if current_ins.arg2.type != 'bool' or current_ins.arg3.type != 'bool':
				error_exit(53, "Both logical operation symbols must be of type bool")
			else: # operandy jsou oba bool
				if opcode == 'AND' and (current_ins.arg2.value and current_ins.arg3.value):
					GeorgeBool = True
				elif opcode == 'OR' and (current_ins.arg2.value or current_ins.arg3.value):
					GeorgeBool = True

			if varSwitch:
				current_ins.arg2.value = orig2value
				current_ins.arg2.type = orig2type
				current_ins.arg3.value = orig3value
				current_ins.arg3.type = orig3type
			if GeorgeBool: # pokud byla podminka splnena
				toVar.value = True
			RM.saveVar(toVar)


		elif opcode == 'NOT':
			if current_ins.arg1.type != 'var' or not isSymbol(current_ins.arg2.type) or current_ins.arg3.type:
				error_exit(32, "Logical NOT instruction expect 2 arguments in this order: var, symbol")
			toVar = RM.loadVar(current_ins.arg1.value)

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig2value = current_ins.arg2.value
			orig2type = current_ins.arg2.type

			if current_ins.arg2.type == 'var': # symbol je promenna
				if current_ins.arg2.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg2.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform logic operation with uninitilized variable")
					current_ins.arg2.type = tmpVar.type
					current_ins.arg2.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg2.type = toVar.type
					current_ins.arg2.value = toVar.value
				varSwitch = True

			toVar.type = 'bool'
			toVar.value = False
			if current_ins.arg2.type != 'bool':
				error_exit(53, "logical NOT operation symbol must be of type bool")
			toVar.value = not current_ins.arg2.value
			if varSwitch:
				current_ins.arg2.value = orig2value
				current_ins.arg2.type = orig2type
			RM.saveVar(toVar)



		elif opcode == 'INT2CHAR':
			if current_ins.arg1.type != 'var' or not isSymbol(current_ins.arg2.type) or current_ins.arg3.type:
				error_exit(32, "INT2CHAR instruction expect 2 arguments in this order: var, symbol")
			toVar = RM.loadVar(current_ins.arg1.value)

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig2value = current_ins.arg2.value
			orig2type = current_ins.arg2.type

			if current_ins.arg2.type == 'var': # symbol je promenna
				if current_ins.arg2.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg2.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform type conversion operation with uninitilized variable")
					current_ins.arg2.type = tmpVar.type
					current_ins.arg2.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg2.type = toVar.type
					current_ins.arg2.value = toVar.value
				varSwitch = True

			toVar.type = 'string'
			if current_ins.arg2.type != 'int':
				error_exit(53, "INT2CHAR instruction symbol must be of type int")
			try:
				toVar.value = chr(current_ins.arg2.value)
			except:
				error_exit(58, "INT2CHAR integer argument out of unicode coding range")

			if varSwitch:
				current_ins.arg2.value = orig2value
				current_ins.arg2.type = orig2type
			RM.saveVar(toVar)



		elif opcode == 'STRI2INT':
			if current_ins.arg1.type != 'var' or not isSymbol(current_ins.arg2.type) or not isSymbol(current_ins.arg3.type):
				error_exit(32, "STRI2INT instruction expect 3 arguments in this order: var, symbol, symbol")
			toVar = RM.loadVar(current_ins.arg1.value)

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig2value = current_ins.arg2.value
			orig2type = current_ins.arg2.type
			orig3value = current_ins.arg3.value
			orig3type = current_ins.arg3.type

			if current_ins.arg2.type == 'var': # prvni symbol je promenna
				if current_ins.arg2.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg2.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform type conversion operation with uninitilized variable")
					current_ins.arg2.type = tmpVar.type
					current_ins.arg2.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg2.type = toVar.type
					current_ins.arg2.value = toVar.value
				varSwitch = True

			if current_ins.arg3.type == 'var': # druhy symbol je promenna
				if current_ins.arg3.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg3.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform type conversion operation with uninitilized variable")
					current_ins.arg3.type = tmpVar.type
					current_ins.arg3.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg3.type = fromVar.type
					current_ins.arg3.value = toVar.value
				varSwitch = True

			if current_ins.arg2.type != 'string' or current_ins.arg3.type != 'int':
				error_exit(53, "STRI2INT instruction symbols must be of type string and int")

			toVar.type = 'int'
			if current_ins.arg3.value < 0: # specialni osetreni, python by me jinak nechal indexovat zapornym cislem bez chyby
				error_exit(58, "Indexation out of range in STRI2INT instruction")
			try:
				toVar.value = ord( current_ins.arg2.value[current_ins.arg3.value] )
			except:
				error_exit(58, "Indexation out of range in STRI2INT instruction")

			if varSwitch:
				current_ins.arg2.value = orig2value
				current_ins.arg2.type = orig2type
				current_ins.arg3.value = orig3value
				current_ins.arg3.type = orig3type
			RM.saveVar(toVar)



		elif opcode == 'READ':
			if current_ins.arg1.type != 'var' or (current_ins.arg2.type != 'type' and current_ins.arg2.type != 'string' and current_ins.arg2.type != 'var') or current_ins.arg3.type:
				error_exit(32, "READ instruction expects two arguments of types var and \'type\' (either type itself or string containing name of type)")
			toVar = RM.loadVar(current_ins.arg1.value)

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig2value = current_ins.arg2.value
			orig2type = current_ins.arg2.type

			if current_ins.arg2.type == 'var': # symbol je promenna
				if current_ins.arg2.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg2.value)
					current_ins.arg2.type = tmpVar.type
					current_ins.arg2.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg2.type = toVar.type
					current_ins.arg2.value = toVar.value
				varSwitch = True

			if current_ins.arg2.type != 'string' and current_ins.arg2.type != 'type':
				error_exit(53, "READ instruction symbol must be of type string or \'type\'")
			if current_ins.arg2.value == 'int':
				toVar.type = 'int'
				try:
					toVar.value = int(input_file.readline().rstrip('\n'))
				except:
					toVar.type = 'nil'
					toVar.value = None
			elif current_ins.arg2.value == 'string':
				toVar.type = 'string'
				try:
					toVar.value = input_file.readline().rstrip('\n')
				except:
					toVar.type = 'nil'
					toVar.value = None
			elif current_ins.arg2.value == 'bool':
				toVar.type = 'bool'
				try:
					tmp = input_file.readline().rstrip('\n')
					if tmp.lower() == 'true':
						toVar.value = True
					else:
						toVar.value = False
				except:
					toVar.type = 'nil'
					toVar.value = None
			else:
				error_exit(53, "Type in READ instruction can only be \'int\', \'string\' or \'bool\'")

			if varSwitch:
				current_ins.arg2.value = orig2value
				current_ins.arg2.type = orig2type
			RM.saveVar(toVar)



		elif opcode == 'WRITE':
			if not isSymbol(current_ins.arg1.type) or current_ins.arg2.type or current_ins.arg3.type:
				error_exit(32, "WRITE instruction expects only one argument that must be symbol")

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig1value = current_ins.arg1.value
			orig1type = current_ins.arg1.type

			if current_ins.arg1.type == 'var':
				tmpVar = RM.loadVar(current_ins.arg1.value) # symbol je promenna
				if tmpVar.type == None:
					error_exit(56, "Cannot print uninitilized variable")
				current_ins.arg1.type = tmpVar.type
				current_ins.arg1.value = tmpVar.value
				RM.saveVar(tmpVar)
				varSwitch = True

			if current_ins.arg1.type == 'string' or current_ins.arg1.type == 'int' or current_ins.arg1.type == 'type':
				print(current_ins.arg1.value, end='')
			elif current_ins.arg1.type == 'bool':
				if current_ins.arg1.value:
					print("true", end='')
				else:
					print("false", end='')
			else: # typ nil, ten ma vytisknout prazdny retezec
				print('', end='')

			if varSwitch:
				current_ins.arg1.value = orig1value
				current_ins.arg1.type = orig1type



		elif opcode == 'CONCAT':
			if current_ins.arg1.type != 'var' or not isSymbol(current_ins.arg2.type) or not isSymbol(current_ins.arg3.type):
				error_exit(32, "CONCAT instruction expect 3 arguments in this order: var, symbol, symbol")
			toVar = RM.loadVar(current_ins.arg1.value)

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig2value = current_ins.arg2.value
			orig2type = current_ins.arg2.type
			orig3value = current_ins.arg3.value
			orig3type = current_ins.arg3.type

			if current_ins.arg2.type == 'var':
				if current_ins.arg2.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg2.value) # prvni symbol je promenna
					if tmpVar.type == None:
						error_exit(56, "Cannot perform string-specific instructions with uninitilized variable")
					current_ins.arg2.type = tmpVar.type
					current_ins.arg2.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg2.type = toVar.type
					current_ins.arg2.value = toVar.value
				varSwitch = True

			if current_ins.arg3.type == 'var':
				if current_ins.arg3.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg3.value) # druhy symbol je promenna
					if tmpVar.type == None:
						error_exit(56, "Cannot perform string-specific instructions with uninitilized variable")
					current_ins.arg3.type = tmpVar.type
					current_ins.arg3.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg3.type = toVar.type
					current_ins.arg3.value = toVar.value
				varSwitch = True

			toVar.type = 'string'
			if current_ins.arg2.type == 'string' and current_ins.arg3.type == 'string':
				toVar.value = current_ins.arg2.value + current_ins.arg3.value # konkatenace retezcu
			else:
				error_exit(53, "You can only concatenate two strings")

			if varSwitch:
				current_ins.arg2.value = orig2value
				current_ins.arg2.type = orig2type
				current_ins.arg3.value = orig3value
				current_ins.arg3.type = orig3type
			RM.saveVar(toVar)



		elif opcode == 'STRLEN':
			if current_ins.arg1.type != 'var' or not isSymbol(current_ins.arg2.type) or current_ins.arg3.type:
				error_exit(32, "STRLEN instruction expect 2 arguments in this order: var, symbol")
			toVar = RM.loadVar(current_ins.arg1.value)

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig2value = current_ins.arg2.value
			orig2type = current_ins.arg2.type

			if current_ins.arg2.type == 'var': # symbol je promenna
				if current_ins.arg2.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg2.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform string-specific instructions with uninitilized variable")
					current_ins.arg2.type = tmpVar.type
					current_ins.arg2.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg2.type = toVar.type
					current_ins.arg2.value = toVar.value
				varSwitch = True

			toVar.type = 'int'
			if current_ins.arg2.type != 'string':
				error_exit(53, "STRLEN instruction symbol must be of type string")
			toVar.value = len(current_ins.arg2.value)

			if varSwitch:
				current_ins.arg2.value = orig2value
				current_ins.arg2.type = orig2type
			RM.saveVar(toVar)



		elif opcode == 'GETCHAR':
			if current_ins.arg1.type != 'var' or not isSymbol(current_ins.arg2.type) or not isSymbol(current_ins.arg3.type):
				error_exit(32, "GETCHAR instruction expects 3 arguments in this order: var, symbol, symbol")
			toVar = RM.loadVar(current_ins.arg1.value)

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig2value = current_ins.arg2.value
			orig2type = current_ins.arg2.type
			orig3value = current_ins.arg3.value
			orig3type = current_ins.arg3.type

			if current_ins.arg2.type == 'var': # prvni symbol je promenna
				if current_ins.arg2.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg2.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform string-specific instructions with uninitilized variable")
					current_ins.arg2.type = tmpVar.type
					current_ins.arg2.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg2.type = toVar.type
					current_ins.arg2.value = toVar.value
				varSwitch = True

			if current_ins.arg3.type == 'var': # druhy symbol je promenna
				if current_ins.arg3.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg3.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform string-specific instructions with uninitilized variable")
					current_ins.arg3.type = tmpVar.type
					current_ins.arg3.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg3.type = toVar.type
					current_ins.arg3.value = toVar.value
				varSwitch = True

			toVar.type = 'string'
			if current_ins.arg2.type != 'string' or current_ins.arg3.type != 'int':
				error_exit(53, "GETCHAR instruction symbols must be of type string and int")
			if current_ins.arg3.value < 0:
				error_exit(58, "Character index in GETCHAR instruction must be an integer >= 0")
			try:
				toVar.value = current_ins.arg2.value[current_ins.arg3.value]
			except:
				error_exit(58, "Character index in GETCHAR is out of range")

			if varSwitch:
				current_ins.arg2.value = orig2value
				current_ins.arg2.type = orig2type
				current_ins.arg3.value = orig3value
				current_ins.arg3.type = orig3type
			RM.saveVar(toVar)



		elif opcode == 'SETCHAR':
			if current_ins.arg1.type != 'var' or not isSymbol(current_ins.arg2.type) or not isSymbol(current_ins.arg3.type):
				error_exit(32, "SETCHAR instruction expects 3 arguments in this order: var, symbol, symbol")
			toVar = RM.loadVar(current_ins.arg1.value)
			if toVar.type == None:
				error_exit(56, "Cannot perform string-specific instructions with uninitilized variable")
			if toVar.type != 'string':
				error_exit(53, "The first argument of SETCHAR instruction must be a variable containing string")

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig2value = current_ins.arg2.value
			orig2type = current_ins.arg2.type
			orig3value = current_ins.arg3.value
			orig3type = current_ins.arg3.type

			if current_ins.arg2.type == 'var': # prvni symbol je promenna
				if current_ins.arg2.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg2.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform string-specific instructions with uninitilized variable")
					current_ins.arg2.type = tmpVar.type
					current_ins.arg2.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg2.type = toVar.type
					current_ins.arg2.value = toVar.value
				varSwitch = True

			if current_ins.arg3.type == 'var': # druhy symbol je promenna
				if current_ins.arg3.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg3.value)
					if tmpVar.type == None:
						error_exit(56, "Cannot perform string-specific instructions with uninitilized variable")
					current_ins.arg3.type = tmpVar.type
					current_ins.arg3.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg3.type = toVar.type
					current_ins.arg3.value = toVar.value
				varSwitch = True

			if current_ins.arg2.type != 'int' or current_ins.arg3.type != 'string':
				error_exit(53, "SETCHAR instruction symbols must be of type int and string")
			if toVar.value == '' or current_ins.arg3.value == '':
				error_exit(58, "Both strings passed as arguments to GETCHAR must not be empty")
			if current_ins.arg2.value < 0:
				error_exit(58, "Character index in SETCHAR instruction must be an integer >= 0")
			elif current_ins.arg2.value >= len(toVar.value):
				error_exit(58, "Character index in SETCHAR instruction is out of range")
			try:
				toVar.value = toVar.value[:current_ins.arg2.value] + current_ins.arg3.value[0] + toVar.value[current_ins.arg2.value+1:]
			except:
				print(toVar.value[current_ins.arg2.value], current_ins.arg3.value[0])
				error_exit(58, "Character index in SETCHAR instruction is out of range")

			if varSwitch:
				current_ins.arg2.value = orig2value
				current_ins.arg2.type = orig2type
				current_ins.arg3.value = orig3value
				current_ins.arg3.type = orig3type
			RM.saveVar(toVar)



		elif opcode == 'TYPE':
			if current_ins.arg1.type != 'var' or not isSymbol(current_ins.arg2.type) or current_ins.arg3.type:
				error_exit(32, "Instruction TYPE expects 2 arguments in this order: var, symbol")
			toVar = RM.loadVar(current_ins.arg1.value)

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig2value = current_ins.arg2.value
			orig2type = current_ins.arg2.type

			if current_ins.arg2.type == 'var': # symbol je promenna
				if current_ins.arg2.value != current_ins.arg1.value:
					tmpVar = RM.loadVar(current_ins.arg2.value)
					current_ins.arg2.type = tmpVar.type
					current_ins.arg2.value = tmpVar.value
					RM.saveVar(tmpVar)
				else:
					current_ins.arg2.type = toVar.type
					current_ins.arg2.value = toVar.value
				varSwitch = True

			toVar.value = current_ins.arg2.type
			if current_ins.arg2.type == None:
				toVar.value = '' # pokud zjistujeme typ neinicializovane promenne
			toVar.type = 'string'

			if varSwitch:
				current_ins.arg2.value = orig2value
				current_ins.arg2.type = orig2type
			RM.saveVar(toVar)



		# LABELY JSOU OSETRENE V PRE-RUNU, PRI JEJICH NALEZENI SE POUZE INKREMENTUJE PROGRAM COUNTER



		elif opcode == 'JUMP':
			if current_ins.arg1.type != 'label' or current_ins.arg2.type or current_ins.arg3.type:
				error_exit(32, "Unconditional jump instruction accepts single argument of type label")
			pc = RM.readLabel(current_ins.arg1.value)



		elif opcode == 'JUMPIFEQ' or opcode == 'JUMPIFNEQ':
			if current_ins.arg1.type != 'label' or not isSymbol(current_ins.arg2.type) or not isSymbol(current_ins.arg3.type):
				error_exit(32, "Conditional jump instructions expect 3 arguments in this order: label, symbol, symbol")
			GeorgeBool = False

			if not RM.isLabel(current_ins.arg1.value):
				error_exit(52, "Cannot evaluate conditional jump for undefined label: " + current_ins.arg1.value)

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig2value = current_ins.arg2.value
			orig2type = current_ins.arg2.type
			orig3value = current_ins.arg3.value
			orig3type = current_ins.arg3.type

			if current_ins.arg2.type == 'var': # prvni symbol je promenna
				tmpVar = RM.loadVar(current_ins.arg2.value)
				if tmpVar.type == None:
					error_exit(56, "Cannot conditional jump instructions with uninitilized variable")
				current_ins.arg2.type = tmpVar.type
				current_ins.arg2.value = tmpVar.value
				RM.saveVar(tmpVar)
				varSwitch = True

			if current_ins.arg3.type == 'var':
				tmpVar = RM.loadVar(current_ins.arg3.value) # druhy symbol je promenna
				if tmpVar.type == None:
					error_exit(56, "Cannot conditional jump instructions with uninitilized variable")
				current_ins.arg3.type = tmpVar.type
				current_ins.arg3.value = tmpVar.value
				RM.saveVar(tmpVar)
				varSwitch = True

			if current_ins.arg2.type != current_ins.arg3.type: # operandy jsou ruznych typu
				if current_ins.arg2.type != 'nil' and current_ins.arg3.type != 'nil':
					error_exit(53, "Both symbols must be of the same type (or at least one of them must be nil and the operation must be EQ) when using comparing instructions")
			else:
				if current_ins.arg2.value == current_ins.arg3.value:
					GeorgeBool = True
			if opcode == 'JUMPIFNEQ':
				GeorgeBool = not GeorgeBool

			if varSwitch:
				current_ins.arg2.value = orig2value
				current_ins.arg2.type = orig2type
				current_ins.arg3.value = orig3value
				current_ins.arg3.type = orig3type
			if GeorgeBool: # pokud byla podminka splnena
				pc = RM.readLabel(current_ins.arg1.value)



		elif opcode == 'EXIT':
			if not isSymbol(current_ins.arg1.type) or current_ins.arg2.type or current_ins.arg3.type:
				error_exit(32, "EXIT instruction expects only one argument that must be symbol")

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig1value = current_ins.arg1.value
			orig1type = current_ins.arg1.type

			if current_ins.arg1.type == 'var':
				tmpVar = RM.loadVar(current_ins.arg1.value) # symbol je promenna
				if tmpVar.type == None:
					error_exit(56, "Cannot use uninitilized variable as exit value")
				current_ins.arg1.type = tmpVar.type
				current_ins.arg1.value = tmpVar.value
				RM.saveVar(tmpVar)
				varSwitch = True

			if current_ins.arg1.type == 'int':
				if current_ins.arg1.value >= 0 and current_ins.arg1.value <= 49:
					sys.exit(current_ins.arg1.value)
				else:
					error_exit(57, "The return value for EXIT must be an integer with value in interval <0, 49>")
			else:
				error_exit(53, "The EXIT instruction operand must be either integer constant or variable")

			if varSwitch:
				current_ins.arg1.value = orig1value
				current_ins.arg1.type = orig1type



		elif opcode == 'DPRINT':
			if not isSymbol(current_ins.arg1.type) or current_ins.arg2.type or current_ins.arg3.type:
				error_exit(32, "DPRINT instruction expects only one argument that must be symbol")

			varSwitch = False # v pripade ze prepiseme puvodni argumenty obsahy promenne
			orig1value = current_ins.arg1.value
			orig1type = current_ins.arg1.type

			if current_ins.arg1.type == 'var':
				tmpVar = RM.loadVar(current_ins.arg1.value) # symbol je promenna
				if tmpVar.type == None:
					error_exit(56, "Cannot print uninitilized variable")
				current_ins.arg1.type = tmpVar.type
				current_ins.arg1.value = tmpVar.value
				RM.saveVar(tmpVar)
				varSwitch = True

			if current_ins.arg1.type == 'string':
				print(current_ins.arg1.value, end='', file=sys.stderr)
			elif current_ins.arg1.type == 'int':
				print(current_ins.arg1.value, end='', file=sys.stderr)
			elif current_ins.arg1.type == 'bool':
				if current_ins.arg1.value:
					print("true", end='', file=sys.stderr)
				else:
					print("false", end='', file=sys.stderr)
			else: # typ nil, ten ma vypsat prazdny retezec
				print('', end='', file=sys.stderr)

			if varSwitch:
				current_ins.arg1.value = orig1value
				current_ins.arg1.type = orig1type



		elif opcode == 'BREAK':
			if current_ins.arg1.type or current_ins.arg2.type or current_ins.arg3.type:
				error_exit(32, "Instruction BREAK does not accept any arguments")
			print("----------------------------------------------------------------", file=sys.stderr)
			print("***Current interpreter state info for debugging***\n", file=sys.stderr)
			print("Operation code of current BREAK instruction: " + str(pc+1), file=sys.stderr)
			print("Number of instructions read/executed: " + str(lines_read), file=sys.stderr)
			print("Number of variables in Global Frame: " + str(len(RM.GF.vars)), file=sys.stderr)
			print("Number of Local Frames in LF stack: " + str(len(RM.LFs)), file=sys.stderr)
			if len(RM.LFs) > 0:
				print("Local Frame currently active: YES", file=sys.stderr)
				print("Number of variables in active Local Frame: " + str(len(RM.LFs[len(RM.LFs)-1].vars)), file=sys.stderr)
			else:
				print("Local Frame currently active: NO", file=sys.stderr)
			if RM.TF != None:
				print("Temporary Frame currently active: YES", file=sys.stderr)
				print("Number of variables in Temporary Frame: " + str(len(RM.TF.vars)), file=sys.stderr)
			else:
				print("Temporary Frame currently active: NO", file=sys.stderr)
			print("----------------------------------------------------------------", file=sys.stderr)



		pc = pc + 1 # inkrementace program counteru
		lines_read = lines_read + 1 # inkrementace poctu prectenych radku s instrukci



	if input_fn:
		input_file.close()
	sys.exit(0)





# spusteni funkce main, pokud tedy tento soubor nepouzivame pouze jako modul
if __name__ == '__main__':
	main()
