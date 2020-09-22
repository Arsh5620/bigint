#include "LibBigInt.h"
#include <iostream>
#include <windows.h>
#include <sstream>
//This function creates its own copy of the data, so you are responsible for deallocating your own data
LibBigInt::LibBigInt(char *c_string, int length)
{
	char *string_my = this->noSpaces(c_string, length);
	char *string_my_deallocation_pointer = string_my;
	int newLength = ::strlen(string_my);
	//so now there should be no spaces or tabs
	//check if the first char is a sign
	if (*string_my == '-')
	{
		string_my++;
		data.isNegative = true;
	}
	else if (*string_my == '+') {
		string_my++;
		data.isNegative = false;
	}
	else {
		data.isNegative = false;
	}
	if (LibBigInt::isNumeric(c_string)) {
		data.data = string_my;
		this->data.deallocation_pointer = string_my;//_deallocation_pointer;
		data.length = newLength - (string_my - string_my_deallocation_pointer);
		//Just because noSpaces function returns a fresh new copy of the data we don't need to recopy anything
	}
	else {
		::MessageBox(NULL, c_string, "", NULL);
		throw "LibBigInt does only accepts integers";
	}
	return;
}
//assuming that if used inside of another division like long division the maximum quotient will be 10 but this is used temporarily
char * LibBigInt::division_by_subs(char * dividor, char * dividend, int length, int length2, int *quotientOut)
{
	//Assumingly everything is correct at this point
	char *master = (char*)::malloc(length);
	::memcpy(master, dividor, length);
	int length_ = length;
	int quotient = 0;
	while (LibBigInt::compare(master, dividend, length_, length2)) {
		bool borrow = false;
		char *bak_master = master;
		master = negation(master, dividend, length_, length2, &borrow);
		::free(bak_master);
		length_ = ::strlen(master);
		quotient++;
	}
	*quotientOut = quotient;
	return(master);
}

char * LibBigInt::division(LibBigInt *object, char ** remainderOut)
{
	char *g_quotient = (char*)::malloc(this->data.length);
	char *bak_quotient = g_quotient;
	char *first_mem = this->data.data;
	char *second_mem = object->data.data;
	int first_length = this->data.length;
	int second_length = object->data.length;
	char *first_mem_max = first_mem + first_length;
	int length = 1;
	char *master = (char*)::malloc(length);
	//unlike other operations the division actually starts from left to right
	bool hasStartedPhasing = false;
	while (first_mem < first_mem_max) {
		char thisChar = *first_mem++;//Iterate the loop as many times as there are the items in the first dividor
		*(master + length - 1) = thisChar;//And the current character from the dividor to the masternotepad

		if (compare(master, second_mem, length, second_length)) {//check if the string contained in the master is greater than or equals to the dividor
			int quotient = 0;
			char *remainder = division_by_subs(master, second_mem, length, second_length, &quotient);//Now you can just do division by substraction
			//Now cast the quotient which is an integer to a string so that it can be concat with the next part of the string
			char *string = (char*)::malloc(8);
			::_itoa_s(quotient, (char*)string, 8, 10);//Now casting happens here

			*(g_quotient++) = string[0];//And a lot of assumptions are made at this step
			::free(string);
			//Now change the master to the remainder with all preceding '0' chars removed
			::free(master);
			master = remainder;
			hasStartedPhasing = true;
		}
		else {
			if (hasStartedPhasing)
				*(g_quotient++) = '0';
		}
		length += 1;
		char *bak_master = master;
		master = (char*)::malloc(length);
		::memcpy(master, bak_master, length - 1);
		::free(bak_master);
	}
	int lengthDiff = g_quotient - bak_quotient;
	char *new_q = LibBigInt::getOver(bak_quotient, '0');
	int skip = new_q - bak_quotient;
	char *new_memory = (char*)malloc((lengthDiff - skip) + 1);
	*(new_memory + lengthDiff - skip) = 0;
	::memcpy(new_memory, new_q, lengthDiff - skip);
	::free(bak_quotient);
	//Create a buffer for comparsion against empty buffer
	char *empty = "0";
	if (LibBigInt::compare(master, empty, length, 1)) {
		char *master_round = LibBigInt::getOver(master, '0');
		int diff = master_round - master;
		int new_length = length - diff - 1;
		char *new_memory = (char*)::malloc(new_length + 1);
		*(new_memory + new_length) = 0;
		::memcpy(new_memory, master_round, new_length);
		*remainderOut = new_memory;
		::free(master);
	}
	else {
		*remainderOut = 0;
		::free(master);
	}
	return new_memory;
}

