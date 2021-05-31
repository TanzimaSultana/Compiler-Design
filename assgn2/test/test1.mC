int main(){
	int a;
	int b;
	char x;
	string str;
	int x;
	int y;
	int z;

	y = 10;
	z = 10 * 44;
	xy = (z * 10) / 3;

	a = 100;
	x = 'X';

	if(a==1000)
		b =10;
	if(a >= 10 && b <= 5)
		a = a + b;
	if(!a)
		a = 100;

	a = a + 100;
	b = a * b + 33;

	if(a >= 100)
		b = 200;
	if(b <= 50)
		a = b;

	if(a == 100)
		b = 400;
	else
		b = 800;

	if(a != b){
		a = a - 100;
		if(b > 5)
			b = 5;
		if(b < 0)
			b = 0;

	}
	else{
		a = b * 100;
		b = a % 10;
	}

	while(a < 10){
		b = b / 10;
		a = a + 1;
	}

	return a;

	/* This is comment */
}