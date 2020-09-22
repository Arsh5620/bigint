#pragma once
//At first we were going to use an already written library called Terminal.Port as it already has an implementation for the BigIntegers 
//written in C and assembly and it uses the base 2 so it has some tradeoffs, for a number that is as big as a trillion digits, that library can process
//it way faster than this library can but the serialization of the data time was a critical
#define STRING_DELIMITER 1
#define MAX(X,Y) (X>Y?X:Y)
typedef struct {
	bool isNegative;
	char *data;
	char *deallocation_pointer;
	int length;
	//Floating point not yet supported
} BigIntDefine;
typedef struct {
	char *memory;
	int size;
} AdditionalOperations;
class LibBigInt
{
private:
	BigIntDefine data = { 0 };
	char *noSpaces(char *data, int length);
	bool compare(char *, char *, int, int);
	char *negation(char *first, char *object, int length_first, int length_second, bool *);
	char *substraction(LibBigInt *, bool *);
	char *addition(LibBigInt*, bool*);
	char *multiplication(LibBigInt *);
	char *singular_mult(char *string, char multiply, int length, int shift, int *);
	char *_addition(char*, char*, int, int);
	char *additionalOperations;
	int additionalOpsSize;
	void setAddOps(char *ops, int length);
public: static bool isNumeric(char *);
public:
	static char *getOver(char *, char);
	BigIntDefine getData() const;
	int getLength() const;
	LibBigInt (char *,int);
	char *division_by_subs(char *dividor, char *dividend, int length, int length2,int*);
	char *division(LibBigInt *, char **remainder);
	AdditionalOperations getAddOps();
	LibBigInt *operator+(LibBigInt*);
	LibBigInt *operator-(LibBigInt *);
	LibBigInt *operator*(LibBigInt *);
	LibBigInt *operator/(LibBigInt *);
	~LibBigInt();
};