AdditionalOperations LibBigInt::getAddOps()
{
	AdditionalOperations aOps;
	aOps.memory = this->additionalOperations;
	aOps.size = this->additionalOpsSize;
	return aOps;
}

LibBigInt *LibBigInt::operator+(LibBigInt *object) {
	bool isNegative = false;
	char *_add = addition(object, &isNegative);
	int _addSize = ::strlen(_add);
	LibBigInt *add = new LibBigInt(_add, _addSize);
	add->data.isNegative = isNegative;
	::free(_add);
	return(add);
}
LibBigInt * LibBigInt::operator-(LibBigInt *object)
{
	bool isNegative = false;
	char *substract = this->substraction(object, &isNegative);
	int _subSize = ::strlen(substract);
	LibBigInt *sub = new LibBigInt(substract, _subSize);
	sub->data.isNegative = isNegative;
	::free(substract);
	return(sub);
}

LibBigInt * LibBigInt::operator*(LibBigInt *object)
{
	bool isNegative = false;
	char *multiply = this->multiplication(object);
	int _mulSize = ::strlen(multiply);
	LibBigInt *mul = new LibBigInt(multiply, _mulSize);
	if ((object->data.isNegative&&this->data.isNegative) || (!this->data.isNegative && !object->data.isNegative)) {
		mul->data.isNegative = false;
	}
	else
		mul->data.isNegative = true;
	::free(multiply);
	return(mul);
}

LibBigInt * LibBigInt::operator/(LibBigInt *object)
{
	bool isNegative = false;
	char *remainder = 0;
	char *quotient = this->division(object, &remainder);
	int quotientSize = ::strlen(quotient);
	int remainderSize = ::strlen(remainder);
	LibBigInt *div = new LibBigInt(quotient, quotientSize);
	div->setAddOps(remainder, remainderSize);
	if ((object->data.isNegative&&this->data.isNegative) || (!this->data.isNegative && !object->data.isNegative)) {
		div->data.isNegative = false;
	}
	else
		div->data.isNegative = true;
	::free(quotient);
	//We cannot free remainder as it is stored by the Class, but it might leak memory
	return(div);
}

char *LibBigInt::substraction(LibBigInt *object, bool *resultOut) {
	bool resultIsNegative;
	char *result = 0;
	if ((this->data.isNegative&&object->data.isNegative) || (!this->data.isNegative && !object->data.isNegative)) {
		//This means both of the data elements are negative
		bool big = compare(this->data.data, object->data.data, this->data.length, object->data.length);
		if (big) {
			result = negation(this->data.data, object->data.data, this->data.length, object->data.length, &resultIsNegative);
			resultIsNegative = true;
		}
		else {
			result = negation(object->data.data, this->data.data, object->data.length, this->data.length, &resultIsNegative);
			resultIsNegative = false;
		}
		if ((!this->data.isNegative&&!object->data.isNegative))resultIsNegative = !resultIsNegative;
	}
	else if (this->data.isNegative) {
		resultIsNegative = true;
		result = _addition(this->data.data, object->data.data, this->data.length, object->data.length);
	}
	else if (object->data.isNegative) {
		resultIsNegative = false;
		result = _addition(this->data.data, object->data.data, this->data.length, object->data.length);
	}
	*resultOut = resultIsNegative;
	return(result);
}

char * LibBigInt::addition(LibBigInt *object, bool *resultOut)
{
	bool resultIsNegative;
	char *result = 0;
	if ((this->data.isNegative && !object->data.isNegative) || (!this->data.isNegative&&object->data.isNegative)) {
		bool big = compare(this->data.data, object->data.data, this->data.length, object->data.length);
		if (big) {
			result = negation(this->data.data, object->data.data, this->data.length, object->data.length, &resultIsNegative);
			resultIsNegative = true;
		}
		else {
			result = negation(object->data.data, this->data.data, object->data.length, this->data.length, &resultIsNegative);
			resultIsNegative = false;
		}
		if ((!this->data.isNegative&&!object->data.isNegative))resultIsNegative = !resultIsNegative;
	}
	else if (this->data.isNegative) {
		if (object->data.isNegative) {
			result = LibBigInt::_addition(this->data.data, object->data.data, this->data.length, object->data.length);
			resultIsNegative = true;
		}
	}
	else {
		if (!object->data.isNegative) {
			result = _addition(this->data.data, object->data.data, this->data.length, object->data.length);
			resultIsNegative = false;
		}
	}
	*resultOut = resultIsNegative;
	return(result);
}

