#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <set>
#include <cctype>
using namespace std;
string lenght, ZEROS = "000000";

class hexacalculater {
public:
	string toupperHex(string s) {
		string temp = "";
		for (int i = 0;i < s.size();++i) {
			temp += toupper(s[i]);
		}
		return temp;
	}
	int todecimal(string s) {
		int v;
		stringstream ss;
		ss << hex << s;
		ss >> v;
		return v;
	}
	string tohex(int value) {
		stringstream ss;
		ss << hex << value;
		return ss.str();
	}
	string add(string loc, int value) {
		int sum = todecimal(loc);
		sum += value;
		return toupperHex(tohex(sum));
	}
};
class pass1 : hexacalculater {
	set <string> dir;
	map <string, string> symtab, optab;
	void fillOpcode() {
		optab["ADD"] = "18";			optab["LPS"] = "D0";
		optab["ADDF"] = "58";			optab["MUL"] = "20";
		optab["ADDR"] = "90";			optab["MULF"] = "60";
		optab["AND"] = "40";			optab["MULR"] = "98";
		optab["CLEAR"] = "B4";			optab["NORM"] = "C8";
		optab["COMP"] = "28";			optab["OR"] = "44";
		optab["COMPF"] = "88";			optab["RD"] = "D8";
		optab["COMPR"] = "A0";			optab["RMO"] = "AC";
		optab["DIV"] = "24";			optab["RSUB"] = "4C";
		optab["DIVF"] = "64";			optab["SHIFIT"] = "A4";
		optab["DIVR"] = "9C";		optab["SHIFTR"] = "A8";
		optab["FIX"] = "C4";			optab["SIO"] = "F0";
		optab["FLOAT"] = "C0";			optab["SSK"] = "EC";
		optab["HIO"] = "F4";			optab["STA"] = "0C";
		optab["J"] = "3C";				optab["STB"] = "78";
		optab["JEO"] = "30";			optab["STCH"] = "54";
		optab["JLT"] = "38";			optab["STF"] = "80";
		optab["JSUB"] = "48";			optab["STI"] = "D4";
		optab["LDA"] = "00";			optab["STL"] = "14";
		optab["LDB"] = "68";			optab["STS"] = "7C";
		optab["LDCH"] = "50";			optab["STSW"] = "E8";
		optab["LDF"] = "70";			optab["STT"] = "84";
		optab["LDL"] = "08";			optab["STX"] = "10";
		optab["LDS"] = "6C";			optab["SUB"] = "1C";
		optab["LDT"] = "74";			optab["SUBF"] = "5C";
		optab["LDX"] = "04";			optab["SUBR"] = "94";
		optab["STL"] = "14";			optab["WD"] = "DC";
		optab["TIXR"] = "B8";		optab["TIX"] = "2C";
		optab["TIO"] = "F8";			optab["TD"] = "E0";
		optab["SVC"] = "B0";
	}
	void fillDirectives() {
		dir.insert("START");
		dir.insert("END");
		dir.insert("BYTE");
		dir.insert("WORD");
		dir.insert("RESW");
		dir.insert("RESB");
	}
	void writeInterMed(ofstream& print, string loc, string label, string opcode, string operand) {
		if (opcode != "END")
			print << loc;
		else
			print << "\t\t\t\t ";

		if (label != "")
			print << setw(13) << label;
		else if (opcode != "END")
			print << "\t\t\t ";
		print << setw(13) << opcode;
		if (operand != "")
			print << setw(13) << operand;
		print << "\n";
	}
	void get(string line, string& label, string& opcode, string& operand, string loc) {
		line = cutspace(line);
		string temp = "";
		int i = 0;
		while (line[i] != ' ')
			temp += line[i++];
		++i;
		while (i < line.size() && line[i] == ' ') ++i;
		if (optab.find(temp) != optab.end() || dir.find(temp) != dir.end())
			opcode = temp;
		else
		{
			label = temp;
			if (symtab.find(label) != symtab.end())
			{
				cout << "Duplicate-error\n";
				system("pause");
			}
			symtab[label] = loc;
		}

		temp = "";
		while (i < line.size() && line[i] != ' ')
			temp += line[i++];
		++i;
		if ((optab.find(temp) != optab.end() || dir.find(temp) != dir.end()) && opcode == "")
		{
			opcode = temp;
		}
		else {
			operand = temp;
			return;
		}

		temp = "";
		while (i < line.size() && line[i] != ' ')
			temp += line[i++];

		operand = temp;
	}
	string cutspace(string s) {
		for (int i = 0; i < s.size(); i++)
		{
			if (s[i] == '\t') s[i] = ' ';
		}
		int i = 0;
		string temp = "";
		for (int i = 0;i < s.size();++i)
		{
			if (s[i] == ' ') continue;
			while (i < s.size() && s[i] != ' ')
				temp += s[i++];
			temp += ' ';
		}
		return temp;
	}
public:
	pass1() {
		fillOpcode();
		fillDirectives();
	}
	void generatingAddresses() {

		ifstream read("source.txt");
		ofstream print("interMed.txt"); // to open files
		string inputLine, label, opcode, operand, loc, startingAdd, oldloc;

		while (true) {
			getline(read, inputLine);
			if (inputLine[0] == '.') continue; // checking for comment lines
			label = opcode = operand = "";
			get(inputLine, label, opcode, operand, loc);
			oldloc = loc;
			if (opcode == "START")
			{
				loc = operand;
				startingAdd = loc;
				symtab[opcode] = loc;
				oldloc = loc;
			}
			else if (opcode == "END")
				break;
			else if (optab.find(opcode) != optab.end() || opcode == "WORD") {
				loc = add(loc, 3);
			}
			else if (opcode == "RESW") {
				loc = add(loc, 3 * stoi(operand));
			}
			else if (opcode == "RESB") {
				loc = add(loc, stoi(operand));
			}
			else if (opcode == "BYTE") {
				if (operand[0] == 'C')
					loc = add(loc, operand.size() - 3);
				else
					loc = add(loc, ceil(double((operand.size() - 3)) / 2));
			}
			else
			{
				cout << "Invalid-operation\n";
				system("pause");
			}
			writeInterMed(print, oldloc, label, opcode, operand);
		}
		writeInterMed(print, oldloc, label, opcode, operand);
		lenght = toupperHex(tohex(todecimal(oldloc) - todecimal(startingAdd)));
		lenght.insert(0, ZEROS, 0, 6 - lenght.size());
		read.close();
		print.close();
	}
};

