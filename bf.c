#include <stdio.h>
#define SIZE 30000


enum command {ADD = 1, MOVE, PRINT, READ, GOTO, ASSIGN};
struct code{
	enum command cmd;
	int arg;
};
struct code src[SIZE];
struct code* stk[SIZE];
struct code** top;
unsigned char mem[SIZE];
unsigned char* ptr;
char str[SIZE];

int prs_0(struct code* p){
	if(p->cmd == ADD){
		p->arg++;
		if(p->arg == 256 || p->arg == 0){
			return -1;
		};
		return 0;
	};
	if(p->cmd == ASSIGN){
		p->arg++;
		p->arg %= 256;
		return 0;
	};
	p++;
	p->cmd = ADD;
	p->arg = 1;
	return 1;
};

int prs_1(struct code* p){
	if(p->cmd == ADD){
		p->arg--;
		if(p->arg == -256 || p->arg == 0){
			return -1;
		};
		return 0;
	};
	if(p->cmd == ASSIGN){
		p->arg--;
		p->arg %= 256;
		return 0;
	};
	p++;
	p->cmd = ADD;
	p->arg = -1;
	return 1;
};

int prs_2(struct code* p){
	if(p->cmd == MOVE){
		p->arg++;
		if(p->arg == 0){
			return -1;
		};
	}else{
		p++;
		p->cmd = MOVE;
		p->arg = 1;
		return 1;
	};
	return 0;
};

int prs_3(struct code* p){
	if(p->cmd == MOVE){
		p->arg--;
		if(p->arg == 0){
			return -1;
		};
	}else{
		p++;
		p->cmd = MOVE;
		p->arg = -1;
		return 1;
	};
	return 0;
};

int prs_4(struct code* p){
	if(p->cmd == PRINT){
		p->arg++;
	}else{
		p++;
		p->cmd = PRINT;
		p->arg = 1;
		return 1;
	};
	return 0;
};

int prs_5(struct code* p){
	if(p->cmd == READ){
		p->arg++;
	}else{
		p++;
		p->cmd = READ;
		p->arg = 0;
		return 1;
	};
	return 0;
};

int prs_6(struct code* p){
	p++;
	p->cmd = GOTO;
	p->arg = 0;
	*top = p;
	top++;
	return 1;
};

int prs_7(struct code* p){
	top--;
	if(p->cmd == ADD && (p->arg == 1 || p->arg == -1) && (p - 1)->cmd == GOTO){
		p--;
		p->cmd = ASSIGN;
		p->arg = 0;
		return -1;
	};
	p++;
	(*top)->arg = p - (*top);
	p->arg = (*top) - p;
	p->cmd = GOTO;
	return 1;
};

int nothing(struct code* p){
	return 0;
};

int find(char ch){
	static char s[] = "+-><.,[]";
	int i = 0;
	while(s[i] != ch && i < 8){
		i++;
	};
	return i;
};

int brackets(){
	int c = 0;
	char* i;
	for(i = str; *i != '\0' && c >= 0; ++i){
		if(*i == '[')c++;
		if(*i == ']')c--;
	};
	return (c != 0);
};

int exe_a(int arg){
	*ptr += arg;
	return 0;
};

int exe_b(int arg){
	ptr += arg;
	if(ptr >= mem + SIZE){
		ptr -= SIZE;
	};
	if(ptr < mem){
		ptr += SIZE;
	};
	return 0;
};

int exe_c(int arg){
	int c;
	for(c = 0; c < arg; ++c){
		putchar(*ptr);
	};
	return 0;
};

int exe_d(int arg){
	int c;
	for(c = 0; c < arg; ++c){
		getchar();
	};
	*ptr = getchar();
	return 0;
};

int exe_e(int arg){
	if((arg > 0 && *ptr == 0) || (arg < 0 && *ptr != 0)){
		return arg;
	};
	return 0;
};

int exe_f(int arg){
	*ptr = arg;
	return 0;
};

void out(){
	struct code* p = src + 1;
	for(; p->cmd; ++p){
		printf("%c %d\n", p->cmd + 'a' - 1, p->arg);
	};
};

void parse(){
	struct code *p = src;
	p->cmd = 0;
	p->arg = 0;
	char* ch = str;
	top = stk;
	int (*prs[])(struct code*) = {prs_0, prs_1, prs_2, prs_3, prs_4, prs_5, prs_6, prs_7, nothing};
	for(; *ch != '\0'; ++ch){
		p += (*prs[find(*ch)])(p);
	};
	(p + 1)->cmd = 0;
};

int main(int argc, char* argv[]){
	if(argc == 1){
		fprintf(stderr, "%s: Wrong argument count\n", argv[0]);
		return 1;
	};
	FILE* f = fopen(argv[1], "r");
	if(f == NULL){
		fprintf(stderr, "%s: Can't open %s\n", argv[0], argv[1]);
		return 2;
	};
	int n = fread(str, sizeof(char), SIZE - 1, f);
	if(n == 0){
		fprintf(stderr, "%s: Can't read data from %s\n", argv[0], argv[1]);
		return 3;
	};
	str[n] = '\0';
	fclose(f);
	if(brackets()){
		fprintf(stderr, "%s: Wrong brackets sequence\n", argv[0]);
		return 4;
	};
	parse();
	ptr = mem;
	int (*exe[])(int) = {exe_a, exe_b, exe_c, exe_d, exe_e, exe_f};
	struct code* p = src + 1;
	for(; p->cmd; ++p){
		p += (*exe[p->cmd - 1])(p->arg);
	};
	return 0;
};