//returns mallocated block , need to do some testing
char * LibBigInt::multiplication(LibBigInt *object)
{
	//Lets try doing a simple multiplication, how does this even work
	//FOR THE SAKE OF SIMPLICITY WE DO NOT USE KARATSUBA, MIGHT GET IMPLEMENTED LATER
	char *mem_one = this->data.data;
	char *mem_two = object->data.data;
	int len_one = this->data.length;
	int len_two = object->data.length;
	int required_memory = len_one + len_two;
	char *memory = (char*)malloc(required_memory + 1);
	*(memory + required_memory) = 0;
	for (int i = 0; i < required_memory; i++)
		*(memory + i) = '0';
	int thisLength = required_memory;
	for (int i = len_two - 1; i >= 0; i--) {
		char this_multiply = *(mem_two + i);
		int newLength = 0;
		char *data_content = singular_mult(mem_one, this_multiply, len_one, len_two - i - 1, &newLength);
		if (newLength > thisLength) {
			::MessageBox(NULL, "We have got a problem here, the memory required is more than assume fully calculated", "Error", NULL);
			throw "Program exit handle unexpected";
		}
		char *p_old = memory;
		memory = _addition(memory, data_content, required_memory, newLength);
		::free(p_old);
		::free(data_content);
	}
	return memory;
}

//returns the malloc block
char * LibBigInt::singular_mult(char * string, char multiply, int length, int shift, int *returnLength)
{
	char carry = 0;
	multiply -= '0';
	char *memory = (char*)malloc(length + 1/*1 is added as the size increment*/ + STRING_DELIMITER + shift);
	for (int i = shift - 1; i >= 0; i--)
		*(memory + length + 1 + i) = '0';//All shifts must be zero'0'
		//Also just set the first to zero too because we are not sure if we are going to use it or not
	*(memory) = '0';
	*(memory + 1 + length + shift) = 0;//End the string with a NULL
	for (int q = length - 1; q >= 0; q--) {
		char this_char = *(string + q) - '0';
		char mult = (this_char*multiply) + carry;
		carry = 0;
		if (mult > 9) {
			carry = (mult) / 10;
			mult %= 10;
		}
		*(memory + q + 1) = mult + '0';
	}
	if (carry > 0)
		*(memory) = carry + '0';
	*returnLength = length + 1 + shift;//Just copied from the above statement,just remove the delimiter
	return (memory);
}



//The function just return the forward reference and does not do any copy of the variables and so the return value is just the same location + the next value
//NO malloc block
char * LibBigInt::getOver(char *string, char character)
{
	while (*string == character)
		*string++;
	return(string);
}

//return true if the first string is equal or greater, returns boolean, no malloc
bool LibBigInt::compare(char *string_one, char *string_two, int length_one, int length_two)
{
	if (length_one != length_two) {
		char *bak_one = string_one;
		char *bak_two = string_two;
		while (*string_one == '0')string_one++;
		while (*string_two == '0')string_two++;
		int new_one = string_one - bak_one;
		int new_two = string_two - bak_two;
		length_one -= new_one;
		length_two -= new_two;
		if (length_one == length_two)
			goto label_continue;
		return(length_one > length_two);
	}
label_continue:
	for (int i = 0; i < length_one; i++)
	{
		if (*(string_one + i) != *(string_two + i))
			return(*(string_one + i) > *(string_two + i));
	}
	return(true);
}

//Pure negation no checks on positive or negativeness just negation, and returns the malloc block
char *LibBigInt::negation(char *first, char *second, int length_first, int length_second, bool *borrow_) {
	bool borrow = false;
	int size_one = length_first;
	int size_two = length_second;
	char *mem_one = first + size_one - 1;
	char *mem_two = second + size_two - 1;
	char *bak_mem_one = first;
	char *bak_mem_two = second;
	int maxima = MAX(size_one, size_two);
	char *memory = (char*)malloc(maxima + 1);
	//Considering it is a substraction nothing is ever bigger than the maximum size of the number if you forget about the negative sign for a bit
	//As in addition we always start from the right most 
	for (int i = maxima - 1; i >= 0; i--) {
		char this_one = *mem_one - '0';
		char this_two = *mem_two - '0';
		if (mem_one >= bak_mem_one)mem_one--;
		else this_one = 0;
		if (mem_two >= bak_mem_two)mem_two--;
		else this_two = 0;
		char char_sub = this_one - this_two - borrow;
		borrow = false;
		if (char_sub < 0) {
			char_sub = char_sub + 10;//This should be positive according to me as 12-9 , if no carry 2-9 = -7, -7+10 = 3 that is whats left if we borrow
			borrow = true;
		}
		char_sub += '0';
		*(memory + i) = char_sub;
	}
	*(memory + maxima) = 0;
	*borrow_ = borrow;
	return(memory);
}