class pass2 : hexacalculater {
	set <string> dir;
	map <string, string> symtab, optab, hexcode;
	void fillOpcode() {
		optab["ADD"] = "18";			optab["LPS"] = "D0";
		optab["ADDF"] = "58";			optab["MUL"] = "20";
		optab["ADDR"] = "90";			optab["MULF"] = "60";
		optab["AND"] = "40";			optab["MULR"] = "98";
		optab["CLEAR"] = "B4";			optab["NORM"] = "C8";
		optab["COMP"] = "28";			optab["OR"] = "44";
		optab["COMPF"] = "88";			optab["RD"] = "D8";
		optab["COMPR"] = "A0";			optab["RMO"] = "AC";
		optab["DIV"] = "24";			optab["RSUB"] = "4C";
		optab["DIVF"] = "64";			optab["SHIFIT"] = "A4";
		optab["DIVR"] = "9C";		optab["SHIFTR"] = "A8";
		optab["FIX"] = "C4";			optab["SIO"] = "F0";
		optab["FLOAT"] = "C0";			optab["SSK"] = "EC";
		optab["HIO"] = "F4";			optab["STA"] = "0C";
		optab["J"] = "3C";				optab["STB"] = "78";
		optab["JEO"] = "30";			optab["STCH"] = "54";
		optab["JLT"] = "38";			optab["STF"] = "80";
		optab["JSUB"] = "48";			optab["STI"] = "D4";
		optab["LDA"] = "00";			optab["STL"] = "14";
		optab["LDB"] = "68";			optab["STS"] = "7C";
		optab["LDCH"] = "50";			optab["STSW"] = "E8";
		optab["LDF"] = "70";			optab["STT"] = "84";
		optab["LDL"] = "08";			optab["STX"] = "10";
		optab["LDS"] = "6C";			optab["SUB"] = "1C";
		optab["LDT"] = "74";			optab["SUBF"] = "5C";
		optab["LDX"] = "04";			optab["SUBR"] = "94";
		optab["STL"] = "14";			optab["WD"] = "DC";
		optab["TIXR"] = "B8";		optab["TIX"] = "2C";
		optab["TIO"] = "F8";			optab["TD"] = "E0";
		optab["SVC"] = "B0";

	}
	void fillDirectives() {
		dir.insert("START");
		dir.insert("END");
		dir.insert("BYTE");
		dir.insert("WORD");
		dir.insert("RESW");
		dir.insert("RESB");
	}
	void fillHexacodes() {
		hexcode["NUL"] = "00";			hexcode["RS"] = "1E";
		hexcode[" "] = "00";
		hexcode["SOH"] = "01";			hexcode["US"] = "1F";
		hexcode["STX"] = "02";			hexcode["SP"] = "20";
		hexcode["ETX"] = "03";			hexcode["!"] = "21";
		hexcode["EOT"] = "04";			hexcode["\""] = "22";
		hexcode["ENQ"] = "05";			hexcode["#"] = "23";
		hexcode["ACK"] = "06";			hexcode["$"] = "24";
		hexcode["BEL"] = "07";			hexcode["%"] = "25";
		hexcode["BS"] = "08";			hexcode["&"] = "26";
		hexcode["HT"] = "09";			hexcode["'"] = "27";
		hexcode["LF"] = "0A";			hexcode["("] = "28";
		hexcode["VT"] = "0B";			hexcode[")"] = "29";
		hexcode["FF"] = "0C";			hexcode["*"] = "2A";
		hexcode["CR"] = "0D";			hexcode["+"] = "2B";
		hexcode["SO"] = "0E";			hexcode[","] = "2C";
		hexcode["SI"] = "0F";			hexcode["-"] = "2D";
		hexcode["DLE"] = "10";			hexcode["."] = "2E";
		hexcode["DC1"] = "11";			hexcode["\/"] = "2F";
		hexcode["DC2"] = "12";			hexcode["0"] = "30";
		hexcode["DC3"] = "13";			hexcode["1"] = "31";
		hexcode["DC4"] = "14";			hexcode["2"] = "32";
		hexcode["NAK"] = "15";			hexcode["3"] = "33";
		hexcode["SYN"] = "16";			hexcode["4"] = "34";
		hexcode["ETB"] = "17";			hexcode["5"] = "35";
		hexcode["CAN"] = "18";			hexcode["6"] = "36";
		hexcode["EM"] = "19";			hexcode["7"] = "37";
		hexcode["SUB"] = "1A";			hexcode["8"] = "38";
		hexcode["ESC"] = "1B";			hexcode["9"] = "39";
		hexcode["FS"] = "1C";			hexcode[":"] = "3A";
		hexcode["GS"] = "1D";			hexcode[";"] = "3B";
		hexcode["<"] = "3C";			hexcode["Z"] = "5A";
		hexcode["="] = "3D";			hexcode["["] = "5B";
		hexcode[">"] = "3E";			hexcode["\\"] = "5C";
		hexcode["?"] = "3F";			hexcode["]"] = "5D";
		hexcode["@"] = "40";			hexcode["^"] = "5E";
		hexcode["A"] = "41";			hexcode["_"] = "5F";
		hexcode["B"] = "42";			hexcode["`"] = "60";
		hexcode["C"] = "43";			hexcode["a"] = "61";
		hexcode["D"] = "44";			hexcode["b"] = "62";
		hexcode["E"] = "45";			hexcode["c"] = "63";
		hexcode["F"] = "46";			hexcode["d"] = "64";
		hexcode["G"] = "47";			hexcode["e"] = "65";
		hexcode["H"] = "48";			hexcode["f"] = "66";
		hexcode["I"] = "49";			hexcode["g"] = "67";
		hexcode["J"] = "4A";			hexcode["h"] = "68";
		hexcode["K"] = "4B";			hexcode["i"] = "69";
		hexcode["L"] = "4C";			hexcode["j"] = "6A";
		hexcode["M"] = "4D";			hexcode["k"] = "6B";
		hexcode["N"] = "4E";			hexcode["l"] = "6C";
		hexcode["O"] = "4F";			hexcode["m"] = "6D";
		hexcode["P"] = "50";			hexcode["n"] = "6E";
		hexcode["Q"] = "51";			hexcode["o"] = "6F";
		hexcode["R"] = "52";			hexcode["p"] = "70";
		hexcode["X"] = "53";			hexcode["q"] = "71";
		hexcode["T"] = "54";			hexcode["r"] = "72";
		hexcode["U"] = "55";			hexcode["s"] = "73";
		hexcode["V"] = "56";			hexcode["t"] = "74";
		hexcode["W"] = "57";			hexcode["u"] = "75";
		hexcode["X"] = "58";			hexcode["v"] = "76";
		hexcode["Y"] = "59";			hexcode["w"] = "77";
		hexcode["x"] = "78";			hexcode["{"] = "7B";
		hexcode["y"] = "79";			hexcode["|"] = "7C";
		hexcode["z"] = "7A";			hexcode["}"] = "7D";
		hexcode["7E"] = "~";			hexcode["DEL"] = "7F";
	}
	void getsymtabel() {
		ifstream read("interMed.txt");
		string add, symbol, opcode = "", s1, rabesh;
		while (opcode != "END") {
			getline(read, s1);
			add = symbol = opcode = "";
			get(s1, add, symbol, opcode, rabesh);
			if (symbol != "")
				symtab[symbol] = add;
		}
		read.close();
	}
	string cutspace(string s) {
		for (int i = 0; i < s.size(); i++)
		{
			if (s[i] == '\t') s[i] = ' ';
		}
		int i = 0;
		string temp = "";
		for (int i = 0;i < s.size();++i)
		{
			if (s[i] == ' ') continue;
			while (i < s.size() && s[i] != ' ')
				temp += s[i++];
			temp += ' ';
		}
		return temp;
	}
	void get(string s, string& s1, string& s2, string& s3, string& s4, int num = 3) {
		s = cutspace(s);
		string temp = "";
		bool first = true, allowed = false;
		for (int i = 0;i < s.size();++i) {
			if (s[i] == ' ') {
				if (allowed)
				{
					s4 = temp;
					return;
				}
				if (optab.find(temp) != optab.end() || dir.find(temp) != dir.end()) {
					s3 = temp;
					if (num == 3)
						return;
					else
						allowed = true;
				}
				else
				{
					if (first)
					{
						s1 = toupperHex(temp), first = false;
					}
					else
						s2 = temp;
				}
				temp = "";
			}
			else
				temp += s[i];
		}
	}
	void printout(ofstream& print, string text, int txtsize) {
		string textsize;
		textsize = toupperHex(tohex(txtsize - 7));
		textsize.insert(0, ZEROS, 0, 2 - textsize.size());
		text.insert(7, textsize, 0, text.size());
		print << text << "\n";
	}
public:
	pass2() {
		fillOpcode();
		fillDirectives();
		fillHexacodes();
	}
	void generatingObjectProgram() {
		ifstream read("interMed.txt");
		ofstream print("output.txt");
		getsymtabel();

		string inputLine, address, label, opcode = "", operand = "", text = "T", textsize = "0";
		while (true) {
			getline(read, inputLine);
			if (inputLine[0] == '.') continue;
			operand = "";
			get(inputLine, address, label, opcode, operand, 4);
			if (opcode == "START")
			{
				print << "H" << label << address.insert(0, ZEROS, 0, 6 - address.size()) << lenght << "\n";
			}
			else if (opcode == "END")
			{
				printout(print, text, text.size());
				break;
			}
			else {
				if (text == "T")
					text += address.insert(0, ZEROS, 0, 6 - address.size());
				string objcode = "";
				if (optab.find(opcode) != optab.end())
				{
					objcode += optab[opcode];
					if (operand != "")
					{
						bool index = false;
						if (operand[operand.size() - 1] == 'X' && operand[operand.size() - 2] == ',')
							operand = operand.erase(operand.size() - 2, 2), index = true;
						if (symtab.find(operand) != symtab.end())
						{
							objcode += index ? add(symtab[operand], todecimal("8000")) : symtab[operand];
						}
						else
						{
							cout << "Invalid-symbol";
							system("pause");
						}
					}
					else
						objcode += "0000";
				}
				else if (opcode == "WORD")
				{
					operand = tohex(stoi(operand));
					operand.insert(0, ZEROS, 0, 6 - operand.size());
					objcode = operand;
				}
				else if (opcode == "BYTE")
				{
					if (operand[0] == 'X')
					{
						objcode = operand.substr(2, operand.size() - 3);
						objcode.insert(0, ZEROS, 0, 6 - objcode.size());
					}
					else
					{
						string temp = "";
						for (int i = 2;i < operand.size() - 1;++i) {
							temp += operand[i];
							if (hexcode.find(temp) != hexcode.end())
							{
								objcode += hexcode[temp];
								temp = "";
							}
						}

					}
				}
				else
				{
					if (text.size() > 7)
						printout(print, text, text.size());
					text = "T";
				}
				if (text.size() + objcode.size() > 31)
				{
					printout(print, text, text.size());
					text = "T";
				}

				text += objcode;
			}
		}
		print << "E";
		if (operand != "")
			print << symtab[operand].insert(0, ZEROS, 0, 6 - symtab[operand].size());
		else
			print << ZEROS;
		print.close();
		read.close();
	}
};


int main() {
	pass1 obj1;
	pass2 obj2;
	obj1.generatingAddresses();
	obj2.generatingObjectProgram();
	return 0;
}