int global;

int add_one(int i){
    i=i+1;
    return i;
}

int test_arithmic(int a, int b, int c){
    a=b+c;
    a=b-c;
    a=b*c;
    a=b/c;
    a = -b;
    a = !b;
    a = b && c;
    a = b || c;
    a = b > c;
    a = b < c;
    a = b != c;
    a = b % c;
    a = b == c;
    return a;
}

int test_adv_arithmic(){
    int a;
    int b;
    int c;
    a = 1;
    b = 1;
    c = 1;
    a = a + b * c;
    a = a * b + c;
    a = a / b + c;
    a = (a + b) / c;
    a = (a + b) * c;
    a = a * (b + c);
    a = a / (b + c);
    a = (a + b) / c;
    a = -a + b;
    a = a + b > c;
    a = a > b + c;
    a = a == b + c;
    a = a + b == c;
    a = a + b && c;
    a = a && b + c;
    return 0;
}

int test_vtb(){
	int a;
	a = 0;
	if(a == 0){
		int a;
		a = 0;
	}
	a = 0;
	if(a == 0){
		int a;
		a = 0;
	}else{
		int a;
		a = 0;
	}
	a = 0;
	while(a == 0){
		int a;
		a = 0;
	}
	a = 0;
	return a;
}

int test_array(int a[5]){
	a[0] = 1;
	int b;
	b = a[0];
	return b;
}

int test_buildin_func(){
    int b;
    b = getint();
    b = putint(b);
    b = getchar();
    b = putchar(b);
    return 0;
}

int main(){
    int a;
    int b;
    int c;
    b=1;
    c=1;
    a=test_arithmic(a,b,c);
    a=add_one(a);
    a = test_adv_arithmic();
    int d[5];
    a = test_array(d);
    return 0;
}