//returns the malloc block
char *LibBigInt::_addition(char *string_one, char *string_two, int length_one, int length_two) {
	/*
	First of all write all what we need to achieve using this addition function
	when adding numbers there is a carry, should be able to do a carry
	when adding numbers of uneven length, should be able to add numbers of uneven length
	also for now that is it
	*/
	//First of all get the size of both of the objects that you are going to use
	//also all variables having two in their name must belong to object
	bool carry = false;
	int size_one = length_one;
	int size_two = length_two;
	char *mem_one = string_one + size_one - 1;
	char *mem_two = string_two + size_two - 1;
	char *bak_mem_one = string_one;
	char *bak_mem_two = string_two;
	//So now we have got everything that is required to go on and start adding numbers, but wait we still need the maximum number out of both of the numbers
	int maximum = MAX(size_one, size_two);
	bool addition = (size_one == size_two) && ((*bak_mem_one + *bak_mem_two - '0' - '0') > 9);//That means that the numbers are of the same length and the sum of their first numbers is greater than 9
	addition = addition || (size_one > size_two ? *bak_mem_one == '9' : *bak_mem_two == '9');//now adding another condition that the first letter of any of them is 9 that means there is a probability is high
	//We can be 100 sure but that require too complex computations 
	//If we are going to add the extra number make sure to set it to '0'
	char *memory = (char*)::malloc(maximum + addition + STRING_DELIMITER);
	for (int i = maximum - 1 + addition; i >= (addition ? 1 : 0); i--) {
		char char_one = *mem_one - '0',
			char_two = *mem_two - '0';
		if (mem_one >= bak_mem_one)mem_one--;
		else char_one = 0;
		if (mem_two >= bak_mem_two)mem_two--;
		else char_two = 0;
		char char_add = char_one + char_two + carry;
		carry = false;
		if (char_add > 9) {
			carry = true;
			char_add %= 10;
		}
		char_add += '0';
		*(memory + i) = char_add;
	}
	if (addition)
		if (carry)
			*(memory) = '1';
		else
			*(memory) = '0';
	*(memory + maximum + addition) = 0;
	return(memory);
}

LibBigInt::~LibBigInt()
{
	if (data.deallocation_pointer!=NULL) {
		::free(this->data.deallocation_pointer);
	}
	if (this->additionalOperations)
	{
		::free(this->additionalOperations);
	}
}

BigIntDefine LibBigInt::getData() const {
	return(data);
}
int LibBigInt::getLength() const {
	return(data.length);
}
//returns the mallocated char block
char * LibBigInt::noSpaces(char * data, int length)
{
	char *space = (char*)malloc(length);
	//We need to define what kind of string we want to have escaped, ' ','	'
	::ZeroMemory(space, length);
	char *bak_space = space;
	while (*data != NULL/*or the string hasn't came to an end yet as C style strings end with a NULL*/) {
		if (*data == ' ' || *data == '\t') {
			continue;
		}
		else {
			*(space++) = *data;
		}
		data++;
	}
	char *newMemory = (char*)malloc((space - bak_space) + 1);//+1 for the null terminating character as in c style strings
	//We did not need it in the former case because we were explicitily storing the size
	::memcpy(newMemory, bak_space, space - bak_space);
	*(newMemory + (space - bak_space)) = 0;
	::free(bak_space);
	return newMemory;
}
//returns value type
void LibBigInt::setAddOps(char * ops, int length)
{
	this->additionalOperations = ops;
	this->additionalOpsSize = length;
}
//returns value type
bool LibBigInt::isNumeric(char *c_str) {
	if (*c_str == '+' || *c_str == '-')
		c_str++;
	while (*c_str != 0) {
		if (!(*c_str >= '0'&&*c_str <= '9')) { return(false); }
		c_str++;
	}
	return(true);
}